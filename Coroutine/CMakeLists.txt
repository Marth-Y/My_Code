cmake_minimum_required(VERSION 3.22)
project(conroutine)

include_directories("./include")

SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} 
    ${PROJECT_SOURCE_DIR}/src/Fiber.cc
    ${PROJECT_SOURCE_DIR}/src/Simple_Fiber_Scheduler.cc
    ${PROJECT_SOURCE_DIR}/test.cc
)

# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${PROJECT_SOURCE_DIR}/include
# )
