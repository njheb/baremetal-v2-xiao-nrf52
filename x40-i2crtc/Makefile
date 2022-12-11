# x18/Makefile
# Copyright (c) 2020-21 J. M. Spivey

APPNAME = i2crtc

all: $(APPNAME).hex

CC = arm-none-eabi-gcc
CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
INCLUDE = -I ../microbian
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

vpath %.h ../microbian

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.elf: %.o accel.o PCF8563.o ../microbian/microbian.a ../microbian/startup.o
	$(CC) $(CPU) $(CFLAGS) -T ../microbian/XIAOnRF52840.ld \
		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
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
