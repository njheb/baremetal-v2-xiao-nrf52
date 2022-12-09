#include "microbian.h"
#include "hardware.h"
#include "lib.h"

#define BUTTON_A  DEVPIN(0, 3)

#define GROUP 17
/*
static const image letter_A =
    IMAGE(0,1,1,0,0,
          1,0,0,1,0,
          1,1,1,1,0,
          1,0,0,1,0,
          1,0,0,1,0);

static const image letter_B =
    IMAGE(1,1,1,0,0,
          1,0,0,1,0,
          1,1,1,0,0,
          1,0,0,1,0,
          1,1,1,0,0);
*/
void receiver_task(int dummy)
{
    byte buf[RADIO_PACKET];
    int n;

    printf("Hello\n");
    led_neo(WHITE);
//    display_show(letter_A);
//    timer_delay(1000);
//    display_show(letter_B);
//    timer_delay(1000);
//    display_show(blank);
    

//ASSUME A UBIT is sending
    while (1) {
        n = radio_receive(buf);
        if (n == 1 && buf[0] == '1') {
            printf("Button A\n");
	    //SET USER YELLOW
	    led_neo(YELLOW);
//          display_show(letter_A);
        } else if (n == 1 && buf[0] == '2') {
            printf("Button B\n");
            //SET USER MAGENTA
	    led_neo(MAGENTA);
//          display_show(letter_B);
        } else {
	    //SET USER RED
	    led_neo(RED);
            printf("Unknown packet, length %d: %d\n", n, buf[0]);
        }
    }
}

void sender_task(int dummy)
{
    int alternate = 0;
    gpio_connect(BUTTON_A);
//    gpio_connect(BUTTON_B);
    gpio_pull(BUTTON_A, GPIO_PULL_Pullup);

    while (1) {
        if (gpio_in(BUTTON_A) == 0) {
	       alternate++;
           if (alternate & 1)
	       {
               printf("Press A\n");
               radio_send("1", 1);
	           led_neo(GREEN);
           }
	       else
	       {
              printf("Press B\n");
              radio_send("2", 1);
 	          led_neo(BLUE);
           }
            //radio_send("2", 1); might try randomly sending "1" or "2" as only have one button handily available
        }

        timer_delay(100);
    }
}

void init(void)
{
    serial_init();
    radio_init();
    radio_group(GROUP);
    timer_init();
//    display_init();
    led_init(); //instead of display
    led_neo(WHITE);
    start("Receiver", receiver_task, 0, STACK);
    start("Sender", sender_task, 0, STACK);
}
