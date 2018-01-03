#include "slt_runtime/gui/system.h"
#include "slt_runtime/drawing/uniform.h"
#include "slt/log.h"

namespace slt {
  namespace gui {
    std::unique_ptr<System> System::instance;

    System::System()
      : viewport_ubo_(slt::render::registerUniformBlock<ViewportUBO>("ViewportUBO")) {
      log->info("starting gui system");
    }

    System::~System() {
      log->info("shuting down gui system");
    }
  }
}
