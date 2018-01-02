@0xf65c6685f0250eaa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("slt::data");

struct ResourceRegistry {
  images @0 :List(Resource);
  shaders @1 :List(Resource);
  fonts @2 :List(Resource);
}

struct Resource {
  name @0 :Text;
}
