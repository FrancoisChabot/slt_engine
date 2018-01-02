cmake_minimum_required(VERSION 3.1)

project(slt_engine_core)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Threads REQUIRED)
find_package(CapnProto CONFIG REQUIRED)

SET(SLT_BUILD_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/public/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/private/include
)

IF(MSVC)
  add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
ENDIF(MSVC)


include_directories(
  ${SLT_BUILD_INCLUDE_DIRS}
)

add_library(lodepng OBJECT
  third_party/src/lodepng.cpp
)

set_target_properties(lodepng PROPERTIES FOLDER "third_party")

add_subdirectory(src/slt_capnp)

include_directories(${SLT_CAPNP_INTERNAL_INCLUDE_DIR})

add_subdirectory(src/slt)
add_subdirectory(src/slt_runtime)
add_subdirectory(src/slt_runtime_glfw)

#install headers
install(DIRECTORY include/slt DESTINATION include)
install(DIRECTORY include/slt_runtime DESTINATION include)
install(DIRECTORY third_party/public/include/ DESTINATION include)

#install build tools
install(DIRECTORY tools/build DESTINATION bin)

#install cmake export
install(EXPORT slt_engineTargets DESTINATION share/slt_engine/cmake)

configure_file(cmake/slt_engineConfig.cmake.in
  "${PROJECT_BINARY_DIR}/slt_engineConfig.cmake" @ONLY)

install(FILES
    cmake/slt_engineMacros.cmake
    "${PROJECT_BINARY_DIR}/slt_engineConfig.cmake"
    DESTINATION share/slt_engine/cmake)

file(
  GLOB_RECURSE CORE_HEADERS "include/slt/*"
)

add_subdirectory(tools)
