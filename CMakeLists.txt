cmake_minimum_required(VERSION 3.18.1)
project("gpuhook")

# 导入Dobby预编译静态库
add_library(dobby STATIC IMPORTED)
set_target_properties(dobby PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/dobby/arm64-v8a/libdobby.a
)
target_include_directories(dobby INTERFACE ${CMAKE_SOURCE_DIR}/dobby/include)

add_library(
        gpuhook
        SHARED
        hook.cpp
)

find_library(log-lib log)
target_link_libraries(gpuhook dobby ${log-lib} GLESv2)
