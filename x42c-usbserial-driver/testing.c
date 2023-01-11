/* x3300/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "lib.h"
#include "accel.h"
#include "PCF8563.h"
#include <string.h>
//#define TASKS

//#include "bsp/board.h"
//going to need lots of includes so add to Makefile
//take a look at linker script 
/*
extern int TUD_TASK;
extern int USBSERIAL_TASK0;
extern int USBSERIAL_TASK1;

extern void tud_runner(int n);
extern void usb_serial(int n);
*/
extern void usb_cdc_dual_init(void);
extern void usbprint1_buf(char *buf, int n);
extern void usbserial1_putc(char ch);
extern int usbserial1_getc(void);

//------------- prototypes -------------//
//static void cdc_task(void);
#define BUTTON_A  DEVPIN(0, 3)


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
static void maintask(int n)
{
   static char buffer[80];

    int x, y, z;
    int count = 0;

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
	//    led_neo(GREEN);
        timer_delay(50);

	    PCF8563__getTime(&nT);//get current time

	//    led_neo(BLUE);
        //timer_delay(50);

        if (count++ % 20 == 0)
        {
           sprintf(buffer, "%d/%d/20%d %d:%d:%d \n", nT.day, nT.month, nT.year, nT.hour, nT.minute, nT.second);
           int i=strlen(buffer);//0;
        //for ( ; buffer[i] != '\0'; i++);

            //usbserial1_putc(buffer[i]);
           usbprint1_buf(buffer, i); //do a strlen later
           usbserial1_putc('\r');
        }
     }
}

//initially non blocking, setting up to test with blocking
static void echotask(int n)
{
    while (1)
    {
//       timer_delay(20);
       yield();
       int k=usbserial1_getc();
       if ( k != -1 )
       {
          if (k == '\r')
             usbserial1_putc('\n');
          usbserial1_putc((char)k);
       }
    }

}


static unsigned char buf[64];

void init(void)
{
//    void (*workaroundlinkage)(void); //did not help ; __attribute__((__used__));
//    __asm__ __volatile__("" :: "m" (workaroundlinkage));
//    workaroundlinkage = &POWER_CLOCK_IRQHandler;
    serial_init();
    timer_init();
    i2c_init(I2C_EXTERNAL);
    led_init();

    led_neo(WHITE);
//    display_init();

    gpio_connect(BUTTON_A);
    gpio_pull(BUTTON_A, GPIO_PULL_Pullup);

//leave outside force on so linkage can pick up ISRs
// call with usb_init() caused HardFault
    //just return and do no work in usb_init();
#if 0
    TUD_TASK = start("tud", tud_runner,  0, STACK); //remember to tune stacksize
    USBSERIAL_TASK0 = start("Port0", usb_serial,  0, STACK);
    USBSERIAL_TASK1 = start("Port1", usb_serial,  1, STACK);
#else
    usb_cdc_dual_init();
#endif 
    start("Echo", echotask, 0, STACK);
    start("Main", maintask, 0, STACK);
}
