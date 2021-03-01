// x3800/buggy.c
// Copyright (c) 2020 J. M. Spivey

#include "microbian.h"
#include "hardware.h"

/* PWM driver */

/* Pulse width modulation.  We'll use timer 2 to generate two PWM
   signals, one for each wheel motor.  It counts at 1MHz and resets
   once evey 20000 counts or 20 millisec.

   The timer has four compare-and-capture channels.  We use channel 2
   to set both the output signals high at the beginning of the period,
   and channels 0 and 1 to set them low again at the end of the pulse.

   Events on each counter channel are connected via the "Programmable
   Peripheral Interconnect" (PPI) system to GPIO Tasks in the GPIOTE
   module, and these tasks indirectly toggle the I/O pins. */

static int PWM_TASK;            // Process ID

#define WIDTH 23                // Message type for pulse width change

void pwm_setup(void) {
    // Pads 1 and 2 are outputs
    gpio_dir(PAD1, 1); gpio_dir(PAD2, 1);

    // GPIOTE channels 0 and 1 toggle the two pins
    GPIOTE_CONFIG[0] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, PAD1)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);
    GPIOTE_CONFIG[1] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, PAD2)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);

    // Set up timer 2 to count at 1MHz and reset at 20000 counts
    TIMER2_STOP = 1;
    TIMER2_MODE = TIMER_MODE_Timer;
    TIMER2_BITMODE = TIMER_BITMODE_16Bit;
    TIMER2_PRESCALER = 4;       // 1MHz=16MHz / 2^4
    TIMER2_CLEAR = 1;
    TIMER2_CC[0] = 65535;
    TIMER2_CC[1] = 65535;
    TIMER2_CC[2] = 20000;
    TIMER2_SHORTS = BIT(TIMER_COMPARE2_CLEAR);

    // PPI channels 0 and 2 toggle pin 1; channels 1 and 3 toggle the other
    PPI_CH[0].EEP = &TIMER2_COMPARE[2];
    PPI_CH[0].TEP = &GPIOTE_OUT[0];
    PPI_CH[1].EEP = &TIMER2_COMPARE[2];
    PPI_CH[1].TEP = &GPIOTE_OUT[1];
    PPI_CH[2].EEP = &TIMER2_COMPARE[0];
    PPI_CH[2].TEP = &GPIOTE_OUT[0];
    PPI_CH[3].EEP = &TIMER2_COMPARE[1];
    PPI_CH[3].TEP = &GPIOTE_OUT[1];
    PPI_CHENSET = BIT(2) | BIT(3);

    // Start your engines!
    TIMER2_START = 1;
}

static volatile unsigned width[2];

/* timer2_handler -- special interrupt handler */
void timer2_handler(void) {
    // Set the CC registers to their new values
    TIMER2_CC[0] = width[0];
    TIMER2_CC[1] = width[1];

    // No more interrupts needed
    TIMER2_INTENCLR = BIT(TIMER_INT_COMPARE2);

    // Wake up the driver process
    interrupt(PWM_TASK);
}

void set_width(int chan, int wid) {
    if (wid >= 5) {
        // Genuine pulses: enable the start-of-pulse transition and set width
        width[chan] = wid;
        PPI_CHENSET = BIT(chan);
    } else {
        // No pulses: disable pulse start, and set impossible width;
        width[chan] = 65535;
        PPI_CHENCLR = BIT(chan);
    }
}

void pwm_task(int arg) {
    message m;
    
    pwm_setup();

    while (1) {
        receive(ANY, &m);
        switch (m.m_type) {
        case WIDTH:
            set_width(0, m.m_i1);
            set_width(1, m.m_i2);

            // Enable an interrupt to update the timer
            TIMER2_COMPARE[2] = 0;
            clear_pending(TIMER2_IRQ);
            TIMER2_INTENSET = BIT(TIMER_INT_COMPARE2);
            enable_irq(TIMER2_IRQ);
            receive(INTERRUPT, NULL);
            break;

        default:
            badmesg(m.m_type);
        }
    }
}

void pwm_change(int width0, int width1) {
    message m;
    m.m_i1 = width0;
    m.m_i2 = width1;
    send(PWM_TASK, WIDTH, &m);
}

void pwm_init(void) {
    PWM_TASK = start("PWM", pwm_task, 0, STACK);    
}


/* Main program */

#define REST 1500
#define SPEED 500

void control_task(int dummy) {
    timer_delay(1000);
    while (1) {
        pwm_change(REST-SPEED, REST+SPEED);
        timer_delay(1000);
        pwm_change(REST+SPEED, REST+SPEED);
        timer_delay(1000);
    }
}

void init(void) {
    timer_init();
    pwm_init();
    start("Control", control_task, 0, STACK);
}