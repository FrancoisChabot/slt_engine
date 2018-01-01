#ifndef SLT_RUNTIME_RUNTIME_H
#define SLT_RUNTIME_RUNTIME_H

#include "slt/concur/event_queue.h"
#include "slt/math/vec.h"
#include "slt/settings.h"
#include "slt_runtime/render.h"

#include <memory>
#include <stdexcept>

namespace slt {

namespace settings {
extern Setting<std::string> window_title;
extern Setting<iVec2> window_size;
}

struct RuntimeInitError : public std::runtime_error {
  RuntimeInitError(std::string error);
};

class RuntimeSystemInterface {
 public:
  virtual ~RuntimeSystemInterface() {}
};

struct RuntimeBackendOpaqueMembers;
class Runtime {
 public:
  // singleton access
  static Runtime* instance;

  Runtime();
  ~Runtime();

  bool keepRunning();
  void sync();

  concur::EventQueue& getMainQueue() { return main_queue; }

  //  void finishAllPendingLoads();
  // These functions are actually implemented in the backend classes
 private:
  void commonInit();
  void platformSync();

  concur::EventQueue main_queue;
  std::unique_ptr<RuntimeBackendOpaqueMembers> members_;

  friend struct render::FrameScope;
};
}  // namespace slt

#endif
