function(PublishRelease target logo_filepath)
  # 仅处理Release编译
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")

    # 定义输出路径
    set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
    set(OUTPUT_FILE_PATH ${INSTALL_DIR}/${target})
    message("build release output file path: ${OUTPUT_FILE_PATH}")

    # 判断不同的编译平台
    if (WIN32)
      # 获取windeployqt.exe的绝对路径
      set(QT5_WINDEPLOYQT_EXECUTABLE "${_qt5Core_install_prefix}/bin/windeployqt.exe")
      message("windeployqt executable path: ${QT5_WINDEPLOYQT_EXECUTABLE}")
      # 添加编译后导出命令
      add_custom_command(TARGET ${target} POST_BUILD
          # 创建目录
          COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALL_DIR}
          # 拷贝源文件
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target}> ${INSTALL_DIR}
          #执行windeployqt导出依赖库
          #COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --no-translations ${INSTALL_DIR}/$<TARGET_FILE_NAME:${target}>
          # 拷贝符号文件
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${target}> ${INSTALL_DIR}
          # 拷贝应用图标
          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${logo_filepath} ${INSTALL_DIR}
          #导出sym文件
          COMMAND ${Bugreport_DIR}/../../../analyze_tool/dump_syms.exe ${OUTPUT_FILE_PATH}.pdb > ${OUTPUT_FILE_PATH}.sym
          # 复制bugreport
          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Bugreport_DIR}/../../../bin/windows/bugreport.exe ${INSTALL_DIR}/bugreport.exe
          )
    else ()
      if (${CMAKE_C_COMPILER} MATCHES "arm-linux-gnueabihf-gcc")
        set(OBJ_BIN arm-linux-gnueabihf-objcopy)
      else ()
        set(OBJ_BIN aarch64-linux-gnu-objcopy)
      endif ()
      add_custom_command(TARGET ${target} POST_BUILD
          # 创建目录
          COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALL_DIR}
          # 拷贝源文件
          COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target}> ${OUTPUT_FILE_PATH}
          # 导出符号文件
          COMMAND /opt/breakpad_tools/dump_syms ${OUTPUT_FILE_PATH} > ${OUTPUT_FILE_PATH}.sym
          # 移除调试信息
          COMMAND ${OBJ_BIN} --strip-debug ${OUTPUT_FILE_PATH}
          # 复制bugreport
          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Bugreport_DIR}/../../../bin/armv7-8.3/bugreport ${INSTALL_DIR}/bugreport
          )
    endif ()
  endif ()
endfunction()