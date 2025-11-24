    page    , 132
;---------------------------------------------------------------------------;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   File:	oemjoy.asm
;   Content:	main entry points for OEMJOY.VXD
;
;---------------------------------------------------------------------------;

        .386p

;---------------------------------------------------------------------------;
;                            I N C L U D E S
;---------------------------------------------------------------------------;

        .xlist
        include vmm.inc
        include debug.inc
        include oemjoy.inc
        include vjoyd.inc
        .list
	
EXTRN _PollRoutine@8:PROC
EXTRN _HWCapsRoutine@8:PROC
EXTRN _JoyIdRoutine@8:PROC
EXTRN _CfgRoutine:PROC
	
;============================================================================
;               V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================


DECLARE_VIRTUAL_DEVICE OEMJOY,1,0,\
            OEMJOY_Control,OEMJOY_Device_Id,OEMJOY_Init_Order

;---------------------------------------------------------------------------;
;                   P A G E A B L E   D A T A
;---------------------------------------------------------------------------;
VxD_PAGEABLE_DATA_SEG
VxD_PAGEABLE_DATA_ENDS

;---------------------------------------------------------------------------;
;                   N O N P A G E A B L E   C O D E
;---------------------------------------------------------------------------;

VxD_PAGEABLE_CODE_SEG

VXD_PAGEABLE_CODE_ENDS

VxD_LOCKED_CODE_SEG
;---------------------------------------------------------------------------;
;
;   OEMJOY_Control
;
;   DESCRIPTION:
;       Dispatch control messages to the correct handlers. Must be in locked
;       code segment. (All VxD segments are locked in 3.0 and 3.1)
;
;   ENTRY:
;
;   EXIT:
;       Carry Clear success; Carry Set if fail.
;
;---------------------------------------------------------------------------;
BeginProc OEMJOY_Control
        Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, OEMJOY_DeviceInit
        Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, OEMJOY_DeviceExit
        clc
        ret

EndProc OEMJOY_Control

;---------------------------------------------------------------------------;
;
;   OEMJOY_DeviceInit
;
;   DESCRIPTION: handle the init of our device
;
;---------------------------------------------------------------------------;
BeginProc OEMJOY_DeviceInit
	;
	; call VJOYD's service to register our two callback routines:
	; on for polling the joystick, and one for handling config manager
	; messages
	;
        mov     eax, offset32 _PollRoutine@8
        mov     ebx, offset32 _CfgRoutine
	mov	ecx, offset32 _HWCapsRoutine@8
	mov	edx, offset32 _JoyIdRoutine@8
        VxDcall VJOYD_Register_Device_Driver
        clc                                     ; succeed
        ret

EndProc OEMJOY_DeviceInit

;---------------------------------------------------------------------------;
;
;   OEMJOY_DeviceExit
;
;   DESCRIPTION: handle the exiting of our device
;
;---------------------------------------------------------------------------;
BeginProc OEMJOY_DeviceExit
        clc                                     ; succeed
        ret

EndProc OEMJOY_DeviceExit

VXD_LOCKED_CODE_ENDS

end
