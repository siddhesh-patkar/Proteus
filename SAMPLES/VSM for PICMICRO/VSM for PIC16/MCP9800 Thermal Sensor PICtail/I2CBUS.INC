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

; I2CBUS.INC

; This file contains all the definitions needed to link in the file
; I2CBUS.ASM.


	extern	I2C_ErrorCode
	extern	I2C_Flags
	extern	I2C_ReceiveByte
	extern	I2C_StartBit
	extern	I2C_StopBit
	extern	I2C_SendACK
	extern	I2C_SendNAK
	extern	I2C_TransmitByte

#define I2C_READ	1
#define I2C_WRITE	0
