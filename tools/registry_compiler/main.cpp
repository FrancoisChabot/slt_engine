#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <fstream>
#include <iostream>

#include "json.hpp"

#include "slt/concur/event_queue.h"
#include "slt/core.h"
#include "slt/file/read.h"
#include "slt/file/write.h"
#include "slt/settings.h"
#include "slt_capnp/resources.capnp.h"

using json = nlohmann::json;


slt::Setting<std::string> input_file("", "input",
                                     "the file containing the registry");
slt::Setting<std::string> output_file(
    "", "output", "Where to store the resulting compiled registry");

int main(int argc, const char* argv[]) {
  slt::Core slt_core(argc, argv);

  slt::concur::EventQueue evt_queue;

  std::string input_path{input_file.get()};

  auto ref_path = input_path.substr(0, input_path.find_last_of('/')+1);

  int pending_op = 1;
  json cfg;
  slt::file::asyncRead(input_path, evt_queue,
                       [&](slt::DataBlock data) {
                         auto stream_buf = data.getStreamBuf();
                         std::istream stream(&stream_buf);
                         stream >> cfg;
                         --pending_op;
                       },
                       [&](slt::file::ReadError const& e) {
                         --pending_op;
                         throw e;
                       });

  evt_queue.executeUntil([&] { return pending_op <= 0; });

  ::capnp::MallocMessageBuilder message;
  slt::data::ResourceRegistry:: Builder reg =
      message.initRoot<slt::data::ResourceRegistry>();

  auto img_dst = reg.initImages((unsigned int)cfg["images"].size());
  int i = 0;
  for (json::iterator img = cfg["images"].begin(); img != cfg["images"].end(); ++img) {
    img_dst[i].setName(img.key());
    ++i;
  }

  auto shdr_dst = reg.initShaders((unsigned int)cfg["shaders"].size());

  i = 0;
  for (json::iterator shdr = cfg["shaders"].begin(); shdr != cfg["shaders"].end(); ++shdr) {
    shdr_dst[i].setName(shdr.key());
    ++i;
  }

  auto fnt_dst = reg.initFonts((unsigned int)cfg["fonts"].size());
  i = 0;
  for (json::iterator fnt = cfg["fonts"].begin(); fnt != cfg["fonts"].end(); ++fnt) {
    fnt_dst[i].setName(fnt.key());
    ++i;
  }

  auto snd_dst = reg.initSounds((unsigned int)cfg["sounds"].size());
  i = 0;
  for (json::iterator snd = cfg["sounds"].begin(); snd != cfg["sounds"].end(); ++snd) {
    snd_dst[i].setName(snd.key());
    ++i;
  }

  slt::file::write(message, output_file.get());
  return 0;
}
