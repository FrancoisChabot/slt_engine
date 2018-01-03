#ifndef MYGUI_WIDGETS_IMAGE_H
#define MYGUI_WIDGETS_IMAGE_H

#include "slt_runtime/gui/widget.h"
#include "slt_runtime/drawing/texture.h"

#include <string>

namespace slt {
  namespace gui {

    // A simple image displayer
    class Image : public Widget {
    public:
      Image(std::string const& package, std::string const& asset);
      void render(WidgetRenderer&, WidgetRenderRequest const& r) override;

    private:
      render::TextureRef texture_;
    };
  }
}

#endif
