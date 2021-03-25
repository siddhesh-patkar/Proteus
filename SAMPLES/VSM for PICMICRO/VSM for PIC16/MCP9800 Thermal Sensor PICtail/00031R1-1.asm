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

; 00031R1-1.asm

; This file is demonstration firmware for the MCP9800 Temperature
; sensor PICtail daughter board.  It is targeted to run on a PIC16F684 
; installed in the evaluation socket of a PICkit, with the MCP9800 
; PICtail daughter board installed in connector J3 on the PICkit.

; This firmware will program the MCP9800 on the daughter board as 
; directed by constants in the firmware and the potentiometer on the 
; PICkit.  The resolution and fault queue size are set based on 
; constants in the firmware.  To set the alert temperature of the 
; MCP9800, turn the potentiometer RP1 to select a temperature from 15 
; to 48 degrees Celsius (59 to 118 degrees Fahrenheit).  When the alert 
; temperature changes, the MCP9800 set point and hysteresis will be 
; programmed with the new selection.  

; The temperature reading from the MCP9800 is monitored and sent to the 
; PICtail for display on the seven-segment LED's.  By default, the 
; temperature is displayed in Celsius.  To display the temperature in 
; Fahrenheit, press and hold switch SW1. The temperature is read and
; the display is updated approximately four times per second.

; This firmware can also respond to commands from a PC through the
; USB port, similar to the Signal Analysis PICtail daughter board.
; This firmware supports the commands to return the firmware version
; and to return a single 10-bit value.  The firmware will return the
; top ten bits of the temperature value, scaled such that 0 degrees
; Celsius is 512 counts.  The other commands are stubbed out so the PC 
; and the firmware do not get out of sync.

; NOTES: 

; The PICkit gives us about 100us to notice that the clock line
; is low when it sends us a command.  Since our chip is running at
; 8MHz, that gives us about 200 cycles.  Since our timer interrupt is
; not critical, we will use an interrupt on change on Port A to watch
; for this transition rather than polling.  We must disable the 
; interrupt during the A/D conversion of the potentiometer, but since 
; the conversion requires only 43 cycles, it is not a problem.

; The MCP9800 requires 1-2ms to complete its power-on sequence before
; we can configure it.  We can easily account for this requirement by
; enabling the PIC16F684's power-up timer.

; Version   Date        Description
;--------------------------------------------------------------------
; 0.00.01   03-Dec-2004 KLO Initial Release
; 0.00.02   08-Dec-2004 KLO Debug Release for start-up issue
; 0.00.03   08-Dec-2004 KLO Power-up timer enabled, default Celsius,
;                           constant update rate

#include "project.inc"
#include "comm.inc"
#include "I2CBUS.INC"

;********************************************************************
;********************************************************************
;	User Defined Constants                                     
;
; These values may be modified for your application.  To make it 
; easier to read, the default radix for this section is decimal.
;********************************************************************
;********************************************************************

	list	r=DEC

; Set the fault queue size to control how many consecutive conversions
; Ta must remain above Tset to assert Alert and Ta must remain below
; Thyst to deassert Alert.  This value can be 1, 2, 4, or 6.
#define FAULT_QUEUE_SIZE	1

; Set the conversion resolution to the number of bits calculated for
; each temperature conversion.  This value can be 9, 10, 11, or 12.
#define CONVERSION_RESOLUTION	12

; Set the delta between the alert temperature and the hysteresis 
; temperature.  Thyst will be programmed to the alert temperature minus
; this value in degrees Celsius.  This value can be 1-10.
#define THYST_DELTA		5

;********************************************************************
;********************************************************************
;	End User Constants
;********************************************************************
;********************************************************************

;--------------------------------------------------------------------
; Check User Constants
;--------------------------------------------------------------------

#if FAULT_QUEUE_SIZE == 1
	#define FAULT_QUEUE	b'00000000'
#else
#if FAULT_QUEUE_SIZE == 2
	#define FAULT_QUEUE	b'00001000'
#else
#if FAULT_QUEUE_SIZE == 2
	#define FAULT_QUEUE	b'00010000'
#else
#if FAULT_QUEUE_SIZE == 2
	#define FAULT_QUEUE	b'00011000'
#else
	error 'Fault Queue Size incorrectly specified.'
#endif
#endif
#endif
#endif

#if CONVERSION_RESOLUTION == 9
	#define RESOLUTION 	b'00000000'
#else
#if CONVERSION_RESOLUTION == 10
	#define RESOLUTION 	b'00100000'
#else
#if CONVERSION_RESOLUTION == 11
	#define RESOLUTION 	b'01000000'
