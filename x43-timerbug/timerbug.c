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
	unsigned u0 = TIMER0.START;
	unsigned v0 = TIMER0.STOP;
	unsigned c0 = TIMER0.CC[0];
	unsigned u1 = TIMER1.START;
	unsigned v1 = TIMER1.STOP;
	unsigned c1 = TIMER1.CC[0];
	unsigned u2 = TIMER2.START;
	unsigned v2 = TIMER2.STOP;
	unsigned c2 = TIMER2.CC[0];
	unsigned u3 = TIMER3.START;
	unsigned v3 = TIMER3.STOP;
	unsigned c3 = TIMER3.CC[0];
	unsigned u4 = TIMER4.START;
	unsigned v4 = TIMER4.STOP;
	unsigned c4 = TIMER4.CC[0];

        int count = 0;
	int press = 0;
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
				press = 0;
	                        led_pwr_on();           //add red on button press
		printf("0:%d, %d, %d\n", c0, u0, v0);
		printf("1:%d, %d, %d\n", c1, u1, v1);
		printf("2:%d, %d, %d\n", c2, u2, v2); //TIMER2.CC[0]==1538
		printf("3:%d, %d, %d\n", c3, u3, v3);
		printf("4:%d, %d, %d\n", c4, u4, v4);
		printf("0:%d\n", TIMER0.PRESCALER);
		printf("1:%d\n", TIMER1.PRESCALER);
		printf("2:%d\n", TIMER2.PRESCALER);
		printf("3:%d\n", TIMER3.PRESCALER);
		printf("4:%d\n", TIMER4.PRESCALER);
		printf("0:%d\n", TIMER0.START); 
		printf("1:%d\n", TIMER1.START);
		printf("2:%d\n", TIMER2.START);
		printf("3:%d\n", TIMER3.START);
		printf("4:%d\n", TIMER4.START);
			}
	       }
                else
			{
			press = 0;
                        led_pwr_off();
			}
//		yield();
//		led_neo(BLACK);
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
