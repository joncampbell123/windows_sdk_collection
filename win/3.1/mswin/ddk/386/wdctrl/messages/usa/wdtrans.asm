PAGE 58,132
;******************************************************************************
TITLE WDTRANS.ASM-Strings that must be translated to localize WDCTRL
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1991
;
;   Title:	WDTRANS.ASM-Strings that must be translated to localize WDCTRL
;
;   Version:	1.00
;
;   Date:	18-Jan-1991
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   18-Jan-1991 RAL Original
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	.LIST

VxD_IDATA_SEG

PUBLIC	WD_Init_Error_String
PUBLIC	WD_Time_Out_Ini_String
PUBLIC	WD_IO_Access_Error

WD_Init_Error_String LABEL BYTE
	db	 "The Microsoft Windows 32-bit disk driver (WDCTRL) cannot be loaded. There", 13, 10
	db	 "is a conflict with another virtual device.", 0

WD_Time_Out_Ini_String db "WDCtrlTimeOut", 0

VxD_IDATA_ENDS

PUBLIC WD_Time_Out_Error_Title
PUBLIC WD_Time_Out_Error_Message

VxD_DATA_SEG

WD_Time_Out_Error_Title LABEL BYTE
    db	"Disk Time-Out Error", 0

WD_Time_Out_Error_Message LABEL BYTE
    db	"The hard disk on your computer has failed to respond. The Windows 32-bit "
    db	"disk-access feature (WDCTRL) may not be compatible with your computer's "
    db	"hard-disk controller. This is often the case with battery-powered portable "
    db	"computers whose power-saving features are enabled. ", 13, 10
    db	10
    db	"You can turn off 32-bit disk access by choosing the 386 Enhanced icon "
    db	"in Control Panel and then using the Virtual Memory dialog box. Or you can "
    db	"specify 32BitDiskAccess=Off in the [386Enh] section of the SYSTEM.INI file.", 0


WD_IO_Access_Error LABEL BYTE
    db	"This program tried to access your hard disk in a way that is incompatible "
    db	"with the Windows 32-bit disk-access feature (WDCTRL). This may cause your "
    db	"system to become unstable.", 0

VxD_DATA_ENDS


VxD_REAL_INIT_SEG

PUBLIC	No_Fixed_Disk_String
PUBLIC	Invalid_Win386_Ver_String
PUBLIC	Invalid_IRQ_String
PUBLIC	Invalid_Controller_String
PUBLIC	Validation_Failed_String
PUBLIC	Invalid_DOS_Ver_String
PUBLIC	Invalid_Int13_Chain
PUBLIC	WD_Fatal_Error_Code
PUBLIC	WD_Fatal_Error_Msg
PUBLIC	WD_Incompatible_Sw_Msg
PUBLIC	Pause_String
PUBLIC  WD_Env_String_Bail
PUBLIC  ES_Debug      
PUBLIC  ES_Debug_Len  
PUBLIC  ES_Disable    
PUBLIC	ES_Disable_Len
PUBLIC	PS_Enable_Wdctrl
PUBLIC  PS_Force_Enable_80  
PUBLIC  PS_Force_Enable_81  
PUBLIC	PS_Force_Alt_Status0
PUBLIC	PS_Force_Alt_Status1


No_Fixed_Disk_String LABEL BYTE
	db	"No fixed disk drive was detected on this computer. The Microsoft Windows", 13, 10
	db	"32-bit disk driver (WDCTRL) cannot be loaded. $"

Invalid_Win386_Ver_String LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) requires Windows", 13, 10
	db	"version 3.1 or later. $"

Invalid_IRQ_String LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) cannot be loaded on", 13, 10
	db	"this computer because of interrupt conflicts. $"

Invalid_Controller_String LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) is incompatible with", 13, 10
	db	"this computer's hard-disk controller. $"

Validation_Failed_String LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) validation failed at", 13, 10
	db	"phase $"

