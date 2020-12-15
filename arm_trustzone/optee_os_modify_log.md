# OPTEE OPENENCLAVE

# Boost

```bash
https://sites.google.com/site/robdevelopertips/how-to-build-boost-1-64-0-for-android

cd ~/boost_1_64_0
./b2 -d+2 -j 4 target-os=linux toolset=gcc-arm --prefix=/home/sgx/boost-arm install
```

```bash
#!/bin/bash

execute_test() {

    echo -e "\e[32m$1>>>>>>>>>>>>>>>>>>>>>>>>>"
    echo -e "\e[39m"
    cp openenclave_qemu/build/tests/$1/enc/$2.ta /lib/optee_armtz
    openenclave_qemu/build/tests/$1/host/$1_host $2
}

execute_test "c99_compliant" "b843807a-e05c-423c-bcfb-1062cadb482f"
execute_test "create-errors" "1083bbac-751e-4d26-ada6-c254bbfbe653"
execute_test "crypto" "f0be7db0-ce7c-4dc4-b8c8-b161f4216225"
execute_test "edl_opt_out" "892e7f65-5da1-45d0-8209-53795ce5be8f"
execute_test "hexdump" "126830b9-eb9f-412a-89a7-bcc8a517c12e"
# execute_test "hostcalls" ""
execute_test "initializers" "62f73b00-bdfe-4763-a06a-dc561a3a34d8"
execute_test "mixed_c_cpp" "952c55c8-59f3-47a0-814c-ae3276a9808f"
execute_test "pingpong" "0a6cbbd3-160a-4c86-9d9d-c9cf1956be16"
execute_test "pingpong-shared" "e229cc0f-3199-4ad3-91a7-47906fcbcc59"
echo -e "\e[32mLNTEE>>>>>>>>>>>>>>>>>>>>>>>>>"
echo -e "\e[39m"

cp openenclave_qemu2/build/tests/lntee/lntee/enclave/b843807a-e05c-423c-bcfb-1062cadb483f.ta /lib/optee_armtz
LD_LIBRARY_PATH="boost-arm/lib" openenclave_qemu2/build/tests/lntee/lntee/host/LNTEE_HOST  openenclave_qemu2/sdk/tests/lntee/lntee/host/contract b843807a-e05c-423c-bcfb-1062cadb483f 10000

mkdir /mnt/home
mount -t 9p -o trans=virtio sh0 /mnt/home -oversion=9p2000.L

cd /mnt/home
```

