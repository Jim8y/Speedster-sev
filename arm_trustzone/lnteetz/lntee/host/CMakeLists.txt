add_custom_command(OUTPUT lntee_u.h lntee_u.c lntee_args.h
        DEPENDS ${LNTEE_ROOT}/lntee/lntee.edl edger8r
        COMMAND edger8r --untrusted ${LNTEE_ROOT}/lntee/lntee.edl
        --search-path /home/sgx/op_qemu/sdk/include)

                  
file(GLOB LNTEE_HOST_SRCS
        ${LNTEE_ROOT}/lntee/host/src/*.cpp
        )

add_executable(LNTEE_HOST
        ${LNTEE_HOST_SRCS}
        ${CMAKE_CURRENT_BINARY_DIR}/lntee_u.c)

target_include_directories(LNTEE_HOST PRIVATE
        ${LNTEE_ROOT}/lib/eEVM/3rdparty
        ${LNTEE_ROOT}/lib/eEVM/include
        ${LNTEE_ROOT}/lib/libf2f/include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${LNTEE_ROOT}/lntee/host/include
        )

target_link_libraries(LNTEE_HOST eevm f2f oehost)
message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")