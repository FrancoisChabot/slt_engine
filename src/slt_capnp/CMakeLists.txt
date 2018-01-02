capnp_generate_cpp(RenderSrc RenderHdr render.capnp)
capnp_generate_cpp(ResourceSrc ResHdr resources.capnp)

add_library(slt_capnp
  ${RenderSrc}
  ${ResourceSrc}
)

SET(SLT_CAPNP_INTERNAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/.. PARENT_SCOPE)

target_link_libraries(slt_capnp CapnProto::capnp)



install(TARGETS slt_capnp EXPORT slt_engineTargets
    ARCHIVE  DESTINATION lib
    LIBRARY  DESTINATION lib
    RUNTIME  DESTINATION bin
)
