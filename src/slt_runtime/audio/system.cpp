#include "slt_runtime/audio/system.h"
#include "slt/log.h"

namespace slt {
  namespace audio {
    std::unique_ptr<System> System::instance;

    System::System() {
      log->info("starting audio system");

      device_ = alcOpenDevice(NULL);
      if(!device_) {
        throw std::runtime_error("failed to aquire audio device");
      }

      ctx_ = alcCreateContext(device_, NULL);
      if(!ctx_ || alcMakeContextCurrent(ctx_) == ALC_FALSE) {
        if(ctx_) {
          alcDestroyContext(ctx_);
        }
        alcCloseDevice(device_);
        throw std::runtime_error("failed to initialize audio device");
      }
    }

    System::~System() {
      alcMakeContextCurrent(NULL);
      alcDestroyContext(ctx_);
      alcCloseDevice(device_);

      log->info("shuting down audio system");
    }
  }
}
