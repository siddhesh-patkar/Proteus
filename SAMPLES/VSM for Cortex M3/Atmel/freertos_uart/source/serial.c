/*
    FreeRTOS V7.6.0 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART1.

	***Note*** This example uses queues to send each character into an interrupt
	service routine and out of an interrupt service routine individually.  This
	is done to demonstrate queues being used in an interrupt, and to deliberately
	load the system to test the FreeRTOS port.  It is *NOT* meant to be an
	example of an efficient implementation.  An efficient implementation should
	use FIFO's or DMA if available, and only use FreeRTOS API functions when
	enough has been received to warrant a task being unblocked to process the
	data.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"

#include "sam3n.h"
   
#include "serial.h"
/*-----------------------------------------------------------*/

/** Master clock frequency (when using board_lowlevel.c) */
#define MAINCK               4000000      // 4 MHz

/* Misc defines. */
#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serNO_BLOCK						( ( portTickType ) 0 )
#define serPMC_USART_ID					( BOARD_ID_USART )

/* The USART supported by this file. */
#define serUSART_PORT					( USART1 )
#define serUSART_IRQ					( USART1_IRQn )

/* Every bit in the interrupt mask. */
#define serMASK_ALL_INTERRUPTS			( 0xffffffffUL )

#define BIT(x,b) ((x&b)==b)

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;

/*-----------------------------------------------------------*/

/*
 * See the serial.h header file.
 */
xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
uint32_t ulChar;
xComPortHandle xReturn;

	/* Create the queues used to hold Rx/Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	/* If the queues were created correctly then setup the serial port
	hardware. */
	if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
	{

      Usart* usart0 = (Usart*)0x40024000;
      Usart* usart1 = (Usart*)0x40028000;
      Pmc* pmc  = (Pmc*)0x400E0400;
      Pio* pioa  = (Pio*)0x400E0E00;

      /* Enable usart1 peripheral clock */
      pmc->PMC_PCER0 = 1 << ID_USART1;

/*
#define PIO_PA25A_CTS1
#define PIO_PA24A_RTS1
#define PIO_PA21A_RXD1
#define PIO_PA23A_SCK1
#define PIO_PA22A_TXD1
*/

      uint32_t abcdsr;
      uint32_t pin_mask = PIO_PA21A_RXD1 | PIO_PA22A_TXD1;
      pioa->PIO_IDR = pin_mask;
      pioa->PIO_PUDR = pin_mask;
      abcdsr = pioa->PIO_ABCDSR[0];
      pioa->PIO_ABCDSR[0] &= (~pin_mask & abcdsr);
      abcdsr = pioa->PIO_ABCDSR[1];
      pioa->PIO_ABCDSR[1] &= (~pin_mask & abcdsr);
      pioa->PIO_PDR = pin_mask;
   
   	/* Reset and disable receiver & transmitter */
   	usart1->US_CR = US_CR_RSTRX | US_CR_RSTTX
         | US_CR_RXDIS | US_CR_TXDIS;
   
      /* Setup interrupts */
      usart1->US_IDR = 0xFFFFFF;        // All Interrupts Disable
   
      /* Configure baudrate */
      /* Asynchronous, no oversampling*/
   	usart1->US_BRGR = (MAINCK / ulWantedBaud) / 16;        // if ulWantedBaud = 9600 => baudrate = 9600
   
   /**
    * Configures USART in normal (serial rs232) mode, asynchronous, 8 bits, 1 stop
    * bit, no parity, 9600 bauds and enables its transmitter and receiver.
    */
      unsigned int mode = US_MR_USART_MODE_NORMAL
         | US_MR_USCLKS_MCK
         | US_MR_CHRL_8_BIT
         | US_MR_PAR_NO
         | US_MR_NBSTOP_1_BIT
         | US_MR_CHMODE_NORMAL;
   
   	/* Configure mode */
   	usart1->US_MR = mode;        // even parity, normal mode
   
   	/* Disable PDC channel */
   	usart1->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;
   
   	/* Enable transmitter and receiver */
   	usart1->US_CR = US_CR_TXEN | UART_CR_RXEN;

      usart1->US_IER = US_IER_RXRDY;        // Rx Interrupt Enable

		/* Configure and enable interrupt of USART. */
		NVIC_SetPriority( serUSART_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
		NVIC_EnableIRQ( serUSART_IRQ );
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no block time is used! */

	/* The port handle is not required as this driver only supports USART1. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* This simple example only supports one port. */
	( void ) pxPort;
   Usart* usart0 = (Usart*)0x40024000;
   Usart* usart1 = (Usart*)0x40028000;

	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
	{
		xReturn = pdPASS;
      /* Setup interrupts */
      usart1->US_IER = US_IER_TXRDY;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
	( void ) xPort;
}
/*-----------------------------------------------------------*/

/*
 * It should be noted that the com test tasks (which use make use of this file)
 * are included to demonstrate queues being used to communicate between tasks
 * and interrupts, and to demonstrate a context switch being performed from
 * inside an interrupt service routine.  The serial driver used here is *not*
 * intended to represent an efficient implementation.  Real applications should
 * make use of the USARTS peripheral DMA channel (PDC).
 */
void USART1_Handler( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t ucChar;
uint32_t ulChar;
uint32_t ulUSARTStatus, ulUSARTMask;

   Usart* usart0 = (Usart*)0x40024000;
   Usart* usart1 = (Usart*)0x40028000;

	ulUSARTStatus = usart1->US_CSR;

	if(BIT(ulUSARTStatus, US_CSR_RXRDY))
	{
		/* A character has been received on the USART, send it to the Rx handler task. */
      ulChar = usart1->US_RHR;
		ucChar = ( uint8_t ) ( ulChar & 0xffUL );
		xQueueSendFromISR( xRxedChars, &ucChar, &xHigherPriorityTaskWoken );
	}

	/* If sending or receiving from a queue has caused a task to unblock, and
	the unblocked task has a priority equal to or higher than the currently
	running task (the task this ISR interrupted), then xHigherPriorityTaskWoken
	will have automatically been set to pdTRUE within the queue send or receive
	function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns
	directly to the higher priority unblocked task. */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}






