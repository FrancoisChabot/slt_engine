@0xf65c6685f0250eaa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("slt::data::render");

struct ProgramData {
  vertexShader @0 :Text;
  fragmentShader @1 :Text;
  geometryShader @2 :Text;
}


struct VertexDescriptor {
  channel @0 :Text;
  dimension @1 :Int8;
}

struct ModelData {
   descriptors @0 :VertexDescriptor;
   vertexData @1 :Data;
   indexCount @2 :Int32;
   indexData @3 :Data;
}