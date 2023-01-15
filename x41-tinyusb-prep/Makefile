# x18/Makefile
# Copyright (c) 2020-21 J. M. Spivey

APPNAME = testing

all: $(APPNAME).hex

CC = arm-none-eabi-gcc
CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding -DCFG_TUSB_MCU=OPT_MCU_NRF5X
INCLUDE = -I ../microbian -I ../../tinyusb/src -I ../../tinyusb/examples/device/cdc_XIAO_donor/src
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy



X41LIBMICROBIANLDFLAGS = -L/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx/mdk \
	-DNRF52840_XXAA -flto --use-linker-plugin -mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DCFG_TUSB_MCU=OPT_MCU_NRF5X -DCONFIG_GPIO_AS_PINRESET \
	-Wno-error=undef -Wno-error=unused-parameter -Wno-error=cast-align -Wno-error=cast-qual -ggdb -fdata-sections -ffunction-sections -fsingle-precision-constant -fno-strict-aliasing \
	-Wall -Wextra -Werror -Wfatal-errors -Wdouble-promotion -Wstrict-prototypes -Wstrict-overflow -Werror-implicit-function-declaration -Wfloat-equal -Wundef -Wshadow -Wwrite-strings \
	-Wsign-compare -Wmissing-format-attribute -Wunreachable-code -Wcast-align -Wcast-function-type -Wcast-qual -Wnull-dereference -Wuninitialized -Wunused -Wredundant-decls -Os \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/bsp/nrf/boards/xiao_nrf52840 \
	-I/home/nick/work/xiao/baremetal/tinyusb/lib/CMSIS_5/CMSIS/Core/Include \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx/mdk \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx/hal \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx/drivers/include \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/mcu/nordic/nrfx/drivers/src \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw/bsp/nrf \
	-Isrc \
	-I/home/nick/work/xiao/baremetal/tinyusb/hw \
	-I/home/nick/work/xiao/baremetal/tinyusb/src \
	-Wl,-T./microbian-XIAO-tinyusb-nrf52840_s140_v7.ld \
	-Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nosys.specs -specs=nano.specs


vpath %.h ../microbian

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

#USE local tinyusb-cdc-lib.a for  now
#LINKER file needs looking at
#ADD INCLUDES for tinyusb-cdc-lib.a
#%.elf: %.o accel.o PCF8563.o ../tinyusb-lib/tinyusb-lib.a ../microbian/microbian.a ../microbian/startup.o
%.elf: %.o PCF8563.o ../../tinyusb/examples/device/cdc_XIAO_donor/tinyusb-cdc-lib.a ../microbian/microbian.a ../microbian/usb-startup.o
#	$(CC) $(CPU) $(CFLAGS) -T ../microbian/XIAOnRF52840.ld \
#		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
#	$(CC) $(CPU) $(CFLAGS) -T ./microbian-XIAO-tinyusb-nrf52840_s140_v7.ld \
#		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
	$(CC) $(CPU) $(CFLAGS) -T ./microbian-XIAO-tinyusb-nrf52840_s140_v7.ld \
		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map -Wl,-cref -Wl,-gc-sections -specs=nosys.specs -specs=nano.specs

#	$(CC) -o $@ \
#	$(X41MICROBIANLDFLAGS) \
#	$^ \
#	-Wl,--start-group -lgcc -lm -lnosys -lc -Wl,--end-group
#	arm-none-eabi-objdump -d $@ > disassembly.txt
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
