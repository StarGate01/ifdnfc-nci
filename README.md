# ifdnfc-nci

PC/SC IFD Handler based on linux_libnfc-nci

## Requirements

Get and install the (forked) libnfc-nci library from https://github.com/StarGate01/linux_libnfc-nci/ , which works with the kernel driver from https://github.com/jr64/nxp-pn5xx .

Other requirements are `gcc`, `cmake`, `pkg-config`, `pcsclite` and `make` .

## Building

```
mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../install -DCMAKE_BUILD_TYPE=Debug ..
make
```
