/*
 * 32bitmicro startup_LPC11xx.s  Startup File for the NXP LPC11xx Device Series 

 * $URL$
 * $ID$

Copyright (c) 2010, 32bitmicro
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.
Neither the name of the 32bitmicro nor the names of its contributors may be used to endorse or promote products derived from this software without 
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

	.syntax unified
	.cpu cortex-m0
	.fpu softvfp
	.thumb

/* interrupt vectors table */

	.global	__isr_vectors
	.weak	__isr_vectors

 	.section ".isr_vector","a",%progbits
	.type	__isr_vectors, %object
	.size	__isr_vectors, .-__isr_vectors


__isr_vectors:
	.word	_estack                   /*  0 Top of Stack                 */
/* CM0 system interrupts */
	.word	Reset_Handler             /*  1 Reset Handler                */
	.word	NMI_Handler               /*  2 NMI Handler                  */
	.word	HardFault_Handler         /*  3 Hard Fault Handler           */
	.word	0                         /*  4 Reserved                     */ 
	.word	0                         /*  5 Reserved                     */
	.word	0                         /*  6 Reserved                     * 
	.word	0                         /*  7 Reserved                     */
	.word	0                         /*  8 Reserved                     */
	.word	0                         /*  9 Reserved                     */
	.word	0                         /* 10 Reserved                     */
	.word	SVC_Handler               /* 11 SVCall Handler               */
	.word	0                         /* 12 Reserved                     */
	.word	0                         /* 13 Reserved                     */
	.word	PendSV_Handler            /* 14 PendSV Handler               */
	.word	SysTick_Handler           /* 15 SysTick Handler              */

/* LPC11xx interrupts */
	.word   WAKEUP_IRQHandler         /* 16  0 Wake-up on  pin  PIO0_0             */
	.word   WAKEUP_IRQHandler         /* 17  1 Wake-up on  pin  PIO0_1             */
	.word   WAKEUP_IRQHandler         /* 18  2 Wake-up on  pin  PIO0_2             */
	.word   WAKEUP_IRQHandler         /* 19  3 Wake-up on  pin  PIO0_3             */
	.word   WAKEUP_IRQHandler         /* 20  4 Wake-up on  pin  PIO0_4             */
 	.word   WAKEUP_IRQHandler         /* 21  5 Wake-up on  pin  PIO0_5             */
	.word   WAKEUP_IRQHandler         /* 22  6 Wake-up on  pin  PIO0_6             */
	.word   WAKEUP_IRQHandler         /* 23  7 Wake-up on  pin  PIO0_7             */	
	.word   WAKEUP_IRQHandler         /* 24  8 Wake-up on  pin  PIO0_8             */
	.word   WAKEUP_IRQHandler         /* 25  9 Wake-up on  pin  PIO0_9             */
	.word   WAKEUP_IRQHandler         /* 26 10 Wake-up on  pin  PIO0_10            */
	.word   WAKEUP_IRQHandler         /* 27 11 Wake-up on  pin  PIO0_11            */
	.word   WAKEUP_IRQHandler         /* 28 12 Wake-up on  pin  PIO1_0             */
	.word   Default_Handler           /* 29 13                                     */         
	.word   SSP1_IRQHandler           /* 30 14 SSP1                                */
	.word   I2C_IRQHandler            /* 31 15 I2C0 SI (state change)              */
	.word   TIMER16_0_IRQHandler      /* 32 16 CT16B0 16 bit timer 0               */
	.word   TIMER16_1_IRQHandler      /* 33 17 CT16B1 16 bit timer 1               */
	.word   TIMER32_0_IRQHandler      /* 34 18 CT32B0 32 bit timer 0               */
	.word   TIMER32_1_IRQHandler      /* 35 19 CT32B1 32 bit timer 1               */
	.word   SSP0_IRQHandler           /* 36 20 SSP                                 */
	.word   UART_IRQHandler           /* 37 21 UART                                */
	.word   Default_Handler           /* 38 22                                     */         
	.word   Default_Handler           /* 39 23                                     */         
	.word   ADC_IRQHandler            /* 40 24 ADC end of conversion               */
	.word   WDT_IRQHandler            /* 41 25 Watchdog interrupt (WDINT)                  */
	.word   BOD_IRQHandler            /* 42 26 BOD Brown-out detect                        */
	.word   Default_Handler           /* 43 27                                     */         
	.word   PIOINT3_IRQHandler        /* 44 28 PIO_3  GPIO interrupt status of port 3      */
	.word   PIOINT2_IRQHandler        /* 45 29 PIO_2  GPIO interrupt status of port 2      */
	.word   PIOINT1_IRQHandler        /* 46 30 PIO_1  GPIO interrupt status of port 1      */
	.word   PIOINT0_IRQHandler        /* 47 31 PIO_0  GPIO interrupt status of port 0      */

