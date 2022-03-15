/* microbian/adc.c */
/* Copyright (c) 2021 J. M. Spivey */

/* Device driver for analog to digital converter on micro:bit V1 or V2 */

/* NJH MODIFIYING FOR BATTERY ADC */
/* NJH REMOVED UBIT_V1 ifdefs */

#include "microbian.h"
#include "hardware.h"

static int ADC_TASK;            /* PID of driver process */

/* adc_task -- device driver */
static void adc_task(int dummy)
{
    int client, chan;
    short result;
    message m;

#ifdef UBIT_V2
    /* Initialise the SAADC: 10 bit resolution,
       compare 1/4 of the input with 1/4 of Vdd 
       with acquisition window of 10 microsec. */
    ADC.CHAN[0].CONFIG = FIELD(ADC_CONFIG_GAIN, ADC_GAIN_1_6)
        | FIELD(ADC_CONFIG_REFSEL, ADC_REFSEL_Internal)
        | FIELD(ADC_CONFIG_TACQ, ADC_TACQ_10us);
    ADC.RESOLUTION = ADC_RESOLUTION_10bit;
#endif


#ifdef UBIT_V2
    /* Run a calibration cycle to set the zero point. */
    ADC.ENABLE = 1;
    ADC.CALIBRATE = 1;
    while (!ADC.CALDONE) {};
    assert(ADC.CALDONE);
    ADC.CALDONE = 0;
    ADC.ENABLE = 0;
#endif

    while (1) {
        /* Wait for a request */
        receive(REQUEST, &m);
        client = m.sender;
        chan = m.int1;

#ifdef UBIT_V2
        /* Carry out an ADC acquisition */
        ADC.CHAN[0].PSELP = chan+1;
        ADC.ENABLE = 1;
        ADC.RESULT.PTR = &result;
        ADC.RESULT.MAXCNT = 1;
        ADC.START = 1;
        ADC.SAMPLE = 1;
        while (!ADC.END) {};
        assert(ADC.END);
        assert(ADC.RESULT.AMOUNT == 1);
        ADC.END = 0;
        ADC.ENABLE = 0;

        /* Result can still be slightly negative even after calibration */
        if (result < 0) result = 0;
#endif


        /* Reply to the client */
        m.int1 = result;
        send(client, REPLY, &m);
    }
}

/* chantab -- translate pin numbers to ADC channels */
static const int chantab[] = {
#ifdef UBIT_V2
    PAD0, 0, PAD1, 1, PAD2, 2, PAD3, 7, PAD4, 4, PAD10, 6,
#endif
    0
};

/* adc_reading -- get ADC reading on specfied pin */
int adc_reading(int pin)
{
    int i, chan = -1;
    message m;

    for (i = 0; chantab[i] != 0; i += 2) {
        if (chantab[i] == pin) {
            chan = chantab[i+1];
            break;
        }
    }

    if (chan < 0)
        panic("Can't use pin %d for ADC", pin);

    m.int1 = chan;
//    m.int1 = 8; //need to look at docs again to see if this is VDD
    sendrec(ADC_TASK, REQUEST, &m);
    return m.int1;
}

/* adc_init -- start ADC driver */
void adc_init(void)
{
    ADC_TASK = start("ADC", adc_task, 0, 256);
}
