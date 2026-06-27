cmake_minimum_required(VERSION 3.18.1)
project("gpuhook")

add_library(
        gpuhook
        SHARED
        hook.cpp
)

find_library(log-lib log)
target_link_libraries(gpuhook ${log-lib} GLESv2)
