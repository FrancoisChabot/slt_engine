#include "slt_runtime/audio/emmiter.h"
#include "slt/file/read.h"
#include "slt_runtime/runtime.h"


namespace slt {
  namespace audio {
      Emmiter::Emmiter() {
        alGenSources(1, &source_);

          // Set the source and listener to the same location
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
      }

      Emmiter::~Emmiter() {
        alDeleteSources(1, &source_);
      }

      void Emmiter::play(SoundRef sound) {
        
        current_sound_ = std::move(sound);
        alSourcei(source_, AL_BUFFER, current_sound_->getNativeBuffer());
        alSourcePlay(source_);
      }
  }
}
