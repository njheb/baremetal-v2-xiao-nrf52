# <u>Microbian port to XIAO nRF52840 with tinyusb cdc</u>
**For founding material look at original.README.md**

#install tinyusb repo and patches with:

"cd baremetal/baremetal-v2-xiao-nrf52/setup-tinyusb/"

"./install-tinyusb-cdc-lib.a.sh"

"../../tinyusb/examples/device/cdc_XIAO_donor/make tinyusb-cdc-lib"

tinyusb will be parallel with baremetal-v2-xiao-nrf2

#Hardware

I've been working on the cheaper Seeed Studio XIAO nRF52840 without 
IMU and mic. The expansion board has SWD and an easier to use reset button.

Seeed Studio XIAO nRF52840 (Sense)
[https://wiki.seeedstudio.com/XIAO_BLE/](URL) has an IMU and microphone

Using Seeed XIAO Expansion board 
[https://wiki.seeedstudio.com/Seeeduino-XIAO-Expansion-Board/
](URL)

There is also a SAMD21 version [https://wiki.seeedstudio.com/Seeeduino-XIAO/](URL)
which can be bought with headers pre-soldered. 
It lacks a reset button so the expansion board is pretty much required 
although pads are exposed for shorting reset in place of a button.

*I thought it was of interest because of my unfinished Adafruit trinket USB 
microbian port which could benefit from a better approach to adding cdc. 
And could provide a route for adding Seeed SAMD21 to microbian.*

# <u> Folders I remember touching or have started.</u>

 * x19b-buzzer

 * x20-radio - bytes are rx'd and tx'd but it's not fluid, not fully investigated

 * x31-adc-battery - hang over from earlier hacking, need fix blocking

 * x40-i2crtc - need to clean up api rapid conversion from c++ with search and replace

 * x41-tinyusb-prep pre usb-startup.o

 * x42b-fromx41 pre usb-startup.o

 * x42c-usbserial-driver - "minicom -D /dev/ttyACM1" cat file.txt > /dev/ttyACM1

 * x42-testwithout-tinyusb -  checking for stability since getting hardfaults

 * x43-timerbug - checking for hardfaults, don't have a DAPLINK yet, potential problem 
   inherited from bootloader as Seeed bootloader source closed can't tell if fixed.

 * x44-oled - I2C oled driver outside of microbian

 * x45-pingpong - oled bouncing ball with buzzer on rebound


# TODO:
tinyusb-cdc-lib.a needs a clean

Would blocking usbserial rx be desirable? Currently returns (int)-1 
for not available. Would fprintf be a good idea?

investigate radio - have not read datasheet for differences if any from UBITv2

Drivers to make:

PWM

SPIM on SPI2

SPIM on SPI2 with DMA

QSPI with DMA

microphone (Sense)

IMU (Sense)


**Probably never:**
SOFTDEVICE would require usb priority going back down from 0 to 2 and
adding semaphore primitive to support that if i've understood things correctly.
I'm getting away with -DCFG_TUSB_OS=OPT_OS_NONE and using NVIC priority 0 
for USB.

#<u>Programming</u>


I've not tried the UF2 interface.

For DFU serial. Install  Adafruit_nRF52_nrfutil
https://github.com/adafruit/Adafruit_nRF52_nrfutil

or  pip3 install --user adafruit-nrfutil

**Double reset enters DFU bootloader.**

I used the toolchain that came with Arduino 1.8.19

Navigate to File > Preferences, and fill "Additional Boards Manager URLs" with 
the url below:
 
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json

Then Navigate to Tools > Board > Boards Manager...,
 
type the keyword "seeed nrf52" in the search box,
 
select the latest version of the board you want, and install it. 
You can install both. The mbed one is much more mature.

Used Seeed arduino ide and board package for hints and the tools chain. 

Right now my microbian port is a bit untity but it works. 
It's untidier than it should be because of getting tinyusb-cdc-lib.a going, 
but not polished in anyway it's just proof of concept.

I have yet to learn the tinyusb API properly.

