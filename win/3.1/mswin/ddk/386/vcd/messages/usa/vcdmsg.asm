PAGE 58,132
;******************************************************************************
TITLE vcdmsg.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	vcdmsg.asm -
;
;   Version:	1.00
;
;   Date:	11-Apr-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   11-Apr-1989 RAP
;
;==============================================================================
.386

	.xlist
	include vmm.inc
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

PUBLIC verify_base_ini
verify_base_ini db  'COMVERIFYBASE', 0
; Set to TRUE, if base values specified with the COMxBASE switch should be
; verified before being used.  The default is FALSE.


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


VxD_DATA_SEG
BeginMsg
PUBLIC VCD_IRQ_Conflict_Msg
;
; This message is displayed when VCD detects an IRQ conflict while trying to
; open a COM port for a Windows application.  The user should check their
; system configuration to remove the conflict.
;
VCD_IRQ_Conflict_Msg	label byte
    db	"The requested COM port is not available, because a hardware interrupt "
    db	"conflict has been detected.  You must change your hardware "
    db	"configuration to be able to use this COM port from Windows "
    db	"applications.", 0
EndMsg

BeginMsg
PUBLIC VCD_30_Comm_Driver_Msg
;
; This message is dispalayed when VCD detects that a 3.0 comm driver is
; installed, but COMMDRV30 is not set to TRUE in SYSTEM.INI

VCD_30_Comm_Driver_Msg	label byte
    db	"You have installed a Windows 3.0 compatible communications "
    db	"driver.  To properly support this driver you must set "
    db	'"COMMDRV30=TRUE" in the [386Enh] section of SYSTEM.INI.',0
EndMsg
VxD_DATA_ENDS

END
