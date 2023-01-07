/**
 * \file
 *
 * \brief Linker script for running in internal FLASH on the SAMD21G18A
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */


OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
  rom      (rx)  : ORIGIN = 0x00000000 + 8K, LENGTH = 0x00040000 - 8K
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
/*STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x2000;*/
STACK_SIZE = 2048;

/*ENTRY(Reset_Handler)*/
ENTRY(__reset)

/* Section Definitions */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */


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

    } > rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;
	__etext = _etext; /*assume for now*/
    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .;
        __data_start = _srelocate; /*assume for now*/
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;
	__data_end = _erelocate; /*assume for now*/
    } > ram

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        _szero = .;
	__bss_start = _sbss; /*assume for now*/
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
        end = .;
	__bss_end = _ebss; /*assume for now*/
	__end = end; /*assume for now*/
    } > ram

    /* stack section */
    .stack (NOLOAD):
    {
/*
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
*/
    __stack = ORIGIN(ram) + LENGTH(ram);
    __stack_limit = __stack - STACK_SIZE; /*2048 should fit in with align(8)*/
    _sstack = __stack_limit;
    _estack = __stack;

    } > ram

/*    . = ALIGN(4);
    _end = . ;      */
 /* Check if data + stack exceeds RAM limit */
    ASSERT(__stack_limit >= __end, "region RAM overlaps with stack")
}
