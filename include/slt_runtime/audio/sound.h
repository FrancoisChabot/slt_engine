#ifndef SLT_RUNTIME_AUDIO_SOUND_H
#define SLT_RUNTIME_AUDIO_SOUND_H

#include <memory>
#include <string>
#include <functional>


#include "AL/al.h"
#include "slt/mem/data_block.h"

#include <vorbis/vorbisfile.h>

namespace slt {
  namespace audio {
    class Sound {
    public:
      Sound(slt::DataBlock d);
      ~Sound();

      ALuint getNativeBuffer() const {
        return buffer_;
      }

    private:
      ALuint buffer_;

      slt::DataBlock raw_data_;
      long cursor_ = 0;
      OggVorbis_File decoder_;

      static size_t decode_read(void *ptr, size_t size, size_t nmemb, void *datasource);
      static int decode_seek(void *datasource, ogg_int64_t offset, int whence);
      static int decode_close(void *datasource);
      static long decode_tell(void *datasource);

      static ov_callbacks callbacks_;
    };

    using SoundRef = std::shared_ptr<Sound>;
  }

  void loadSound(std::string const&, std::string const&,
                 std::function<void(audio::SoundRef)>);
}

#endif
