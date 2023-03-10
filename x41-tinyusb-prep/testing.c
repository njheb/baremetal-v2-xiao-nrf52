/* x3300/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
//#include "lib.h"
//#include "accel.h"
#include "PCF8563.h"

//#include "bsp/board.h"
#include "tusb.h" //set path ? in Makefile or locally for now
//going to need lots of includes so add to Makefile
//take a look at linker script 

//------------- prototypes -------------//
static void cdc_task(void);
#define BUTTON_A  DEVPIN(0, 3)

extern void usb_init(void); 

static void i2c_map(void)
{
    static char *hex = "0123456789abcdef";

    printf("I2C bus map:\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    for (int a = 0; a < 8; a++) {
        printf("%c0:", hex[a]);
        for (int b = 0; b < 16; b++) {
            int addr = (a<<4) + b;
            if (addr < 0x08 || addr >= 0x78)
                /* Reserved addresses */
                printf("   ");
            else {
                int status = i2c_probe(I2C_EXTERNAL, addr);
                if (status == OK)
                    printf(" %c%c", hex[a], hex[b]);
                else
                    printf(" --");
            }
        }
        printf("\n");
    }
}

/* main_task -- show the spirit level */
static void main(int n)
{
    int x, y, z;

    printf("Hello\n\n");
    i2c_map();
    printf("\n");
    timer_delay(1000);
//    accel_start();
  PCF8563__init();//initialize the clock
  PCF8563__stopClock();//stop the clock
  PCF8563__setYear(20);//set year
  PCF8563__setMonth(10);//set month
  PCF8563__setDay(23);//set dat
  PCF8563__setHour(17);//set hour
  PCF8563__setMinut(33);//set minut
  PCF8563__setSecond(0);//set second
  PCF8563__startClock();//start the clock

    struct PCF8563_Time nT;

    while (1) {
	    led_neo(GREEN);
        timer_delay(500);

	    PCF8563__getTime(&nT);//get current time

	    led_neo(BLUE);
        timer_delay(500);

        printf("%d/%d/20%d %d:%d:%d\n", nT.day, nT.month, nT.year, nT.hour, nT.minute, nT.second);
    }
}
/* reflect button state on power led and drive hacked together  tinyusb stack */
static void expt(int n)
{
    usb_init(); //call to what was tinyusb board_init before it was hacked
    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    while (1)
    {
	timer_delay(40);
//	board_led_write(board_button_read());
        if (gpio_in(BUTTON_A) == 0)
	{
	   led_pwr_on();
	   led_neo(GREEN);
	}
	else
	{
	   led_pwr_off();
	   led_neo(BLUE);
	}

	tud_task(); // tinyusb device task
	cdc_task();
    }
}

// echo to either Serial0 or Serial1
static void echo_serial_port(unsigned char itf, unsigned char buf[], unsigned int count)
{
//  uint8_t const case_diff = 'a' - 'A';


  for(unsigned int i=0; i<count; i++)
  {
/*
    if (itf == 0)
    {
      // echo back 1st port as lower case
      if (isupper(buf[i])) buf[i] += case_diff;
    }
    else
    {
      // echo back 2nd port as upper case
      if (islower(buf[i])) buf[i] -= case_diff;
    }
*/
    tud_cdc_n_write_char(itf, buf[i]);
  }
  tud_cdc_n_write_flush(itf);
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static unsigned char buf[64];
static void cdc_task(void)
{
  unsigned char itf;

  for (itf = 0; itf < CFG_TUD_CDC; itf++)
  {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    if ( tud_cdc_n_connected(itf) )
    {
      if ( tud_cdc_n_available(itf) )
      {
//        unsigned char buf[64];

        unsigned int count = tud_cdc_n_read(itf, buf, sizeof(buf));

        // echo back to both serial ports
        //echo_serial_port(0, buf, count);
        echo_serial_port(1, buf, count);
      }
    }
  }
}


void init(void)
{
    serial_init();
    timer_init();
    i2c_init(I2C_EXTERNAL);
    led_init();

    led_neo(WHITE);
//    display_init();

    gpio_connect(BUTTON_A);
    gpio_pull(BUTTON_A, GPIO_PULL_Pullup);


    start("Dual", expt,  0, STACK);
//    start("Main", main, 0, STACK);
}
