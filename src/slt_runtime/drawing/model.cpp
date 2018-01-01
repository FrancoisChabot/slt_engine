#include "slt_runtime/drawing/model.h"
#include "slt/log.h"
#include "slt_runtime/drawing/model.h"

namespace slt {
namespace models {
// Creates a single Quad model
render::ModelRef createQuad(Vec2 const& tl, Vec2 const& br) {
  log->info("creating quad");
  //  0   1
  //  2   3
  std::vector<std::uint8_t> index_data = {0, 1, 2, 2, 1, 3};

  std::vector<float> vertex_data;
  vertex_data.reserve(4 * 4);
  vertex_data.emplace_back(tl.x);
  vertex_data.emplace_back(tl.y);
  vertex_data.emplace_back(0.0f);
  vertex_data.emplace_back(0.0f);

  vertex_data.emplace_back(br.x);
  vertex_data.emplace_back(tl.y);
  vertex_data.emplace_back(1.0f);
  vertex_data.emplace_back(0.0f);

  vertex_data.emplace_back(tl.x);
  vertex_data.emplace_back(br.y);
  vertex_data.emplace_back(0.0f);
  vertex_data.emplace_back(1.0f);

  vertex_data.emplace_back(br.x);
  vertex_data.emplace_back(br.y);
  vertex_data.emplace_back(1.0f);
  vertex_data.emplace_back(1.0f);

  std::vector<render::VertexDescriptor> desc(2);
  desc[0].channel = "slt_vertex";
  desc[0].dimensions = 2;
  desc[1].channel = "slt_uv";
  desc[1].dimensions = 2;

  return createModel(render::PrimitiveType::TRIANGLES, desc, vertex_data,
                     index_data, 6);
}
}
}