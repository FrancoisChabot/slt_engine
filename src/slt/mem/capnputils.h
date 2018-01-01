#ifndef SLT_MEM_CAPNPUTILS_H
#define SLT_MEM_CAPNPUTILS_H

#include "capnp/serialize.h"

namespace slt {
inline auto getCapReader(DataView d) {
  SLT_ASSERT(d.size() % 8 == 0);
  kj::ArrayPtr<capnp::word const> arr(
      reinterpret_cast<capnp::word const*>(d.data()),
      d.size() / sizeof(capnp::word));
  return capnp::FlatArrayMessageReader(arr);
}
}

#endif