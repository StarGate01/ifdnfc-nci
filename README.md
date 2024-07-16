# ifdnfc-nci

PC/SC IFD Handler for PCSClite based on `linux_libnfc-nci` and `ifdnfc` (https://github.com/nfc-tools/ifdnfc).

This stack targets the `NXP PN54x chipset`. Extended APDUs are (mostly) supported, extensions for the German eID card protocol are implemented as well.

## Requirements

Get and install the (forked) libnfc-nci library from https://github.com/StarGate01/linux_libnfc-nci/ , which works with the kernel driver from https://github.com/jr64/nxp-pn5xx .

Other requirements are `gcc`, `cmake`, `pkg-config`, `pcsclite` and `make` .

## Building

```
mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../install ..
make
```

## Installing

Place the `./install/lib/libifdnfc-nci.so` file somewhere you like, e.g. `/usr/lib/pcsc/drivers/serial/libifdnfc-nci.so`

Append to `/etc/reader.conf` (or wherever your pcsc serial reader configuration is):

```
FRIENDLYNAME "NFC NCI"
LIBPATH      /usr/lib/pcsc/drivers/serial/libifdnfc-nci.so
CHANNELID    0
```