#else
#if CONVERSION_RESOLUTION == 12
	#define RESOLUTION 	b'01100000'
#else
	error 'Conversion Resolution incorrectly specified.'
#endif
#endif
#endif
#endif

#if (THYST_DELTA < 1) || (THYST_DELTA > 10)
	error 'Thyst Delta incorrectly specified.'
#endif

	list	r=HEX

;--------------------------------------------------------------------
; End Check 
;--------------------------------------------------------------------

;--------------------------------------------------------------------
; Define Configuration Bits

; Power-up Timer - Enabled to account for start-up delay of MCP9800.
; Oscillator - Internal RC, Clock-out, to monitor chip operation.
;	Internal RC, No Clock-out can also be used.
;--------------------------------------------------------------------

	__config	_FCMEN_OFF & _IESO_OFF & _BOD_OFF & _CPD_OFF & _CP_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_CLKOUT

;--------------------------------------------------------------------
; Define Constants
;--------------------------------------------------------------------

#define ADC_GO				0x02
#define ADC_INTERNAL_RC			0x70
#define ADC_ON				0x01
#define ADC_LEFT_JUSTIFIED      	0x00
#define ADC_RIGHT_JUSTIFIED		0x80
#define ADC_REF_VDD            		0x00
#define COMPARATOR_OFF			0x07
#define DELAY_COUNT			0x08
#define GEIE_MASK			0x80
#define RAIE_MASK			0x08
#define T0IE_MASK			0x20

#define OSC_4MHZ			b'01100000'
#define OSC_8MHZ			b'01110000'
#define OSC_STS_INTRC			b'00000000'
#define OSC_STABLE			b'00000110'
#define OSC_USE_CONFIG_BITS		b'00000000'

#define OPTION_PS_256			b'00000111'
#define OPTION_PULLUPS_ON		b'00000000'
#define OPTION_PULLUPS_OFF		b'10000000'

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

AlertTemp	res	1
BCD100		res	1
BCD10		res	1
BCD1		res	1
BCD10th		res	1
BCD100th	res	1
DelayCounter	res	1
DigitsSent	res	1
Groups		res	1
IsNegative	res	1
NewAlert	res	1
PCReading	res	2
RawReading	res	2
ReadyToConvert	res	1
Scratch		res	3
ShiftCount	res	1
SPIcounter      res     1
SPIdata		res	1
SwitchReading	res	1
Temp            res     1
Temperature	res	3

;******************************************************************************
; Reset Vector
;******************************************************************************

STARTUP        code	

	goto	Init			;   Set up the reset and interrupt
	nop				; vectors.
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
	errorlevel	-302		; at the desired frequency.
	movlw	OSC_8MHZ | OSC_STS_INTRC | OSC_STABLE | OSC_USE_CONFIG_BITS
	movwf	OSCCON
	bcf	STATUS, RP0
	errorlevel	+302

	movlw	COMPARATOR_OFF		;   Initialize Bank 0 registers.
	movwf	CMCON0
	movlw	PORTC_INIT
	movwf	PORTC	
	movlw	ADC_LEFT_JUSTIFIED | ADC_REF_VDD | ADC_ON
	movwf	ADCON0			
					
	bsf	STATUS, RP0		;   Initialize Bank 1 registers.
	errorlevel	-302
	movlw	OPTION_PULLUPS_ON | OPTION_PS_256
	movwf	OPTION_REG
	movlw	b'11111111'
	movwf	TRISA		
	movlw	TRISC_INIT			
	movwf	TRISC
	clrf	ANSEL
	movlw	ADC_INTERNAL_RC
	movlw	ADCON1
	bsf	IOCA, INT_ON_CLOCK_PIN
	bcf	STATUS, RP0
	errorlevel	+302

        movlw   MCP9800_TSET_DEFAULT	;   Initialize all global variables.
        movwf   AlertTemp		; Initialize the LED value after the
	movlw	DELAY_COUNT		; default gain and channel are set.
	movwf	DelayCounter		
	clrf	I2C_ErrorCode		
	clrf	I2C_Flags
	clrf	ReadyToConvert

	movlw 	GEIE_MASK | RAIE_MASK | T0IE_MASK	
	movwf	INTCON			;   Enable interrupts.  We need to 
					; enable the Port A interrupt as soon 
					; as possible so we can respond to a 
					; version check from the PC.

	call	ProgramConfigReg	;   Initialize the MCP9800.
	call	ProgramAlertTemperature

;******************************************************************************
;******************************************************************************
; Main Loop
;******************************************************************************
;******************************************************************************

