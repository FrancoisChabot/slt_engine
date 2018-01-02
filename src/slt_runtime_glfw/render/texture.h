#ifndef SLT_RUNTIME_GLFW_TEXTURE_H
#define SLT_RUNTIME_GLFW_TEXTURE_H

#include "slt/mem/data_view.h"
#include "slt_runtime/drawing/texture.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
class GLFWTexture : public slt::render::Texture {
 public:
   GLFWTexture(TexConfig const& cfg);
   GLFWTexture(DataView data);
  ~GLFWTexture();

  GLuint handle() const { return handle_; }

 private:
  GLuint handle_;
};
}
}

#endif