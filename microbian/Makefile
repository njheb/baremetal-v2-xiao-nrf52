# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

all: microbian.a startup.o usb-startup.o usb-serial.o

TUSBFLAGS = -DNRF52840_XXAA -DCFG_TUSB_MCU=OPT_MCU_NRF5X

TINYUSBPATH = ../../tinyusb/examples/device/cdc_XIAO_donor

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
INCLUDE = -I ../../tinyusb/src -I $(TINYUSBPATH)/src
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

#DRIVERS = timer.o serial.o i2c.o radio.o display.o
#DRIVERS = timer.o serial.o i2c.o radio.o $(TINYUSBPATH)/tinyusb-cdc-lib.a usb-serial.o
DRIVERS = timer.o serial.o i2c.o radio.o

MICROBIAN = microbian.o mpx-m4.o $(DRIVERS) lib.o

microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

usb-startup.o: startup.c
	$(CC) $(CPU) $(CFLAGS) -DTINYUSBEXPT -c $< -o $@


usb-serial.o: usb-serial.c
	$(CC) $(CPU) $(CFLAGS) $(TUSBFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

clean: force
	rm -f microbian.a *.o

force:

###

$(MICROBIAN) startup.o: microbian.h hardware.h lib.h
