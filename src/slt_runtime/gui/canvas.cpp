#include "slt_runtime/gui/layouts/canvas.h"

namespace slt {
  namespace gui {

    void CanvasLayout::add(std::unique_ptr<Widget> w, Vec2 pos, Vec2 size) {
      widgets_.emplace_back( placed_widget{std::move(w), pos, size});
    }

    void CanvasLayout::render(WidgetRenderer& r, WidgetRenderRequest const& rr) {
      for(auto const & elem : widgets_) {
        elem.widget->render(r, WidgetRenderRequest{ elem.position, elem.size });
      }
    }
  }
  }
