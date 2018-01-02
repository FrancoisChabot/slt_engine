#ifndef SLT_RUNTIME_GUI_SYSTEM_H
#define SLT_RUNTIME_GUI_SYSTEM_H

#include "slt/math/vec.h"
#include "slt_runtime/drawing/uniform.h"

namespace slt {
  namespace gui {

    struct ViewportUBO {
      slt::Vec2 viewport_size;
    };

    class System {
    public:
      System();
      ~System();
      static std::unique_ptr<System> instance;

      slt::render::UniformBlockRef const& getViewportUBO() {
        return viewport_ubo_;
      }
    private:
      slt::render::UniformBlockRef viewport_ubo_;
    };


  }
}
#endif
