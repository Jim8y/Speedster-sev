cmake_minimum_required(VERSION 3.10)
project(eevm)

set(CMAKE_CXX_STANDARD 17)

# set(Boost_ADDITIONAL_VERSIONS "1.67" "1.67.0")
# find_package(Boost 1.60.0 REQUIRED)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Set Linux compiler options
add_compile_options(-Wall)

# -Werror)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    separate_arguments(COVERAGE_FLAGS UNIX_COMMAND "--coverage -fprofile-arcs -ftest-coverage")
    set(PLATFORM_SPECIFIC_TEST_LIBS "gcov")
else ()
    separate_arguments(COVERAGE_FLAGS UNIX_COMMAND "-fprofile-instr-generate -fcoverage-mapping")
    set(PLATFORM_SPECIFIC_TEST_LIBS "-fprofile-instr-generate")
endif ()

file(GLOB KECCAK_SOURCES
        3rdparty/keccak/*.c
        )

add_subdirectory(3rdparty)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Options 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
option(RECORD_TRACE "Record a detailed trace of EVM execution during test runs" OFF)
if (RECORD_TRACE)
    add_definitions(-DRECORD_TRACE)
endif (RECORD_TRACE)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Common variables 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
set(EEVM_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )


set(EEVM_CORE_SRCS
        src/disassembler.cpp
        src/processor.cpp
        src/stack.cpp
        src/common.cpp
        src/encoding.cpp
        src/transaction.cpp
        src/util.cpp
        src/simple/simpleaccount.cpp
        src/simple/simpleglobalstate.cpp
        src/simple/simplestorage.cpp)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Libraries
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
add_library(eevm STATIC
        ${EEVM_CORE_SRCS}
        ${KECCAK_SOURCES}
        )

target_include_directories(eevm PRIVATE
        ${EEVM_INCLUDE_DIRS}
        )

target_link_libraries(eevm
        intx::intx
        )

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Executables
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
add_executable(eevm_tests
        tests/main.cpp
        #        tests/harness.cpp
        tests/rlp.cpp
        ${EEVM_SIMPLE_SRCS}
        )

target_include_directories(eevm_tests PRIVATE
        ${EEVM_INCLUDE_DIRS}
        )

target_compile_options(eevm_tests PRIVATE ${COVERAGE_FLAGS})
target_link_libraries(eevm_tests eevm ${PLATFORM_SPECIFIC_TEST_LIBS})


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Tests
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
enable_testing()
add_test(
        NAME eevm_tests
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_test_wrapper.sh eevm_tests
)

if (NOT ENV{TEST_DIR})
    set_tests_properties(eevm_tests
            PROPERTIES
            ENVIRONMENT TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/test_cases
            )
endif ()