add_custom_command(OUTPUT lntee_u.h lntee_u.c lntee_args.h
        DEPENDS ${CMAKE_SOURCE_DIR}/lntee/lntee.edl
        COMMAND openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/lntee/lntee.edl
        --search-path ${OE_INCLUDEDIR} --search-path
        ${OE_INCLUDEDIR}/openenclave/edl/sgx)


file(GLOB LNTEE_HOST_SRCS
        ${CMAKE_SOURCE_DIR}/lntee/host/src/*.cpp
        )

add_executable(LNTEE_HOST
        ${LNTEE_HOST_SRCS}
        ${CMAKE_CURRENT_BINARY_DIR}/lntee_u.c)

target_include_directories(LNTEE_HOST PRIVATE
        ${EEVM_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lntee/host/include
        )

target_link_libraries(LNTEE_HOST eevm f2f openenclave::oehost)
