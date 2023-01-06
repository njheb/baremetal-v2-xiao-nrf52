/* Based on level example, exercise main builtin seeed expansion board devices
   button
   rgb leds
   buzzer
   rtc i2c chip PCF8563
   oled ssd1306
   serial TX
*/
/* x3300/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "lib.h"
#include "accel.h"
#include "PCF8563.h"
#include "ssd1306.h"
#include "pwm.h"

#define BUTTON_A  DEVPIN(0, 3)

static int OLED_TARGET;

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

static int xp;
static int yp;
static int oldxp;
static int oldyp;
static int xv;
static int yv;

static void init_pingpong_vars(void)
{
    xp = 12;
    yp = 4;
    xv = +1;
    yv = +1;
    oldxp = xp;
    oldyp = yp;
}

static void draw_pingpong(byte x, byte y, char ch)
{
    int status;
    status = ssd1306_set_position(x*5, y);
    status = ssd1306_draw_character(ch);
}

static void pong(int n)
{
        int first_press = 0;
        int count = 0;
	int press = 0;
        int oldbeep = 0;
        int beep = 0;
        int toggle_sound = 1; /*1 is sound on*/
        int maxy = 6;

    init_pingpong_vars();
    receive(OLED_READY, NULL);

    ssd1306_clear_screen();
    ssd1306_set_position(0,7);
    ssd1306_draw_string("Button sets sound 1/0");

	while(1){
	       timer_delay(40);
	       count++;
               if ((count % 50) == 0)          //0.5 seconds green
                  led_neo(GREEN);
               else if ((count % 50) == 25)    //0.5 seconds blue
                  led_neo(BLUE);


               if ((first_press == 1) && ((count % 10) == 0))
               {
                 char buffer[24]="";
		 short count = adc_reading(A0);
                 int decidegree = 3000*count/(680-count);
                 sprintf(buffer, "c=%d %d    ", count, decidegree);
                 ssd1306_set_position(0, 7);
                 ssd1306_draw_string(buffer);
               }

               if (gpio_in(BUTTON_A) == 0)
	       {
			press++;
			if (press>=4)
			{

                            if (first_press == 1)
                            {
                                first_press = 2;
                                ssd1306_set_position(0,7);
                                ssd1306_draw_string("                     ");
			        maxy = 7;
                            }

                            if (first_press == 0)
                            {
                                first_press = 1;
                                ssd1306_set_position(0,7);
                                ssd1306_draw_string("                     ");
			        maxy = 6;
                            }

                            /*blank out ball and re init*/
                            draw_pingpong(xp, yp, ' ');
                            init_pingpong_vars();
                            press = 0;
                            toggle_sound = !toggle_sound;
			}
	       }
               else
	       {
			press = 0;
		}

               oldbeep = beep;
               beep = 0;
               oldxp = xp, oldyp = yp;
               xp += xv, yp += yv;
               if (xp<=0)     xv = -xv, xp = 0,    beep=1;
               if (xp>=24)    xv = -xv, xp = 24,   beep=1;
               if (yp<=0)     yv = -yv, yp = 0,    beep=1;
               if (yp>=maxy)  yv = -yv, yp = maxy, beep=1;

               if ((oldxp != xp) || (oldyp != yp))
               {
                   draw_pingpong(oldxp, oldyp, ' ');
                   draw_pingpong(xp, yp, 'O');
               }

               if ((beep == 1) && (toggle_sound == 1)) pwm_change(1250);
               if ((oldbeep == 1) && (beep == 0)) pwm_change(0);

	}
}

/* main_task -- show the spirit level */
static void main(int n)
{

    printf("Hello\n\n");
    i2c_map();
    printf("\n");
    timer_delay(1000);

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
        timer_delay(60000);

	  PCF8563__getTime(&nT); //get current time

          printf("%d/%d/20%d %d:%d:%d\n", nT.day, nT.month, nT.year, nT.hour, nT.minute, nT.second);
    }
}

void init(void)
{
    serial_init();
    timer_init();

    pwm_init();
    adc_init();

    i2c_init(I2C_EXTERNAL);
    led_init();
    led_neo(WHITE);
//    display_init();

    gpio_connect(BUTTON_A);
    gpio_pull(BUTTON_A, GPIO_PULL_Pullup);

    OLED_TARGET = start("Pong", pong, 0, STACK);
    ssd1306_init(OLED_TARGET);

    start("Main", main, 0, STACK);
}
