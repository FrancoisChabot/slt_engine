#ifndef SLT_RUNTIME_GLFW_PROGRAM_H
#define SLT_RUNTIME_GLFW_PROGRAM_H

#include "slt/mem/data_view.h"
#include "slt_runtime/drawing/program.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
class GLFWProgram : public slt::render::Program {
 public:
  GLFWProgram(DataView data);
  ~GLFWProgram();

  GLuint handle() const { return handle_; }

 private:
  GLuint handle_;

  void link_();
  void resolveAttributes_();
  void resolveUniformBuffers_();
};
}
}

#endif