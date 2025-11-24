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

VxD_IMESSAGE_SEG

;
; Following occurs if the call to VPICD to virtualize IRQ 1 fails.
;
LONG_MESSAGE_START <MSG_VKD_failed_irq_init>
LONG_MESSAGE <"The virtual keyboard driver failed to virtualize ">
LONG_MESSAGE <"hardware interrupt",13,10>
LONG_MESSAGE <"number 1 for the keyboard.", 13, 10, 0>
LONG_MESSAGE_END <MSG_VKD_failed_irq_init>

;
; The following occurs if WINBOOT.SYS did not answer the callout
; to locate the Int 16h back-end hook.
;
LONG_MESSAGE_START <MSG_VKD_Cant_Install_Int16_Hook>
LONG_MESSAGE <"The virtual keyboard driver failed to install its hook for ">
LONG_MESSAGE <"software interrupt",13,10>
LONG_MESSAGE <"number 16h for the keyboard.",13,10>
LONG_MESSAGE <13,10>
LONG_MESSAGE <"Please run the Windows Setup program again.", 13, 10, 0>
LONG_MESSAGE_END <MSG_VKD_Cant_Install_Int16_Hook>

VxD_IMESSAGE_ENDS
