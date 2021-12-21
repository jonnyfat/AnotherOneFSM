cmake_minimum_required(VERSION 3.14.0)

project(AnotherOneFSM)

set(ANOTHERONEFSM_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options( -ggdb )

option(BUILD_TEST "Unit-Test for aofsm" ON)

# aofsm files mus be included with "aofsm/...."
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

if (BUILD_TEST)
    
    include_directories( ${gtest_SOURCE_DIR}/googlemock/include )
    include_directories( ${gtest_SOURCE_DIR}/googletest/include )
    
    add_subdirectory( aofsm)

endif()



