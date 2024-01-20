cmake_minimum_required(VERSION 3.5)
project(test)

set(SOURCES 
    ./system/src/ls.cc
    ./system/src/tree.cc
    ./system/src/chmod.cc
    ./test.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/system/include
)