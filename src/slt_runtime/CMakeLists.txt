include_directories(
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

include_directories(${CAPNP_INCLUDE_DIRECTORY})

IF(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17")
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} --std=c++17")
ENDIF(MSVC)

add_library(slt_runtime OBJECT
  resources.cpp
  runtime.cpp
  audio/emmiter.cpp
  audio/sound.cpp
  audio/system.cpp
  drawing/model.cpp
  gui/font.cpp
  gui/image.cpp
  gui/system.cpp
  gui/text.cpp
  gui/viewport.cpp
  gui/renderer/raw_renderer.cpp
)
