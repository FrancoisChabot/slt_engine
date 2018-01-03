#ifndef SLT_FILE_READ_H
#define SLT_FILE_READ_H

#include <functional>
#include <stdexcept>
#include "slt/concur/event_queue.h"
#include "slt/mem/data_block.h"

namespace slt {

namespace file {

// Thrown/returned when something goes wrong when attempting to read a file.
struct ReadError : public std::runtime_error {
  ReadError(std::string);
};

//
using ReadCallback = std::function<void(DataBlock)>;
using StreamReadFinishedCallback = std::function<void(void)>;
using ReadFailedCallback = std::function<void(ReadError)>;

// Asynchronously loads file from the native filesystem.
// cb or err_cb will be put into queue upon completion.
// Args:
//   file: The path of the file to load
//   queue: the event queue where completion should be posted
//   cb: Handler completion upon successful fle load
//   err_cb: Handler to invoke if the read operation fails.
void asyncRead(std::string file, concur::EventQueue &queue, ReadCallback cb,
               ReadFailedCallback err_cb = nullptr);

// Asynchronously loads file and directly emits the ReadCallback as a general task.
// TODO: What do we do in case of failure here?
void asyncReadtoWorker(std::string file, ReadCallback cb);

// Asynchronously loads file from the native filesystem in packets.
// This can be usefull when loading very large files to avoid having
// to make very large allocations.
// Args:
//   file: The path of the file to load
//   queue: the event queue where completion should be posted
//   cb: Handler completion upon successful fle load
//   err_cb: Handler to invoke if the read operation fails.
void asyncStreamRead(std::string file, concur::EventQueue &queue,
                     std::size_t packet_size, ReadCallback cb,
                     StreamReadFinishedCallback fin_cb,
                     ReadFailedCallback err_cb = nullptr);

}  // namespace file
}  // namespace slt

#endif
