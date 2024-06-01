# apo-semestral

This repository contains an implementation of semestral assignment from b0b35apo (Computer Architectures).

## TOC
1. Hardware
2. Build dependencies
3. Building
4. Wiki
5. Notice

## Hardware

Current version of the code is build for following hardware:
  - M5 Stamp-C3U (Esp32-C3 based microcontroller) as the main controller
  - ATmega328p as the IO Expander
  - ILI9341-based screen, connected through the 16-bit parallel interface and shift registers 
  - DHT11 temperature and humidity sensor
  - MAX6675 temperature sensor
Alhtough some aspects of the hardware can be configured via software (screen resolution), some assume exact hardware listed above (I/O expander firmware).

## Build Dependencies
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
- Doxygen (optional)


## Building
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
[Optional] *To build/read the docs use*
```
make docs
```
Docs are only built once, successive calls only open them for reading.

## Wiki
For more informations about this project (current state, roadmap, etc.) please refer to the [wiki](https://github.com/vgeffer/apo-semestral/wiki) of this repository.

## Notice
This project uses the port of a [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library/) by Adafruit 

**Copyright (c) 2020 Adafruit Industries**

