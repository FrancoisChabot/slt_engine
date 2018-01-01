#ifndef SLT_MEMORY_READ_BUF_H
#define SLT_MEMORY_READ_BUF_H

#include <streambuf>

namespace slt {

struct ReadBuf : public std::streambuf {
  ReadBuf(char const* start, std::size_t size) {
    char* p = const_cast<char*>(start);
    this->setg(p, p, p + size);
  }
};
}
#endif