;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	VMDA.INC - Include file for VMDOSAPP/GRABBER SHELL interaction
;
;   Version:	1.00
;
;   Date:	05-May-1988
;
;   Author:	ARR
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   05-May-1988 ARR Original
;
;==============================================================================
;
; EQUATES for VMDOSAPP device calls
;
SHELL_Call_Dev_VDD	EQU	0000Ah	; Actually GRABBER services
SHELL_Call_Dev_VMD	EQU	0000Ch
SHELL_Call_Dev_VKD	EQU	0000Dh
SHELL_Call_Dev_SHELL	EQU	00017h

SHELL_Call_Dev_Version	EQU	0

;
; SHELL VMDA interface services
;
SHELL_Get_Ver		  EQU	 0
SHELL_Get_SYSVM_Info	  EQU	 1
SHELL_Set_SYSVM_Info	  EQU	 2
SHELL_Crt_VM		  EQU	 3
SHELL_Destroy_VM	  EQU	 4
SHELL_Set_Focus 	  EQU	 5
SHELL_Get_VM_State	  EQU	 6
SHELL_Set_VM_State	  EQU	 7
SHELL_Debug_Out 	  EQU	 8
SHELL_VMDA_Init 	  EQU	 9
SHELL_VMDA_Exit 	  EQU	10
SHELL_Get_Message_Txt	  EQU	11
SHELL_Event_Complete	  EQU	12
SHELL_Get_Contention_Info EQU	13
SHELL_Get_Clip_Info	  EQU	14
SHELL_Set_Paste 	  EQU	15
SHELL_Switcher_Assist	  EQU	16
SHELL_Get_FileSysChng	  EQU	17
SHELL_Query_Destroy	  EQU	18
;
; WARNING: The following two entries are used by Windows KERNEL/USER.
;	   Do not change them without also changing KERNEL/USER.
;
SHELL_SetFocus_Cur_VM	  EQU	19
SHELL_User_Busy_API	  EQU	20
SHELL_Chng_Hot_Key	  EQU	21
SHELL_Get_TermInfo	  EQU	22

;
; Structure used on SHELL_VMDA_Init call
;
VMDA_Init_Struc struc
VMDA_MssgScnBCol	dd	?
VMDA_MssgScnFCol	dd	?
VMDA_PostMesssageProc	df	?
VMDA_Init_Struc ends

;
; Structure used on SHELL_Switcher_Assist call
;
VMDA_SA_Colors struc
VMDA_SA_DeskTop 	dd	?
VMDA_SA_ActvCap 	dd	?
VMDA_SA_InacCap 	dd	?
VMDA_SA_WindowFrm	dd	?
VMDA_SA_CaptText	dd	?
VMDA_SA_White		dd	?
VMDA_SA_Black		dd	?
VMDA_SA_Colors ends

;
; THIS IS THE MAXIMUM SIZE IN BYTES OF THE INFO RETURNED ON
;	VDD CALLS OTHER THAN GET CONTOLLER STATE MADE BY THE GRABBER
;
; This is the size of an area reserved for use on grabber calls
;
VDD_MOD_MAX		EQU	256

;
; THIS IS THE MAXIMUM SIZE IN BYTES OF THE INFO RETURNED ON
;	THE GET CONTOLLER STATE VDD CALL MADE BY THE GRABBER
;
; This is the size of an area reserved for use on this grabber call
;
VDD_CTRL_STATE_MAX	EQU	128

;
; Stuff specific to VMDA events
;

WMX_USER		 EQU	 0400H