MainLoop

	btfss	ReadyToConvert, 0	;   Wait some time so we update the 
	goto	MainLoop		; display in a readable time frame.
	
	bcf	ReadyToConvert, 0	;   Read and display the ambient
	call	GetAlertTemperature	; temperature.
	call	ReadTemperature
	call	ConvertTemperature
	call	ConvertToBCD
	call	DisplayTemperature

	goto	MainLoop

;******************************************************************************
; GetAlertTemperature
;
; This routine calculates the alert temperature from the potentiometer on the
; PICkit.  It converts the A/D reading to a value from 10 to 55 degrees
; Celsius.  If this value is different from the current alert temperature, the
; MCP9800 is programmed with the new value.
;
;	Input:		None
;	Output:		AlertTemp	Alert Temperature of the MCP9800
;	Side Effects:	WREG destroyed
;			PORTA interrupt temporarily disabled
;******************************************************************************

GetAlertTemperature

	bsf	STATUS, RP0		;   Configure RP1 as an analog input
	errorlevel	-302		; and disable the interrupt on change
	movlw	RP1_ANSEL 		; on port A.
	movwf	ANSEL
	bcf	IOCA, INT_ON_CLOCK_PIN
	bcf	STATUS, RP0
	errorlevel	+302

	movlw	ADC_LEFT_JUSTIFIED | ADC_REF_VDD | RP1_ADC_CHANNEL | ADC_ON
	movwf	ADCON0	 		;    Start the A/D conversion, and wait
	call	ADCSettlingDelay	; until it's done.  The conversion 
	bsf	ADCON0, GO		; takes 43 cycles, so we can safely
WaitForRP1				; ignore the PIC16C745 clock line.
	btfss	ADCON0, NOT_DONE	
	goto	RP1_Done
	goto	WaitForRP1

RP1_Done

	bsf	STATUS, RP0		;   Configure the line back as digital
	errorlevel	-302		; and enable the interrupt so we know
	clrf	ANSEL			; when the PIC16C745 is talking to us.
	bsf	IOCA, INT_ON_CLOCK_PIN
	bcf	STATUS, RP0
	errorlevel	+302

	movlw	0xF0			;   Convert the potentiometer reading
	andwf	ADRESH, w		; to the alert temperature.  First,
	movwf	NewAlert		; get just the top four bits and put
	swapf	NewAlert, f		; them in the lower nibble.  Then
	movf	NewAlert, w		; convert that to a value between 10
	addwf	NewAlert, w		; and 55 degrees Celsius by multiplying
	addwf	NewAlert, w		; by three and adding 10.
	addlw	d'10'
	movwf	NewAlert
	
	subwf	AlertTemp, w		;   If the alert temperature has not
	btfsc	STATUS, Z		; changed, exit without reprogramming
	goto	AlertSelectDone		; the MCP9800.
		
	movf	NewAlert, w		;   Store the new alert temperature and
	movwf	AlertTemp		; program the MCP9800.
	call	ProgramAlertTemperature

AlertSelectDone

	return

;******************************************************************************
; ReadTemperature
;
; This routine reads the ambient temperature value from the MCP9800.  It also
; converts this reading to a value that can be sent to the PC.  Since the
; PCReading registers will be sent during an interrupt, disable interrupts
; while we update the value, so we do not send garbage.
;
;	Input:		None
;	Output:		RawReading registers	Raw temperature value
;			PCReading registers	Scaled temperature reading for 
;						sending to the PC
;	Side Effects:	WREG destroyed
;******************************************************************************

ReadTemperature
	
	call	I2C_StartBit				;   Get the raw ambient 
	movlw	(MCP9800_ADDRESS << 1) | I2C_WRITE	; temperature reading 
	call	I2C_TransmitByte			; from the MCP9800.
	movlw	MCP9800_TEMPERATURE
	call	I2C_TransmitByte
	call	I2C_StartBit			
	movlw	(MCP9800_ADDRESS << 1) | I2C_READ
	call	I2C_TransmitByte
	call	I2C_ReceiveByte
	movwf	RawReading
	call	I2C_SendACK
	call	I2C_ReceiveByte
	movwf	RawReading+1
	call	I2C_SendNAK
	call	I2C_StopBit

	bcf	INTCON, GIE		;   Disable interrupts so we can update
					; PCReading.

	movf	RawReading, w		;   Get the value ready for sending to 
	addlw	h'80'			; the PC. Shift up the raw reading such 
	movwf	PCReading		; that the lowest temperature reading is 0 
	movf	RawReading+1, w		; and the highest temperature value is 
	movwf	PCReading+1		; 4095.

	movlw	6			;   Shift over the value such that the
	movwf	ShiftCount		; ten most significant bits are right
