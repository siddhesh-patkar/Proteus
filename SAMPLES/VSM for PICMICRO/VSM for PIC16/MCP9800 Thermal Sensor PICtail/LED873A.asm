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

; LED737.asm

; This firmware resides on the MCP9800 Temperature Sensor PICtail 
; daughter board.  It is targeted to run on the PIC16F737 
; installed on the daughter board.

; This firmware will update the four seven-segment displays with a
; value sent to it by the PIC16F684 running in the evaluation socket of 
; the PICkit.  The value is received using the SPI peripheral with the 
; following protocol.

; Each received byte has the following format:
;	Bit 7	Start of Display
;	Bit 6	Light Trailing Dot
;	Bit 5	No Digit Displayed
;	Bit 4	Dash
;	Bit 3	Value Bit 3
;	Bit 2	Value Bit 2
;	Bit 1	Value Bit 1
;	Bit 0	Value Bit 0

; The firmware must receive five bytes in order to update the display.
; The first byte must be of the format b'1xxxxxxx'.  The next four bytes
; will be used to updated the display from left to right.  If another 
; start byte is received before four data bytes, whatever data bytes 
; had been received will be discarded.  The display will not be updated
; until four valid data bytes have been received.

; The value portion can be between 0 and 15 (h'0' and h'F').  The MCP9800 
; Temperature Sensor PICtail uses only decimal digits, but the other values 
; are present for future use.

; The firmware is hardcoded for the four digits on the PICtail. Since only
; one digit can be lit at a time, a timer interrupt is used to time slice
; the display update.  The timing is not critical, so the timer interrupt
; is simply allowed to interrupt on roll-over from a full count.

; Version   Date        Description
;--------------------------------------------------------------------
; 1.00.00   24-Jan-2005 KLO Initial Release

#include "p16f873a.inc"

	errorlevel	-302

;--------------------------------------------------------------------
; Define Configuration Bits
;--------------------------------------------------------------------

;#define USING_ICD2

;#ifdef USING_ICD2
;	__config	_CONFIG1, _CP_OFF & _CCP2_RC1 & _DEBUG_ON & _VBOR_2_0 & _BOREN_0 & _MCLR_ON & _PWRTE_OFF & _WDT_OFF & _INTRC_CLKOUT ;_INTRC_IO
;	__config	_CONFIG2, _BORSEN_0 & _IESO_OFF & _FCMEN_OFF
;#else
;	__config	_CONFIG1, _CP_OFF & _CCP2_RC1 & _DEBUG_OFF & _VBOR_2_0 & _BOREN_0 & _MCLR_OFF & _PWRTE_OFF & _WDT_OFF & _INTRC_CLKOUT ;_INTRC_IO
;	__config	_CONFIG2, _BORSEN_0 & _IESO_OFF & _FCMEN_OFF
;#endif
	__config	_CP_OFF & _DEBUG_ON & _PWRTE_OFF & _WDT_OFF & _HS_OSC ;_INTRC_IO
;--------------------------------------------------------------------
; Define Constants
;--------------------------------------------------------------------

#define COMPARATOR_OFF			h'07'
#define DISABLE_ANALOG			h'0f'

#define DIGIT_0				b'11000000'
#define DIGIT_1				b'11111001'
#define DIGIT_2				b'10100100'
#define DIGIT_3				b'10110000'
#define DIGIT_4				b'10011001'
#define DIGIT_5				b'10010010'
#define DIGIT_6				b'10000010'
#define DIGIT_7				b'11111000'
#define DIGIT_8				b'10000000'
#define DIGIT_9				b'10011000'
#define DIGIT_A				b'10001000'
#define DIGIT_B				b'10000011'
#define DIGIT_C				b'11000110'
#define DIGIT_D				b'10100001'
#define DIGIT_E				b'10000110'			
#define DIGIT_F				b'10001110'
#define DIGIT_BLANK			b'11111111'
#define DIGIT_DASH			b'10111111'
#define DIGIT_DOT			b'01111111'
#define DIGIT_MASK			b'00001111'