Invalid_Int13_Chain LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) cannot be loaded. There", 13, 10
	db	"is unrecognizable disk software installed on this computer.", 13, 10
	db	10
	db	"The address that MS-DOS uses to communicate with the hard disk has been", 13, 10
	db	"changed. Some software, such as disk-caching software, changes this address.", 13, 10
	db	10
	db	"If you aren't running such software, you should run a virus-detection", 13, 10
	db	"program to make sure there is no virus on your computer.$"

Invalid_DOS_Ver_String	  LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) requires MS-DOS version", 13, 10
	db	"3.10 or later.$"

WD_Fatal_Error_Msg LABEL BYTE
	db	"Fatal error #"
WD_Fatal_Error_Code LABEL BYTE
	db	"0"			;Value to be inserted here
	db	" during the initialization of the Microsoft", 13, 10
	db	"Windows 32-Bit disk driver (WDCTRL).  Restart your computer by pressing", 13, 10
	db	"CTRL+ALT+DEL.$"

WD_Incompatible_Sw_Msg LABEL BYTE
	db	"Fatal error. Disk software is installed that is incompatible with the", 13, 10
	db	"Microsoft Windows 32-bit disk driver (WDCTRL). Restart your computer by", 13, 10
	db	"pressing CTRL+ALT+DEL.$"

WD_Env_String_Bail LABEL BYTE
	db	"The Microsoft Windows 32-bit disk driver (WDCTRL) will not be loaded because", 13, 10
	db	"WDCTRLDISABLE=Y is set in this machine's environment. Remove this environment", 13, 10
	db	"string and the 32-bit disk driver will load normally.$"

Pause_String LABEL BYTE
	db	13, 10, 10
	db	"To continue starting Windows without using the 32-bit", 13, 10
	db	"disk driver, press any key.$"


BeginDoc
;******************************************************************************
;
;WDCTRL Environment variable strings
;
;WDCTRLDEBUG:
;       This is for debugging of WDCTRL VxDs and when TRUE, forces
;       the debugger to stop at an INT 3 in the real mode initialization.
;       The default for this switch is FALSE.  TRUE values include strings
;       starting with 1, Y, y, T, and t.
;
;WDCTRLDISABLE:
;       When TRUE, disables the installation of the WDCTRL VxD.  This allows
;       a test of system start without the VxD.  This is meant to be
;       a temporary disable as it brings up a message on each startup.
;       The default for this switch is FALSE.  TRUE values include strings
;       starting with 1, Y, y, T, and t.
;
;==============================================================================
EndDoc

ES_Debug                db      "WDCTRLDEBUG="
ES_Debug_Len            db      $ - ES_Debug
ES_Disable              db      "WDCTRLDISABLE="
ES_Disable_Len          db      $ - ES_Disable

BeginDoc
;******************************************************************************
;
;WDCTRL SYSTEM.INI Profile strings
;
;32BITDISKACCESS:
;	This switch MUST be set to true in System.Ini to enable WDCTRL.  It's
;	default value is FALSE.
;
;WDCTRLDRIVE0:
;WDCTRLDRIVE1:
;       These switches are used to force WDCTRL to be enabled on either
;       physical drive 0 or drive 1.  Setting these switches is very
;       dangerous as it bypasses all of WDCTRL's validation process for
;       determining whether the drive is compatible.  These are boolean
;       switches and the default is TRUE.
;
;WDCTRLALTSTATUS0:
;WDCTRLALTSTATUS1:
;       This switch is set ONLY when one or both of the above strings are
;       set.  Setting this switch causes WDCTRL to use the alternate status
;       register instead of the "normal" status register.  This switch should
;       only be set if necessary for the drive/controller combination.
;       This is a boolean switch and the default is FALSE.
;
;==============================================================================
EndDoc

PS_Enable_Wdctrl	db	"32BITDISKACCESS", 0
PS_Force_Enable_80      db      "WDCTRLDRIVE0", 0
PS_Force_Enable_81      db      "WDCTRLDRIVE1", 0
PS_Force_Alt_Status0	db	"WDCTRLALTSTATUS0", 0
PS_Force_Alt_Status1	db	"WDCTRLALTSTATUS1", 0

VxD_REAL_INIT_ENDS

	END
