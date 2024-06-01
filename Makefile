# Programs
CC_AVR := avr-gcc
LD_AVR := avr-gcc
OBJCPY_AVR := avr-objcopy
FLASH_AVR := avrdude

# AVR Bridge
FW_PORT := /dev/ttyUSB0
FW_BOARD := avr5

# Nuttx
NUTTX_REPO := https://github.com/apache/nuttx.git 
NUTTX_APPS := https://github.com/apache/nuttx-apps.git
NUTTX_BOOTLADER := https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32c3.bin
NUTTX_PARTITION := https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32c3.bin

# Directories
NUTTX_DIR := nuttx
NUTTX_APP_DIR := apps
DOCS_DIR := docs
BLD_DIR := .build
BIN_DIR := bin

# Styling
bold := $(shell tput bold)
sgr0 := $(shell tput sgr0)

# Sources
ioexpsrc := $(shell find ioexp -name '*.c')
ioexpobj := $(patsubst %.c, $(BLD_DIR)/%.o, $(ioexpsrc))

# Flags
AVR_CCFLAGS := -mmcu=atmega328p -c -pipe -Wall -pedantic -Os -w
AVR_LDFLAGS := -w -Os -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=atmega328p
AVR_OBJCPYFLAGS := -j .text -j .data -O ihex
AVR_FLASHFLAGS := -c arduino -b 115200 -p atmega328p -D
ESP_FLASHFLAGS := --chip esp32c3 --baud 921600 

# Targets
$(ioexpobj): $(BLD_DIR)/%.o : %.c
	@printf '$(bold)[CC]$(sgr0)$(patsubst %.o, %.c, $(@F)) -> $(@F)\n' && \
	mkdir -p $(dir $@) && \
	$(CC_AVR) $(AVR_CCFLAGS) $(patsubst $(BLD_DIR)/%.o, %.c, $@) -o $@

deps:
	@printf '$(bold)Downloading NUTTX:$(sgr0)\n' && \
	git clone $(NUTTX_REPO) $(NUTTX_DIR)
	@printf '$(bold)Downloading NUTTX apps:$(sgr0)\n' && \
	git clone $(NUTTX_APPS) $(NUTTX_APP_DIR)
	@printf '$(bold)Downloading bootloader and partition table$(sgr0)\n' && \
	mkdir -p $(BIN_DIR) && \
	curl $(NUTTX_BOOTLADER) -o $(BIN_DIR)/bootloader.bin && \
	curl $(NUTTX_PARTITION) -o $(BIN_DIR)/partition-table.bin
	

firmware: $(ioexpobj)
	@printf '$(bold)[LD]$(sgr0) ioexp-fw.elf\n' && \
	mkdir -p $(BLD_DIR) && \
	$(LD_AVR) $(AVR_LDFLAGS) $(ioexpobj) -o $(BLD_DIR)/ioexp-fw.elf
	@printf '$(bold)[OBJCPY]$(sgr0) ioexp-fw.elf -> ioexp-fw.bin\n' && \
	mkdir -p $(BIN_DIR) && \
	$(OBJCPY_AVR) $(AVR_OBJCPYFLAGS) $(BLD_DIR)/ioexp-fw.elf $(BIN_DIR)/ioexp-fw.bin
	@printf '$(bold)Use \"make flash-fw\" to upload the image to the arduino\n'

.PHONY: nuttx
nuttx:
	@printf '$(bold)Updating nxcontrol app:$(sgr0)\n' && \
	rm -rf $(NUTTX_APP_DIR)/nxcontrol && \
	cp -r nxcontrol $(NUTTX_APP_DIR)/ 
	@printf '$(bold)Creating nuttx config:$(sgr0)\n' && \
	./$(NUTTX_DIR)/tools/configure.sh esp32c3-devkit:usbconsole && \
	rm -f $(NUTTX_DIR)/.config && \
	cp -v .config $(NUTTX_DIR)/
	@printf '$(bold)Building NUTTX:$(sgr0)\n' && \
	$(MAKE) -C $(NUTTX_DIR)
	@printf '$(bold)Use \"make flash-nuttx\" to upload the image to the ESP32\n' && \
	mkdir -p $(BIN_DIR) && \
	cp $(NUTTX_DIR)/nuttx.bin $(BIN_DIR)/

.PHONY: flash-nuttx
flash-nuttx:
	@[ "$(ESP_FLASH_PORT)" ] || ( printf '$(bold)Port not provided! Please provide it via \"ESP_FLASH_PORT\"\n$(sgr0)'; exit 1 )
	@printf '$(bold)Flashing nuttx to $(ESP_FLASH_PORT)$(sgr0)\n' && \
	esptool.py $(ESP_FLASHFLAGS) --port $(ESP_FLASH_PORT) write_flash 0x0 $(BIN_DIR)/bootloader.bin 0x8000 $(BIN_DIR)/partition-table.bin 0x10000 $(BIN_DIR)/nuttx.bin

.PHONY: flash-fw
flash-fw:
	@[ "$(AVR_FLASH_PORT)" ] || ( printf '$(bold)Port not provided! Please provide it via \"AVR_FLASH_PORT\"\n$(sgr0)'; exit 1 )
	@printf '$(bold)Flashing ioexp firmware to $(ESP_FLASH_PORT)$(sgr0)\n'
	$(FLASH_AVR) $(AVR_FLASHFLAGS) -P $(AVR_FLASH_PORT) -U flash:w:$(BIN_DIR)/ioexp-fw.bin

.PHONY: docs
docs:
	@stat $(DOCS_DIR)/index.html >/dev/null 2>/dev/null || ( printf '$(bold)Generating documentation$(sgr0)\n' && DOXYGEN_QUIET=YES doxygen )
	@printf '$(bold)Opening documentation$(sgr0)\n' && \
	xdg-open $(DOCS_DIR)/index.html

.PHONY: clean
clean:
	@printf '$(bold)Removing build files$(sgr0)\n' && \
	rm -rfv $(BLD_DIR) && \
	$(MAKE) clean -C $(NUTTX_DIR)

.PHONY: cleanall
cleanall: clean
	@printf '$(bold)Cleaning the project$(sgr0)\n' && \
	rm -rf $(BIN_DIR) && \
	rm -rf $(DOCS_DIR) && \
	rm -rf $(NUTTX_DIR) && \
	rm -rf $(NUTTX_APP_DIR)