ShiftOneBit				; justified.
	bcf	STATUS, C		  
	rrf	PCReading, f
	rrf	PCReading+1, f
	decfsz	ShiftCount, f
	goto	ShiftOneBit

	bsf	INTCON, GIE		;   We are done with updating PCReading,
					; so enable interrupts.

	return

;******************************************************************************
; ConvertTemperature
;
; This routine converts the temperature to the desired units, Fahrenheit or
; Celsius.  It also converts the temperature to a positive value, with the sign 
; indicator stored in another byte.
;
; We multiply the temperature by 9 by shifting the original value three times
; to multiply by 8, and then adding the original value back in to multiply by 9.
; When we are done, the final value will be in the Scratch registers.  We then 
; divide by 5 by seeing how many times we can subtract 5 from the number (or add
; 5 to the number if the original value was negative).  When we are done with 
; this step, the answer will be in the Temperature registers.  Then we add 32, 
; shifted over one byte to allow for the fractional portion. 
;
; Note that the switch is active low.
;
;	Input:	RawReading registers	Raw temperature value
;	Output:	Temperature registers	Absolute value of converted 
;					temperature value
;		IsNegative		Sign of converted temperature value
;	Side Effects:			WREG destroyed
;******************************************************************************

ConvertTemperature

	clrw				;   Copy the raw data into the 
	btfsc	RawReading, 7		; conversion areas.  Sign extend the
	movlw	0xFF			; value into the upper bytes.
	movwf	Temperature		
	movwf	Scratch
	movf	RawReading, w		
	movwf	Temperature+1
	movwf	Scratch+1
	movf	RawReading+1, w
	movwf	Temperature+2
	movwf	Scratch+2	

CheckSwitch

	movf	SW1_PORT, w		;   Read SW1 to see if we're going to
	andlw	SW1_MASK		; convert the temperature to deg F.
	movwf	SwitchReading		; Read the switch twice.  If the two
	xorwf	SW1_PORT, w		; readings are the same, we're done.
	andlw	SW1_MASK
	btfsc	STATUS, Z
	goto	SwitchDone

	movf	SwitchReading, w	;   Read the switch one more time.  If
	xorwf	SW1_PORT, w		; it now matches the first reading,
	andlw	SW1_MASK		; we're done.
	btfss	STATUS, Z
	goto	SwitchDone

	movf	SwitchReading, w	;   The last two readings were the same
	xorlw	SW1_MASK		; and were opposite the first reading.
	andlw	SW1_MASK		; Flip the first reading and go on.
	movwf	SwitchReading

SwitchDone

	btfsc	SwitchReading, SW1_BIT	;   If the switch is not pressed, leave
	goto	CheckSign		; the temperature in Celsius.

Fahrenheit

	movlw	3			;   Convert the temperature to 
	movwf	ShiftCount		; Fahrenheit.  First, multiply the
Times2					; value by eight by shifting.  
	bcf	STATUS, C		
	rlf	Scratch+2, f	
	rlf	Scratch+1, f
	rlf	Scratch, f
	decfsz	ShiftCount, f
	goto	Times2

	movf	Temperature+2, w	;   Then add one more back in, as 
	addwf	Scratch+2, f		; saved in Scratch, to multiply by
	movf	Temperature+1, w	; nine.  Note that this it 3-byte
	btfss	STATUS, C		; addition, so we have to watch for
	goto	NoCarry1		; propagation of the carry.  When we 
	incf	Scratch+1, f		; are done, the result is in Scratch.
	btfsc	STATUS, Z
	incf	Scratch, f
NoCarry1
	addwf	Scratch+1, f
	movf	Temperature, w
	btfsc	STATUS, C
	incf	Scratch, f
	addwf	Scratch, f

Div5	
	clrf	Temperature		;   Get ready to divide by 5.  We will
	clrf	Temperature+1		; handle positive and negative numbers
	clrf	Temperature+2		; separately.
	btfsc	Scratch, 7
	goto	Add5

	goto	Subtract5		;   Skip the increment on the first time
					; around.
DividePositive
	incf	Temperature+2, f	;   Increment the count of how many times
	btfss	STATUS, Z		; 5 went into the number.
	goto	Subtract5
	incf	Temperature+1, f
	btfsc	STATUS, Z
	incf	Temperature, f

Subtract5
	movlw	5			;   Subtract 5 from the number.  Account
	subwf	Scratch+2, f		; for borrows across all three bytes.  We	
	btfsc	STATUS, C		; need the carry flag, so we cannot use
	goto	NoCarry2		; decrement.
	movlw	1
	subwf	Scratch+1, f
	btfss	STATUS, C
	decf	Scratch, f
