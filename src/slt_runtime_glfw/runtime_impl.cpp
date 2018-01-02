#include "slt_runtime/runtime.h"
#include "slt_runtime_glfw/render/gl_includes.h"

#include "slt/debug/assert.h"
#include "slt/log.h"
#include "slt_runtime/render.h"

namespace {
slt::Setting<bool> enable_opengl_debugging(true, "opengl_debug",
                                           "enable opengl debug logging");

slt::Setting<bool> vsync(true, "vsync", "enable vsync");

void glfw_error_callback(int error, const char* description) {
  slt::log->error("GLFW error: {}, {}", error, description);
}

void APIENTRY opengl_error_callback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar* message,
                                    const void* userParam) {
  switch (severity) {
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_HIGH:
      slt::log->error("OpenGL error: {}", message);
      break;

    case GL_DEBUG_SEVERITY_LOW:
    default:
      // slt::log->info("OpenGL message: {}", message);
      break;
  }
}
}
namespace slt {
struct RuntimeBackendOpaqueMembers {
  GLFWwindow* window = nullptr;
};

Runtime::Runtime() : members_(std::make_unique<RuntimeBackendOpaqueMembers>()) {
  SLT_ASSERT(instance == nullptr);
  instance = this;

  slt::log->info("Initializing platform (GLFW)");
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw RuntimeInitError("(glfw) init failure.");
  }

  slt::log->info("Creating runtime window (GLFW)");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  if (enable_opengl_debugging.get()) {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  members_->window = glfwCreateWindow(
      settings::window_size.get().x, settings::window_size.get().y,
      settings::window_title.get().c_str(), nullptr, nullptr);
  if (!members_->window) {
    glfwTerminate();
    throw RuntimeInitError("(glfw) Failed to create window.");
  }

  glfwMakeContextCurrent(members_->window);
  glfwSwapInterval(vsync.get() ? 1 : 0);

  glfwMakeContextCurrent(members_->window);

  auto glad_status = gladLoadGL();
  if (!glad_status) {
    glfwDestroyWindow(members_->window);
    glfwTerminate();
    throw RuntimeInitError(
        "(glad) Failed to initialize opengl function pointers.");
  }

  if (enable_opengl_debugging.get()) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_error_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          true);
  }

  commonInit();
}

Runtime::~Runtime() {
  commonShutdown();
  SLT_ASSERT(instance != nullptr);

  slt::log->info("Tearing down platform (GLFW)");

  glfwDestroyWindow(members_->window);
  glfwTerminate();

  instance = nullptr;
}

void Runtime::platformSync() { glfwPollEvents(); }

bool Runtime::keepRunning() {
  return glfwWindowShouldClose(members_->window) == 0;
}

namespace render {
FrameScope::FrameScope() {}
FrameScope::~FrameScope() {
  glfwSwapBuffers(slt::Runtime::instance->members_->window);
}
}
}
