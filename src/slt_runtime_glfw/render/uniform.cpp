#include "slt_runtime/drawing/uniform.h"
#include "slt_runtime_glfw/render/uniform.h"

#include "slt/debug/assert.h"

#include <unordered_map>
#include <vector>

namespace {
std::unordered_map<std::string, std::weak_ptr<slt::render::GLFWUniformBlock>>
    blocks_;
std::unordered_map<std::string, std::weak_ptr<slt::render::GLFWUniformBuffer>>
    named_ubos_;

GLuint next_uniform_block_location = 0;
GLint max_uniform_block_bindings = 0;
std::vector<GLuint> bound_buffers;
}

namespace slt {
namespace render {

//
UniformBlockRef registerUniformBlock(std::string const& name,
                                     std::size_t size) {
  if (next_uniform_block_location == 0) {
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_block_bindings);
    bound_buffers.resize(max_uniform_block_bindings);
  }

  if (next_uniform_block_location >= (GLuint)max_uniform_block_bindings) {
    // TODO: there is no way to alias/recycle uniform blocks at the moment.
    throw std::runtime_error("Ran out of uniform blocks");
  }

  auto result =
      std::make_shared<GLFWUniformBlock>(size, next_uniform_block_location++);
  blocks_[name] = result;
  return result;
}

UniformBlockRef getUniformBlock(std::string const& name) {
  return blocks_.at(name).lock();
}

UniformBufferRef getUbo(UniformBlockRef ref, std::string const& name) {
  auto glfw_ref = std::static_pointer_cast<GLFWUniformBlock>(std::move(ref));
  if (name.empty()) {
    return std::make_shared<GLFWUniformBuffer>(std::move(glfw_ref));
  } else {
    auto found = named_ubos_.find(name);
    if (found != named_ubos_.end()) {
      if (!found->second.expired()) {
        auto locked = found->second.lock();
        SLT_ASSERT(locked->block() == ref);
        return locked;
      } else {
        auto result = std::make_shared<GLFWUniformBuffer>(std::move(glfw_ref));
        found->second = result;
        return result;
      }
    } else {
      auto result = std::make_shared<GLFWUniformBuffer>(std::move(glfw_ref));
      named_ubos_.emplace(name, result);
      return result;
    }
  }
}

GLFWUniformBuffer::GLFWUniformBuffer(std::shared_ptr<GLFWUniformBlock> b)
  : block_(std::move(b)) {
    glGenBuffers(1, &ubo_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferData(GL_UNIFORM_BUFFER, block_->size(), nullptr,
      GL_STREAM_DRAW);
}

GLFWUniformBuffer::~GLFWUniformBuffer() {
  glDeleteBuffers(1, &ubo_);
}

UniformBufferUsage::UniformBufferUsage(UniformBufferRef const& buf) {
  auto native = static_cast<GLFWUniformBuffer*>(buf.get());
  auto native_block = static_cast<GLFWUniformBlock*>(native->block().get());

  glBindBufferBase(GL_UNIFORM_BUFFER, native_block->location(),
                   native->ubo());
}

UniformBufferUsage::~UniformBufferUsage() {

}

void updateUboData(UniformBufferRef const& buf, DataView data) {
  auto native = static_cast<GLFWUniformBuffer*>(buf.get());
  glBindBuffer(GL_UNIFORM_BUFFER, native->ubo());
  glBufferSubData(GL_UNIFORM_BUFFER, 0, data.size(), data.data());
};

}  // namespace render
}  // namespace slt
