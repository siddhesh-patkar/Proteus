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

; I2CBUS.ASM

; This file contains the routines necessary to bit-bang an I2C interface
; on a mid-range PICmicro device.

; Best Case Timings:
;
;	I2C_ReceiveByte		246 cycles
;	I2C_StartBit		11 cycles
;	I2C_StopBit		16 cycles
;	I2C_SendACK		20 cycles
;	I2C_SendNAK		20 cycles
;	I2C_TransmitByte	227 cycles

; Version   Date        Description
;--------------------------------------------------------------------
; 0.00.01   xx-Dec-2004 KLO Initial Release

#include "project.inc"

	global	I2C_ErrorCode
	global	I2C_Flags
	global	I2C_ReceiveByte
	global	I2C_StartBit
	global	I2C_StopBit
	global	I2C_SendACK
	global	I2C_SendNAK
	global	I2C_TransmitByte


#define I2C_BUS_BUSY_CLOCK	1	; Clock locked low by device (bus is still busy)
#define I2C_BUS_BUSY_DATA	2	; Data locked low by device (bus is still busy)
#define I2C_NO_ACK		3	; No acknowledge from device (no handshake)
#define I2C_NO_STOP		4	; Data bus not released for master to generate STOP bit

#define I2C_INPUT_BIT		7	; I2C_TempBuffer bit number of current input bit value
#define I2C_OUTPUT_BIT		6	; I2C_TempBuffer bit number of current output bit value

#define I2C_ERROR_BIT		0	; I2C_Flags error indicator bit

;        udata_shr
        udata

I2C_Counter		res	1
I2C_ErrorCode		res	1
I2C_Flags		res	1
I2C_ReceiveBuffer	res	1
I2C_TransmitBuffer	res	1
I2C_TempBuffer		res	1


	code
;-----------------------------------------------------------------------------
; I2C_Error
;
; This routine is called when an error has occurred.  If there is currently
; no error, then store this error code and set the error flag bit.
;
;         code          error status mode
;       -------         ------------------------------------------------------
;           1   :       SCL locked low by device (bus is still busy)
;           2   :       SDA locked low by device (bus is still busy)
;           3   :       No acknowledge from device (no handshake)
;           4   :       SDA bus not released for master to generate STOP bit
;
;	Input	: WREG			ErrorCode
;	Output	: I2C_ErrorCode		Error code
; 	Affects : none
;	Time	: 5 cycles
;-----------------------------------------------------------------------------

I2C_Error

	btfss   I2C_Flags, I2C_ERROR_BIT	;   If we do not currently
	movwf   I2C_ErrorCode          		; have an error code stored,
	bsf     I2C_Flags, I2C_ERROR_BIT        ; store this one and set the
						; error flag.
	retlw   0

;-----------------------------------------------------------------------------
; I2C_StartBit
;
; This routine generates a start bit to begin communication.  A start bit is
; characterized by the data line going from high to low while the clock line
; is high.  If we cannot successfully drive the clock line high, then the bus 
; is busy, and we report an error.
;
;	Input	: none
;	Output	: none
; 	Affects : none
;	Time	: 17 cycles with errors
;		  11 cycles with no errors
;-----------------------------------------------------------------------------

I2C_StartBit

	bsf     I2C_DATA          	;   Set the data and clock lines high
	bsf     I2C_CLOCK          	; to begin the start bit.

	movlw   I2C_BUS_BUSY_CLOCK      ;   If we cannot drive the clock line
	btfss   I2C_CLOCK          	; high, report an error.
	call    I2C_Error             

	bcf     I2C_DATA          	;   Set the data line low, and then
	nop                     	; set the clock line low to end the
	nop				; start bit.
	bcf     I2C_CLOCK          

	retlw	0

;-----------------------------------------------------------------------------
; I2C_StopBit
;
; This routine generates a stop bit to end communications.  A stop bit is 
; characterized by the data line going from low to high while the clock line
; is high.  If we cannot successfully drive the clock or the data line high, 
; then the bus is busy, and we report an error.
;
;	Input	: none
;	Output	: none
; 	Affects : none
;	Time	: 28 cycles with errors
;		  16 cycles with no errors
;-----------------------------------------------------------------------------

