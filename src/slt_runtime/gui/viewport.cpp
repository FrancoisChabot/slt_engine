#include "slt_runtime/gui/viewport.h"
#include "slt_runtime/runtime.h"
#include "slt_runtime/drawing/blending.h"

namespace {

  slt::render::UniformBlockRef gui_rendering_ubo_block;
  struct GuiRenderingUbo {
    slt::Vec2 viewport_size;
  };

  //urg... please replace this with a proper gui "system"
  void guiViewportGlobalInit() {
    static bool done = false;
    if (!done) {
      done = true;
    }

    gui_rendering_ubo_block = slt::render::registerUniformBlock<GuiRenderingUbo>("GuiRendering");
  }
}
namespace slt {
    namespace gui {
  Viewport::Viewport(Vec2 size)
      : size_(size) {
    guiViewportGlobalInit();

    ubo_ = slt::render::getUbo(gui_rendering_ubo_block);

    updateUboData(ubo_, DataView((char const*)&size_, sizeof(size_)));
  }

  void Viewport::render(WidgetRenderer& r) {
    if(root_) {
      render::BlendingUsage use_blending;
      render::UniformBufferUsage use_ubo(ubo_);

      WidgetRenderRequest req;
      req.size = size_;

      root_->render(r, req);
    }
  }
}
}
