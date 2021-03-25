;******************************************************************************
;Software License Agreement                                         
;                                                                    
;The software supplied herewith by Microchip Technology             
;Incorporated (the "Company") is intended and supplied to you, the  
;Company’s customer, for use solely and exclusively on Microchip    
;products. The software is owned by the Company and/or its supplier,
;and is protected under applicable copyright laws. All rights are   
;reserved. Any use in violation of the foregoing restrictions may   
;subject the user to criminal sanctions under applicable laws, as   
;well as to civil liability for the breach of the terms and         
;conditions of this license.                                        
;                                                                    
;THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,  
;WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED  
;TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A       
;PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,  
;IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR         
;CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.       
; *******************************************************************

; MCP9800 Temperature Sensor PICtail Project Include file.
; This file defines the device and pin assignments.

; Version   Date        Description
;--------------------------------------------------------------------
; 0.00.01   03-Dec-2004 KLO Initial Release
; 0.00.03   08-Dec-2004 KLO Power-up timer enabled, default Celsius
;                           constant update rate

#include <p16F684.inc>

#define VERSION		0x10		; Version 1.0

; Define the port and pin for the lines that communicate with
; the PIC16C745

#define DATAPIN				PORTA,0
#define CLOCKPIN			PORTA,1
#define INT_ON_CLOCK_PIN		1

; Define the port and pin for the PICkit components

#define RP1_ADC_CHANNEL 		0x00
#define RP1_PIN				PORTA,0
#define RP1_ANSEL			0x01
#define SW1_BIT				3
#define SW1_PIN				PORTA,SW1_BIT
#define SW1_MASK			0x08
#define SW1_PORT			PORTA
#define LED_LINE0			PORTA,4
#define LED_LINE1			PORTA,5
#define LED_LINE2			PORTA,2
#define LED_LINE3			PORTA,1

; Define the port and pin for the MCP9800 Temperature Sensor PICtail components

#define I2C_CLOCK			PORTC,0
#define I2C_DATA			PORTC,1
#define SPI_CLOCK			PORTC,3
#define SPI_DATA			PORTC,5
; should be #define SPI_DATA			PORTC,5

; Our SPI is idle low, so be sure at least the clock lines is low
; when we start.

#define PORTC_INIT			b'11000111'
#define TRISC_INIT			b'00000000'

#define I2C_TRIS			TRISC
#define I2C_TRIS_DATA_IN		b'00000010'
#define I2C_TRIS_DATA_OUT		b'00000000'

; Define constants for use with the MCP9800

#define MCP9800_ADDRESS			h'48'
#define MCP9800_ALERT_ACTIVE_LOW	h'00'
#define MCP9800_ALERT_ACTIVE_HIGH	h'04'
#define MCP9800_COMP_MODE		h'00'
#define MCP9800_CONFIGURATION		1
#define MCP9800_HYSTERESIS		2
#define MCP9800_INT_MODE		h'02'
#define MCP9800_LIMITSET		3
#define MCP9800_ONESHOT_OFF		h'00'
#define MCP9800_ONESHOT_ON		h'80'
#define MCP9800_SHUTDOWN_OFF		h'00'
#define MCP9800_SHUTDOWN_ON		h'01'
#define MCP9800_TEMPERATURE		0
#define MCP9800_TSET_DEFAULT		d'80'

; Define constants for the PIC16F72 Communication Protocol

#define DISPLAY_DIGITS			4
#define DISPLAY_START			b'10000000'
#define DISPLAY_DOT			b'01000000'
#define DISPLAY_BLANK			b'00100000'
#define DISPLAY_DASH			b'00010000'


#define CLOCKSPEED			.8000000	; 8MHz internal Osc

