#ifndef MYGUI_VIEWPORT_H
#define MYGUI_VIEWPORT_H

#include "slt_runtime/gui/widget.h"
#include "slt/math/vec.h"
#include "slt_runtime/drawing/uniform.h"

#include <vector>
#include <memory>

namespace slt {
  namespace gui {
  class Viewport {
  public:
    Viewport(Vec2 size);

    void update(float dt) {
      root_->update(dt);
    }

    void render(WidgetRenderer& r);

    void setRoot(std::unique_ptr<Widget> r) {
      root_ = std::move(r);
    }

  private:
    Vec2 size_;
    render::UniformBufferRef ubo_;
    std::unique_ptr<Widget> root_;
  };
}
}

#endif
