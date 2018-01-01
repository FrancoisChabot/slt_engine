@0xf65c6685f0250eaa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("slt::data");

struct ResourceRegistry {
  images @0 :List(Resource);
}

struct Resource {
  name @0 :Text;
  file @1 :Text;
}