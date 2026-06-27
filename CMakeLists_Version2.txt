cmake_minimum_required(VERSION 3.18.1)
project("gpuhook")

include(FetchContent)
FetchContent_Declare(
  dobby
  GIT_REPOSITORY https://github.com/jmpews/Dobby.git
  GIT_TAG master
)
FetchContent_MakeAvailable(dobby)

add_library(
        gpuhook
        SHARED
        hook.cpp
)

find_library(log-lib log)
target_link_libraries(gpuhook dobby ${log-lib} GLESv2)
