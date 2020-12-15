# QEMU NET TAP script

```bash
https://help.ubuntu.com/community/Network%20Bridge%20with%20a%20Tap%21
https://docs.windriver.com/bundle/Wind_River_Linux_Open_Virtualization_Features_Guide_LTS_1/page/evf1537990866354.html
https://unix.stackexchange.com/questions/609241/creating-vms-using-kvm-error-unit-libvirtd-service-could-not-be-found
http://cn.voidcc.com/question/p-ovcblprh-rs.html

change the net config in common.mk to 

QEMU_EXTRA_ARGS +=\
	-device virtio-net-pci,netdev=vm0 \
	-netdev tap,id=vm0,script=/etc/qemu-ifup,downscript=/etc/qemu-ifdown
```

```jsx
#!/bin/bash
 
# let host and guests talk to each other over macvlan
# configures a macvlan interface on the hypervisor
# run this on the hypervisor (e.g. in /etc/rc.local)
# made for IPv4; need modification for IPv6
# meant for a simple network setup with only eth0,
# and a static (manual) ip config
# Evert Mouw, 2013
 
HWLINK=enp0s31f6
#eth0
MACVLN=macvlan0
TESTHOST=www.google.com
 
# ------------
# wait for network availability
# ------------
 
while ! ping -q -c 1 $TESTHOST > /dev/null
do
    echo "$0: Cannot ping $TESTHOST, waiting another 5 secs..."
    sleep 5
done
 
# ------------
# get network config
# ------------
 
IP=$(ip address show dev $HWLINK | grep "inet " | awk '{print $2}')
NETWORK=$(ip -o route | grep $HWLINK | grep -v default | awk '{print $1}')
GATEWAY=$(ip -o route | grep default | awk '{print $3}')
 
# ------------
# setting up $MACVLN interface
# ------------
 
ip link add link $HWLINK $MACVLN type macvlan mode bridge
ip address add $IP dev $MACVLN
ip link set dev $MACVLN up
 
# ------------
# routing table
# ------------
 
# empty routes
ip route flush dev $HWLINK
ip route flush dev $MACVLN
 
# add routes
ip route add $NETWORK dev $MACVLN metric 0
 
# add the default gateway
ip route add default via $GATEWAY

//https://blog.csdn.net/zdd_11111/article/details/51462475
auto lo
　　iface lo inet loopback
　　#auto p4p1//p4p1网卡的配置要注释掉
　　#iface p4p1 inet manual
　　auto br0
　　iface br0 inet static
　　bridge_ports p4p1
　　address 223.129.0.243
　　netmask 255.255.255.128
　　gateway 223.129.0.254
　　bridge_fd 9
　　bridge_hello 2
　　bridge_maxage 12
　　bridge_stp off
　　dns-nameservers 202.115.32.36 202.115.32.39
　　auto tap0
　　iface tap0 inet manual
　　pre-up tunctl -t tap0 -u root
　　pre-up ifconfig tap0 0.0.0.0 promisc up
　　post-up brctl addif br0 tap0
```

