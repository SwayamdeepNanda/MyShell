cmake_minimum_required(VERSION 3.16)
project(MyShell C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fstack-protector-strong -D_GNU_SOURCE")

include_directories(include)
file(GLOB SOURCES "src/*.c")

add_executable(myshell ${SOURCES})
target_link_libraries(myshell PRIVATE dl)

# Optional build types
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)

install(TARGETS myshell DESTINATION bin)
