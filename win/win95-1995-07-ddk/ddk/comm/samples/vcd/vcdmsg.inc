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

VxD_MESSAGE_SEG

BeginMsg
;
; This message is displayed when VCD detects an IRQ conflict while trying to
; open a COM port for a Windows application.  The user should check their
; system configuration to remove the conflict.
;
SQZD_MSG_START <MSG_VCD_IRQ_Conflict_Msg>
SQZD_MSG <"The requested COM port is not available, because a hardware interrupt ">
SQZD_MSG <"conflict has been detected.  You must change your hardware ">
SQZD_MSG <"configuration to be able to use this COM port from Windows ">
SQZD_MSG <"applications.", 0>
SQZD_MSG_END <MSG_VCD_IRQ_Conflict_Msg>
EndMsg

BeginMsg
;
; This message is displayed when VCD detects that a 3.0 comm driver is
; installed, but COMMDRV30 is not set to TRUE in SYSTEM.INI

SQZD_MSG_START <MSG_VCD_30_Comm_Driver_Msg>
SQZD_MSG <"You have installed a Windows 3.0 compatible communications ">
SQZD_MSG <"driver.  To properly support this driver you must set ">
SQZD_MSG <'"COMMDRV30=TRUE" in the [386Enh] section of SYSTEM.INI.',0>
SQZD_MSG_END <MSG_VCD_30_Comm_Driver_Msg>
EndMsg

BeginMsg
;
; This message is displayed when a VxD is attempting to take control of a
; COM port that is already in use by an application in the system.
;
SQZD_MSG_START <MSG_VCD_VxD_Contention>
SQZD_MSG <"This virtual device is trying to get access to %s">
SQZD_MSG <" which is already in use by an application.", 13, 10, 10>
SQZD_MSG <"Do you want the virtual device to take control of the port? ", 0>
SQZD_MSG_END <MSG_VCD_VxD_Contention>
EndMsg

BeginMsg
;
; This message is displayed the first time that an application attempts
; to access a port that is already owned by a VxD.
;
SQZD_MSG_START <MSG_VCD_Assign_Fail_Msg>
SQZD_MSG <"This application's attempt to get access to %s">
SQZD_MSG <" has been failed, because the port is in use by a virtual device.", 0>
SQZD_MSG_END <MSG_VCD_Assign_Fail_Msg>

VxD_MESSAGE_ENDS