NoCarry2

	btfss	Scratch, 7		;   If we haven't gone negative, subtract 
	goto	DividePositive		; 5 again.  Otherwise, we're done.
	goto	Add32

DivideNegative
	incf	Temperature+2, f	;   Increment the count of how many times
	btfss	STATUS, Z		; 5 went into the number.
	goto	Add5
	incf	Temperature+1, f
	btfsc	STATUS, Z
	incf	Temperature, f

Add5
	movlw	5 			;   Add 5 to the number.  Account
	addwf	Scratch+2, f		; for carries across all three bytes.
	btfss	STATUS, C
	goto	NoCarry3
	incf	Scratch+1, f
	btfsc	STATUS, Z
	incf	Scratch, f
NoCarry3

	btfsc	Scratch, 7		;   If we haven't gone positive, add 
	goto	DivideNegative		; 5 again.  Otherwise, we need to see
	movf	Scratch+2, f		; if we hit 0 exactly.  If we did, we
	btfsc	STATUS, Z		; need to loop one more time to count
	goto	DivideNegative		; that last subtraction.  Since we're 
					; subtracting only 5, we only need to
					; look at the least significant byte.

	comf	Temperature+2, f	;   Since we started out with a 
	comf	Temperature+1, f	; negative number, we must make our 
	comf	Temperature, f		; answer negative.
	incf	Temperature+2, f		
	btfsc	STATUS, Z
	incf	Temperature+1, f
	btfsc	STATUS, Z
	incf	Temperature, f

	
Add32
	movlw	d'32'			;   Add 32 to complete the conversion.
	addwf	Temperature+1, f	; Note that the third byte is the
	btfsc	STATUS, C		; fractional portion, so we add to the
	incf	Temperature, f		; second byte.
	
CheckSign
	
	clrf	IsNegative		;   See if the temperature reading is
	btfss	Temperature, 7		; positive.  If it is, we are done.
	goto	ConversionDone		

	comf	IsNegative, f		;   Set the sign indicator and convert
	comf	Temperature+2, f	; the temperature to a positive value.
	comf	Temperature+1, f		
	comf	Temperature, f		
	incf	Temperature+2, f		
	btfsc	STATUS, Z
	incf	Temperature+1, f
	btfsc	STATUS, Z
	incf	Temperature, f

ConversionDone

	return

;******************************************************************************
; ConvertToBCD
;
; This routine converts the temperature value into BCD digits that can be 
; displayed on the seven-segment display by the PIC16F737.  We could just 
; convert the digits as needed, but it uses the same amount of code space,
; and this looks cleaner.
;
; Since we do not have a digit carry status bit, we will calculate the BCD 
; values by repetatively subtracting the digit values.
;
;	Input:	Temperature registers	Absolute value of converted 
;					temperature value
;	Output:	BCDxxx registers	BCD equivalent of the temperature
;	Side Effects:			WREG destroyed
;					Temperature registers destroyed
;******************************************************************************

ConvertToBCD

	clrf	BCD100			;   Find the BCD 100's digit.  To save
	goto	Subtract100		; code, this portion is structured
					; differently from the other digits.
					; We need to do multiple precision math,
					; so to minimize the math, we increment 
					; after we know the subtraction is valid,
					; and save the old value in case it was 
					; not.

Next100
	incf	BCD100, f		;   Increment the 100's digit.

Subtract100
	movlw	d'100'			;   Subtract 100 from the integer part
	subwf	Temperature+1, w	; of the temperature.  Note that this is
	movwf	Scratch+1		; 16-bit math.  If we didn't borrow, we
	btfsc	STATUS, C		; do not have to worry about the upper
	goto	NoBorrow100		; byte.

	movlw	1			;   We had to borrow from the upper 
	subwf	Temperature, w		; byte. Continue the subtraction.  If we
	btfss	STATUS, C		; borrow again, we have no more 100's, 
	goto	Check10s		; and BCD100 and Temperature+1 are ready 
					; for the next digit.

	movwf	Temperature		;   The subtraction was successful, so 
					; store the result for the next pass.

NoBorrow100
	movf	Scratch+1, w		;   Get the result of the first byte
	movwf	Temperature+1		; subtraction and get it ready for the
	goto	Next100			; next pass.

Check10s
	clrf	BCD10			;   Get ready to determine the 10's 
	movlw	d'10'			; digit.
	
