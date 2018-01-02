add_executable(registry_compiler main.cpp)

set_target_properties(registry_compiler PROPERTIES FOLDER "tools")

target_link_libraries(registry_compiler
  slt_core
)

install(TARGETS registry_compiler DESTINATION bin)
