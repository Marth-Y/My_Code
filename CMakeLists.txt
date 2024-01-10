cmake_minimum_required(VERSION 3.5)
project(ls)

set(SOURCES 
    ./system/src/ls.cc
    ./test.cc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/system/include
)