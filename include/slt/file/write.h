#ifndef SLT_FILE_WRITE_H
#define SLT_FILE_WRITE_H

#include "slt/mem/data_view.h"

#include <capnp/message.h>
#include <string_view>

namespace slt {
namespace file {
struct WriteError : public std::runtime_error {
  WriteError(std::string);
};

// Synchronously writes a chunk of data to a file.
void write(DataView data, std::string_view file);

// Special overload for writing capnp messages, as the library
// really prefers to interact directly with the OS.
void write(capnp::MessageBuilder& data, std::string const& file);

}  // namespace file
}  // namespace slt

#endif
