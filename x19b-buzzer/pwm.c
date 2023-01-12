/* x3500/pwm.c */
/* Copyright (c) 2021 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"

/* Pulse width modulation.  We'll use timer 2 to generate one PWM
   signal, for the speaker.  It counts at 1MHz and resets
   once evey 2500 counts or 400Hz.

   The timer has four compare-and-capture channels.  We use channel 2
   to set the output signal high at the beginning of the period,
   and channels 0 to set it low again at the end of the pulse.

   Events on each counter channel are connected via the "Programmable
   Peripheral Interconnect" (PPI) system to GPIO Tasks in the GPIOTE
   module, and these tasks indirectly toggle the I/O pins. 

      Timer 2         PPI          GPIOTE        GPIO
      COMPARE       EEP TEP         OUT
         0 ----------> 2 -----+
                              +----> 0 --------> SPK
         2 ----------> 0 -----+


edited from the above stratergy
x suffix marks removals
      Timer 2         PPI          GPIOTE        GPIO
      COMPARE       EEP TEP         OUT
         0 ----------> 2 -----+
                              +----> 0 --------> PAD1
                +----> 0 -----+
         2 -----+
                +----> 1x-----+
                              +----> 1x--------> PAD2x
         1x----------> 3x-----+

*/

static int PWM_TASK;            /* Process ID */

#define WIDTH 23                /* Message type for pulse width change */

static void pwm_setup(void)
{
    /* Speaker output */
//#define SPEAKER BUZZER_D3
#define SPEAKER PAD3
    gpio_dir(SPEAKER, 1);

    /* GPIOTE channels 0 and 1 toggle the two pins */
    GPIOTE.CONFIG[0] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, SPEAKER)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);

    /* Set up timer 2 to count at 1MHz and reset at 20000 counts */
    TIMER2.STOP = 1;
    TIMER2.MODE = TIMER_MODE_Timer;
    TIMER2.BITMODE = TIMER_BITMODE_16Bit;
    TIMER2.PRESCALER = 4;       /* 1MHz=16MHz / 2^4 */
    TIMER2.CLEAR = 1;
    TIMER2.CC[0] = 65535;
//    TIMER2.CC[2] = 20000; //50Hz    with prescale 4;
//    TIMER2.CC[2] = 10000;   //100Hz
//    TIMER2.CC[2] =  5000;   //200Hz
    TIMER2.CC[2] =    2500;   //400Hz need to check with logic analyzer

    TIMER2.SHORTS = BIT(TIMER_COMPARE2_CLEAR);

    /* PPI channels 0 and 2 toggle SPKR */
    PPI.CH[0].EEP = &TIMER2.COMPARE[2];
    PPI.CH[0].TEP = &GPIOTE.OUT[0];
    PPI.CH[2].EEP = &TIMER2.COMPARE[0];
    PPI.CH[2].TEP = &GPIOTE.OUT[0];
    PPI.CHENSET = BIT(2);

    /* Start pwm */
    TIMER2.START = 1;
}

static volatile unsigned width;

/* timer2_handler -- special interrupt handler */
void timer2_handler(void)
{
    /* Set the CC registers to their new values */
    TIMER2.CC[0] = width;

    /* No more interrupts needed */
    TIMER2.INTENCLR = BIT(TIMER_INT_COMPARE2);

    /* Wake up the driver process */
    interrupt(PWM_TASK);
}

static void set_width(int wid)
{
    if (wid >= 5) {
        /* Genuine pulses: enable the start-of-pulse transition and set width */
        width = wid;
        PPI.CHENSET = BIT(0);
    } else {
        /* No pulses: disable pulse start, and set impossible width; */
        width = 65535;
        PPI.CHENCLR = BIT(0);
    }
}

static void pwm_task(int arg)
{
    message m;
    int client;

    pwm_setup();

    while (1) {
        receive(ANY, &m);
        switch (m.type) {
        case WIDTH:
            client = m.sender;
            set_width(m.int1);

            /* Enable an interrupt to update the timer */
            TIMER2.COMPARE[2] = 0;
            clear_pending(TIMER2_IRQ);
            TIMER2.INTENSET = BIT(TIMER_INT_COMPARE2);
            enable_irq(TIMER2_IRQ);
            receive(INTERRUPT, NULL);
            send(client, REPLY, NULL);
            break;

        default:
            badmesg(m.type);
        }
    }
}

void pwm_change(int width)
{
    message m;
    m.int1 = width;
    sendrec(PWM_TASK, WIDTH, &m);
}

void pwm_init(void)
{
    PWM_TASK = start("PWM", pwm_task, 0, STACK);
}
