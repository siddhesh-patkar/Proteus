;====================================================================
; Main.asm file generated by New Project wizard
;
; Created:   %%DATE%%
; Processor: %%CONTROLLER%%
; Compiler:  %%COMPILER%%
;====================================================================

;====================================================================
; DEFINITIONS
;====================================================================

#include "%%INCFILE%%"	            ; Include register definition file

;====================================================================
; VARIABLES
;====================================================================

;====================================================================
; RESET and INTERRUPT VECTORS
;====================================================================

      ; Reset Vector
      ASEGN CODE_SEG02:CODE,0   
      ljmp  Start

;====================================================================
; CODE SEGMENT
;====================================================================

      RSEG CODE_SEG:CODE            ; Switch to this code segment.
                                    ; Register bank 0 by default

Start: 			
      ; Write your code here
Loop:	
      jmp   Loop

;====================================================================
          END