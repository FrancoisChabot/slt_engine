#include <fstream>
#include <iostream>

#include "json.hpp"

#include "slt/concur/event_queue.h"
#include "slt/core.h"
#include "slt/file/read.h"
#include "slt/file/write.h"
#include "slt/settings.h"
#include "slt_capnp/render.capnp.h"

#include <boost/filesystem.hpp>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

using json = nlohmann::json;
namespace fs = boost::filesystem;

slt::Setting<std::string> input_file("", "input",
                                     "the file to load the shader config from");
slt::Setting<std::string> output_file(
    "", "output", "Where to store the resulting compiled shader");

int main(int argc, const char* argv[]) {
  slt::Core slt_core(argc, argv);

  slt::concur::EventQueue evt_queue;

  fs::path input_path{input_file.get()};
  input_path = absolute(canonical(input_path));
  auto ref_path = input_path.parent_path();

  json cfg;
  int pending_op = 1;
  slt::file::asyncRead(input_path.native(), evt_queue,
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
  slt::data::render::ProgramData::Builder prog =
      message.initRoot<slt::data::render::ProgramData>();

  fs::path v_shader_path = cfg.value("vertex", "");
  fs::path f_shader_path = cfg.value("fragment", "");
  fs::path g_shader_path = cfg.value("geometry", "");

  if (v_shader_path != "") {
    ++pending_op;
    v_shader_path = absolute(canonical(ref_path / v_shader_path));
    slt::file::asyncRead(
        v_shader_path.native(), evt_queue,
        [&](slt::DataBlock data) {
          prog.setVertexShader(std::string(data.data(), data.size()));
          --pending_op;
        },
        [&](slt::file::ReadError const& e) {
          --pending_op;
          throw e;
        });
  }

  if (f_shader_path != "") {
    f_shader_path = absolute(canonical(ref_path / f_shader_path));
    ++pending_op;
    slt::file::asyncRead(
        f_shader_path.native(), evt_queue,
        [&](slt::DataBlock data) {
          prog.setFragmentShader(std::string(data.data(), data.size()));
          --pending_op;
        },
        [&](slt::file::ReadError const& e) {
          --pending_op;
          throw e;
        });
  }

  if (g_shader_path != "") {
    g_shader_path = absolute(canonical(ref_path / g_shader_path));
    ++pending_op;
    slt::file::asyncRead(
        g_shader_path.native(), evt_queue,
        [&](slt::DataBlock data) {
          prog.setGeometryShader(std::string(data.data(), data.size()));
          --pending_op;
        },
        [&](slt::file::ReadError const& e) {
          --pending_op;
          throw e;
        });
  }

  evt_queue.executeUntil([&] { return pending_op <= 0; });

  slt::file::write(message,
                   absolute(fs::weakly_canonical(output_file.get())).native());

  return 0;
}