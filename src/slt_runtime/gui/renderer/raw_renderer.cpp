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


      // We create a simple 1x1 white texture to use as placeholder.
      render::TexConfig white_tex_cfg;
      white_tex_cfg.size = { 1, 1 };
      white_tex_cfg.levels = 1;
      white_tex_cfg.tex_format = render::TexFormat::RGBA_8;

      white_tex_ = render::createTexture(white_tex_cfg);
      std::uint32_t tex_data = 0xFFFFFFFF;

      render::updateTexture(white_tex_, { 0,0 }, { 1,1 }, &tex_data, render::TexDataFormat::RGBA_8);
    }

    void RawRenderer::draw(std::vector<Vertex> verts,
                           std::vector<Index> indices,
                           render::TextureRef color_texture,
                           render::TextureRef mask_texture) {
      render::ProgramUsage prog_use(program_);
      render::TextureUsage col_tex_use(color_texture ? color_texture : white_tex_ , 0);
      render::TextureUsage mask_tex_use(mask_texture ? mask_texture : white_tex_, 1);

      updateModelData(model_, verts, indices, indices.size());
      slt::render::draw(model_);
    }
  }
}
