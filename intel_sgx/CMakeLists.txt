cmake_minimum_required(VERSION 3.15)
project(LNTEESGX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(EEVM_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/lib/eEVM/3rdparty
        ${CMAKE_SOURCE_DIR}/lib/eEVM/include
        ${CMAKE_SOURCE_DIR}/lib/libf2f/include
        )

if (LVI_MITIGATION MATCHES ControlFlow)
    # Configure the cmake to use customized compilation toolchain.
    # This package has to be added before `project()`.
    find_package(OpenEnclave-LVI-Mitigation CONFIG REQUIRED)
endif ()

find_package(OpenEnclave CONFIG REQUIRED)

add_subdirectory(lib/eEVM)
add_subdirectory(lib/libf2f)

add_subdirectory(lntee/enclave)
add_subdirectory(lntee/host)

# Generate key
add_custom_command(OUTPUT private.pem public.pem
        COMMAND openssl genrsa -out private.pem -3 3072
        COMMAND openssl rsa -in private.pem -pubout -out public.pem)

# Sign enclave
add_custom_command(OUTPUT LNTEE_ENCLAVE.signed
        DEPENDS LNTEE_ENCLAVE ${CMAKE_SOURCE_DIR}/lntee/lntee.conf private.pem
        COMMAND openenclave::oesign sign -e $<TARGET_FILE:LNTEE_ENCLAVE> -c ${CMAKE_SOURCE_DIR}/lntee/lntee.conf -k private.pem)

add_custom_target(sign ALL DEPENDS LNTEE_ENCLAVE.signed)

################################ # # # # # # # #
# TEST
################################ # # # # # # # #

add_custom_target(LNTEE
        DEPENDS LNTEE_HOST LNTEE_ENCLAVE sign)

add_custom_target(run1
        DEPENDS LNTEE_HOST LNTEE_ENCLAVE sign
        COMMAND LNTEE_HOST ${CMAKE_SOURCE_DIR}/lntee/host/contract ${CMAKE_BINARY_DIR}/lntee/enclave/LNTEE_ENCLAVE.signed 127.0.0.1 10000)

add_custom_target(run2
        DEPENDS LNTEE_HOST LNTEE_ENCLAVE sign
        COMMAND LNTEE_HOST ${CMAKE_SOURCE_DIR}/lntee/host/contract ${CMAKE_BINARY_DIR}/lntee/enclave/LNTEE_ENCLAVE.signed 127.0.0.1 10001)

add_custom_target(scp_contract
        COMMAND scp -r ${CMAKE_SOURCE_DIR}/lntee/host/contract fog@40.76.59.33:contract
        COMMAND scp -r ${CMAKE_SOURCE_DIR}/lntee/host/contract fog@40.91.198.207:contract
        )

add_custom_target(scp
        DEPENDS LNTEE_HOST LNTEE_ENCLAVE sign

        COMMAND scp -r ${CMAKE_BINARY_DIR}/lntee/host/LNTEE_HOST fog@40.76.59.33:LNTEE_HOST
        COMMAND scp -r ${CMAKE_BINARY_DIR}/lntee/host/LNTEE_HOST fog@40.91.198.207:LNTEE_HOST

        COMMAND scp -r ${CMAKE_BINARY_DIR}/lntee/enclave/LNTEE_ENCLAVE.signed fog@40.76.59.33:LNTEE_ENCLAVE.signed
        COMMAND scp -r ${CMAKE_BINARY_DIR}/lntee/enclave/LNTEE_ENCLAVE.signed fog@40.91.198.207:LNTEE_ENCLAVE.signed)


# add_custom_command(TARGET LNTEE_ENCLAVE
#         POST_BUILD
#         COMMAND scp LNTEE_ENCLAVE sev@141.217.220.162:LNTEE_ENCLAVE
#         COMMAND scp LNTEE_ENCLAVE sev@141.217.220.95:LNTEE_ENCLAVE)

#add_program(LNTEECMAKE)
# ssh sev@141.217.220.160
# ssh sev@141.217.220.224