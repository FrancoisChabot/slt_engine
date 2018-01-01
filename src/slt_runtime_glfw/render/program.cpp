#include "slt_runtime/drawing/program.h"
#include "slt/file/read.h"
#include "slt/mem/capnputils.h"
#include "slt_capnp/render.capnp.h"
#include "slt_runtime/drawing/attribute.h"
#include "slt_runtime/runtime.h"
#include "slt_runtime_glfw/render/gl_includes.h"
#include "slt_runtime_glfw/render/program.h"
#include "slt_runtime_glfw/render/uniform.h"

#include <capnp/serialize.h>
#include "slt/log.h"

#include <unordered_set>

namespace {
std::unordered_map<std::string, std::weak_ptr<slt::render::GLFWProgram>>
    loaded_programs_;

std::string getFileForProgram(std::string const& package,
                              std::string const& name) {
  return package + "/shaders/" + name + ".cap";
}

GLuint createShader(GLenum type, std::string_view code) {
  GLuint result = glCreateShader(type);

  GLsizei length = GLsizei(code.length());

  const char* src = code.data();
  glShaderSource(result, 1, &src, &length);
  glCompileShader(result);
  GLint isCompiled = 0;
  glGetShaderiv(result, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint info_length = 0;
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &info_length);

    std::vector<GLchar> errlog(info_length);
    glGetShaderInfoLog(result, info_length, &info_length, &errlog[0]);
    std::string err_str(errlog.begin(), errlog.end());
    slt::log->error("shader compilation failure: {}", err_str);
    glDeleteShader(result);

    throw std::runtime_error(err_str);
  }

  return result;
}
}

namespace slt {
namespace render {

GLFWProgram::GLFWProgram(DataView data) {
  auto cap_reader = getCapReader(data);
  auto prg_data = cap_reader.getRoot<data::render::ProgramData>();

  GLuint vshader = 0;
  GLuint fshader = 0;
  GLuint gShader = 0;

  try {
    auto v_str = prg_data.getVertexShader();
    log->info("compiling vertex shader...");
    vshader = createShader(GL_VERTEX_SHADER,
                           std::string_view(v_str.begin(), v_str.size()));
    log->info("success");

    auto f_str = prg_data.getFragmentShader();
    log->info("compiling fragment shader...");
    fshader = createShader(GL_FRAGMENT_SHADER,
                           std::string_view(f_str.begin(), f_str.size()));
    log->info("success");

    if (prg_data.hasGeometryShader()) {
      auto g_str = prg_data.getGeometryShader();
      log->info("compiling gemoetry shader...");
      gShader = createShader(GL_GEOMETRY_SHADER,
                             std::string_view(g_str.begin(), g_str.size()));
      log->info("success");
    }
  } catch (std::exception&) {
    if (vshader) {
      glDeleteShader(vshader);
    }
    if (fshader) {
      glDeleteShader(vshader);
    }
    if (gShader) {
      glDeleteShader(vshader);
    }
    throw;
  }

  handle_ = glCreateProgram();

  glAttachShader(handle_, vshader);
  glDeleteShader(vshader);
  glAttachShader(handle_, fshader);
  glDeleteShader(fshader);
  if (gShader) {
    glAttachShader(handle_, gShader);
    glDeleteShader(gShader);
  }

  try {
    link_();
    resolveAttributes_();
    link_();  // We need to relink for the attribute bindings to take effect.
    resolveUniformBuffers_();
  } catch (std::exception&) {
    glDeleteProgram(handle_);
    throw;
  }

  glDetachShader(handle_, vshader);
  glDetachShader(handle_, fshader);
  if (gShader) {
    glDetachShader(handle_, gShader);
  }
}

void GLFWProgram::link_() {
  glLinkProgram(handle_);

  GLint isLinked = 0;
  glGetProgramiv(handle_, GL_LINK_STATUS, (int*)&isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errlog(maxLength);
    glGetProgramInfoLog(handle_, maxLength, &maxLength, &errlog[0]);
    std::string err_str(errlog.begin(), errlog.end());
    slt::log->error("program linking failure: {}", err_str);

    throw std::runtime_error(err_str);
  }
}

void GLFWProgram::resolveAttributes_() {
  GLint attribs_count = 0;
  glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTES, &attribs_count);

  std::unordered_set<unsigned int> used_attrib_locations;

  for (int i = 0; i < attribs_count; ++i) {
    const int max_name_length = 128;
    char name[max_name_length];
    GLsizei name_length = 0;
    GLint size = 0;
    GLenum type;
    glGetActiveAttrib(handle_, i, max_name_length, &name_length, &size, &type,
                      name);

    try {
      auto bind_loc = getAttributeBindLoc(name);

      if (used_attrib_locations.find(bind_loc) != used_attrib_locations.end()) {
        slt::log->error(
            "Render program uses the same attib location more than once");
        throw std::runtime_error("invalid atttrib setup");
      }
      used_attrib_locations.insert(bind_loc);
      glBindAttribLocation(handle_, bind_loc, name);
    } catch (std::runtime_error&) {
      throw std::runtime_error("invalid atttrib setup");
    }
  }
}

void GLFWProgram::resolveUniformBuffers_() {
  GLint max_name_len = 0;
  GLint blocks_count = 0;

  glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
                 &max_name_len);
  glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_BLOCKS, &blocks_count);

  std::vector<char> block_name(max_name_len);
  for (GLint i = 0; i < blocks_count; ++i) {
    GLsizei len = 0;
    glGetActiveUniformBlockName(handle_, i, max_name_len, &len,
                                block_name.data());
    auto layout = std::static_pointer_cast<GLFWUniformBlock>(
        getUniformBlock(std::string(block_name.data(), len)));
    if (!layout) {
      throw std::runtime_error(
          "Program uses non-registered uniform buffer layout");
    }
    glUniformBlockBinding(handle_, i, layout->location());
  }
}

GLFWProgram::~GLFWProgram() { glDeleteProgram(handle_); }

ProgramUsage::ProgramUsage(ProgramRef const& prg) {
  glUseProgram(static_cast<GLFWProgram*>(prg.get())->handle());
}

ProgramUsage::~ProgramUsage() {}
}

void loadProgram(std::string const& package, std::string const& name,
                 std::function<void(render::ProgramRef)> cb) {
  std::string prg_key = package + "/" + name;
  auto found = loaded_programs_.find(prg_key);
  if (found != loaded_programs_.end() && !found->second.expired()) {
    cb(found->second.lock());
  } else {
    slt::file::asyncRead(getFileForProgram(package, name),
                         slt::Runtime::instance->getMainQueue(),
                         [prg_key, cb](slt::DataBlock d) {
                           auto result =
                               std::make_shared<render::GLFWProgram>(d);
                           loaded_programs_[prg_key] = result;
                           cb(result);
                         });
  }
}
}