Subtract10
	incf	BCD10, f		;   Since we are not doing multiple 
	subwf	Temperature+1, f	; precision math, we can save some code
	btfsc	STATUS, C		; by incrementing first and patching up
	goto	Subtract10		; in case we subtract too much.  
					; Increment the digit and subtract 10
					; from the temperature.  If we go less
					; than 0, we are done.

Done10
	decf	BCD10, f		;   We subtracted one too many times,
	addwf	Temperature+1, w	; so correct the number and the digit.

Check1s
	movwf	BCD1			;   Whatever is left is the one's digit.

	clrw				;   Calculate the fractional portion of
	btfsc	Temperature+2, 7	; the temperature in hundredths.
	addlw	d'50'
	btfsc	Temperature+2, 6
	addlw	d'25'
	btfsc	Temperature+2, 5
	addlw	d'13'
	btfsc	Temperature+2, 4
	addlw	d'6'
	movwf	Temperature+2

Check10ths
	clrf	BCD10th			;   Get ready to calculate the 10th's
	movlw	d'10'			; digit.

Subtract10th
	incf	BCD10th, f		;   Increment the digit and subtract 10
	subwf	Temperature+2, f	; from the hundredths.  If we go less
	btfsc	STATUS, C		; than 0, we are done.
	goto	Subtract10th

Done10th
	decf	BCD10th, f		;   We subtracted one too many times,
	addwf	Temperature+2, w	; so correct the number and the digit.
	
Check100ths
	movwf	BCD100th		;   Whatever is left is the hundredths
					; digit.
	
	return

;******************************************************************************
; DisplayTemperature
;
; This routine sends the BCD temperature digits to the PIC16F737 for display on
; the seven-segment display.
;
;	Input:	BCDxxx registers	BCD equivalent of the temperature
;		IsNegative		Sign of converted temperature value
;	Output:	None
;	Side Effects:	WREG destroyed
;******************************************************************************

DisplayTemperature

	movlw	DISPLAY_DIGITS		;   Initialize the number of digits
	movwf	DigitsSent

	movlw	DISPLAY_START		;   Send the start byte.
	call	BitBangSPI

	btfss	IsNegative, 0		;   If the temperature is negative,
	goto	Check100Digit		; send a dash for the negative sign.
	movlw	DISPLAY_DASH
	call	BitBangSPI
	decf	DigitsSent, f

Check100Digit
	movf	BCD100, w		;   See if the 100's digit is 0.
	btfsc	STATUS, Z
	goto	Check10Digit

	call	BitBangSPI		;   If the 100's digit is not 0, send 
	decf	DigitsSent, f		; it.  Then go to send the 10's digit.
	goto	Send10Digit

Check10Digit
	movf	BCD10, w		;   See if the 10's digit is 0.
	btfss	STATUS, Z
	goto	Send10Digit

	movf	DigitsSent, f		;   See if we're still trying to send
	xorlw	DISPLAY_DIGITS		; the first digit.
	btfsc	STATUS, C
	goto	Send1Digit	

	movlw	DISPLAY_BLANK		;   If we haven't sent a digit yet,
	call	BitBangSPI		; then send a blank.
	decf	DigitsSent, f
	goto	Send1Digit

Send10Digit	
	movf	BCD10, w		;   Send the 10's digit if it is not 0
	call	BitBangSPI		; or if we sent the 100's digit.
	decf	DigitsSent, f
	
Send1Digit
	movf	BCD1, w			;   Send the 1's digit with a trailing
	iorlw	DISPLAY_DOT		; decimal point.  Then see if we have 
	call	BitBangSPI		; more digits available.
	decfsz	DigitsSent, f
	goto 	Send10thDigit
	goto 	DigitsDone

Send10thDigit				
	movf	BCD10th, w		;   Send the 10th's digit.  Then see if
	call	BitBangSPI		; we have more digits available.
	decfsz	DigitsSent, f
	goto 	Send100thDigit
	goto 	DigitsDone

Send100thDigit
	movf	BCD100th, w		;   Send the 100th's digit.
	call	BitBangSPI

DigitsDone

	return

;******************************************************************************
;******************************************************************************
; Interrupt Service Routine
;******************************************************************************
;******************************************************************************

ISR

	movwf   ISR_W         		;   Save off the current context of
	movf	STATUS,w		; W, STATUS, and PCLATH
	movwf	ISR_STATUS									 
	movf	PCLATH,w
	movwf	ISR_PCLATH

	btfsc	INTCON, RAIF
	goto	PortAChangeISR

	btfss	INTCON, T0IF
	goto	ISR_Exit