```bash
https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/Contributors/OPTEEGettingStarted.md
https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/OP-TEE/Debugging/QEMU.md

sudo apt update && sudo apt install -y android-tools-adb                       \
    android-tools-fastboot autoconf automake bc bison build-essential ccache   \
    cgdb cscope curl device-tree-compiler expect flex ftp-upload gdb-multiarch \
    gdisk iasl libattr1-dev libc6 libcap-dev libfdt-dev libftdi-dev            \
    libglib2.0-dev libhidapi-dev libncurses5-dev libpixman-1-dev libssl-dev    \
    libstdc++6 libtool libz1 make mtools netcat python-crypto                  \
    python-pyelftools python-serial python-wand python3-pyelftools repo unzip  \
    uuid-dev xdg-utils xterm xz-utils zlib1g-dev

# [ TERM 1 ]

mkdir emulation
cd emulation

repo init -u https://github.com/ms-iot/optee_manifest -m oe_qemu_v8.xml -b oe-3.6.0
repo sync

# [ TERM 1 ]

cd build

make toolchains -j2
make run -j$(nproc)

# [ TERM 2 ]

git clone --recursive https://github.com/openenclave/openenclave.git sdk

cd sdk

# Set up the build environment (only once).
sudo scripts/ansible/install-ansible.sh
sudo ansible-playbook scripts/ansible/oe-contributors-setup-cross-arm.yml

cd ..

mkdir build
cd build

# Configure the SDK
cmake ../sdk -G Ninja -DCMAKE_TOOLCHAIN_FILE=../sdk/cmake/arm-cross.cmake -DOE_TA_DEV_KIT_DIR=$PWD/../emulation/optee_os/out/arm/export-ta_arm64 -DCMAKE_BUILD_TYPE=Debug

# Build the SDK
ninja

cd ..

mkdir /mnt/home
mount -t 9p -o trans=virtio sh0 /mnt/home -oversion=9p2000.L
cd /mnt/home

cp openenclave_qemu/build/tests/hexdump/enc/126830b9-eb9f-412a-89a7-bcc8a517c12e.ta /lib/optee_armtz
openenclave_qemu/build/tests/hexdump/host/hexdump_host 126830b9-eb9f-412a-89a7-bcc8a517c12e

cp op_qemu/build/tests/hexdump/enc/126830b9-eb9f-412a-89a7-bcc8a517c12e.ta /lib/optee_armtz
op_qemu/build/tests/hexdump/host/hexdump_host 126830b9-eb9f-412a-89a7-bcc8a517c12e

cp op_qemu/build/tests/print/enc/58f3e795-00c3-45e0-9435-3e6fcf734acc.ta /lib/optee_armtz
op_qemu/build/tests/print/host/print_host 58f3e795-00c3-45e0-9435-3e6fcf734acc

cp openenclave_qemu/build/tests/hexdump/enc/126830b9-eb9f-412a-89a7-bcc8a517c12e.ta /lib/optee_armtz

openenclave_qemu/build/tests/hexdump/host/hexdump_host 126830b9-eb9f-412a-89a7-bcc8a517c12e

cp openenclave_qemu/build/tests/lntee/lntee/enclave/b843807a-e05c-423c-bcfb-1062cadb483f.ta /lib/optee_armtz

LD_LIBRARY_PATH="boost-arm/lib" openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST  openenclave_qemu/sdk/tests/lntee/lntee/host/contract b843807a-e05c-423c-bcfb-1062cadb483f 10000

cp op_qemu/build/tests/lntee/lntee/enclave/b843807a-e05c-423c-bcfb-1062cadb483f.ta /lib/optee_armtz

LD_LIBRARY_PATH="boost-arm/lib" op_qemu/build/tests/lntee/lntee/host/LNTEE_HOST  op_qemu/sdk/tests/lntee/lntee/host/contract b843807a-e05c-423c-bcfb-1062cadb483f 10000

```

```bash
cd ~/openenclave_qemu/build

cmake ../sdk     -G Ninja -DCMAKE_TOOLCHAIN_FILE=../sdk/cmake/arm-cross.cmake -DOE_TA_DEV_KIT_DIR=$PWD/../emulation/optee_os/out/arm/export-ta_arm64 -DCMAKE_BUILD_TYPE=Debug
sudo ninja > log.txt

##############################################

cd ~/openenclave_qemu/emulation/build

sudo make run -j$(nproc)
```

# ERROR

```bash
cc1plus: all warnings being treated as errors

//////////////////////////////////////////////

3rdparty/optee/libutee/liboeutee.a(tee_api_operations.c.o): In function `rand':
/home/sgx/openenclave_qemu/sdk/3rdparty/optee/optee_os/lib/libutee/tee_api_operations.c:1803: multiple definition of `rand'
output/lib/openenclave/enclave/liboelibc.a(rand.c.o):/home/sgx/openenclave_qemu/sdk/3rdparty/musl/musl/src/prng/rand.c:13: first defined here
output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info()':
/home/sgx/openenclave_qemu/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:401: undefined reference to `oe_get_thread_data'
output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info_fast()':
/home/sgx/openenclave_qemu/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:410: undefined reference to `oe_get_thread_data'
output/lib/enclave/liboelibcxx.a(Lstep.c.o): In function `aarch64_handle_signal_frame':
/home/sgx/openenclave_qemu/sdk/3rdparty/libunwind/libunwind/src/aarch64/Gstep.c:65: undefined reference to `_ULaarch64_is_signal_frame'
output/lib/enclave/liboelibcxx.a(Lstep.c.o): In function `__libunwind_unw_step':
/home/sgx/openenclave_qemu/sdk/3rdparty/libunwind/libunwind/src/aarch64/Gstep.c:143: undefined reference to `_ULaarch64_is_signal_frame'

/////////////////////////////////////////////////
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: error while loading shared libraries: libboost_system.so.1.64.0: cannot open shared object file: No such file or directory

///////////////////////////////////////////////
unresolvable R_AARCH64_ADR_PREL_PG_HI21 relocation against symbol `_ZTVSt14basic_ifstreamIcSt11char_traitsIcEE@@GLIBCXX_3.4'

