cmake_minimum_required(VERSION 3.18.1)
project("gpuhook")

include(FetchContent)
FetchContent_Declare(
  dobby
  GIT_REPOSITORY https://github.com/jmpews/Dobby.git
  GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(dobby)

# 修复1：补全Dobby头文件包含路径
target_include_directories(dobby_static PUBLIC "${dobby_SOURCE_DIR}/source")

# 修复2：强制传递Android平台宏，解决OSMemory未声明问题
target_compile_definitions(dobby_static PRIVATE __ANDROID__=1)

add_library(
        gpuhook
        SHARED
        hook.cpp
)

find_library(log-lib log)

# 修复3：修正链接目标名为 dobby_static
target_link_libraries(gpuhook dobby_static ${log-lib} GLESv2)