#define DISPLAY_DIGIT_BIT		3
#define DISPLAY_START			b'10000000'
#define DISPLAY_DOT			b'01000000'
#define DISPLAY_BLANK			b'00100000'
#define DISPLAY_DASH			b'00010000'
#define DISPLAY_START_BIT		7
#define DISPLAY_DOT_BIT			6
#define DISPLAY_BLANK_BIT		5
#define DISPLAY_DASH_BIT		4

#define OPTION_INIT			b'01010000'
#define TIMER0_2			b'00000000'
#define TIMER0_4			b'00000001'
#define TIMER0_8			b'00000010'
#define TIMER0_16			b'00000011'

#define TRISA_INIT			b'00000000'
#define TRISB_INIT			b'11110000'
#define TRISC_INIT			b'11011001'

#define PORTA_INIT			b'11111111'
#define PORTB_INIT			b'11110000'
#define PORTC_INIT			b'11111111'

#define SSPCON_IDLE_HIGH		b'00010000'
#define SSPCON_IDLE_LOW			b'00000000'
#define SSPCON_ENABLE			b'00100000'
#define SSPCON_DISABLE			b'00000000'
#define SSPCON_SLAVE			b'00000101'

#define OSC_8MHZ			b'01110000'
#define OSC_STS_INTRC			b'00000000'
#define OSC_STABLE			b'00000100'
#define OSC_USE_CONFIG_BITS		b'00000000'

#define GEIE_MASK			0x80
#define T0IE_MASK			0x20

;--------------------------------------------------------------------
; Reserve space for interrupt save/restore
;--------------------------------------------------------------------

        udata_shr

ISR_W		res	1
ISR_PCLATH	res	1
ISR_STATUS	res	1

;--------------------------------------------------------------------
; Reserve space for variables
;--------------------------------------------------------------------

	udata

DigitLit	res	1
DisplayDigits	res	4
NewDigits	res	4
OneDigit	res	1
Scratch		res	1
ScratchInt	res	1

;******************************************************************************
; Reset Vector
;******************************************************************************

STARTUP        code	

	goto	Init
	nop
	nop
	nop
	goto	ISR

;******************************************************************************
; Main Line
;******************************************************************************

	code

;******************************************************************************
; Initialization
;******************************************************************************

Init

	bsf	STATUS, RP0		;   First, get the oscillator running 
	errorlevel	-302		; at the desired speed.
	;movlw	OSC_8MHZ | OSC_STS_INTRC | OSC_STABLE | OSC_USE_CONFIG_BITS
	;movwf	OSCCON
	bcf	STATUS, RP0
	errorlevel	+302
	
	movlw	PORTA_INIT		;   Initialize Bank 0 registers.
	movwf	PORTA	
	movlw	PORTB_INIT
	movwf	PORTB	
	movlw	PORTC_INIT
	movwf	PORTC	
	movlw	SSPCON_ENABLE | SSPCON_IDLE_LOW | SSPCON_SLAVE
	movwf	SSPCON
					
	bsf	STATUS, RP0		;   Initialize Bank 1 registers.
	errorlevel	-302
	movlw	OPTION_INIT | TIMER0_2		
	movwf	OPTION_REG
	movlw	TRISA_INIT			
	movwf	TRISA
	movlw	TRISB_INIT			
	movwf	TRISB
	movlw	TRISC_INIT			
	movwf	TRISC
	movlw	COMPARATOR_OFF		
	movwf	CMCON
	clrf	SSPSTAT
	;movlw	OSC_8MHZ | OSC_STS_INTRC | OSC_STABLE | OSC_USE_CONFIG_BITS
	;movwf	OSCCON
	movlw	DISABLE_ANALOG
	movwf	ADCON1
	bcf	STATUS, RP0
	errorlevel	+302

	movlw	DIGIT_DASH		;   Initialize all global variables.
        movwf   DisplayDigits
        movwf   DisplayDigits+1
        movwf   DisplayDigits+2
        movwf   DisplayDigits+3
	clrf	DigitLit

	movlw	GEIE_MASK | T0IE_MASK	;   Enable timer interrupts.
	movwf	INTCON

;******************************************************************************
;******************************************************************************
; Main Loop
;******************************************************************************
;******************************************************************************

