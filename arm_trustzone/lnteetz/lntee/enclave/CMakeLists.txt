
#set(CMAKE_SYSTEM_NAME "System V")

add_custom_command(OUTPUT lntee_t.h lntee_t.c lntee_args.h
        DEPENDS ${LNTEE_ROOT}/lntee/lntee.edl
        COMMAND edger8r --trusted ${LNTEE_ROOT}/lntee/lntee.edl)


aux_source_directory(${LNTEE_ROOT}/lib/eEVM/src  EEVM_SOURCE1)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/src/simple EEVM_SIMPLE_SOURCE)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/3rdparty/fmt 3RD_PARTY_FMT)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/3rdparty/intx/lib/intx 3RD_PARTY_INTX1)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/3rdparty/intx/include/intx 3RD_PARTY_INTX2)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/3rdparty/keccak 3RD_PARTY_KECCAK)
aux_source_directory(${LNTEE_ROOT}/lib/eEVM/3rdparty/nlohmann 3RD_PARTY_NLOHMANN)

#message(STATUS "LNTEE_ROOT = ${LNTEE_ROOT}")
#message(STATUS "LNTEE_ROOT_BUILD = ${LNTEE_ROOT_BUILD}")



#set(EEVM_SOURCE1
#        ${LNTEE_ROOT}/lib/eEVM/src/util.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/common.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/encoding.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/transaction.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/stack.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/disassembler.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/processor.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/simple/simpleaccount.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/simple/simpleglobalstate.cpp
#        ${LNTEE_ROOT}/lib/eEVM/src/simple/simplestorage.cpp
#        )


file(GLOB LNTEE_ENCLAVE_SRCS
        ${LNTEE_ROOT}/lntee/enclave/src/*.cpp
#        ${LNTEE_ROOT}/lntee/enclave/main.cpp
        )

add_enclave(
        TARGET
        LNTEE_ENCLAVE
        UUID
        b843807a-e05c-423c-bcfb-1062cadb483f
        CXX
        SOURCES
        ${LNTEE_ENCLAVE_SRCS}
        ${CMAKE_CURRENT_BINARY_DIR}/lntee_t.c
        ${LNTEE_ROOT}/lib/eEVM/3rdparty/intx/include/intx/intx.hpp
        ${EEVM_SOURCE1}
        ${EEVM_SIMPLE_SOURCE}
        ${3RD_PARTY_FMT}
        ${3RD_PARTY_INTX1}
        ${3RD_PARTY_INTX2}
        ${3RD_PARTY_KECCAK}
        ${3RD_PARTY_NLOHMANN}
)

message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

enclave_include_directories(LNTEE_ENCLAVE PRIVATE
        ${LNTEE_ROOT}/lib/eEVM/3rdparty
        ${LNTEE_ROOT}/lib/eEVM/3rdparty/intx/include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${LNTEE_ROOT}/lib/eEVM/include
        ${LNTEE_ROOT}/lntee/enclave/include/
        )

target_compile_definitions(LNTEE_ENCLAVE PUBLIC OE_API_VERSION=2)

enclave_compile_features(LNTEE_ENCLAVE PRIVATE cxx_auto_type)
#message("------- Target compiler flags are: ${CMAKE_CXX_FLAGES}")
#enclave_link_libraries(LNTEE_ENCLAVE oelibc oelibcxx)