/////////////////////////////////////////////
output/lib/openenclave/enclave/liboelibc.a(rand.c.o): In function `rand':
/home/sgx/openenclave_qemu2/sdk/3rdparty/musl/musl/src/prng/rand.c:13: multiple definition of `rand'
3rdparty/optee/libutee/liboeutee.a(tee_api_operations.c.o):/home/sgx/openenclave_qemu2/sdk/3rdparty/optee/optee_os/lib/libutee/tee_api_operations.c:1803: first defined here
output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info()':
/home/sgx/openenclave_qemu2/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:401: undefined reference to `oe_get_thread_data'
output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info_fast()':
/home/sgx/openenclave_qemu2/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:410: undefined reference to `oe_get_thread_data'
ninja: build stopped: subcommand failed.

///////////////////////////////////
	output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info()':
/home/sgx/openenclave_qemu2/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:401: undefined reference to `oe_get_thread_data'
output/lib/enclave/liboelibcxx.a(exception.cc.o): In function `thread_info_fast()':
/home/sgx/openenclave_qemu2/sdk/3rdparty/libcxxrt/libcxxrt/src/exception.cc:410: undefined reference to `oe_get_thread_data'
ninja: build stopped: subcommand failed.

//////////////////////////////////////////////////

TIME LOG: Start to load the enclave >>> 1598904438300118
####   test from optee linux enclave line 397
####   test from optee linux enclave line 408
####   test from optee linux enclave line 422
####   test from optee linux enclave line 432
2020-08-31T20:07:18+0000.404696Z [(H)ERROR] tid(0xffffb38a2010) | :OE_FAILURE [/home/sgx/openenclave_qemu2/sdk/host/optee/linux/enclave.c:oe_create_enclave:447

//////////////////////////////////////////////////
2020-08-31T22:56:32.000000Z [(H)ERROR] tid(0xffff9ac74010) | :OE_INVALID_PARAMETER [/home/sgx/openenclave_qemu/sdk/host/optee/linux/enclave.c:oe_create_enclave:406]

///////////////////////////////////////////////////

```

```cpp
in compiler_setting.cmake line 92

add_compile_options(-Wall -Werror -Wpointer-arith -Wconversion -Wextra
                      -Wno-missing-field-initializers)

//////////////////////////////////////////

/**
 * Returns the thread info structure, creating it if it is not already created.
 */
static __cxa_thread_info *thread_info()
{
    // oe_thread_data_t* td = oe_get_thread_data();
    // return (__cxa_thread_info*) td->__cxx_thread_info;
	return NULL;
}
/**
 * Fast version of thread_info().  May fail if thread_info() is not called on
 * this thread at least once already.
 */
static __cxa_thread_info *thread_info_fast()
{
   // oe_thread_data_t* td = oe_get_thread_data();
    //return (__cxa_thread_info*) td->__cxx_thread_info;
	return NULL;
}

////////////////////////////////////////////////////////

#include "Gis_signal_frame.c" in aarch64/Gstep.c

#####################################################
set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost REQUIRED ...)

#####################################################
LD_LIBRARY_PATH="boost-arm/lib" sh trustzone_test.sh
```

