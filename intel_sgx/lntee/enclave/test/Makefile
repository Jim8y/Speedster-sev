# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Detect C and C++ compiler options
# if not gcc, default to clang-7

C_COMPILER=$(notdir $(CC))
ifeq ($(C_COMPILER), gcc)
        CXX_COMPILER=$(notdir $(CXX))
        USE_GCC = true
endif

ifeq ($(USE_GCC),)
        CC = clang-7
        CXX = clang++-7
        C_COMPILER=clang
        CXX_COMPILER=clang++
endif

CFLAGS=$(shell pkg-config oeenclave-$(C_COMPILER) --cflags) $(shell pkg-config protobuf --cflags)
CXXFLAGS=$(shell pkg-config oeenclave-$(CXX_COMPILER) --cflags) $(shell pkg-config protobuf --cflags)
LDFLAGS=$(shell pkg-config protobuf_oe_enclave --libs) $(shell pkg-config oeenclave-$(CXX_COMPILER) --libs)

MAKE_SOURCE_PATH=/home/compass/LNTEE
MAKE_BOOST_LIB_PAT=/usr/lib/x86_64-linux-gnu

MAKE_LIB_PATH=${MAKE_SOURCE_PATH}/lib

INCLUDE_EEVM=-I${MAKE_LIB_PATH}/eEVM/3rdparty -I${MAKE_LIB_PATH}/eEVM/include -I${MAKE_LIB_PATH}/eEVM/tee_lib


all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

build:
#	$(MAKE) clean
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../../lntee.edl --trusted
#	protoc -I../../ ../../payload.proto --cpp_out=../src
	$(CXX) -g ${INCLUDE_EEVM}  -I${MAKE_SOURCE_PATH}/lib/include -I${MAKE_SOURCE_PATH}/lntee/enclave/include $(CXXFLAGS) $(INCLUDES) -DOE_API_VERSION=2 -Wall -std=gnu++1z   -c ../src/*.cpp
	$(CC) -g -c $(CFLAGS) $(CINCLUDES) -DOE_API_VERSION=2 lntee_t.c -o lntee_t.o
	$(CXX) -o lntee *.o  ${MAKE_LIB_PATH}/eEVM/build/libeevm.a $(LDFLAGS) -static-libgcc -lgcc

sign:
	oesign sign --enclave-image lntee --config-file lntee.conf --key-file private.pem

clean:
	rm -f *.o lntee lntee.signed private.pem public.pem lntee_t.o lntee_t.h lntee_t.c lntee_args.h *.tmp ../src/payload.*

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem
