add_executable(shader_compiler main.cpp)

set_target_properties(shader_compiler PROPERTIES FOLDER "tools")

target_link_libraries(shader_compiler
  slt_core
)

install(TARGETS shader_compiler DESTINATION bin)