I2C_StopBit

	bcf     I2C_DATA		;   Set the data line low and the
	bsf     I2C_CLOCK          	; clock line high to begin the stop
	nop				; bit.
	nop

	movlw   I2C_BUS_BUSY_CLOCK      ;   If we cannot drive the clock line
	btfss   I2C_CLOCK          	; high, report an error.
	call    I2C_Error             

	bsf     I2C_DATA                ;   Set the data line high.
	nop
	nop
	nop

	movlw   I2C_NO_STOP             ;   If we cannot drive the data line
	btfss   I2C_DATA          	; high, report an error.
	call    I2C_Error             

	retlw   0

;-----------------------------------------------------------------------------
; I2C_BitIn
;
; This routine receives one data bit from the bus and places it in the 
; temporary input buffer.  The data bit is read by driving the clock high,
; reading the data bit, and then setting the clock low.  If we cannot drive
; the clock high, then we report an error.
;
;	Input	: none
;	Output	: I2C_TempBuffer	I2C_INPUT_BIT is new data bit
; 	Affects : WREG destroyed
;	Time	: 27 cycles with errors
;		  21 cycles with no errors
;-----------------------------------------------------------------------------

I2C_BitIn

	movlw   I2C_TRIS_DATA_IN	;   Set the data line to an input. 
	bsf	STATUS, RP0		; Make sure we do not drive the line
	errorlevel	-302		; low.
	movwf	I2C_TRIS		
	bcf	STATUS, RP0
	errorlevel	+302
	bsf	I2C_DATA

	bcf     I2C_TempBuffer, I2C_INPUT_BIT
	bsf     I2C_CLOCK          	;   Set the clock line high to clock 
	movlw   I2C_BUS_BUSY_CLOCK	; out the bit.  If we cannot drive it
	btfsc   I2C_CLOCK          	; high, report an error.
	goto    I2C_BitInRead
	call    I2C_Error

I2C_BitInRead

	btfsc   I2C_DATA          		;   Read the data line, and 
	bsf     I2C_TempBuffer, I2C_INPUT_BIT  	; set the buffer bit to 
	nop                     		; match. Then return the clock
	bcf     I2C_CLOCK          		; to low.

	movlw   I2C_TRIS_DATA_OUT	;   Set the data line to an output.
	bsf	STATUS, RP0
	errorlevel	-302
	movwf	I2C_TRIS
	bcf	STATUS, RP0
	errorlevel	+302

	retlw   0

;-----------------------------------------------------------------------------
; I2C_BitOut
;
; This routine takes the data bit from the temporary input buffer and sends it
; on the bus.  The data bit is sent by setting the data line, then driving the
; clock line high then low.  If we cannot drive the clock or data line high, 
; then we report an error.
;
;	Input	: I2C_TempBuffer	I2C_OUTPUT_BIT is transmitted data bit
;	Output	: none
; 	Affects : WREG destroyed
;	Time	: 29 cycles with errors
;		  15 cycles with no errors
;-----------------------------------------------------------------------------

I2C_BitOut

	btfss   I2C_TempBuffer, I2C_OUTPUT_BIT	;   See if we need to send a
	goto    I2C_BitOutLow			; 1 or a 0.

	bsf     I2C_DATA          	;   Send a high bit.  If we can't
	movlw   I2C_BUS_BUSY_DATA	; drive it high, give an error.
	btfsc   I2C_DATA          
	goto    I2C_BitOutClock
	call    I2C_Error
	goto    I2C_BitOutClock            

I2C_BitOutLow

	bcf     I2C_DATA		;   Send a low bit.
	nop                     

I2C_BitOutClock

	bsf     I2C_CLOCK		;   Set the clock high.  If we can't
	movlw   I2C_BUS_BUSY_CLOCK      ; drive it high, give an error.
	btfsc   I2C_CLOCK        
	goto    I2C_BitOutDone   
	call    I2C_Error

I2C_BitOutDone

	bcf     I2C_CLOCK 		;   Return the clock to low.

	retlw   0

;-----------------------------------------------------------------------------
; I2C_SendACK
;
; This routine sends an acknowledge bit.
;
;	Input	: none
;	Output	: none
; 	Affects : WREG destroyed
;	Time	: 5 + I2C_BitOut = 34 cycles with errors
;		                   20 cycles with no errors
;-----------------------------------------------------------------------------