INCLUDE VDDGRB.INC
VDA_Hot_Key		EQU	((WMX_USER+20)+0)
VDA_Switch_Context	EQU	((WMX_USER+20)+1)
.ERRE VDA_Type_Chng	EQ	((WMX_USER+20)+2) ; Defined in VDDGRB.INC!!
VDA_ClipBrd_Event	EQU	((WMX_USER+20)+3)
VDA_Terminated		EQU	((WMX_USER+20)+4)
VDA_Display_Message	EQU	((WMX_USER+20)+5)
.ERRE VDA_Display_Event EQ	((WMX_USER+20)+6) ; Defined in VDDGRB.INC!!
VDA_Crash_Event 	EQU	((WMX_USER+20)+7)
VDA_Paste_Complete	EQU	((WMX_USER+20)+8)
VDA_Contention		EQU	((WMX_USER+20)+9)
VDA_Start_SwitchScn	EQU	((WMX_USER+20)+10)
; This is defined in SHELLFSC.inc
;;;;VDA_FileSysChange	    EQU     ((WMX_USER+20)+11)
VDA_CheckFocus		EQU	((WMX_USER+20)+12)
VDA_Switch_CntxtPanic	EQU	((WMX_USER+20)+13)

;
; This is a special "VMDOSAPP message" which actually results in no
;   message being sent to VMDOSAPP. It is used internally by the SHELL
;   to give the SYS VM a Boost, just as it does for normal events, but
;   without sending VMDOSAPP a message.
;
VDA_Nul_Boost_Event	EQU	0FFFFh

;
; lParam is ALWAYS the "Event ID". This is used on the VMDOSAPP call backs
;  to the shell to identify the event which is being processed.
;

;
; On VDA_Hot_Key event wParam is the Key identifier (See following EQUs)
;   VMDOSAPP instance which gets the message is the "target" of the hot key
;
VDA_HK_ALTSPACE 	EQU	0
VDA_HK_ALTENTER 	EQU	1
VDA_HK_DIRVM		EQU	2

;
; On VDA_Terminated event wParam is 0 if this is a normal termination. If it is
;   non-zero, use SHELL_Get_TermInfo to get error information.
;   VMDOSAPP instance which gets the message has terminated.
;

;
; NOTE that VDA_Crash_Event is very much like VDA_Terminated, the only
;   real difference being the reason for the termination.
;   Use SHELL_Get_TermInfo to get error information.
;   wParam is not used
;   VMDOSAPP instance which gets the message has crashed
;

;
; On VDA_ClipBrd_Event, wParam is the Client_AX identifying the call.
;   VMDOSAPP instance which gets the message had a clipboard event
;

;
; On VDA_Display_Message event, wParam == 0 if normal message
;				       != 0 if ASAP or SYSMODAL message
;   VMDOSAPP instance which gets the message is messaging VM
;

;
; On VDA_Paste_Complete event, wParam == 0 if normal completion
;				      == 1 if paste canceled by user
;				      == 2 if paste canceled for other reason
;   VMDOSAPP instance which gets the message has completed paste
;

;
; On VDA_Switch_Context event, wParam == 0 if context is switched to
;   VMDOSAPP instance which gets the message (that VM now has focus)
;   if wParam != 0, SYS VM now has the focus
; VDA_Switch_CntxtPanic is an alternate form that should only occur with
;   wParam != 0 and indicates that the Windows activation should be moved
;   away from any VM (in other words, only a Windows app should be active
;   now).
;

;
; On VDA_Type_Chng event, wParam is not used
;   VMDOSAPP instance which gets the message has had its type changed by
;   protected mode code
;

;
; On VDA_Start_SwitchScn, wParam = 0 Go Forward
;				 = 1 Go Backward
;   Particular VMDOSAPP instance which gets the message is the Start of the
;   Z order traversal
;

;
; On VDA_FileSysChange, SEE SHELLFSC.INC
;

;
; VDA_CheckFocus This is sent as part of the Contention handling to deal with
;   a case where the the Focus is manipulated and needs to get reset.
;   wParam is not used.
;

;
; Special VMDOSAPP exit codes
;
VMDA_EXIT_NoFile	EQU	81h
VMDA_EXIT_NoMem 	EQU	82h
VMDA_EXIT_Crash 	EQU	83h
VMDA_EXIT_BadVer	EQU	84h
VMDA_EXIT_ExecFail	EQU	85h
VMDA_EXIT_TaskAPIFail	EQU	86h
