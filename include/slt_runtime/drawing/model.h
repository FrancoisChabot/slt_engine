#ifndef SLT_RUNTIME_RENDER_MODEL_H
#define SLT_RUNTIME_RENDER_MODEL_H

#include "slt/math/vec.h"
#include "slt/mem/data_view.h"

#include <functional>
#include <memory>

namespace slt {
namespace render {

enum class PrimitiveType {
  TRIANGLES,
};

struct VertexDescriptor {
  std::string channel;
  std::uint8_t dimensions;
};

struct Model {
  virtual ~Model() {}
};

using ModelRef = std::shared_ptr<Model>;

}  // namespace render

void loadModel(std::string const&, std::string const&,
               std::function<void(render::ModelRef)>);
render::ModelRef createModel(render::PrimitiveType,
                             std::vector<render::VertexDescriptor> const&,
                             DataView vertex_data, DataView index_data,
                             int index_count);

namespace models {
// Creates a single Quad model

render::ModelRef createQuad(Vec2 const& tl, Vec2 const& br);
}
}  // namespace slt
#endif