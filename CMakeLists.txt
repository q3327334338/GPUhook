cmake_minimum_required(VERSION 3.18.1)
project("gpuhook")

# 优先使用仓库内预编译的 Dobby 静态库（如果存在）
set(DOBBY_STATIC_PATH "${CMAKE_SOURCE_DIR}/dobby/arm64-v8a/libdobby.a")
if(EXISTS ${DOBBY_STATIC_PATH})
  add_library(dobby STATIC IMPORTED)
  set_target_properties(dobby PROPERTIES
      IMPORTED_LOCATION ${DOBBY_STATIC_PATH}
  )
  target_include_directories(dobby INTERFACE ${CMAKE_SOURCE_DIR}/dobby/include)
else()
  # 如果仓库里没有预编译库，则使用 FetchContent 拉取并构建 Dobby
  include(FetchContent)
  FetchContent_Declare(
    dobby
    GIT_REPOSITORY https://github.com/jmpews/Dobby.git
    GIT_TAG v2.0.0
  )
  FetchContent_MakeAvailable(dobby)

  # 兼容性处理：Dobby 在不同版本可能导出 dobby 或 dobby_static 两个 target
  # 我们根据存在的 target 来设置 include 路径和编译宏，避免引用不存在的路径导致 CMake 生成失败
  if(TARGET dobby_static)
    # 补全Dobby头文件包含路径
    target_include_directories(dobby_static PUBLIC "${dobby_SOURCE_DIR}/source")
    # 强制传递Android平台宏，解决OSMemory未声明问题
    target_compile_definitions(dobby_static PRIVATE __ANDROID__=1)
  elseif(TARGET dobby)
    target_include_directories(dobby PUBLIC "${dobby_SOURCE_DIR}/source")
    target_compile_definitions(dobby PRIVATE __ANDROID__=1)
  else()
    message(WARNING "Dobby target not found as dobby_static or dobby; attempting to continue but configuration may fail later.")
  endif()
endif()

# 可选的调试输出：打印导出的 include 路径，帮助排查不存在路径的问题
if(TARGET dobby_static)
  get_target_property(_inc dobby_static INTERFACE_INCLUDE_DIRECTORIES)
  message(STATUS "dobby_static INTERFACE_INCLUDE_DIRECTORIES=${_inc}")
endif()
if(TARGET dobby)
  get_target_property(_inc2 dobby INTERFACE_INCLUDE_DIRECTORIES)
  message(STATUS "dobby INTERFACE_INCLUDE_DIRECTORIES=${_inc2}")
endif()

add_library(
        gpuhook
        SHARED
        hook.cpp
)

find_library(log-lib log)

# 根据实际存在的 Dobby target 来链接
if(TARGET dobby_static)
  target_link_libraries(gpuhook dobby_static ${log-lib} GLESv2)
elseif(TARGET dobby)
  target_link_libraries(gpuhook dobby ${log-lib} GLESv2)
elseif(TARGET dobby)
  # 备用：导入的预编译库名为 dobby（如果上面逻辑进入预编译分支会创建此 target）
  target_link_libraries(gpuhook dobby ${log-lib} GLESv2)
else()
  message(FATAL_ERROR "Could not find a Dobby target to link against (expected dobby_static or dobby). Ensure FetchContent fetched Dobby correctly or prebuilt library is present at ${DOBBY_STATIC_PATH}.")
endif()
