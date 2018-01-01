add_library(slt_core
  concur/event_queue.cpp
  core.cpp
  debug/assert.cpp
  file/file.cpp
  log/log.cpp
  settings/settings.cpp
)

target_include_directories(slt_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

target_link_libraries(slt_core ${CMAKE_THREAD_LIBS_INIT} slt_capnp)

target_compile_options(slt_core PUBLIC
                       "$<$<C_COMPILER_ID:Clang>:-std=c++17>"
                       "$<$<C_COMPILER_ID:GNU>:-std=c++17>"
                       "$<$<C_COMPILER_ID:MSVC>:/std:c++17>")

install(TARGETS slt_core EXPORT slt_engineTargets
    ARCHIVE  DESTINATION lib
    LIBRARY  DESTINATION lib
    RUNTIME  DESTINATION bin
)

IF(MSVC)
  INSTALL ( FILES ${PROJECT_BINARY_DIR}/src/slt/Debug/slt_core.pdb
    DESTINATION lib
    CONFIGURATIONS Debug
  )
ENDIF(MSVC)
