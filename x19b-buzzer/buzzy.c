/* x3500/buggy.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "pwm.h"
#define BUTTON_A DEVPIN(0, 3)

/* control_task -- buzz on button A pressed */
void control_task(int dummy)
{
    timer_delay(1000);
    while (1) {
	if (gpio_in(BUTTON_A) == 0)
	    pwm_change(1250); //should be 50% duty at 400Hz
	else
	    pwm_change(0); //should be off
	timer_delay(1000);
    }
}

void init(void)
{
    gpio_connect(BUTTON_A);
//    gpio_connect(BUTTON_B);
    gpio_pull(BUTTON_A, GPIO_PULL_Pullup);

    led_init();
    led_neo(WHITE);


    timer_init();
    pwm_init();
    start("Control", control_task, 0, STACK);
}