```cpp
# sh ./test_optee_lntee.sh 
[   49.688879] random: fast init done
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST: /lib64/libstdc++.so.6: no version information available (required by openenclave_qemu/build/tests/lntee/lntee/host/LNTEE_HOST)
TIME LOG: Start to load the enclave >>> 1598917492284099
####   test from optee linux tee_client_api line 634
####   test from optee linux tee_client_api line 643
####   test from optee linux tee_client_api line 650
####   test from optee linux tee_client_api line 662
####   test from optee linux tee_client_api line 678
[   71.028202] INFO: rcu_preempt detected stalls on CPUs/tasks:
[   71.028647]  0-...: (1 GPs behind) idle=37a/140000000000000/0 softirq=1185/1186 fqs=2625 
[   71.028968]  (detected by 1, t=5252 jiffies, g=124, c=123, q=1)
[   71.029227] Task dump for CPU 0:
[   71.029390] LNTEE_HOST      R  running task        0  1406   1404 0x00000002
[   71.029692] Call trace:
[   71.029800] [<ffff000008085ba4>] __switch_to+0x94/0xd0
[   71.030074] [<0000000000000001>] 0x1

D/TC:? 0 system_open_ta_binary:286 Lookup user TA ELF b843807a-e05c-423c-bcfb-1062cadb483f (Secure Storage TA)
D/TC:? 0 system_open_ta_binary:289 res=0xffff0008
D/TC:? 0 system_open_ta_binary:286 Lookup user TA ELF b843807a-e05c-423c-bcfb-1062cadb483f (REE [buffered])
E/TC:0 0 Panic 'Failed to spread pgdir on small tables' at core/arch/arm/mm/core_mmu.c:1546 <core_mmu_map_pages>
E/TC:0 0 Call stack:
E/TC:0 0  0x000000000e109888
E/TC:0 0  0x000000000e1151c8
E/TC:0 0  0x000000000e10b058
E/TC:0 0  0x000000000e10f87c
E/TC:0 0  0x000000000e10f994
E/TC:0 0  0x000000000e115148
E/TC:0 0  0x000000000e106444
E/TC:0 0  0x000000000e103e88
E/TC:0 0  0x000000000e113ba0
E/TC:0 0  0x000000000e10488c
E/TC:0 0  0x000000000e1163bc
E/TC:0 0  0x000000000e11b4c0
E/TC:0 0  0x000000000e10fa90
E/TC:0 0  0x000000000e102a60

https://optee.readthedocs.io/en/latest/debug/abort_dumps.html
https://www.bilibili.com/video/av55057371/

////////////////////////////////////////////////////////////////////////////
sgx@sgx:./op_qemu/emulation/optee_os/scripts/symbolize.py -d /home/sgx/op_qemu/emulation/optee_os/out/arm/core -d /home/sgx/op_qemu/build/tests/lntee/lntee/enclave/*

^D

E/TC:0 0 Panic 'Failed to spread pgdir on small tables' at core/arch/arm/mm/core_mmu.c:1546 <core_mmu_map_pages>
E/TC:0 0 Call stack:
E/TC:0 0  0x000000000e109888
E/TC:0 0  0x000000000e1151c8
E/TC:0 0  0x000000000e10b058
E/TC:0 0  0x000000000e10f87c
E/TC:0 0  0x000000000e10f994
E/TC:0 0  0x000000000e115148
E/TC:0 0  0x000000000e106444
E/TC:0 0  0x000000000e103e88
E/TC:0 0  0x000000000e113ba0
E/TC:0 0  0x000000000e10488c
E/TC:0 0  0x000000000e1163bc
E/TC:0 0  0x000000000e11b4c0
E/TC:0 0  0x000000000e10fa90
E/TC:0 0  0x000000000e102a60

E/TC:0 0 Panic 'Failed to spread pgdir on small tables' at core/arch/arm/mm/core_mmu.c:1546 <core_mmu_map_pages>
E/TC:0 0 Call stack:
E/TC:0 0  0x000000000e109888 read_pc at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/include/arm64.h:227
E/TC:0 0  0x000000000e1151c8 __do_panic at /home/sgx/openenclave_qemu/emulation/optee_os/core/kernel/panic.c:30
E/TC:0 0  0x000000000e10b058 core_mmu_map_pages at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/mm/core_mmu.c:1526
E/TC:0 0  0x000000000e10f87c mobj_reg_shm_inc_map at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/mm/mobj_dyn_shm.c:376
E/TC:0 0  0x000000000e10f994 mobj_mapped_shm_alloc at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/mm/mobj_dyn_shm.c:425
E/TC:0 0  0x000000000e115148 msg_param_mobj_from_noncontig at /home/sgx/openenclave_qemu/emulation/optee_os/core/kernel/msg_param.c:141
E/TC:0 0  0x000000000e106444 get_rpc_alloc_res at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/kernel/thread.c:1657
E/TC:0 0  0x000000000e103e88 rpc_load at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/kernel/ree_fs_ta.c:53
E/TC:0 0  0x000000000e113ba0 system_open_ta_binary at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/pta/system.c:288
E/TC:0 0  0x000000000e10488c pseudo_ta_enter_invoke_cmd at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/kernel/pseudo_ta.c:195
E/TC:0 0  0x000000000e1163bc tee_ta_invoke_command at /home/sgx/openenclave_qemu/emulation/optee_os/core/kernel/tee_ta_manager.c:758
E/TC:0 0  0x000000000e11b4c0 syscall_invoke_ta_command at /home/sgx/openenclave_qemu/emulation/optee_os/core/tee/tee_svc.c:869
E/TC:0 0  0x000000000e10fa90 tee_svc_do_call at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/tee/arch_svc_a64.S:143
E/TC:0 0  0x000000000e102a60 el0_svc at /home/sgx/openenclave_qemu/emulation/optee_os/core/arch/arm/kernel/thread_a64.S:840

sulotion
#define MAX_XLAT_TABLES		~~5~~ 6 in core_mmu_lepa.c

LOG DMSG(">>>>>  Log from core_mmu_lpae line 811");

ELF format error in ta_elf.c
if (ehdr->e_ident[EI_OSABI] != ELFOSABI_NONE) This is supposed to be NONE while we got Linux
	{
		DMSG(">>>>>  Log from ta_elf line 98 %d",ehdr->e_ident[EI_OSABI]);
		return TEE_ERROR_BAD_FORMAT;
	}
/* Values for e_ident[EI_OSABI]. */
#define	ELFOSABI_NONE		0	/* UNIX System V ABI */
#define	ELFOSABI_HPUX		1	/* HP-UX operating system */
#define	ELFOSABI_NETBSD		2	/* NetBSD */
#define	ELFOSABI_LINUX		3	/* GNU/Linux */

////////////////////////////////////////////////////////////////////
Issue description, while passing value from trustzone enclave to the REE world, only one character can be passed. [out][user_check] do no help.

Solution, add size to the property of the parameter along with [out] making it [out, size=len], and add a len parameter.

export PATH=$PATH:$HOME/toolchain/bin
```

