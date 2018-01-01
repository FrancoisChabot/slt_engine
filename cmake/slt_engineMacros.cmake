function(SLT_BUILD_ASSETS TARGET REG_FILE REG_NAME)
  add_custom_target(${TARGET})
  add_custom_command(
    TARGET 
      ${TARGET} 
    COMMAND 
      python3 ${SLT_ENGINE_INSTALL_DIR}/bin/build/slt_build.py --src ${CMAKE_CURRENT_SOURCE_DIR}/${REG_FILE} --dst ${REG_NAME})
endfunction()