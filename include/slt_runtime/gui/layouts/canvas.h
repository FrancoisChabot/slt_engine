#ifndef MYGUI_LAYOUTS_CANVAS_H
#define MYGUI_LAYOUTS_CANVAS_H

#include "slt_runtime/gui/widget.h"

#include <string>

namespace slt {
  namespace gui {

    class CanvasLayout : public Widget {
    public:
      CanvasLayout() {}

      void add(std::unique_ptr<Widget> w, Vec2 pos, Vec2 size);

      void render(WidgetRenderer&, WidgetRenderRequest const& r) override;

    private:
      struct placed_widget {
        std::unique_ptr<Widget> widget;
        Vec2 position;
        Vec2 size;
      };

      std::vector<placed_widget> widgets_;
    };
  }
}

#endif
