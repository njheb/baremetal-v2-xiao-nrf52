#include "microbian.h"
#include "hardware.h"
#include "tusb.h" //set path ? in Makefile or locally for now

int TUD_TASK;
int USBSERIAL_TASK0;
int USBSERIAL_TASK1;

#define START_CDC 75 //random number for now
#define GETC 76
#define PUTC 77
#define PUTBUF 78

#define BUTTON_A  DEVPIN(0, 3)


extern int pre_usb_init(void);
extern void usb_init(int usb_reg); 
extern void POWER_CLOCK_IRQHandler(void);

//volatile static int baud = -1;
//volatile static int duab;
//static int duab;
//int duab;


void linkme(void)
{
   void (*workaroundlinkage)(void); //did not help ; __attribute__((__used__));
   __asm__ __volatile__("" :: "m" (workaroundlinkage));
   workaroundlinkage = &POWER_CLOCK_IRQHandler;
}

void tud_runner(int n)
{
//    message msg; having problems with receive(ANY, &msg); fixed with change to ping

    int usbreg = pre_usb_init(); //call to what was tinyusb board_init before i>
    // init device stack on configured roothub port
    usb_init(usbreg);

    tud_init(BOARD_TUD_RHPORT);

    timer_pulse(20); //slow this down if add extra call to tud_task() from idle

    send(USBSERIAL_TASK0, START_CDC, NULL);
#if (CFG_TUD_CDC > 1) 
    send(USBSERIAL_TASK1, START_CDC, NULL);
#endif
    while (1)
    {

        //receive(ANY, &msg);
        receive(PING, NULL);
        tud_task(); // tinyusb device task

#if 0
        if (gpio_in(BUTTON_A) == 0)
        {
           cdc_line_coding_t coding;
           tud_cdc_n_get_line_coding(1, &coding);
           baud = coding.bit_rate;
           led_neo(BLACK);
        }
        else
        {
           if (baud == -1)
              led_neo(BLUE);
           else if (baud == 115200)
              led_neo(GREEN);
           else
              led_neo(WHITE); 
        }
#endif

    }
}

void usb_serial(int n)
{
    unsigned char itf = n; //< CFG_TUD_CDC
    message m;
    int client;
    int reader = -1; //allow one per instance max so not static
    char ch;
    char *buf;
    int count;

    receive(START_CDC, NULL);

    while (1)
    {
        receive(ANY, &m);
        client = m.sender;

        tud_task();

        switch (m.type) {

        case GETC:
             m.int1 = -1;
             if (reader >= 0)
                panic("Two clients cannot wait for input at once");
             reader = client;
             if ( tud_cdc_n_connected(itf) )
             {
                if ( tud_cdc_n_available(itf) )
                {
                    m.int1 = tud_cdc_n_read_char(itf);
                }   //can be -1 if not available
             }
             send(reader, REPLY, &m);
             reader = -1;
             break;

        case PUTC:
            ch = m.int1;
            //if (!tud_cdc_n_write_available(itf))
            //  tud_cdc_n_write_flush(itf);
            if (ch == '\n')
               tud_cdc_n_write_char(itf, '\r');
            tud_cdc_n_write_char(itf, ch);
            tud_cdc_n_write_flush(itf);
            break;

        case PUTBUF: //fixme for larger than 64
            buf = m.ptr1;
            n = m.int2;
            assert(n<=64); //fixme

            //for (int i = 0; i < n; i++) {
            //    char ch = buf[i];
            //    if (ch == '\n') queue_char('\r');
            //    queue_char(ch);
            //}
            //NEED TO FIND OUT MORE ABOUT TINYUSB API
            //if (tud_cdc_n_write_available(itf))
               tud_cdc_n_write_flush(itf);
            count = tud_cdc_n_write(itf, buf, n);
            tud_cdc_n_write_flush(itf);
            send(client, REPLY, NULL);
            break;

        default:
           badmesg(m.type);
        }

    }
}

/* serial_putc -- queue a character for output */
void usbserial0_putc(char ch)
{
    message m;
    m.int1 = ch;
    send(USBSERIAL_TASK0, PUTC, &m);
}

/* serial_getc -- request an input character */
//think about blocking
int usbserial0_getc(void)
{
    message m;
    send(USBSERIAL_TASK0, GETC, NULL);
    receive(REPLY, &m);
    return m.int1;
}

/* print_buf -- output routine for use by printf */
void usbprint0_buf(char *buf, int n)
{
    /* Using sendrec() here avoids a potential priority inversion:
       with separate send() and receive() calls, a lower-priority
       client process can block a reply from the device driver. */

    message m;
    m.ptr1 = buf;
    m.int2 = n;
    sendrec(USBSERIAL_TASK0, PUTBUF, &m);
}

/* serial_putc -- queue a character for output */
void usbserial1_putc(char ch)
{
    message m;
    m.int1 = ch;
    send(USBSERIAL_TASK1, PUTC, &m);
}

/* serial_getc -- request an input character */
//think about blocking
int usbserial1_getc(void)
{
    message m;
//do {
    send(USBSERIAL_TASK1, GETC, NULL);
    receive(REPLY, &m);
//    if (m.int1 == -1)
//       yield();
//}while (m.int1!=-1);
    return m.int1;
}

/* print_buf -- output routine for use by printf */
void usbprint1_buf(char *buf, int n)
{
    /* Using sendrec() here avoids a potential priority inversion:
       with separate send() and receive() calls, a lower-priority
       client process can block a reply from the device driver. */

    message m;
    m.ptr1 = buf;
    m.int2 = n;
    sendrec(USBSERIAL_TASK1, PUTBUF, &m);
}

void usb_cdc_dual_init(void)
{
    TUD_TASK = start("tud", tud_runner,  0, STACK); //remember to tune stacksize
    USBSERIAL_TASK0 = start("Port0", usb_serial,  0, STACK);
    USBSERIAL_TASK1 = start("Port1", usb_serial,  1, STACK);
}
