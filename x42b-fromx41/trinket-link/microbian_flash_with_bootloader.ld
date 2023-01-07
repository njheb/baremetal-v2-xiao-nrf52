/* reworking of common/nRF51822.ld for samd21 on Adafruit trinketm0*/
/* Copyright (c) 2018-20 J. M. Spivey */


MEMORY
{
  FLASH (rx) : ORIGIN = 0x00002000, LENGTH = 0x00040000-0x2000 /* First 8KB used by bootloader */
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}


OUTPUT_FORMAT ("elf32-littlearm")

/* Setting the entry address helps GDB to find the stack bottom */
ENTRY(__reset)

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __etext = .;

        PROVIDE(nmi_handler      = default_handler);
        PROVIDE(hardfault_handler = default_handler);
        PROVIDE(svc_handler      = default_handler);
        PROVIDE(pendsv_handler   = default_handler);
        PROVIDE(systick_handler  = default_handler);

        PROVIDE(PM_Handler       = default_handler);
        PROVIDE(SYSCTRL_Handler  = default_handler);
        PROVIDE(WDT_Handler      = default_handler);
        PROVIDE(RTC_Handler      = default_handler);
        PROVIDE(EIC_Handler      = default_handler);
        PROVIDE(NVMCTRL_Handler  = default_handler);
        PROVIDE(DMAC_Handler     = default_handler);
        PROVIDE(USB_Handler      = default_handler);
        PROVIDE(EVSYS_Handler    = default_handler);
        PROVIDE(SERCOM0_Handler  = default_handler);
        PROVIDE(SERCOM1_Handler  = default_handler);
        PROVIDE(SERCOM2_Handler  = default_handler);
        PROVIDE(SERCOM3_Handler  = default_handler);
        PROVIDE(SERCOM4_Handler  = default_handler);
        PROVIDE(SERCOM5_Handler  = default_handler);
        PROVIDE(TCC0_Handler     = default_handler);
        PROVIDE(TCC1_Handler     = default_handler);
        PROVIDE(TCC2_Handler     = default_handler);
        PROVIDE(TC3_Handler      = default_handler);
        PROVIDE(TC4_Handler      = default_handler);
        PROVIDE(TC5_Handler      = default_handler);
        PROVIDE(TC6_Handler      = default_handler);
        PROVIDE(TC7_Handler      = default_handler);
        PROVIDE(ADC_Handler      = default_handler);
        PROVIDE(AC_Handler       = default_handler);
        PROVIDE(DAC_Handler      = default_handler);
        PROVIDE(PTC_Handler      = default_handler);
        PROVIDE(I2S_Handler      = default_handler);



    } > FLASH

    .data : AT (LOADADDR(.text) + SIZEOF(.text)) {
        __data_start = .;
        *(.data*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        __end = .;
    } > RAM

    /* Set stack top to end of RAM, and move stack limit down by
       size of stack */
    __stack = ORIGIN(RAM) + LENGTH(RAM);
    __stack_limit = __stack - 2048;

    /* Check if data + stack exceeds RAM limit */
    ASSERT(__stack_limit >= __end, "region RAM overlaps with stack")
}
