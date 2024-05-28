# Programs
CC_AVR := avr-gcc
LD_AVR := avr-gcc
OBJCPY_AVR := avr-objcopy

# AVR Bridge
FW_PORT := /dev/ttyUSB0
FW_BOARD := avr5

# Nuttx
NUTTX_REPO := https://github.com/apache/nuttx.git 
NUTTX_APPS := https://github.com/apache/nuttx-apps.git

# Directories
NUTTX_DIR := nuttx
NUTTX_APP_DIR := apps
BLD_DIR := .build

# Styling
bold := $(shell tput bold)
sgr0 := $(shell tput sgr0)

# Sources
ioexpsrc := $(shell find ioexp -name '*.c')
ioexpobj := $(patsubst %.c, $(BLD_DIR)/%.o, $(ioexpsrc))

# Flags
AVR_CCFLAGS := -mmcu=atmega328p -c -pipe -Wall -pedantic -O2
AVR_OBJCPYFLAGS := -j .text -j .data -O ihex

# Targets
$(ioexpobj): $(BLD_DIR)/%.o : %.c
	@printf '$(bold)[CC]$(sgr0)$(patsubst %.o, %.c, $(@F)) -> $(@F)\n' && \
	mkdir -p $(dir $@) && \
	$(CC_AVR) $(AVR_CCFLAGS) $(patsubst $(BLD_DIR)/%.o, %.c, $@) -o $@

.PHONY: nuttx, flash-nuttx, flash-firmware, clean, cleanall

deps:
	@printf '$(bold)Downloading NUTTX:$(sgr0)\n' && \
	git clone $(NUTTX_REPO) $(NUTTX_DIR)
	@printf '$(bold)Downloading NUTTX apps:$(sgr0)\n' && \
	git clone $(NUTTX_APPS) $(NUTTX_APP_DIR)

firmware: $(ioexpobj)
	@printf '$(bold)[LD]$(sgr0) ioexp-fw.elf\n' && \
	mkdir -p $(BLD_DIR) && \
	$(LD_AVR) $(ioexpobj) -o $(BLD_DIR)/ioexp-fw.elf
	@printf '$(bold)[OBJCPY]$(sgr0) ioexp-fw.elf -> ioexp-fw.bin\n' && \
	mkdir -p $(BLD_DIR) && \
	$(OBJCPY_AVR) $(AVR_OBJCPYFLAGS) $(BLD_DIR)/ioexp-fw.elf $(BLD_DIR)/ioexp-fw.bin
	@printf "$(bold)Use \"make flash-fw\" to upload the image to the arduino\n"

nuttx:
	@printf "$(bold)Updating app$(sgr0)\n" && \
	rm -rf $(NUTTX_APP_DIR)/nxcontrol && \
	cp -r nxcontrol $(NUTTX_APP_DIR)/ 
	@printf "$(bold)Creating config$(sgr0)\n" && \
	./$(NUTTX_DIR)/tools/configure.sh esp32c3-devkit:usbconsole && \
	rm -f $(NUTTX_DIR)/.config && \
	cp -v .config $(NUTTX_DIR)/
	@printf "$(bold)Building NUTTX$(sgr0)\n" && \
	$(MAKE) -C $(NUTTX_DIR)
	@printf "$(bold)Use \"make flash-nuttx\" to upload the image to the ESP32\n"

flash-nuttx:
	@[ "$(ESP_FLASH_PORT)" ] || ( printf "$(bold)Port not provided! Please provide it via \"ESP_FLASH_PORT\"\n$(sgr0)"; exit 1 )
	@printf "$(bold)Flashing nuttx to $(ESP_FLASH_PORT)$(sgr0)\n" && \
	$(MAKE) flash ESPTOOL_PORT=$(ESP_FLASH_PORT) -C $(NUTTX_DIR)

flash-fw:
	@[ "$(AVR_FLASH_PORT)" ] || ( printf "$(bold)Port not provided! Please provide it via \"AVR_FLASH_PORT\"\n$(sgr0)"; exit 1 )
	@printf "$(bold)Flashing ioexp firmware to $(ESP_FLASH_PORT)$(sgr0)\n" && \

clean:
	@printf '$(bold)Removing build files$(sgr0)\n' && \
	rm -rfv $(BLD_DIR) && \
	$(MAKE) clean -C $(NUTTX_DIR)

cleanall: clean
	@printf "$(bold)Removing nuttx$(sgr0)\n" && \
	rm -rf $(NUTTX_DIR) && \
	rm -rf $(NUTTX_APP_DIR)