/* Reset handler */
	.section ".text.handler.reset","x",%progbits

Reset_Handler:
	.thumb_func
	.globl  Reset_Handler
	.type   Reset_Handler, %function
	ldr     r0, =SystemInit+1
/*	blx     r0 */
	bl		SystemInit
	/* branch to entry point */
	ldr     r0, =_start+1
/*	bx      r0 */
	bl		_start
	.size	Reset_Handler, .-Reset_Handler


/* Dummy handlers */
	.section ".text.handler.dummy","x",%progbits

NMI_Handler:
	.weak   NMI_Handler
	.type   NMI_Handler, %function
	b .

HardFault_Handler:
	.weak   HardFault_Handler
	.type   HardFault_Handler, %function
	b .

/* weak lint does not work must either .weak or .global */
/***************************************
SVC_Handler:
	.weak   SVC_Handler
	.type   SVC_Handler, %function    
	b .
****************************************/

PendSV_Handler:
	.weak   PendSV_Handler
	.type   PendSV_Handler, %function    
	b .

SysTick_Handler:
	.weak   SysTick_Handler
	.type   SysTick_Handler, %function    
	b .

/* Default handler */
	.section ".text.handler.default","x",%progbits
	.weak	Default_Handler

Default_Handler:
	.weak     WAKEUP_IRQHandler
	.weak     I2C_IRQHandler
	.weak     TIMER16_0_IRQHandler
	.weak     TIMER16_1_IRQHandler
	.weak     TIMER32_0_IRQHandler
	.weak     TIMER32_1_IRQHandler
	.weak     SSP1_IRQHandler
	.weak     UART_IRQHandler
	.weak     ADC_IRQHandler
	.weak     WDT_IRQHandler
	.weak     BOD_IRQHandler
	.weak     PIOINT3_IRQHandler
	.weak     PIOINT2_IRQHandler
	.weak     PIOINT1_IRQHandler
	.weak     PIOINT0_IRQHandler

WAKEUP_IRQHandler:
I2C_IRQHandler:
TIMER16_0_IRQHandler:
TIMER16_1_IRQHandler:
TIMER32_0_IRQHandler:
TIMER32_1_IRQHandler:
SSP0_IRQHandler:
SSP1_IRQHandler:
UART_IRQHandler:
ADC_IRQHandler:
WDT_IRQHandler:
BOD_IRQHandler:
PIOINT3_IRQHandler:
PIOINT2_IRQHandler:
PIOINT1_IRQHandler:
PIOINT0_IRQHandler:

/* catch it here */
	b .
	.size	Default_Handler, .-Default_Handler

/* Default _start */
#if 0
	.section ".text"
	.thumb
	.global	_start_x
	.weak	_start_x
	.type   _start_x, %function    
_start_x:

/* fill .bss with 0 using byte stores */
	ldr     r0, =_bss
	ldr     r1, =_ebss
        movs    r2, #0
.L0:
	strb	r2, [r0]
	adds	r0, #1
	cmps    r0, r1
  	bcc.n   .L0

/* copy initialized .data from rom to ram */
	ldr     r0, =_data
	ldr     r1, =_edata
	ldr     r2, =_ldata
.L1:
	ldrb	r3, [r2]
	adds	r2, #1
	strb	r3, [r0]
	adds	r0, #1
	cmps    r0, r1
	bcc.n   .L1

/* branch to main */
	ldr     r0, =__main+1
	bx      r0 
	.size	_start_x, .-_start_x
#endif

	.section ".CRP._0x02FC","a",%progbits
	.globl  CRP_Key
CRP_Key:
	.word     0xFFFFFFFF
	
	.end