MainLoop

	bsf	STATUS, RP0		;   Wait for the start byte.
	errorlevel	-302
	btfss	SSPSTAT, BF		
	goto	MainLoop

	bcf	STATUS, RP0		;   If it is not a start byte, go
	errorlevel	+302		; back and wait for one.
	btfss	SSPBUF, DISPLAY_START_BIT
	goto	MainLoop
					
WaitForDigit0
	bsf	STATUS, RP0		;   Wait for the first digit.
	errorlevel	-302
	btfss	SSPSTAT, BF
	goto	WaitForDigit0

	bcf	STATUS, RP0		;   If we got a start byte, go back
	errorlevel	+302		; and start over.
	movf	SSPBUF, w	
	movwf	OneDigit		
	btfsc	OneDigit, DISPLAY_START_BIT
	goto	MainLoop

	call	TranslateDigit		;   Translate the value to the segment
	movwf	NewDigits		; display and save it.

WaitForDigit1
	bsf	STATUS, RP0		;   Wait for the second digit.
	errorlevel	-302
	btfss	SSPSTAT, BF
	goto	WaitForDigit1

	bcf	STATUS, RP0		;   If we got a start byte, go back
	errorlevel	+302		; and start over.
	movf	SSPBUF, w
	movwf	OneDigit		
	btfsc	OneDigit, DISPLAY_START_BIT
	goto	MainLoop

	call	TranslateDigit		;   Translate the value to the segment
	movwf	NewDigits+1		; display and save it.

WaitForDigit2
	bsf	STATUS, RP0		;   Wait for the third digit.
	errorlevel	-302
	btfss	SSPSTAT, BF
	goto	WaitForDigit2

	bcf	STATUS, RP0		;   If we got a start byte, go back
	errorlevel	+302		; and start over.
	movf	SSPBUF, w
	movwf	OneDigit
	btfsc	OneDigit, DISPLAY_START_BIT
	goto	MainLoop

	call	TranslateDigit		;   Translate the value to the segment
	movwf	NewDigits+2		; display and save it.

WaitForDigit3
	bsf	STATUS, RP0		;   Wait for the fourth digit.
	errorlevel	-302
	btfss	SSPSTAT, BF
	goto	WaitForDigit3

	bcf	STATUS, RP0		;   If we got a start byte, go back
	errorlevel	+302
	movf	SSPBUF, w		; and start over.
	movwf	OneDigit
	btfsc	OneDigit, DISPLAY_START_BIT
	goto	MainLoop

	call	TranslateDigit		;   Translate the value to the segment
					; display.

	bcf	INTCON, GIE		;   Disable interrupts so we do not
	movwf	DisplayDigits+3		; update the display with a combination
	movf	NewDigits+2, w		; of old and new values.  Copy the new
	movwf	DisplayDigits+2		; digits into the space for the digits
	movf	NewDigits+1, w		; to be displayed.  Then enable 
	movwf	DisplayDigits+1		; interrupts.
	movf	NewDigits, w
	movwf	DisplayDigits
	bsf	INTCON, GIE
	
	goto	MainLoop		;   Wait for the next update.

;******************************************************************************
; TranslateDigit
;
; This routine takes the value in location OneDigit and converts it to the
; value needed to send to the seven-segment display to show that value.  It
; uses a jump table to get the segments to light for values 0-9 and A-F.
;
;	Input:		OneDigit	Value plus flags to be displayed
;	Output:		W		Segment values
;	Effects:	None
;******************************************************************************

TranslateDigit

	btfss	OneDigit, DISPLAY_BLANK_BIT	;   If the digit should be 
	goto	CheckForDash		; blank, load the blank value and 
	movlw	DIGIT_BLANK		; return.
	return

CheckForDash

	btfss	OneDigit, DISPLAY_DASH_BIT	;   If the digit should be a
	goto	CheckForDigit		; dash, load the dash value and return.
	movlw	DIGIT_DASH
	return

CheckForDigit

	movlw	DIGIT_DOT			;   See if we need to light the 
	btfss	OneDigit, DISPLAY_DOT_BIT	; decimal point, and store that
	movlw	b'11111111'			; value for later.
	movwf	Scratch

	movf	OneDigit, w		;   Mask off the flag bits to get ready
	andlw	DIGIT_MASK		; to get the seven-segment value to
	movwf	OneDigit		; display the number.
	call	GetDigitSegments
                                        
	andwf	Scratch, w		;   Add the decimal point if necessary.

	return				;   Return with the digit display value.

