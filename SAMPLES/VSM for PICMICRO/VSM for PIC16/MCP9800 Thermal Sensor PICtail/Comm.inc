; Serial Comm include;******************************************************************************
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

	extern	Comm.RxTxByte
	extern	Comm.SendByte
	extern	Comm.GetByte
