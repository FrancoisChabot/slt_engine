#ifndef SLT_RUNTIME_AUDIO_SYSTEM_H
#define SLT_RUNTIME_AUDIO_SYSTEM_H

#include <memory>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"

namespace slt {
  namespace audio {
    class System {
    public:
      System();
      ~System();
      static std::unique_ptr<System> instance;

    private:
      ALCdevice *device_;
      ALCcontext *ctx_;
    };
  }
}
#endif