;------------------------------------------------------------------------------
; GetDigitSegments
;
; This routine provides the look-up table for TranslateDigit.  If the table
; spans a page boundary, uncomment the paging instructions.
;------------------------------------------------------------------------------

GetDigitSegments

;	pageselw	DigitTable	;   Put the address of the start label
	movlw	DigitTable	        ; in W, and add our counter offset.  If
	addwf	OneDigit, w		; we crossed a 256 byte boundary, then
;	btfsc	STATUS, C		; increment PCLATH. Then load the 
;	incf	PCLATH, f	        ; program counter with computed goto 
	movwf	PCL			; value to get the digit value.

DigitTable

	retlw	DIGIT_0
	retlw	DIGIT_1
	retlw	DIGIT_2
	retlw	DIGIT_3
	retlw	DIGIT_4
	retlw	DIGIT_5
	retlw	DIGIT_6
	retlw	DIGIT_7
	retlw	DIGIT_8
	retlw	DIGIT_9
	retlw	DIGIT_A
	retlw	DIGIT_B
	retlw	DIGIT_C
	retlw	DIGIT_D
	retlw	DIGIT_E
	retlw	DIGIT_F
	
;******************************************************************************
; Interrupt Service Routine
;******************************************************************************

ISR

	movwf   ISR_W         		;   Save off the current context of
	movf	STATUS,w		; W, STATUS, and PCLATH
	movwf	ISR_STATUS									 
	movf	PCLATH,w
	movwf	ISR_PCLATH

	bcf	STATUS, RP0		;   Set to Bank 0.

	btfss	INTCON, T0IF		;   See if the timer interrupt tripped.
	goto	ISR_Exit

;------------------------------------------------------------------------------
; TimerISR
;
; This routine updates the seven-segment display.  Each digit of the display
; is illuminated separately.  First, all digits are turned off.  Then the 
; segments that we want lit are driven low.  Then the digit we want is driven
; high.
;
; Note that the digit in enabled by pins on Port B, and the segments are 
; enabled by pins on Ports A and C, since Port A is only six bits wide.
;------------------------------------------------------------------------------

TimerISR

	bcf	INTCON, T0IF		;   Clear the timer interrupt flag.

	clrf	PORTB			;   Turn off all of the digits

	movf	DigitLit, f		;   See if we have looped through
	btfsc	STATUS, Z		; all of the digits.
	bsf	DigitLit, DISPLAY_DIGIT_BIT

	movf	DisplayDigits+3, w	;   Get the correct value to light
	btfsc	DigitLit, 3		; the seven segment display.  If we 
	goto	DigitReady		; need the first digit, go on.

	movf	DisplayDigits+2, w	;   See if we need the second digit.
	btfsc	DigitLit, 2
	goto	DigitReady

	movf	DisplayDigits+1, w	;   See if we need the third digit.
	btfsc	DigitLit, 1
	goto	DigitReady

	movf	DisplayDigits, w	;   We need the fourth digit.

DigitReady

	movwf	PORTA			;   Light most of the segments using
	movwf	ScratchInt		; Port A.  Shift the remaining bits
	rrf	ScratchInt, f		; to light the other two segments on
	rrf	ScratchInt, f		; Port C.  We do not need to worry 
	rrf	ScratchInt, f		; about the other Port C bits.  Then
	rrf	ScratchInt, f		; turn on the digit we want the value
	rrf	ScratchInt, w		; to appear on.
	movwf	PORTC
	movf	DigitLit, w
	movwf	PORTB

	bcf	STATUS, C		;   Get the counter ready to display
	rrf	DigitLit, f		; the next digit.

;------------------------------------------------------------------------------

ISR_Exit

	movf	ISR_PCLATH,w		;   Restore context and return to the
	movwf	PCLATH			; main processing.  Be sure to restore
	movf	ISR_STATUS,w		; W without corrupting the STATUS bits.
	movwf	STATUS			
	swapf	ISR_W,f			  
	swapf	ISR_W,w			 
	retfie                    

        end
