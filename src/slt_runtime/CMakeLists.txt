include_directories(
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

IF(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17")
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} --std=c++17")
ENDIF(MSVC)

add_library(slt_runtime OBJECT
  resources.cpp
  runtime.cpp
  drawing/model.cpp
)