;------------------------------------------------------------------------------
; TimerISR
;
; We need to time approximate 250ms for the update of the temperature.  If we
; use the maximum prescaler and let the timer roll over from a full count, we
; get a delay, at 8MHz, of 65536 cycles.  At 8MHz, 250ms requires 500000 cycles,
; or approximately 8 interrupts.  When we count 8 interrupts, we will set a 
; flag.

TimerISR

	bcf	INTCON, T0IF		;   If we have not waited for
	decfsz	DelayCounter		; enough interrupts, continue.
	goto	ISR_Exit		

	bsf	ReadyToConvert, 0	;   It is time to begin another
	movlw	DELAY_COUNT		; conversion.  Set the flag and reset
	movwf	DelayCounter		; the counter for the next pass.

;------------------------------------------------------------------------------
; PortAChangeISR
;
; If we get a change on the PORTA pin that we are monitoring, the PIC16C745
; may be trying to talk to us.

PortAChangeISR

	bcf	INTCON, RAIF		;   See if the PC is trying to 
	;call	CheckComm		; communicate.

;------------------------------------------------------------------------------
ISR_Exit

	movf	ISR_PCLATH,w		;   Restore context and return to the
	movwf	PCLATH			; main processing.  Be sure to restore
	movf	ISR_STATUS,w		; W without corrupting the STATUS bits.
	movwf	STATUS			
	swapf	ISR_W,f			  
	swapf	ISR_W,w			 
	retfie                    

;******************************************************************************
; CheckComm
;
; Since the PORTA interrupt is not usable when we're multiplexing between
; input and output, we have to poll for communications
;	Time if no clock: 16 cycles
;	Input:		None
;	Output:		None
;	Side Effects:	WREG destroyed
;	Requires:	Comm.GetByte
;			Comm.SendByte
;******************************************************************************

CheckComm

	btfsc	CLOCKPIN		;   If the clock pin is high, exit. 
	goto	CommDone		

	call	Comm.GetByte		;   Get a byte from the PIC16C745.
	andlw	0xF8			; Check for any errors in W.
	btfss	STATUS,Z	
	goto	CommDone	

	pageselw	CommandTableStart
	movlw	CommandTableStart	;   Get the byte read and use it to
	addwf	Comm.RxTxByte,w		; index into our jump table.  If
	btfsc	STATUS,C		; we crossed a 256 byte boundary, then
	incf	PCLATH,f	        ; increment PCLATH. Then load the 
	movwf	PCL		        ; program counter with computed goto.

CommandTableStart
	goto	GetVersion		; 0x00 - Get Version
	goto	GetRTSample		; 0x01 - Get Real Time sample
	goto	Configure		; 0x02 - stub
	goto	Go			; 0x03 - stub
	goto	ReadBuffer		; 0x04 - Read Buffer, just sends Vout
	goto	AreYouThroughYet	; 0x05
	goto	CommDone		; 0x06
	goto	CommDone		; 0x07

;------------------------------------------------------------------------------
GetVersion

	call	BusTurnAroundDelay
	
	movlw	VERSION			;   Send the encoded version number
	call	Comm.SendByte		; back to the PC.

	goto	CommDone

;------------------------------------------------------------------------------
GetRTSample

	call	BusTurnAroundDelay

	movf	PCReading,w		;   Send the Vout value back to the
	call	Comm.SendByte		; PC, high byte first.

	movf	PCReading+1,w
	call	Comm.SendByte

	goto	CommDone

;------------------------------------------------------------------------------
Configure

	call	Comm.GetByte		;   Stub out this function for
	call	Comm.GetByte		; compatibility.
	call	BusTurnAroundDelay 
	goto	CommDone
	
;------------------------------------------------------------------------------
Go
	goto	CommDone		;   Stub out this function for
					; compatibility.

;------------------------------------------------------------------------------
AreYouThroughYet		
	call	BusTurnAroundDelay	;   For compatibility only.
	movlw	'.'		 
	call	Comm.SendByte
	
	goto	CommDone
	
;------------------------------------------------------------------------------
ReadBuffer
	call	Comm.GetByte		;   For compatibility only.
	movwf	Temp

ReadBufferLoop
	call	BusTurnAroundDelay

	movf	PCReading, w
	call	Comm.SendByte	

	decfsz	Temp,f
	goto	ReadBufferLoop
	goto	CommDone

;------------------------------------------------------------------------------
CommDone

	retlw	0			

;******************************************************************************
; ProgramConfigReg
;
; This routine uses I2C routines to program the configuration register based
; on constants in the code.  
;
;	Input:		None
;	Output:		None
;	Side Effects:	WREG destroyed
;	Time: 		723 cycles
;******************************************************************************

