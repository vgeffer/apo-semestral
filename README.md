# apo-semestral

### Building



The build process requires several dependencies to installed on the system.
> [!NOTE] 
> Following package names are listed as they appear in Arch repos and AUR. They may vary for Your distro. 

**Nuttx:**
- riscv64-unknown-elf-gcc
- riscv64-unknown-elf-binutils
- esptool

**IOExpander fw:**
- arduino-avr-core
- avr-binutils
- avr-gcc
- avr-libc
- avrdude

General dependencies of the build process:
- cURL
- Git
- GNU Make



To build the project, first download required build dependencies (nuttx, nuttx apps, ESP32 bootloader, ...) using
```
make deps
```
Then, build the Nuttx and IOExpander images using
```
make firmware
make nuttx
```
Lastly, to flash the images to the devices use
```
make flash-nuttx ESP_FLASH_PORT=[port]
make flash-fw AVR_FLASH_PORT=[port]
```
