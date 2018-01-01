#include "slt_runtime/render.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
void clearFrame(Color const& c) {
  glClearColor(c.r, c.g, c.b, c.a);
  glClear(GL_COLOR_BUFFER_BIT);
}
}
}
