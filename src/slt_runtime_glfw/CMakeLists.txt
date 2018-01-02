find_package(glfw3 REQUIRED)

add_library(slt_runtime_glfw
  render/attribute.cpp
  render/blending.cpp
  render/model.cpp
  render/program.cpp
  render/render.cpp
  render/texture.cpp
  render/uniform.cpp
  runtime_impl.cpp
  ../../third_party/src/glad.c
  ../../third_party/src/lodepng.cpp
  $<TARGET_OBJECTS:slt_runtime>
)

target_include_directories(slt_runtime_glfw PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

target_link_libraries(slt_runtime_glfw PUBLIC slt_core glfw)
target_compile_options(slt_runtime_glfw PUBLIC
                       "$<$<C_COMPILER_ID:Clang>:--std=c++17>"
                       "$<$<C_COMPILER_ID:GNU>:--std=c++17>"
                       "$<$<C_COMPILER_ID:MSVC>:/std:c++17>")

install(TARGETS slt_runtime_glfw EXPORT slt_engineTargets
    ARCHIVE  DESTINATION lib
    LIBRARY  DESTINATION lib
    RUNTIME  DESTINATION bin
)

IF(MSVC)
  INSTALL ( FILES ${PROJECT_BINARY_DIR}/src/slt_runtime_glfw/Debug/slt_runtime_glfw.pdb
    DESTINATION lib
    CONFIGURATIONS Debug
  )
ENDIF(MSVC)
