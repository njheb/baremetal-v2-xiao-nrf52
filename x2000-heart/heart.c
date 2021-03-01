// x2000/heart.c
// Copyright (c) 2018 J. M. Spivey

#include "hardware.h"

/* delay -- pause for n microseconds */
CODERAM void delay(unsigned n) {
    unsigned t = 16*n;
    while (t > 0) {
        // 62.5nsec per iteration
        nop();
        t--;
    }
}

/* heart -- GPIO values for heart image */
const unsigned heart[] = {
    0xd0200000, 0x00000000,
    0x00400000, 0x00000000,
    0x00008000, 0x00000000,
    0x51000000, 0x00000000,
    0x50080800, 0x00000020
};

/* small -- GPIO values for small heart */
const unsigned small[] = {
    d0200800, 00000020,
    d0400000, 00000000,
    50008000, 00000000,
    51000800, 00000020,
    d0080800, 00000020
}

#define JIFFY 3000              // Delay in microsecs

/* show -- display three rows of a picture n times */
void show(const unsigned *img, int n) {
    while (n-- > 0) {
        // Takes 15msec per iteration
        for (int p = 0; p < 10; p += 2) {
            GPIO0_OUT = img[p];
            GPIO1_OUT = img[p+1];
            delay(JIFFY);
        }
    }
}

/* pressed -- test if a button is pressed */
int pressed(int button) {
    return (GPIO0_IN & BIT(button)) == 0;
}

/* init -- main program */
void init(void) {
    GPIO0_DIR = LED_MASK0;
    GPIO1_DIR = LED_MASK1;
    GPIO0_PINCNF[BUTTON_A] = 0;
    GPIO0_PINCNF[BUTTON_B] = 0;

    // Set row pins to high-drive mode to increase brightness
    gpio_drive(ROW1, GPIO_DRIVE_S0H1);
    gpio_drive(ROW2, GPIO_DRIVE_S0H1);
    gpio_drive(ROW3, GPIO_DRIVE_S0H1);
    gpio_drive(ROW4, GPIO_DRIVE_S0H1);
    gpio_drive(ROW5, GPIO_DRIVE_S0H1);
    
    while (1) {
        show(heart, 70);
        show(small, 10);
        show(heart, 10);
        show(small, 10);
    }
}
