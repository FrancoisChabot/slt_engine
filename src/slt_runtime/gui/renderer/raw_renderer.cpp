#include "slt_runtime/gui/renderer/raw_renderer.h"
#include "slt_runtime/runtime.h"

namespace slt {
  namespace gui {
    RawRenderer::RawRenderer(render::ProgramRef prg)
      : program_(std::move(prg)) {

      std::vector<render::VertexDescriptor> desc(3);
      desc[0].channel = "slt_vertex";
      desc[0].dimensions = 2;
      desc[0].type = render::AttribType::FLOAT;

      desc[1].channel = "slt_uv";
      desc[1].dimensions = 2;
      desc[1].type = render::AttribType::FLOAT;

      desc[2].channel = "slt_color";
      desc[2].dimensions = 4;
      desc[2].type = render::AttribType::FLOAT;

      model_ = createModel(slt::render::PrimitiveType::TRIANGLES, 
        desc, 
        DataView(), DataView(), 0);
    }

    void RawRenderer::draw(std::vector<Vertex> verts,
                           std::vector<Index> indices,
                           render::TextureRef texture) {
      render::ProgramUsage prog_use(program_);
      render::TextureUsage tex_use(texture , 0);

      updateModelData(model_, verts, indices, indices.size());
      slt::render::draw(model_);
    }
  }
}
