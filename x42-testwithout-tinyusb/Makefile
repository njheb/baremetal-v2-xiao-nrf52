# x18/Makefile
# Copyright (c) 2020-21 J. M. Spivey

APPNAME = testwithoutusb

all: $(APPNAME).hex

CC = arm-none-eabi-gcc
CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding -DCFG_TUSB_MCU=OPT_MCU_NRF5X
INCLUDE = -I ../microbian -I ../../tinyusb/src -I ../../tinyusb/examples/device/cdc_XIAO_donor/src
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

vpath %.h ../microbian

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.elf: %.o accel.o PCF8563.o ./tinyusb-cdc-lib.a ../microbian/microbian.a ../microbian/startup.o
#	$(CC) $(CPU) $(CFLAGS) -T ../microbian/XIAOnRF52840.ld \
#		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
#possible linker error
#	$(CC) $(CPU) $(CFLAGS) -T ./XIAOnRF52840.ld \
#		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
#	$(CC) $(CPU) $(CFLAGS) \
#		-T ./microbian-XIAO-tinyusb-nrf52840_s140_v7.ld \
#		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
	$(CC) $(CPU) $(CFLAGS) \
		-T ./microbian-XIAO-tinyusb-nrf52840_s140_v7.ld \
		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map -Wl,-cref -Wl,-gc-sections -specs=nosys.specs -specs=nano.specs
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.dfu.zip: %.hex
	adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application $< $@

../microbian/microbian.a:
	$(MAKE) -C $(@D) all

# Nuke the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

upload: $(APPNAME).dfu.zip
	adafruit-nrfutil --verbose dfu serial -pkg $(APPNAME).dfu.zip -p /dev/ttyACM0 -b 115200 --singlebank
	@echo "Programmed"

clean:
	rm -f *.hex *.elf *.bin *.map *.o $(APPNAME).dfu.zip

# Don't delete intermediate files
.SECONDARY:

###

level.o: accel.h hardware.h lib.h microbian.h
accel.o: accel.h microbian.h
PCF8563.o: PCF8563.h microbian.h
