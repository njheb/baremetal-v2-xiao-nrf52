/* x3300/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "lib.h"
#include "accel.h"
#include "PCF8563.h"

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


static void pong(int n)
{


        int count = 0;
	int press = 0;
   byte ch = ' ';

    ssd1306_init();
    ssd1306_clear_screenX();

    ssd1306_set_position(20,4);
    ssd1306_draw_string("Hello World!");


	while(1){
	       timer_delay(40);
	       count++;
               if ((count % 50) == 0)          //0.5 seconds green
                  led_neo(GREEN);
               else if ((count % 50) == 25)    //0.5 seconds blue
                  led_neo(BLUE);

               if (gpio_in(BUTTON_A) == 0)
	       {
			press++;
			if (press>=4)
			{
			    int status = ssd1306_draw_character(ch);
                            if (status != OK) {
				ssd1306_set_position(0, 0);
                                ssd1306_clear_screen();
                            }
			    ch++;
			    if (ch > '~') ch = ' '; 

				press = 0;
	                        led_pwr_on();           //add red on button press
			}
	       }
                else
			{
			press = 0;
                        led_pwr_off();
			}
	}
}
//all other values above are reported as 0
//all PRESCALERS == 4
//all START == 0, but don't know if readable


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
//	    yield();
//	    led_neo(GREEN);
        timer_delay(60000);

	    PCF8563__getTime(&nT);//get current time

//	    led_neo(BLUE);
//        timer_delay(500);

           printf("%d/%d/20%d %d:%d:%d\n", nT.day, nT.month, nT.year, nT.hour, nT.minute, nT.second);
//	    led_neo(RED);

//        accel_reading(&x, &y, &z);
//        printf("x=%d y=%d z=%d\n", x, y, z);
//        x = scale(x); y = scale(y);
//        light(x, y);
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


    start("Pong", pong, 0, STACK);
    start("Main", main, 0, STACK);
}