```makefile
/usr/local/bin/ld: /opt/openenclave/lib/openenclave/enclave/liboecore.a(cpuid.c.o): in function `oe_initialize_cpuid':
/source/enclave/core/sgx/cpuid.c:29: undefined reference to `oe_sgx_get_cpuid_table_ocall'
collect2: error: ld returned 1 exit status

target_link_libraries(
  common
  PUBLIC # `liboecore`, a dependency of `liboeenclave`, requires the ecalls
         # function table. Because the libraries linking `libcommon` do not
         # directly require this symbol, the linker skips the object in
         # `libcommon` which defines them. So we manually require it.
         #
         # Alternatively we could use a CMake OBJECT library, but that
         # requires a newish version of CMake.
         $<$<PLATFORM_ID:Linux>:-Wl,--require-defined=__oe_ecalls_table>
         openenclave::oeenclave openenclave::oelibcxx)
```

```cpp
/home/sgx/openenclave_qemu/emulation/optee_os/ldelf/ta_elf.c
DMSG(">>>>>  Log from ta_elf line 79");

	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT)
	{
		DMSG(">>>>>  Log from ta_elf line 83");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
	{
		DMSG(">>>>>  Log from ta_elf line 88");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
	{
		DMSG(">>>>>  Log from ta_elf line 93");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_ident[EI_OSABI] != ELFOSABI_NONE)
	{
		DMSG(">>>>>  Log from ta_elf line 98 %d",ehdr->e_ident[EI_OSABI]);
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_type != ET_DYN)
	{
		DMSG(">>>>>  Log from ta_elf line 103");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_machine != EM_AARCH64)
	{
		DMSG(">>>>>  Log from ta_elf line 108");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_flags)
	{
		DMSG(">>>>>  Log from ta_elf line 113");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_phentsize != sizeof(Elf64_Phdr))
	{
		DMSG(">>>>>  Log from ta_elf line 118");
		// return TEE_ERROR_BAD_FORMAT;
	}
	if (ehdr->e_shentsize != sizeof(Elf64_Shdr))
	{
		DMSG(">>>>>  Log from ta_elf line 123");
		// return TEE_ERROR_BAD_FORMAT;
	}
	return TEE_ERROR_BAD_FORMAT;
```

