#include "slt_runtime/gui/viewport.h"
#include "slt_runtime/runtime.h"
#include "slt_runtime/drawing/blending.h"
#include "slt_runtime/gui/system.h"


namespace slt {
    namespace gui {

  Viewport::Viewport(Vec2 size)
      : size_(size) {

    ubo_ = slt::render::getUbo(System::instance->getViewportUBO());

    ViewportUBO ubo_data;
    ubo_data.viewport_size = size;

    updateUboData(ubo_, DataView(&ubo_data));
  }

  void Viewport::render(WidgetRenderer& r) {
    if(root_) {
      render::BlendingUsage use_blending;
      render::UniformBufferUsage use_ubo(ubo_);

      WidgetRenderRequest req;
      req.pos = { 0,0 };
      req.size = size_;

      root_->render(r, req);
    }
  }
}
}
