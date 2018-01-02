#ifndef SLT_GUI_RENDERER_RAW_RENDERER_H
#define SLT_GUI_RENDERER_RAW_RENDERER_H

#include "slt_runtime/gui/widget.h"
#include "slt_runtime/drawing/program.h"
#include "slt_runtime/drawing/model.h"

namespace slt {
  namespace gui {
    class RawRenderer : public WidgetRenderer {
    public:
      RawRenderer(render::ProgramRef);
      void draw(std::vector<Vertex>, std::vector<Index>, render::TextureRef texture) override;

    private:
      render::ProgramRef program_;
      render::ModelRef model_;
    };
  }
}
#endif
