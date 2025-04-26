cmake_minimum_required(VERSION 3.17)
project(CppExample1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
option(ENABLE_TEST "Enable tests" OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/sources/*.cpp)
add_executable(main ${SOURCES})

if(ENABLE_TEST)
    message(STATUS "Tests enabled")
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cpp)
    add_executable(test_analyzeType ${TEST_SOURCES})
    target_link_libraries(test_analyzeType PRIVATE gtest_main)
    add_test(NAME AnalyzeTypeTests COMMAND test_analyzeType)
else ()
    message(STATUS "Tests disabled, not building test_analyzeType")
    message(STATUS "To enable tests, run cmake with -DENABLE_TEST=ON")
endif()
