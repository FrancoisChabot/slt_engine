#include "slt_runtime/audio/sound.h"
#include "slt/file/read.h"
#include "slt_runtime/runtime.h"

namespace {
  std::string getFileForSound(std::string const& package,
    std::string const& name) {
    return package + "/sounds/" + name + ".ogg";
  }
}
 

namespace slt {
  namespace audio {
    ov_callbacks Sound::callbacks_ = {
      decode_read,
      decode_seek,
      decode_close,
      decode_tell,
    };

    Sound::Sound(slt::DataBlock d) 
    : raw_data_(std::move(d)) {
      auto err = alGetError();
      alGenBuffers((ALuint)1, &buffer_);
      err = alGetError();
      ov_open_callbacks(this, &decoder_, nullptr, 0, callbacks_);

      auto info = ov_info(&decoder_, -1);
      auto length = ov_pcm_total(&decoder_, -1);

      ALenum format = AL_FORMAT_STEREO16;
      if (info->channels == 1) {
        format = AL_FORMAT_MONO16;
      }
      ALsizei freq = info->rate;

      std::vector<char> samples(length*2);

      int bitstream;
      std::size_t bytes_read = 0;
      while (bytes_read < samples.size()) {
        bytes_read += ov_read(&decoder_, (char*)samples.data() + bytes_read, length * 2 - bytes_read, 0, 2, 1, &bitstream);
      }
      alBufferData(buffer_, format, samples.data(), bytes_read, freq);
      err = alGetError();
    }

    Sound::~Sound() {
      ov_clear(&decoder_);
      alDeleteBuffers(1, &buffer_);
    }

    size_t Sound::decode_read(void *ptr, size_t size, size_t nmemb, void *datasource) {
      slt::audio::Sound* tgt = reinterpret_cast<slt::audio::Sound*>(datasource);
      char* src = tgt->raw_data_.data() + tgt->cursor_;
      long read_size = std::min(size * nmemb, tgt->raw_data_.size() - tgt->cursor_);

      std::memcpy(ptr, src, read_size);
      tgt->cursor_ += read_size;

      return read_size;
    }

    int Sound::decode_seek(void *datasource, ogg_int64_t offset, int whence) {
      slt::audio::Sound* tgt = reinterpret_cast<slt::audio::Sound*>(datasource);

      switch (whence) {
      case SEEK_SET:
        tgt->cursor_ = offset;
        break;
      case SEEK_CUR:
        tgt->cursor_ += offset;
        break;
      case SEEK_END:
        tgt->cursor_ = tgt->raw_data_.size() + offset;
        break;
      }
      return 0;
    }

    int Sound::decode_close(void *datasource) {
      slt::audio::Sound* tgt = reinterpret_cast<slt::audio::Sound*>(datasource);
      tgt->cursor_ = 0;
      return 0;
    }

    long Sound::decode_tell(void *datasource) {
      slt::audio::Sound* tgt = reinterpret_cast<slt::audio::Sound*>(datasource);
      return tgt->cursor_;
    }
  }

  void loadSound(std::string const& package, std::string const& name,
                 std::function<void(audio::SoundRef)> cb) {

    // TODO: Decoding should be delegated to a worker thread
    slt::file::asyncReadtoWorker(getFileForSound(package, name), [cb](DataBlock d) {
      auto new_sound = std::make_shared<audio::Sound>(std::move(d));
      slt::Runtime::instance->getMainQueue().queueEvent([cb, new_sound]() {
        cb(new_sound);
      });
    });
  }


}
