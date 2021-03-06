#include "slt_runtime/runtime.h"
#include "slt_runtime/drawing/attribute.h"
#include "slt_runtime/audio/system.h"
#include "slt_runtime/gui/system.h"

namespace slt {
namespace settings {
Setting<std::string> window_title("Slt Engine", "window_title",
                                  "The title of the os window (if applicable)");
Setting<iVec2> window_size({1280, 720}, "window_size",
                           "The resolution of the runtime window");
}

RuntimeInitError::RuntimeInitError(std::string error)
    : std::runtime_error(std::move(error)) {}

Runtime* Runtime::instance = nullptr;

void Runtime::sync() {
  main_queue.executeAll();

  platformSync();
}

void Runtime::commonInit() {
  render::registerAttribute("slt_vertex", 0);
  render::registerAttribute("slt_uv", 1);
  render::registerAttribute("slt_normal", 2);
  render::registerAttribute("slt_color", 3);

  audio::System::instance = std::make_unique<audio::System>();
  gui::System::instance = std::make_unique<gui::System>();
}

void Runtime::commonShutdown() {
  gui::System::instance.reset();
  audio::System::instance.reset();
}
}
