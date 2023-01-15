#!/bin/bash
if [ -d ../../tinyusb ]; then
 echo "Abort: tinyusb directory already exists"
exit 1
fi
TOP=$(realpath ../..) 
#echo $TOP
pushd "$TOP"
git clone https://github.com/hathach/tinyusb.git
cd tinyusb
git checkout -b pre-xiao_nrf52840-base 9e91b02
#patch for hacking xiao_nrf board into tinyusb
git branch xiao_nrf52840-added
git checkout xiao_nrf52840-added
git apply ../baremetal-v2-xiao-nrf52/setup-tinyusb/xiao_nrf52840-added.patch
git add  hw/bsp/nrf/boards/xiao_nrf52840/
git commit -a -m "test additions by compiling cdc_dual_ports example
    cd baremetal/tinyusb/examples/device/cdc_dual_ports 'make BOARD=xiao_nrf52840 all'
    had to do 'pip3 install --user adafruit-nrfutil' and for some reason did not
    want to flash with 'make BOARD=xiao_nrf5280 SERIAL=/dev/ttyACM0 flash'
    HW serial not tested USB serial tested with 'minicom -D /dev/ttyACM1'"
#patch for changes to allow creation of tinyusb-cdc-lib.a
git branch XIAO-HACK-v0.14.0
git checkout XIAO-HACK-v0.14.0
git apply ../baremetal-v2-xiao-nrf52/setup-tinyusb/tinyusb-cdc-lib.a.patch
git add examples/device/cdc_XIAO_donor/
git commit -a -m "keep tinyusb uart incase it's needed for debug but move from UART0 to UART1
    move UART1 to unused pins as microbian kprintf and serial driver will be using UART0
    This time keep board_init() intact for reference.
    cd cdc_XIAO_donor
    make tinyusb-cdc-lib"
#TODO leave the option to use a pre-existing arduino provided CMSIS_5, untested compatability
echo "*******************************************************************************"
echo "**** CMSIS_5 is very large so expect to wait a while on slower download links."
echo "**** If you want to rebuild it from source use the following"
echo "****"
echo "**** TYPE: git -C "$TOP"/tinyusb submodule update --init lib/CMSIS_5 hw/mcu/nordic/nrfx"
echo "****"
echo "**** from cdc_XIAO_donor 'make tinyusb-cdc-lib' if building from source"
echo "*******************************************************************************"
popd
cp tinyusb-cdc-lib.a ../../tinyusb/examples/device/cdc_XIAO_donor
