# Speedster for ARM TrustZone

project in ./lnteetz

runs upon our version of optee os

## Start Command

```shell
cd /home/sgx/op_qemu/emulation/build/../out/bin && /home/sgx/op_qemu/emulation/build/../qemu/aarch64-softmmu/qemu-system-aarch64 \
	-nographic \
	-serial tcp:localhost:54320 -serial tcp:localhost:54321 \
	-smp 2 \
	-s -S -machine virt,secure=on -cpu cortex-a57 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-initrd rootfs.cpio.gz \
	-kernel Image -no-acpi \
	-append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2' \
	-virtfs local,id=sh0,path=/home/sgx,security_model=passthrough,readonly,mount_tag=sh0 \
	-device virtio-net-pci,netdev=vm1,mac=52:55:cc:d1:55:02 -netdev tap,ifname=tap1,id=vm1,script=/etc/qemu-ifup,downscript=/etc/qemu-ifdown
```
