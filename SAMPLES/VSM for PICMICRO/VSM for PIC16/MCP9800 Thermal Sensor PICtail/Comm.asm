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
;   Serial Communication Interface functions.                                        *
;                                                                   *
;********************************************************************
;   Filename:		Comm.asm                                     *
;   Date:               27 February 2004                            *
;   File Version:       1.00                                        *
;********************************************************************
;  Macros:
;
;  Functions:
;	Comm.GetByte
;	Comm.SendByte

;  Public Variables
;	Comm.RxTxByte	- byte to send or last byte received

; Required pin definitions.  These #defines should be made in the project.inc file
;	#define DATAPIN		PORTA,0
;	#define CLOCKPIN	PORTA,1
;********************************************************************
;   Revision History:                                               *
;      V1.00 - 27 Feb 2004 Initial Release                          *
;********************************************************************
#include <Project.inc>
	udata
Comm.RxTxByte	res	1
	global	Comm.RxTxByte

	udata_shr
BitCounter	res	1
DelayCounter	res	1
		
	code
; *********************************************************************
; Reads a byte from the Synchronous Serial link
; Returns byte in W when the databyte is complete
; *********************************************************************
Comm.GetByte
	global	Comm.GetByte

	clrf	Comm.RxTxByte
	movlw	8
	movwf	BitCounter
	
	btfsc	CLOCKPIN	; Is the clock low?
	goto	$-1		; no: keep waiting
GBLoop
 	btfss	CLOCKPIN	; Is the clock high?
	goto	$-1		; yes - go read the incoming byte

	bcf	STATUS,C	; clear carry now to save the cycle after falling edge

	btfsc	CLOCKPIN	; waiting for low, is it?
	goto	$-1

	btfsc	DATAPIN		; We've already assumed the bit is a 0 is it?
	bsf	STATUS,C	; no, set carry
	rrf	Comm.RxTxByte,f	; rotate carry into byte

	decfsz	BitCounter,f
	goto	GBLoop

	movf	Comm.RxTxByte,w
	return

	
	
; *********************************************************************
; SendByte
; Sends a byte in Synchronous Master Mode.  
; Master reads it on the falling edge.
; *********************************************************************
Comm.SendByte
	global	Comm.SendByte
	movwf	Comm.RxTxByte
	bsf	CLOCKPIN
	bsf	STATUS,RP0	; bank 1
	bcf	CLOCKPIN
	bcf	DATAPIN
	bcf	STATUS,RP0	; bank 0
	movlw	8
	movwf	BitCounter
SCBSLoop
	bsf	CLOCKPIN

	call	HalfBitTimeDelay
	btfsc	Comm.RxTxByte,0	; is the bit high
	bsf	DATAPIN	; yes
	btfss	Comm.RxTxByte,0	; no: is it low
	bcf	DATAPIN	; 
	rrf	Comm.RxTxByte,f
	call	HalfBitTimeDelay

	bcf	CLOCKPIN
	call	BitTimeDelay
		
	decfsz  BitCounter,f
	goto	SCBSLoop
			
EndSCBS
	call	BitTimeDelay ; add in extra delay after
	bcf	DATAPIN
	bsf	STATUS,RP0	; bank 1
	bsf	CLOCKPIN	; clock and data as input
	bsf	DATAPIN
	bcf	STATUS,RP0	; bank 0
	return


HalfBitTimeDelay
	if CLOCKSPEED == .4000000
	    movlw	.4	;  4 Mhz delay
	else
	    if CLOCKSPEED == .8000000
		movlw	.8	;  8 Mhz delay
	    else
		if CLOCKSPEED == .20000000
		    movlw	.6	; 20 Mhz delay
		else 
		    messg "HalfBitTime delay not configured for clockspeed"
		endif
	    endif
	endif
	goto	$+2
BitTimeDelay
	if CLOCKSPEED == .4000000
	    movlw	.7	;  4 Mhz delay
	else
	    if CLOCKSPEED == .8000000
		movlw	.16	;  8 Mhz delay
	    else
		if CLOCKSPEED == .20000000
		    movlw	.12	; 20 Mhz delay
		else 
		    messg "HalfBitTime delay not configured for clockspeed"
		endif
	    endif
	endif
	movwf	DelayCounter
	decfsz	DelayCounter,f
	goto	$-1
	return

	end
