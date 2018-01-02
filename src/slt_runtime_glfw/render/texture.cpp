#include "slt_runtime/drawing/texture.h"
#include "lodepng.h"
#include "slt/file/read.h"
#include "slt/log.h"
#include "slt_runtime/runtime.h"
#include "slt_runtime_glfw/render/texture.h"

#include <unordered_map>

namespace {
std::string getFileForTexture(std::string const& package,
                              std::string const& name) {
  return package + "/images/" + name + ".png";
}

GLenum convertTexFormat(slt::render::TexFormat fmt) {
  switch (fmt) {
  case slt::render::TexFormat::RGBA_8:
    return GL_RGBA8;
  case slt::render::TexFormat::DEPTH:
    return GL_DEPTH_COMPONENT32F;
  case slt::render::TexFormat::SINGLE_8:
    return GL_R8;
  default:
    break;
  }

  return GL_RGBA8;
}

// (format/type)
std::pair<GLenum, GLenum> convertTexDataFormat(slt::render::TexDataFormat fmt) {
  switch (fmt) {
  case slt::render::TexDataFormat::RGBA_8:
    return std::make_pair(GL_RGBA, GL_UNSIGNED_BYTE);
  case slt::render::TexDataFormat::BGRA_8:
    return std::make_pair(GL_BGRA, GL_UNSIGNED_BYTE);
  case slt::render::TexDataFormat::SINGLE_8:
    return std::make_pair(GL_RED, GL_UNSIGNED_BYTE);
  default:
    break;
  }

  return std::make_pair(GL_RGBA, GL_UNSIGNED_BYTE);
}

// Returns the expected byte size for a single piexel
std::size_t expectedSize(slt::render::TexDataFormat fmt, unsigned int w,
  unsigned int h) {
  std::size_t result = 1;
  switch (fmt) {
  case slt::render::TexDataFormat::RGBA_8:
  case slt::render::TexDataFormat::BGRA_8:
    result = slt::mem::alignSize(4 * w, 4);
    break;
  case slt::render::TexDataFormat::SINGLE_8:
    result = slt::mem::alignSize(1 * w, 4);
  default:
    break;
  }
  return result * h;
}
}

namespace slt {
namespace render {
GLFWTexture::GLFWTexture(DataView data) {
  glGenTextures(1, &handle_);

  unsigned char* decoded_data = nullptr;
  unsigned w = 0;
  unsigned h = 0;

  log->info("Decoding texture...");
  auto status = lodepng_decode32(
      &decoded_data, &w, &h, (const unsigned char*)data.data(), data.size());

  if (status != 0) {
    log->error("failed to decode image: {}", status);
    glDeleteTextures(1, &handle_);
    throw std::runtime_error("failed to decode image");
  }
  log->info("success...");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, handle_);
  glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA8, w, h);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE,
    decoded_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  free(decoded_data);
}

GLFWTexture::GLFWTexture(TexConfig const& cfg) {
  glGenTextures(1, &handle_);

  glBindTexture(GL_TEXTURE_2D, handle_);
  glTexStorage2D(GL_TEXTURE_2D, cfg.levels, convertTexFormat(cfg.tex_format),
    cfg.size.x, cfg.size.y);

  std::vector<char> zero(cfg.size.x * cfg.size.y * 4, 0);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cfg.size.x, cfg.size.y, GL_RGBA, GL_UNSIGNED_BYTE, zero.data());
}

GLFWTexture::~GLFWTexture() { glDeleteTextures(1, &handle_); }

TextureUsage::TextureUsage(TextureRef const& tex, int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, static_cast<GLFWTexture*>(tex.get())->handle());
}

TextureUsage::~TextureUsage() {}


TextureRef createTexture(TexConfig const& cfg) {
  return std::make_shared<GLFWTexture>(cfg);
}

void updateTexture(TextureRef const& tex, iVec2 pos, iVec2 size,
  DataView data,
  TexDataFormat format,
  std::size_t row_length) {

  auto native = static_cast<GLFWTexture*>(tex.get());
  SLT_ASSERT_EQ(expectedSize(format, size.x, size.y), data.size());
  glBindTexture(GL_TEXTURE_2D, native->handle());
  auto fmt = convertTexDataFormat(format);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)row_length);
  glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, size.x, size.y, fmt.first, fmt.second,
    data.data());
}
}

void loadTexture(std::string const& package, std::string const& name,
                 std::function<void(render::TextureRef)> cb) {
  slt::file::asyncRead(getFileForTexture(package, name),
                       slt::Runtime::instance->getMainQueue(),
                       [cb](slt::DataBlock d) {
                         cb(std::make_shared<render::GLFWTexture>(d));
                       });
}
}
