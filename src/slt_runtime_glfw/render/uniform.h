#ifndef SLT_RUNTIME_GLFW_UNIFORM_H
#define SLT_RUNTIME_GLFW_UNIFORM_H

#include "slt_runtime/drawing/uniform.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
class GLFWUniformBlock : public slt::render::UniformBlock {
 public:
  GLFWUniformBlock(std::size_t size, GLuint loc)
      : size_(size), location_(loc) {}

  std::size_t size() const { return size_; }

  GLuint location() const { return location_; }

private:
  GLuint location_;
  const std::size_t size_;
};

class GLFWUniformBuffer : public slt::render::UniformBuffer {
 public:
  GLFWUniformBuffer(std::shared_ptr<GLFWUniformBlock> b);
  ~GLFWUniformBuffer();
  
  std::shared_ptr<GLFWUniformBlock> const& block() const { return block_; }
  GLuint ubo() const {
    return ubo_;
  }
private:
  std::shared_ptr<GLFWUniformBlock> block_;
  GLuint ubo_;
};
}
}

#endif