```jsx
#!/bin/bash

#
# user changeable parameters
#
HDA_FILE="./ubuntu-18.04-desktop.qcow2"
GUEST_SIZE_IN_MB="4096"
SEV_GUEST="1"
SMP_NCPUS="4"
CONSOLE="qxl"
QEMU_INSTALL_DIR=/usr/local/bin/
UEFI_BIOS_CODE="/usr/local/share/qemu/OVMF_CODE.fd"
UEFI_BIOS_VARS="OVMF_VARS.fd"
#VNC_PORT=""
USE_VIRTIO="1"

usage() {
	echo "$0 [options]"
	echo "Available <commands>:"
	echo " -hda          hard disk ($HDA_FILE)"
	echo " -nosev        disable sev support"
	echo " -mem          guest memory"
	echo " -smp          number of cpus"
	echo " -console      display console to use (serial or gxl)"
	echo " -vnc          VNC port to use"
	echo " -bios         bios to use (default $UEFI_BIOS_CODE)"
	echo " -kernel       kernel to use"
	echo " -initrd       initrd to use"
	echo " -cdrom        CDROM image"
	echo " -virtio       use virtio devices"
	echo " -gdb          start gdbserver"
	exit 1  
}

add_opts() {
	echo -n "$* " >> ${QEMU_CMDLINE}
}

run_cmd () {
	$*
	if [ $? -ne 0 ]; then
		echo "command $* failed"
		exit 1
	fi
}

if [ `id -u` -ne 0 ]; then
	echo "Must be run as root!"
	exit 1
fi

while [[ $1 != "" ]]; do
	case "$1" in
		-hda) 		HDA_FILE="${2}"
				shift
				;;
		-nosev) 	SEV_GUEST="0"
				;;
		-mem)  		GUEST_SIZE_IN_MB=${2}
				shift
				;;
		-console)	CONSOLE=${2}
				shift
				;;
		-smp)		SMP_NCPUS=$2
				shift
				;;
		-vnc)		VNC_PORT=$2
				shift
				if [ "${VNC_PORT}" = "" ]; then
					usage
				fi
				;;
		-bios)		UEFI_BIOS_CODE="`readlink -f $2`"
				shift
				;;
		-netconsole)	NETCONSOLE_PORT=$2
				shift
				;;
		-initrd)	INITRD_FILE=$2
				shift
				;;
		-kernel)	KERNEL_FILE=$2
				shift
				;;
		-cdrom)		CDROM_FILE=$2
				shift
				;;
		-virtio)        USE_VIRTIO="1"
				;;
		-gdb)      	USE_GDB="1"
				;;
		*) 		usage;;
	esac
	shift
done

# we add all the qemu command line options into a file
QEMU_CMDLINE=/tmp/cmdline.$$
rm -rf ${QEMU_CMDLINE}

add_opts "${QEMU_INSTALL_DIR}qemu-system-x86_64"

# Basic virtual machine property
add_opts "-enable-kvm -cpu EPYC -machine q35"

# add number of VCPUs
[ ! -z ${SMP_NCPUS} ] && add_opts "-smp ${SMP_NCPUS},maxcpus=64"

# define guest memory
add_opts "-m ${GUEST_SIZE_IN_MB}M,slots=5,maxmem=30G"

# The OVMF binary, including the non-volatile variable store, appears as a
# "normal" qemu drive on the host side, and it is exposed to the guest as a
# persistent flash device.
add_opts "-drive if=pflash,format=raw,unit=0,file=${UEFI_BIOS_CODE},readonly"
add_opts "-drive if=pflash,format=raw,unit=1,file=${UEFI_BIOS_VARS}"

# add CDROM if specified
[ ! -z ${CDROM_FILE} ] && add_opts "-drive file=${CDROM_FILE},media=cdrom -boot d"

add_opts "-netdev tap,id=vmnic,ifname=tap1,downscript=no -device e1000,netdev=vmnic,mac=52:55:cc:d1:55:01"

# add_opts "-netdev tap, id=vmnic, ifname=tap0, downscript=no -device e1000, netdev=vmnic,mac=52:55:aa:d1:55:01"
#  -net nic,model=virtio,netdev=hn0 -netdev bridge,br=bridge0,id=hn0
# qemu-kvm -name kvm_linux -m 200 -smp 2 -drive file=/root/cirros-0.3.3-x86_64-disk.img,media=disk,if=virtio,format=qcow2 
# -net nic,model=virtio,macaddr=52:54:00:01:01:01 -net tap,ifname=virtual_eth1 -vnc 192.168.43.11:1 -daemonize

# If harddisk file is specified then add the HDD drive
if [ ! -z ${HDA_FILE} ]; then
	if [ "$USE_VIRTIO" = "1" ]; then
		if [[ ${HDA_FILE} = *"qcow2" ]]; then
			add_opts "-drive file=${HDA_FILE},if=none,id=disk0,format=qcow2"
		else
			add_opts "-drive file=${HDA_FILE},if=none,id=disk0,format=raw"
		fi
		add_opts "-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true"
		add_opts "-device scsi-hd,drive=disk0"
	else
		if [[ ${HDA_FILE} = *"qcow2" ]]; then
			add_opts "-drive file=${HDA_FILE},format=qcow2"
		else
			add_opts "-drive file=${HDA_FILE},format=raw"
		fi
	fi
fi

# If this is SEV guest then add the encryption device objects to enable support
if [ ${SEV_GUEST} = "1" ]; then
	add_opts "-object sev-guest,id=sev1,cbitpos=47,reduced-phys-bits=1"
	add_opts "-machine memory-encryption=sev1"
fi

# if console is serial then disable graphical interface
if [ "${CONSOLE}" = "serial" ]; then
	add_opts "-nographic"
else
	add_opts "-vga ${CONSOLE}"
fi

# if -kernel arg is specified then use the kernel provided in command line for boot
if [ "${KERNEL_FILE}" != "" ]; then
	add_opts "-kernel $KERNEL_FILE"
	add_opts "-append \"console=ttyS0 earlyprintk=serial root=/dev/sda2\""
	[ ! -z ${INITRD_FILE} ] && add_opts "-initrd ${INITRD_FILE}"
fi

# start vnc server
[ ! -z ${VNC_PORT} ] && add_opts "-vnc :${VNC_PORT}" && echo "Starting VNC on port ${VNC_PORT}"

# start monitor on pty
add_opts "-monitor pty"

# add virtio ring
if [ "$USE_VIRTIO" = "1" ]; then
	add_opts "-device virtio-rng-pci,disable-legacy=on,iommu_platform=true"
fi

# log the console  output in stdout.log
QEMU_CONSOLE_LOG=`pwd`/stdout.log

# save the command line args into log file
cat $QEMU_CMDLINE | tee ${QEMU_CONSOLE_LOG}
echo | tee -a ${QEMU_CONSOLE_LOG}

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."
bash ${QEMU_CMDLINE} 2>&1 | tee -a ${QEMU_CONSOLE_LOG}

# restore the mapping
stty intr ^c

rm -rf ${QEMU_CMDLINE}
```

