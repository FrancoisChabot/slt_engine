#include <fstream>
#include "capnp/serialize.h"
#include "slt/concur/blocking_queue.h"
#include "slt/file/file_internal.h"
#include "slt/file/read.h"
#include "slt/file/write.h"
#include "slt/log.h"

namespace {

std::thread filesystem_thread;
slt::concur::BlockingQueue<std::function<void(void)>> pending_load_operations;

void filesystemThreadMain() {
  slt::log->info("loading thread started");
  while (1) {
    auto op = pending_load_operations.pop();
    if (!op) {
      break;
    }
    op();
  }
  slt::log->info("loading thread finished");
}
}  // namespace

namespace slt {
namespace file {

ReadError::ReadError(std::string reason) : std::runtime_error(reason) {}
WriteError::WriteError(std::string reason) : std::runtime_error(reason) {}

void startFilesystemThread() {
  filesystem_thread = std::thread(filesystemThreadMain);
}

void stopFilesystemThread() {
  pending_load_operations.push(nullptr);
  filesystem_thread.join();
  pending_load_operations.clear();
}

DataBlock read(std::string const &file) {
  slt::log->info("Attempting to load file: {}", file);
  std::ifstream in_f(file.c_str(), std::ios::binary);
  if (!in_f.good()) {
    throw ReadError(fmt::format("Failed to load file: {}", file));
  }

  // Do not mess with this string, it's used in log parsing.
  slt::log->info("Reading from file: {}", file);
  in_f.seekg(0, in_f.end);
  auto length = in_f.tellg();
  in_f.seekg(0, in_f.beg);
  DataBlock file_contents(length);
  in_f.read(file_contents.data(), length);
  return file_contents;
}

void asyncRead(std::string file, concur::EventQueue &queue, ReadCallback cb,
               ReadFailedCallback err_cb) {
  pending_load_operations.push(
      [ file, cb{move(cb)}, err_cb{move(err_cb)}, queue{&queue} ]() {
        try {
          DataBlock data = slt::file::read(file);

          struct intermediate {
            ReadCallback cb_;
            DataBlock data_;

            void operator()() { cb_(std::move(data_)); }
          };
          queue->queueEvent(intermediate{move(cb), std::move(data)});
        } catch (ReadError &err) {
          struct intermediate {
            ReadFailedCallback cb_;
            ReadError err_;

            void operator()() {
              if (cb_) {
                cb_(std::move(err_));
              } else {
                log->error("failed to read file {}", err_.what());
              }
            }
          };
          queue->queueEvent(intermediate{move(err_cb), std::move(err)});
        }
      });
}

void write(DataView data, std::string_view file) {
  // Do not mess with this string, it's used in log parsing.
  slt::log->info("Writing to file: {}", file);

  std::ofstream out_file(std::string(file), std::ios::binary);

  if (!out_file.good()) {
    throw WriteError(fmt::format("Failed to write file: {}", file));
  }

  slt::log->info("Wrote {} bytes", data.size());

  out_file.write(data.data(), data.size());
}

void write(capnp::MessageBuilder &message, std::string const &fname) {
  auto as_array = messageToFlatArray(message);
  auto as_bytes = as_array.asBytes();
  write(DataView((char *)as_bytes.begin(), as_bytes.size()), fname);
}
}  // namespace file
}  // namespace slt