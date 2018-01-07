#ifndef MYGUI_WIDGET_H
#define MYGUI_WIDGET_H

#include "slt/math/vec.h"
#include "slt/color.h"
#include "slt/self_nulling_ptr.h"
#include "slt_runtime/drawing/texture.h"

#include <vector>

namespace slt {
  namespace gui {
    struct Vertex {
      Vec2 pos;
      Vec2 uv;
      Color color;
    };

    using Index = uint16_t;

    struct WidgetRenderRequest {
      Vec2 pos;
      Vec2 size;
    };

    class WidgetRenderer {
    public:
      virtual ~WidgetRenderer() {}

      virtual void draw(std::vector<Vertex>,
                        std::vector<Index>,
                        render::TextureRef color_texture,
                        render::TextureRef mask_texture) = 0;
    };

    class Widget : public enable_self_nulling_ptr<Widget> {
    public:
      Widget() {}
      virtual ~Widget() {}

      virtual void render(WidgetRenderer&, WidgetRenderRequest const& r) = 0;
      virtual void update(float dt) {}
      virtual Vec2 getMinSize() { return Vec2{ 0.0f, 0.0f }; }
    };
  }
}

#endif
