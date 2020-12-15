
add_custom_command(OUTPUT lntee_t.h lntee_t.c lntee_args.h
        DEPENDS ${CMAKE_SOURCE_DIR}/lntee/lntee.edl
        COMMAND openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/lntee/lntee.edl
        --search-path ${OE_INCLUDEDIR} --search-path
        ${OE_INCLUDEDIR}/openenclave/edl/sgx)


aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/src  EEVM_SOURCE1)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/src/simple EEVM_SIMPLE_SOURCE)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/fmt 3RD_PARTY_FMT)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/intx/lib/intx 3RD_PARTY_INTX1)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/intx/include/intx 3RD_PARTY_INTX2)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/keccak 3RD_PARTY_KECCAK)
aux_source_directory(${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/nlohmann 3RD_PARTY_NLOHMANN)

file(GLOB LNTEE_ENCLAVE_SRCS
        ${CMAKE_SOURCE_DIR}/lntee/enclave/src/*.cpp
        )

add_executable(LNTEE_ENCLAVE
        ${LNTEE_ENCLAVE_SRCS}
        ${CMAKE_CURRENT_BINARY_DIR}/lntee_t.c
        ${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/intx/include/intx/intx.hpp
        ${EEVM_SOURCE1}
        ${EEVM_SIMPLE_SOURCE}
        ${3RD_PARTY_FMT}
        ${3RD_PARTY_INTX1}
        ${3RD_PARTY_INTX2}
        ${3RD_PARTY_KECCAK}
        ${3RD_PARTY_NLOHMANN}
        )


target_compile_definitions(LNTEE_ENCLAVE PUBLIC OE_API_VERSION=2)

target_include_directories(LNTEE_ENCLAVE PRIVATE
        ${EEVM_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty/intx/include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lntee/enclave/include/
        )

target_link_libraries(LNTEE_ENCLAVE
        $<$<PLATFORM_ID:Linux>:-Wl,--require-defined=__oe_ecalls_table>
        openenclave::oeenclave openenclave::oelibc openenclave::oelibcxx)