cmake_minimum_required(VERSION 3.15)
project(LNTEECMAKE)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set(Boost_ADDITIONAL_VERSIONS "1.67" "1.67.0")
# find_package(Boost 1.60.0 REQUIRED)

################ # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # #
# # # # # # #   Libs
# # # # # # #
################ # # # # # # # # # # # # # # # # # # # # # # # #
add_subdirectory(lib/eEVM/)
add_subdirectory(lib/libf2f)
add_subdirectory(lib/mbedtls)

set(EEVM_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/eEVM/3rdparty
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/eEVM/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libf2f/include
        )

file(GLOB LNTEE_COMMON_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/src/common/*.cpp
        )

file(GLOB LNTEE_HOST_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/src/contract/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/src/host/*.cpp
        )

file(GLOB LNTEE_ENCLAVE_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/src/enclave/*.cpp
        )

set(LNTEE_ENCLAVE_SRCS ${LNTEE_COMMON_SRCS} ${LNTEE_ENCLAVE_SRCS})

set(LNTEE_HOST_SRCS ${LNTEE_COMMON_SRCS} ${LNTEE_HOST_SRCS} lntee/src/host/Helper.cpp)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Program
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

add_executable(LNTEE_HOST
        ${LNTEE_HOST_SRCS}
        )

target_include_directories(LNTEE_HOST PRIVATE
        ${EEVM_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/include/
        )

target_link_libraries(LNTEE_HOST eevm f2f mbedtls mbedcrypto)

add_executable(LNTEE_ENCLAVE
        ${LNTEE_ENCLAVE_SRCS}
        )

target_include_directories(LNTEE_ENCLAVE PRIVATE
        ${EEVM_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/lntee/include/
        )
target_link_libraries(LNTEE_ENCLAVE eevm f2f mbedtls mbedcrypto)

add_custom_command(TARGET LNTEE_ENCLAVE
        POST_BUILD
        COMMAND scp LNTEE_ENCLAVE sev@141.217.221.214:LNTEE_ENCLAVE
        COMMAND scp LNTEE_ENCLAVE sev@141.217.220.65:LNTEE_ENCLAVE)

#add_program(LNTEECMAKE)