```cpp
Assertion failed: "_check_block() panic" == NULL (/home/sgx/op_qemu/sdk/enclave/core/debugmalloc.c: _check_block: 144)

// Caused by size = addr_sizefor pubkey
public int ecall_lntee_init_tee([in,string]const char * seed,
                                        [out,size=pubkey_size]char * pubkey,
                                        size_t pubkey_size,
                                        [out,size=addr_size]char * address,
                                        size_t addr_size);
```

# ELF Head

```wasm
EI_MAG:        0x00000000     0x7F454C46          ELF
EI_CLASS       0x00000004     0x02                64 BIT
EI_DATA        0x00000005     0x01                DATA2LSB (Little-Endian)
EI_VERSION     0x00000006     0x01                EV_CURRENT
EI_OSABI       0x00000007     0x00                UNIX System V ABI
EI_OSABIVER    0x00000008     0x00                
E_TYPE         0x00000010     0x0003              ET_DYN (Shared object file)
E_MACHINE      0x00000012     0x00B7              Unknown
E_VERSION      0x00000014     0x00000001          EV_CURRENT
E_ENTRY        0x00000018     0x000000000000746C  
E_PHOFF        0x00000020     0x0000000000000040  
E_SHOFF        0x00000028     0x000000000013E550  
E_FLAGS        0x00000030     0x00000000          
E_EHSIZE       0x00000034     0x0040              
E_PHENTSIZE    0x00000036     0x0038              
E_PHNUM        0x00000038     0x0005              
E_SHENTSIZE    0x0000003A     0x0040              
E_SHNUM        0x0000003C     0x001A              
E_SHSTRNDX     0x0000003E     0x0019
```

```wasm
EI_MAG:        0x00000000     0x7F454C46          ELF
EI_CLASS       0x00000004     0x02                64 BIT
EI_DATA        0x00000005     0x01                DATA2LSB (Little-Endian)
EI_VERSION     0x00000006     0x01                EV_CURRENT
EI_OSABI       0x00000007     0x03                Linux
EI_OSABIVER    0x00000008     0x00                
E_TYPE         0x00000010     0x0003              ET_DYN (Shared object file)
E_MACHINE      0x00000012     0x00B7              Unknown
E_VERSION      0x00000014     0x00000001          EV_CURRENT
E_ENTRY        0x00000018     0x00000000000D8E44  
E_PHOFF        0x00000020     0x0000000000000040  
E_SHOFF        0x00000028     0x0000000001249A98  
E_FLAGS        0x00000030     0x00000000          
E_EHSIZE       0x00000034     0x0040              
E_PHENTSIZE    0x00000036     0x0038              
E_PHNUM        0x00000038     0x0005              
E_SHENTSIZE    0x0000003A     0x0040              
E_SHNUM        0x0000003C     0x001C              
E_SHSTRNDX     0x0000003E     0x001B
```