ProgramConfigReg

	call	I2C_StartBit
	movlw	(MCP9800_ADDRESS << 1) | I2C_WRITE
	call	I2C_TransmitByte
	movlw	MCP9800_CONFIGURATION
	call	I2C_TransmitByte
	movlw	MCP9800_ONESHOT_OFF|RESOLUTION|FAULT_QUEUE|MCP9800_ALERT_ACTIVE_HIGH|MCP9800_COMP_MODE|MCP9800_SHUTDOWN_OFF
	call	I2C_TransmitByte
	call	I2C_StopBit

	retlw	0

;******************************************************************************
; ProgramAlertTemperature
;
; This routine programs the alert temperature to the value in AlertTemp.  It 
; then programs the hysteresis register to the alert temperature minus the 
; value of THYST_DELTA.  The least significant bits of both temperatures are
; programmed to 0.
;
;	Input:		None
;	Output:		None
;	Side Effects:	WREG destroyed
;******************************************************************************

ProgramAlertTemperature

	call	I2C_StartBit				;   Program the limit 
	movlw	(MCP9800_ADDRESS << 1) | I2C_WRITE	; set register to the 
	call	I2C_TransmitByte			; alert temperature.
	movlw	MCP9800_LIMITSET
	call	I2C_TransmitByte
	movf	AlertTemp, w
	call	I2C_TransmitByte
	clrw	
	call	I2C_TransmitByte
	call	I2C_StopBit
	nop
	nop
	nop

	call	I2C_StartBit				;   Program the 
	movlw	(MCP9800_ADDRESS << 1) | I2C_WRITE	; hysteresis register
	call	I2C_TransmitByte			; to a value THYST_DELTA 
	movlw	MCP9800_HYSTERESIS			; degrees below the 
	call	I2C_TransmitByte			; alert temperature.
	movlw	THYST_DELTA
	subwf	AlertTemp, w
	call	I2C_TransmitByte
	clrw	
	call	I2C_TransmitByte
	call	I2C_StopBit

	retlw	0

;******************************************************************************
; BitBangSPI
;
; This routine sends the value in W to the PIC16F737 using a bit-banged 
; SPI protocol.  Note that this routine uses an idle low clock.
;
;	Input:		WREG
;	Output:		None
;	Side Effects:	WREG destroyed
;******************************************************************************

BitBangSPI

	movwf	SPIdata			;   Save off W so we can check for
					; communications without losing the
					; value.

        movlw 	h'08'   		;   Set up a counter to send out the 
	movwf	SPIcounter		; 8 bits.

SendSPI

       	btfsc   SPIdata, 7	        ;   Check the MSBit of the byte to send.
	bsf     SPI_DATA		; If it is set, then set the data line 
	bsf     SPI_CLOCK		; high (default is low).  Pulse the 
	bcf	SPI_CLOCK		; clock line high, then low.  Then 
	bcf     SPI_DATA		; clear the data line.

	rlf	SPIdata,f	        ;   Rotate the byte we're sending to 
	decfsz	SPIcounter, f	        ; get the next bit in the MSBit.  If we
	goto	SendSPI		        ; haven't done all 8 bits, loop back up.

	retlw	0

;******************************************************************************
; BusTurnAroundDelay
;
; This routine is called as part of the protocol to talk to the PIC16C745
;
;	Input:		None
;	Output:		None
;	Side Effects:	WREG destroyed
;******************************************************************************

BusTurnAroundDelay

	if CLOCKSPEED == .4000000
	    movlw	.60	;  4 Mhz delay
	else
	    if CLOCKSPEED == .8000000
		movlw	.120	; 8 Mhz delay
	    else
		if CLOCKSPEED == .20000000
		    movlw	0	; 20 Mhz delay 
		else 
		    messg "HalfBitTime delay not configured for clockspeed"
		endif
	    endif
	endif

	movwf	Temp
	decfsz	Temp,f
	goto	$-1
	return

;******************************************************************************
; ADCSettlingDelay
;
; This delay routine is used to wait for the ADC to settle after selecting the 
; channel (>=12 microseconds).
;
;	Input:		None
;	Output:		None
;	Side Effects:	WREG destroyed
;	Time: 		12 cycles (4 MHz)
;             		24 cycles (8 MHz)
;******************************************************************************

ADCSettlingDelay

	if CLOCKSPEED == .4000000
	    movlw	3		;  4 Mhz delay
	else
	    if CLOCKSPEED == .8000000
		movlw	7		;  8 Mhz delay
	    endif
	endif
	movwf	Temp
	decfsz	Temp, f
	goto	$-1

	return

        end
