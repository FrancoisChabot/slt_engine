find_package( PythonInterp 3.4 REQUIRED )

function(SLT_BUILD_ASSETS TARGET REG_FILE REG_NAME)
  add_custom_target(${TARGET}
    COMMAND
      ${PYTHON_EXECUTABLE} ${SLT_ENGINE_INSTALL_DIR}/bin/build/slt_build.py --src ${CMAKE_CURRENT_SOURCE_DIR}/${REG_FILE} --dst ${REG_NAME}
    )
endfunction()
