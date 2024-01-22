cmake_minimum_required(VERSION 3.22)
project(conroutine)

add_executable(${PROJECT_NAME} 
    ${PROJECT_SOURCE_DIR}/src/Fiber.cc
    ${PROJECT_SOURCE_DIR}/test.cc
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
