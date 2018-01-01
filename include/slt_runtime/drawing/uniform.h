#ifndef SLT_RUNTIME_RENDER_UNIFORM_H
#define SLT_RUNTIME_RENDER_UNIFORM_H

#include <memory>
#include <string>

namespace slt {
namespace render {

struct UniformBlock {
  virtual ~UniformBlock(){};
};

struct UniformBuffer {
  virtual ~UniformBuffer(){};
};

using UniformBlockRef = std::shared_ptr<UniformBlock>;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;

//
UniformBlockRef registerUniformBlock(std::string const&, std::size_t);

template <typename T>
UniformBlockRef registerUniformBlock(std::string const& name) {
  return registerUniformBlock(name, sizeof(T));
}

UniformBlockRef getUniformBlock(std::string const&);
UniformBufferRef getUbo(UniformBlockRef, std::string const& = "");

}  // namespace render

}  // namespace slt
#endif