project(gnl)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})


FILE(GLOB_RECURSE HeaderFiles    "./gnl/*.h")
FILE(GLOB_RECURSE ExamplesFiles  "./examples/*.cpp")

add_custom_target(headers    SOURCES ${HeaderFiles})
add_custom_target(examples   SOURCES ${ExamplesFiles})

include_directories ("${PROJECT_SOURCE_DIR}")

ADD_DEFINITIONS(
    -std=c++11
)

add_executable(json   examples/json.cpp)
add_executable(path   examples/path.cpp)
