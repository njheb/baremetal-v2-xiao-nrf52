/* x3300/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "lib.h"
#include "accel.h"
#include "PCF8563.h"

/* light -- show one pixel */
void light(int x, int y)
{
//    image screen;
//    image_clear(screen);
//    image_set(x, y, screen);
//    display_show(screen);
}

/* scale -- map acceleration to coordinate */
static int scale(int x)
{
    if (x < -20) return 4;
    if (x < -10) return 3;
    if (x <= 10) return 2;
    if (x <= 20) return 1;
    return 0;
}

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

//        accel_reading(&x, &y, &z);
//        printf("x=%d y=%d z=%d\n", x, y, z);
//        x = scale(x); y = scale(y);
//        light(x, y);
    }
}

void pong(int n)
{
	while (1) {
		led_pwr_on();
		timer_delay(750);
		led_pwr_off();
		timer_delay(750);
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

    start("Pong", pong, 0, STACK);
    start("Main", main, 0, STACK);
}
