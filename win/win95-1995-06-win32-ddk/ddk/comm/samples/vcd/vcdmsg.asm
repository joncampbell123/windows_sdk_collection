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
TITLE vcdmsg.asm -
;******************************************************************************
;
;   Title:	vcdmsg.asm
;
;==============================================================================
.386

	.xlist
	include vmm.inc
        INCLUDE MSGMACRO.INC
CREATE_MESSAGES EQU VMM_TRUE
        INCLUDE VCDMSG.INC
	.list

VxD_IDATA_SEG
BeginMsg
;
; INI entry strings for COM port specifications:
;
PUBLIC Com_Boost_Time
%OUT should this become per COM port boost?
Com_Boost_Time	db  'COMBOOSTTIME'
; Number of sec. to boost a VM when it receives a COM interrupt.
; DEFAULT = .002   (2 msec)


; COMxBASE - specifies base I/O port address (in hex).

; COMxIRQ - specifies IRQ # for COM port's interrupts (in decimal).

; COMxAUTOASSIGN -


PUBLIC VCD_INI_Buf, VCD_INI_Num, VCD_Base_Ini, VCD_IRQ_Ini, VCD_assign_Ini

VCD_Base_Ini	db 'BASE', 0
		?ini_size  = $ - VCD_Base_Ini

VCD_IRQ_Ini	db 'IRQ', 0
		IF ($ - VCD_IRQ_Ini) GT ?ini_size
		?ini_size = $ - VCD_IRQ_Ini
		ENDIF

VCD_assign_Ini	db 'AUTOASSIGN', 0
		IF ($ - VCD_assign_Ini) GT ?ini_size
		?ini_size = $ - VCD_assign_Ini
		ENDIF

VCD_INI_Buf	db  'COM'
VCD_INI_Num	db  2 DUP (?)
		db  ?ini_size DUP (?)

IFDEF SHARE_IRQ
PUBLIC COM_IRQs_Sharable
COM_IRQs_Sharable db 'COMIRQSHARING', 0
; Set to TRUE, if COM IRQ's are sharable between multiple COM ports, or with
; other devices.  DEFAULT for MCA and EISA machines is TRUE, FALSE for all
; others.  Machines with COM3 or COM4 on the same interrupt as either COM1
; or COM2, probably need this switch set to TRUE.
ENDIF

;------------------------------------------------------------------------------
;The following switches are new for 3.1
;
PUBLIC COM_30_Support
COM_30_Support	db 'COMMDRV30', 0
; Set to TRUE, if COMM.DRV is a 3.0 com driver.  If true, then VCD uses it's
; own copy of the com driver's interrupt handler to improve com performance.
; Default=FALSE, if the standard 3.1 COMM.DRV is used, then this switch should
; be set to FALSE or nothing.

PUBLIC Com_Ports_Ini
Com_Ports_Ini	db  'MAXCOMPORT', 0
; Number of supported COM ports.  DEFAULT = 4  (e.g. COM1 thru COM4)

EndMsg

PUBLIC COM_Device_Name_Base, COM_Device_Name_Num_Off
COM_Device_Name_Base	db 'COM'
COM_Device_Name_Num	db '0'
			db '    '
.errnz	$ - COM_Device_Name_Base - 8	; only 8 characters!

COM_Device_Name_Num_Off db  OFFSET COM_Device_Name_Num - COM_Device_Name_Base

VxD_IDATA_ENDS


VxD_PAGEABLE_DATA_SEG

PUBLIC verify_base_ini
verify_base_ini db  'COMVERIFYBASE', 0
; Set to TRUE, if base values specified with the COMxBASE switch should be
; verified before being used.  The default is TRUE.

VxD_PAGEABLE_DATA_ENDS


END
