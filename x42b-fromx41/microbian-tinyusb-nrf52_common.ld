/* NJH modifying for microbian to support tinyusb*/

/* Linker script for Nordic Semiconductor nRF52 devices
 *
 * Version: Sourcery G++ 4.5-1
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
/*NJH OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")*/
OUTPUT_FORMAT ("elf32-littlearm")


/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 * 
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapBase
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
/*NJH ENTRY(Reset_Handler)*/
ENTRY(__reset)

/* Adafruit
 * Stack is used by ISR and SoftDevice only, other code run on thread's allocated stack from heap. 
 * Maximum stack used by S132's spec is 1536 byte. Therefore 2KB should be enough
 */

__StackSize = 1024*2;

/* Set stack top to end of RAM, and stack limit move down by size of stack_dummy section */
STACK_SIZE = __StackSize; 
PROVIDE(__StackTop   = ORIGIN(RAM) + LENGTH(RAM));
PROVIDE(__StackLimit = __StackTop - __StackSize);
PROVIDE(__stack      = __StackTop);



/* Flash region */
/*NJH
__flash_arduino_start = ORIGIN(FLASH);
__flash_arduino_end   = ORIGIN(FLASH) + LENGTH(FLASH);
*/
SECTIONS
{
    .text :
    {
/*        KEEP(*(.isr_vector))NJH*/
        KEEP(*(.vectors))
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))

        PROVIDE(nmi_handler = default_handler);
        PROVIDE(hardfault_handler = default_handler);
        PROVIDE(svc_handler = default_handler);
        PROVIDE(debugmon_handler = default_handler);
        PROVIDE(pendsv_handler = default_handler);
        PROVIDE(systick_handler = default_handler);
        PROVIDE(power_clock_handler = default_handler);
        PROVIDE(radio_handler = default_handler);
        PROVIDE(uart0_handler = default_handler);
        PROVIDE(i2c0_handler = default_handler);  /*spim0 twi0*/
        PROVIDE(i2c1_handler = default_handler);  /*spim1 twi1*/
        PROVIDE(nfc_handler = default_handler);
        PROVIDE(gpiote_handler = default_handler);
        PROVIDE(adc_handler = default_handler);
        PROVIDE(timer0_handler = default_handler);
        PROVIDE(timer1_handler = default_handler);
        PROVIDE(timer2_handler = default_handler);
        PROVIDE(rtc0_handler = default_handler);
        PROVIDE(temp_handler = default_handler);
        PROVIDE(rng_handler = default_handler);
        PROVIDE(ecb_handler = default_handler);
        PROVIDE(ccm_aar_handler = default_handler);
        PROVIDE(wdt_handler = default_handler);
        PROVIDE(rtc1_handler = default_handler);
        PROVIDE(qdec_handler = default_handler);
        PROVIDE(lpcomp_handler = default_handler);
        PROVIDE(swi0_handler = default_handler);
        PROVIDE(swi1_handler = default_handler);
        PROVIDE(swi2_handler = default_handler);
        PROVIDE(swi3_handler = default_handler);
        PROVIDE(swi4_handler = default_handler);
        PROVIDE(swi5_handler = default_handler);
        PROVIDE(timer3_handler = default_handler);
        PROVIDE(timer4_handler = default_handler);
        PROVIDE(pwm0_handler = default_handler);
        PROVIDE(pdm_handler = default_handler);
        PROVIDE(mwu_handler = default_handler);
        PROVIDE(pwm1_handler = default_handler);
        PROVIDE(pwm2_handler = default_handler);
        PROVIDE(spi0_handler = default_handler); /*SPIM2 SPI2 on gcc_startup_nrf52840.s*/
        PROVIDE(rtc2_handler = default_handler);
        PROVIDE(i2s_handler = default_handler);
        PROVIDE(fpu_handler = default_handler);
        PROVIDE(usbd_handler = default_handler);
        PROVIDE(uart1_handler = default_handler); 
/*        PROVIDE(usbd_handler = USBD_IRQHandler);
        PROVIDE(uart1_handler = UARTE1_IRQHandler);
*/
/*QSPI*/
/*CRYPTOCELL*/
        PROVIDE(pwm3_handler = default_handler);  /*PWM3*/
        PROVIDE(spi1_handler = default_handler);  /*?SPIM3*/
       
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    __etext = .;  /*NJH same as microbian*/

/*    .xram : AT (LOADADDR(.text) + SIZEOF(.text))*/ 
    .xram : AT (LOADADDR(.text) + SIZEOF(.text) + SIZEOF(.ARM.extab) + SIZEOF(.ARM.exidx))
    {
        __xram_start = .;
        *(.xram*)
        . = ALIGN(4);
        __xram_end = .;
    } > CODERAM

    .data : AT (LOADADDR(.xram) + SIZEOF(.xram))
    {
        __data_start__ = .;
        __data_start = __data_start__; /*NJH microbian*/
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
        __data_end = __data_end__; /*NJH ???*/

    } > RAM

    .bss ALIGN(4):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        __bss_start = __bss_start__; /*NJH*/
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        __bss_end = __bss_end__; /*NJH*/
    } > RAM
/*NJH not sure about heap*/


    __stack = ORIGIN(RAM) + LENGTH(RAM);
    __stack_limit = __stack - STACK_SIZE; /*2048 should fit in with align(8)*/

    .heap :
    {
        __HeapBase = .;
        __end__ = .;
        __end = __end__; /*NJH*/
        PROVIDE(end = .);
        KEEP(*(.heap*))
 /*       . = ALIGN(__StackLimit);*/
        . = ALIGN(__stack_limit);
 
        __HeapLimit = .;
    } > RAM
    
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    /*
/*NJH not sure about stack might be better NOLOAD*/    
    .stack_dummy (COPY):
    {
        KEEP(*(.stack*))
    _sstack = __stack_limit;
    _estack = __stack;

    } > RAM
    
    
    /* Check if data + heap + stack exceeds RAM limit */
 /*NJH not sure about heap stuff*/
/*
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
    ASSERT(__StackLimit >= __bss_end__, "region RAM overflowed with stack")
*/
    ASSERT(__stack_limit >= __HeapLimit, "region RAM overflowed with stack")
    ASSERT(__stack_limit >= __bss_end__, "region RAM overflowed with stack")

}

