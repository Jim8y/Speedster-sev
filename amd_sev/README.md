## IDE

CLion

## Build Tool

CMAKE

## Start the vm

```bash
sudo launch-qemu.sh -hda ubuntu-18.04.qcow2
```

## Start the host

```bash
cd /home/sev/CLionProjects/LNTEECMAKE/cmake-build-debug &&  ./LNTEE_HOST 141.217.220.143 10000
cd /home/sev/CLionProjects/LNTEECMAKE/cmake-build-debug &&  ./LNTEE_HOST 141.217.220.219 10001
```

## Build with cmake

```bash
mkdir build
cd build
cmake ..
make
```