I2C_SendACK

	bcf     I2C_TempBuffer, I2C_OUTPUT_BIT	;   Set the acknowledge bit to
	call    I2C_BitOut        		; to stop further input.

	retlw	0

;-----------------------------------------------------------------------------
; I2C_SendNAK
;
; This routine sends a negative acknowledge bit.
;
;	Input	: none
;	Output	: none
; 	Affects : WREG destroyed
;	Time	: 5 + I2C_BitOut = 34 cycles with errors
;		                   20 cycles with no errors
;-----------------------------------------------------------------------------

I2C_SendNAK

	bsf     I2C_TempBuffer, I2C_OUTPUT_BIT	;   Set the acknowledge bit to
	call    I2C_BitOut        		; to stop further input.

	retlw	0

;-----------------------------------------------------------------------------
; I2C_ReadACK
;
; This routine reads an acknowledge bit.  If an acknowledge bit is not 
; received, an error is reported.
;
;	Input	: none
;	Output	: none
; 	Affects : WREG destroyed
;	Time	: 8 + I2C_BitIn + I2C_Error = 40 cycles with errors
;		  7 + I2C_BitIn             = 28 cycles with no errors
;-----------------------------------------------------------------------------

I2C_ReadACK

	call    I2C_BitIn		;   Read the acknowledge bit.  If there
	movlw   I2C_NO_ACK		; is no acknowledgement, set an error.
	btfsc   I2C_TempBuffer, I2C_INPUT_BIT       
	call    I2C_Error             

	retlw	0

;-----------------------------------------------------------------------------
; I2C_ReceiveByte
;
; This routine receives one byte from the bus.  It calls the routine I2C_BitIn
; once for each bit, and rotates each new bit into the receive buffer.  No 
; acknowledge bit is sent or read, since the master might need to send
; either an ACK or a NAK.
;
;	Input	: none
;	Output	: WREG			Received data byte
; 	Affects : none
;	Time	: 6 + 8*(9+I2C_BitIn) = 294 cycles with errors
;                                     = 246 cycles with no errors
;-----------------------------------------------------------------------------

I2C_ReceiveByte

	movlw   8              		;  Get ready to shift in 8 bits of data.
	movwf   I2C_Counter
	clrf    I2C_ReceiveBuffer

RXLP
	bcf	STATUS, C		;   Rotate the input byte to get ready
	rlf     I2C_ReceiveBuffer, f    ; for the next bit.  Get the bit, and
	call    I2C_BitIn		; copy it into the receive buffer.
	btfsc	I2C_TempBuffer, I2C_INPUT_BIT
	bsf     I2C_ReceiveBuffer,0

	decfsz  I2C_Counter, f	        ;   If we have not read 8 bits, go back
	goto    RXLP			; and get the next bit.

	movf	I2C_ReceiveBuffer, w	;   Put the received byte into W.

	return

;-----------------------------------------------------------------------------
; I2C_TransmitByte
;
; This routine transmits one byte on the bus.  It calls the routine I2C_BitOut
; once for each bit.  It then reads the acknowlegde bit.
;
;	Input	: WREG			Data to transmit
;	Output	: none
; 	Affects : WREG destroyed
;	Time	: 7 + I2C_ReadACK + 8*(9+I2C_BitOut) = 351 cycles, errors
;		                                       227 cycles, no errors         
;-----------------------------------------------------------------------------

I2C_TransmitByte

	movwf	I2C_TransmitBuffer	;   Store the byte to transmit.

	movlw   8			;   Get ready to shift out 8 bits.
	movwf   I2C_Counter

TXLP
	bcf     I2C_TempBuffer, I2C_OUTPUT_BIT	;   Copy the most significant
	btfsc   I2C_TransmitBuffer, 7		; bit of the transmit byte into 
	bsf     I2C_TempBuffer, I2C_OUTPUT_BIT	; the temporary buffer.  Then
	call    I2C_BitOut          		; send it.

	rlf     I2C_TransmitBuffer, f	;   Get the next bit into position for
					; sending.  

	decfsz  I2C_Counter, f		;   If we haven't sent 8 bits yet, go
	goto    TXLP			; back and send the next bit.

	call	I2C_ReadACK

	retlw   0

	end
