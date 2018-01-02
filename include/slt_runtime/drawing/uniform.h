#ifndef SLT_RUNTIME_RENDER_UNIFORM_H
#define SLT_RUNTIME_RENDER_UNIFORM_H

#include <memory>
#include <string>

#include "slt/mem/data_view.h"

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

struct UniformBufferUsage {
  UniformBufferUsage(UniformBufferRef const& prg);
  ~UniformBufferUsage();
};

//
UniformBlockRef registerUniformBlock(std::string const&, std::size_t);

template <typename T>
UniformBlockRef registerUniformBlock(std::string const& name) {
  return registerUniformBlock(name, sizeof(T));
}

UniformBlockRef getUniformBlock(std::string const&);
UniformBufferRef getUbo(UniformBlockRef, std::string const& = "");
void updateUboData(UniformBufferRef const&, DataView);
}  // namespace render

}  // namespace slt
#endif
