cmake_minimum_required(VERSION 3.0.0)
project(scanner VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

SET(CPP_STANDARD 20)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(scanner ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_library(example_google_tests ${SOURCES})
enable_testing()
add_subdirectory(tests)