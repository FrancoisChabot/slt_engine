#ifndef SLT_RUNTIME_AUDIO_EMMITER_H
#define SLT_RUNTIME_AUDIO_EMMITER_H

#include "slt_runtime/audio/sound.h"

namespace slt {
  namespace audio {
    class Emmiter {
    public:
      Emmiter();
      ~Emmiter();

      void play(SoundRef sound);

    private:
      ALuint source_;

      SoundRef current_sound_;
    };
  }
}

#endif
