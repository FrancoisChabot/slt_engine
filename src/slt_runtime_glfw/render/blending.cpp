#include "slt_runtime/drawing/blending.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
  BlendingUsage::BlendingUsage() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  BlendingUsage::~BlendingUsage() {
    glDisable(GL_BLEND);
  }
}
}
