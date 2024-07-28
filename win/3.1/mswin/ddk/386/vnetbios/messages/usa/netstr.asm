;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	netstr.asm - Strings for VNETBIOS.ASM
;
;   Version:	1.00
;
;   Date:	15-Oct-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   15-Oct-1989 RAL Original
;   29-Oct-1989 RAL Added error messages for terminating VMs with net requests
;
;==============================================================================

	.386p

	PUBLIC	Min_Net_Buf_Str
	PUBLIC	DMA_Net_String
	PUBLIC	Redir_Time_Out_String
	PUBLIC	NetBuff_Error_Title
	PUBLIC	NetBuff_Error_Text
	PUBLIC	VNETBIOS_Cant_Nuke_VM
	PUBLIC	VNETBIOS_Stupid_User
	PUBLIC	VNETBIOS_Big_Huge_Error
	PUBLIC	VNETBIOS_Insuff_Buff_Msg
	PUBLIC	VNETBIOS_Min_Buff_Size
	PUBLIC	VNETBIOS_Hang_Err_Msg
	PUBLIC	Time_Out_Str
	PUBLIC	Map_Non_Buff_Str
	PUBLIC	Poll_Sync_Cmds_String

	INCLUDE VMM.INC


VxD_IDATA_SEG

BeginDoc

;
; SYSTEM.INI things
;
; Specifies the DMA buffer size (in kilobytes) for
; NetBIOS transport software if a network has been
; installed. In this case, the buffer size is the
; larger value between this value and the value of
; DMABufferSize. Value is rounded up to the nearest
; 4K.
;
; DEFAULT
;	Non-MCA (TM) machine  0
;	MCA (TM) machine     32
;
DMA_Net_String	db  "NetDMASize", 0
;
; Specifies the size (in kilobytes) of the buffers
; that Windows in 386 enhanced mode allocates in
; conventional memory for transferring data over a
; network. All values are rounded up to the nearest
; 4K.
;
; DEFAULT
;	12
;
; This value has a MAX which depends on the machine
; config. Values larger than the max will be set to
; the max.
;
Min_Net_Buf_Str db  "NetHeapSize", 0
;
; Specifies the timeout period (in seconds) when
; Windows needs to enter a critical section in
; order to service an asynchronous NetBIOS request.
; It is used only when NetAsynchFallback is
; enabled. This value can include a decimal (such
; as 0.5).
;
; DEFAULT
;	5.0 (five seconds)
;
Time_Out_Str	db  "NetAsynchTimeout", 0
;
; If enabled, tells Windows to attempt to save a
; failing NetBIOS request. When an application
; issues an asynchronous NetBIOS request, Windows
; will attempt to allocate space in its global
; network buffer to receive the data. If there is
; insufficient space in the global buffer, Windows
; will normally fail the NetBIOS request. If this
; setting is enabled, Windows will attempt to save
; such a request by allocating a buffer in local
; memory and preventing any other virtual machines
; from running until the data is received and the
; timeout period (specified by the NetAsynchTimeout
; setting) expires.
;
; DEFAULT
;	FALSE
;
Map_Non_Buff_Str db "NetAsynchFallback", 0
;
; The standard REDIRectors specify a timeout value of 0
; (infinite time out) in all of the NCBs it submits to INT 5Ch.
; This can result in a HANG when servers suddenly go bye-bye.
; This setting (in milliseconds), if set to something non-zero,
; causes VNETBIOS to stuff this value in the time out field
; of all GLOBAL NCBs it sees with a time out of 0 specified.
; Effectively this implements a HACK fix for these redirectors.
;
; DEFAULT
;	0
;
Redir_Time_Out_String db "NetRedirTimeout", 0

;
;   This switch can be used to turn off the automatic conversion of synchronous
;   network commands into async commands. The default for this switch is ON.
;
Poll_Sync_Cmds_String db "NoWaitNetIO", 0

EndDoc

BeginMsg
;
;   This message will be displayed whenever the NetBuff module can not allocate
;   a mapping page for hook control blocks.
;
NetBuff_Error_Title db "Virtual Network Device Error", 0

NetBuff_Error_Text LABEL BYTE
    db	"The Virtual Network Device was unable to initialize. "
    db	"Programs that use the network may not function properly.", 0

EndMsg

VxD_IDATA_ENDS

;******************************************************************************

VxD_DATA_SEG

BeginMsg
;
;   This message will be displayed whenever a user attempts to close a VM
;   that has network requests outstanding.
;
VNETBIOS_Cant_Nuke_VM LABEL BYTE
    db	"This application should not be terminated because it is currently "
    db	"communicating on the network. You should try to quit this "
    db	"application using its exit command.", 0

;
;   This message is displayed if the user exits a VM that has pending network
;   requests and VNETBIOS was able to cancel them.
;
VNETBIOS_Stupid_User LABEL BYTE
    db	"This application was communicating on the network when you terminated "
    db	"it. Although Windows attempted to restore the state of the "
    db	"network, you may have problems with network communications until "
    db	"you restart your computer.", 0

;
;   This message is displayed if the user exits a VM that has pending network
;   requests and VNETBIOS was NOT able to cancel them.
;
VNETBIOS_Big_Huge_Error LABEL BYTE
    db	"This application was communicating on the network when you terminated "
    db	"it. Windows was unable to restore the state of the "
    db	"network. You need to restart your computer.", 0

;
;   This message is displayed whenever a network request is made that is too
;   large to buffer. The Min_Buff_Size field will be filled in by the VNETBIOS
;   device before it displays the message.
;
VNETBIOS_Insuff_Buff_Msg LABEL BYTE
    db	"This application requires a larger buffer for transferring "
    db	"information over the network. You can increase the "
    db	"buffer size by modifying the NetHeapSize setting in "
    db	"your SYSTEM.INI file.",0Dh, 0Ah, 0Ah
    db	"Include or modify the following setting in the [386Enh] "
    db	"section of your SYSTEM.INI file:",0Dh,0Ah,0Ah
    db	"    NetHeapSize="
VNETBIOS_Min_Buff_Size	db  "   "   ; 3 digits for buffer size
    db	0Dh, 0Ah, 0Ah
    db	"If the NetHeapSize value is still too low, this "
    db	"message will appear again, suggesting a new value "
    db	"for you to try.",0

;
;   This message is displayed when a VM enters a critical section due to
;   network I/O and hangs the computer.
;
VNETBIOS_Hang_Err_Msg LABEL BYTE
    db	"This program is causing your system to lock up. Do you want to "
    db	"end it?", 0

EndMsg

VxD_DATA_ENDS

	END
