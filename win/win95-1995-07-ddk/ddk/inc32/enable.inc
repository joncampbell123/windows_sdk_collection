;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

PAGE 58,132
;******************************************************************************
;TITLE Vmini.inc - Vmini VxD includes
;******************************************************************************
;
;   Title:      mini.inc
;
;==============================================================================

        .386p

;------------------------------------------------------------------------------
; 			Define Services 
;------------------------------------------------------------------------------

Begin_Service_Table ENABLE
  ENABLE_Service	VMINI_GetVersion,LOCAL
  ENABLE_Service 	VMINI_Update,LOCAL
  ENABLE_Service 	VMINI_Status,LOCAL
  ENABLE_Service	VMINI_DisplayError,LOCAL
  ENABLE_Service	VMINI_SetTimeStamp, LOCAL
  ENABLE_Service	VMINI_Siren, LOCAL
  ENABLE_Service	VMINI_RegisterAccess,LOCAL
  ENABLE_Service	VMINI_GetData,LOCAL
  ENABLE_Service	VMINI_ShutDownItem,LOCAL
  ENABLE_Service	VMINI_RegisterSK,LOCAL
End_Service_Table ENABLE

; ENABLE Version number

ENABLE_VERSION		equ 0400h


;
; Define equates for sound
;

no_tone_len     equ      1              ; very soft click                   
low_tone_len    equ     20              ; 0.01 seconds                      
high_tone_len   equ     80              ; 0.04 seconds                      
click_tone_len  equ      3              ;                                   
siren_duration  equ	   40
bell_duration   equ	   250

ifdef NEC_98

VSD_Sound_Port          EQU     35h             ; Hardware I/O port for sound
                                                ; enable/disable             
VSD_Beep		EQU	8

else

VSD_Sound_Port          EQU     61h             ; Hardware I/O port for sound
                                                ; enable/disable             

endif

VSD_Counter_Enable      EQU     00000001b       ; Bit enables counter        
VSD_Counter_Enable_Bit  EQU     0                                            
VSD_Data                EQU     00000010b       ; Sound "data" bit           
VSD_Data_Bit            EQU     1                                            

;
; Define siren parameters
;

	; Milliseconds between siren tone changes
TIMERBLOCK		equ	3		; milliseconds between timer events
LOW_TONE_VALUE     	equ 	6c0h		; low bell
HIGH_TONE_VALUE    	equ	520h		; high bell
BELL_VALUE		equ	538h		; Medium bell
SOFT_TONE		equ	14h		; Soft click
CLICK_TONE_VALUE	equ	6c0h		; click
SIREN_INCREMENT	equ	30		; Increment on sound sirens