```jsx
#!/bin/bash

#
# user changeable parameters
#
HDA_FILE="${HOME}/ubuntu-18.04-desktop.qcow2"
GUEST_SIZE_IN_MB="4096"
SEV_GUEST="1"
SMP_NCPUS="4"
CONSOLE="qxl"
QEMU_INSTALL_DIR=/usr/local/bin/
UEFI_BIOS_CODE="/usr/local/share/qemu/OVMF_CODE.fd"
UEFI_BIOS_VARS="OVMF_VARS.fd"
#VNC_PORT=""
USE_VIRTIO="1"

usage() {
	echo "$0 [options]"
	echo "Available <commands>:"
	echo " -hda          hard disk ($HDA_FILE)"
	echo " -nosev        disable sev support"
	echo " -mem          guest memory"
	echo " -smp          number of cpus"
	echo " -console      display console to use (serial or gxl)"
	echo " -vnc          VNC port to use"
	echo " -bios         bios to use (default $UEFI_BIOS_CODE)"
	echo " -kernel       kernel to use"
	echo " -initrd       initrd to use"
	echo " -cdrom        CDROM image"
	echo " -virtio       use virtio devices"
	echo " -gdb          start gdbserver"
	exit 1  
}

add_opts() {
	echo -n "$* " >> ${QEMU_CMDLINE}
}

run_cmd () {
	$*
	if [ $? -ne 0 ]; then
		echo "command $* failed"
		exit 1
	fi
}

if [ `id -u` -ne 0 ]; then
	echo "Must be run as root!"
	exit 1
fi

while [[ $1 != "" ]]; do
	case "$1" in
		-hda) 		HDA_FILE="${2}"
				shift
				;;
		-nosev) 	SEV_GUEST="0"
				;;
		-mem)  		GUEST_SIZE_IN_MB=${2}
				shift
				;;
		-console)	CONSOLE=${2}
				shift
				;;
		-smp)		SMP_NCPUS=$2
				shift
				;;
		-vnc)		VNC_PORT=$2
				shift
				if [ "${VNC_PORT}" = "" ]; then
					usage
				fi
				;;
		-bios)		UEFI_BIOS_CODE="`readlink -f $2`"
				shift
				;;
		-netconsole)	NETCONSOLE_PORT=$2
				shift
				;;
		-initrd)	INITRD_FILE=$2
				shift
				;;
		-kernel)	KERNEL_FILE=$2
				shift
				;;
		-cdrom)		CDROM_FILE=$2
				shift
				;;
		-virtio)        USE_VIRTIO="1"
				;;
		-gdb)      	USE_GDB="1"
				;;
		*) 		usage;;
	esac
	shift
done

# we add all the qemu command line options into a file
QEMU_CMDLINE=/tmp/cmdline.$$
rm -rf ${QEMU_CMDLINE}

add_opts "${QEMU_INSTALL_DIR}qemu-system-x86_64"

# Basic virtual machine property
add_opts "-enable-kvm -cpu EPYC -machine q35"

# add number of VCPUs
[ ! -z ${SMP_NCPUS} ] && add_opts "-smp ${SMP_NCPUS},maxcpus=64"

# define guest memory
add_opts "-m ${GUEST_SIZE_IN_MB}M,slots=5,maxmem=30G"

# The OVMF binary, including the non-volatile variable store, appears as a
# "normal" qemu drive on the host side, and it is exposed to the guest as a
# persistent flash device.
add_opts "-drive if=pflash,format=raw,unit=0,file=${UEFI_BIOS_CODE},readonly"
add_opts "-drive if=pflash,format=raw,unit=1,file=${UEFI_BIOS_VARS}"

# add CDROM if specified
[ ! -z ${CDROM_FILE} ] && add_opts "-drive file=${CDROM_FILE},media=cdrom -boot d"

#add_opts "-netdev user,id=vmnic -device e1000,netdev=vmnic,romfile="
add_opts "-netdev tap, id=vmnic, ifname=tap0, downscript=no -device e1000, netdev=vmnic,mac=52:55:aa:d1:55:01"
#  -net nic,model=virtio,netdev=hn0 -netdev bridge,br=bridge0,id=hn0

# If harddisk file is specified then add the HDD drive
if [ ! -z ${HDA_FILE} ]; then
	if [ "$USE_VIRTIO" = "1" ]; then
		if [[ ${HDA_FILE} = *"qcow2" ]]; then
			add_opts "-drive file=${HDA_FILE},if=none,id=disk0,format=qcow2"
		else
			add_opts "-drive file=${HDA_FILE},if=none,id=disk0,format=raw"
		fi
		add_opts "-device virtio-scsi-pci,id=scsi,disable-legacy=on,iommu_platform=true"
		add_opts "-device scsi-hd,drive=disk0"
	else
		if [[ ${HDA_FILE} = *"qcow2" ]]; then
			add_opts "-drive file=${HDA_FILE},format=qcow2"
		else
			add_opts "-drive file=${HDA_FILE},format=raw"
		fi
	fi
fi

# If this is SEV guest then add the encryption device objects to enable support
if [ ${SEV_GUEST} = "1" ]; then
	add_opts "-object sev-guest,id=sev0,cbitpos=47,reduced-phys-bits=1"
	add_opts "-machine memory-encryption=sev0"
fi

# if console is serial then disable graphical interface
if [ "${CONSOLE}" = "serial" ]; then
	add_opts "-nographic"
else
	add_opts "-vga ${CONSOLE}"
fi

# if -kernel arg is specified then use the kernel provided in command line for boot
if [ "${KERNEL_FILE}" != "" ]; then
	add_opts "-kernel $KERNEL_FILE"
	add_opts "-append \"console=ttyS0 earlyprintk=serial root=/dev/sda2\""
	[ ! -z ${INITRD_FILE} ] && add_opts "-initrd ${INITRD_FILE}"
fi

# start vnc server
[ ! -z ${VNC_PORT} ] && add_opts "-vnc :${VNC_PORT}" && echo "Starting VNC on port ${VNC_PORT}"

# start monitor on pty
add_opts "-monitor pty"

# add virtio ring
if [ "$USE_VIRTIO" = "1" ]; then
	add_opts "-device virtio-rng-pci,disable-legacy=on,iommu_platform=true"
fi

# log the console  output in stdout.log
QEMU_CONSOLE_LOG=`pwd`/stdout.log

# save the command line args into log file
cat $QEMU_CMDLINE | tee ${QEMU_CONSOLE_LOG}
echo | tee -a ${QEMU_CONSOLE_LOG}

# map CTRL-C to CTRL ]
echo "Mapping CTRL-C to CTRL-]"
stty intr ^]

echo "Launching VM ..."
bash ${QEMU_CMDLINE} 2>&1 | tee -a ${QEMU_CONSOLE_LOG}

# restore the mapping
stty intr ^c

rm -rf ${QEMU_CMDLINE}
```

```cpp
try { 
    boost::asio::io_service netService; 
    udp::resolver resolver(netService); 
    udp::resolver::query query(udp::v4(), "google.com", ""); 
    udp::resolver::iterator endpoints = resolver.resolve(query); 
    udp::endpoint ep = *endpoints; 
    udp::socket socket(netService); 
    socket.connect(ep); 
    boost::asio::ip::address addr = socket.local_endpoint().address(); 
    std::cout << "My IP according to google is: " << addr.to_string() << std::endl; 
} catch (std::exception& e){ 
    std::cerr << "Could not deal with socket. Exception: " << e.what() << std::endl; 

}boost 
```