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
;   File:	vjoyd.asm
;   Content:	main entry points for VJOYD.VXD
;
;---------------------------------------------------------------------------;

        .386p

;---------------------------------------------------------------------------;
;                            I N C L U D E S
;---------------------------------------------------------------------------;

        .xlist
        include vmm.inc
        include debug.inc
        include configmg.inc
        include mmdevldr.inc
	include vwin32.inc
Create_VJOYD_Service_Table equ 1
        include vjoyd.inc
RECT    struc
        rcLeft          dd      ?
        rcTop           dd      ?
        rcRight         dd      ?
        rcBottom        dd      ?
RECT    ends
        include mmsystem.inc
        .list
	
EXTRN _VJOYD_GetPos@8:PROC
EXTRN _VJOYD_GetPosEx@8:PROC
EXTRN _VJOYD_GetHWCaps@8:PROC
EXTRN _RegisterDeviceDriver@16:PROC
EXTRN _OtherVxDInit@4:PROC
EXTRN _DeviceInit@0:PROC
EXTRN _DeviceExit@0:PROC
EXTRN _SetData@4:PROC
	
;============================================================================
;               V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================


DECLARE_VIRTUAL_DEVICE vjoyd,VJOYD_Ver_Major,VJOYD_Ver_Minor,\
            VJOYD_Control,VJOYD_Device_Id,VJOYD_Init_Order,,\
	    VJOYD_PM_API_Handler

;---------------------------------------------------------------------------;
;                   P A G E A B L E   D A T A
;---------------------------------------------------------------------------;
VxD_PAGEABLE_DATA_SEG

VJOYDAPI_PM_API_Table LABEL DWORD

	dd	offset32 VJOYDAPI_Get_Version_Handler
	dd	offset32 VJOYDAPI_GetPosEx_Handler
	dd	offset32 VJOYDAPI_GetPos_Handler
	dd	offset32 VJOYDAPI_SetData_Handler
	dd	offset32 VJOYDAPI_GetHWCaps_Handler

VJOYDAPI_PM_API_MAX	EQU ($-VJOYDAPI_PM_API_Table)/4

VJOYDAPI_IOCTL_Table LABEL DWORD

	dd	offset32 VJOYDAPI_IOCTL_Get_Version_Handler
	dd	offset32 VJOYDAPI_IOCTL_GetPosEx_Handler
	dd	offset32 VJOYDAPI_IOCTL_GetPos_Handler

VJOYDAPI_IOCTL_MAX	EQU ($-VJOYDAPI_IOCTL_Table)/4

VxD_PAGEABLE_DATA_ENDS

;---------------------------------------------------------------------------;
;                   N O N P A G E A B L E   C O D E
;---------------------------------------------------------------------------;

VxD_PAGEABLE_CODE_SEG

;---------------------------------------------------------------------------;
;
;   Do_Config_Handler
;
;   DESCRIPTION:  The procedure which we register with mmdevldr to receive our
;               CONFIG_ messages. This simply calls the Config_Handler function in configh.c
;
;   ENTRY: The usual config handler parameters
;
;   EXIT: CONFIGRET code
;
;   USES:
;       FLAGS
;---------------------------------------------------------------------------;
BeginProc Do_Config_Handler,CCALL,PUBLIC
ArgVar  Function,DWORD
ArgVar  SubFunction,DWORD
ArgVar  MyDevNode,DWORD
ArgVar  RefData,DWORD
ArgVar  Flags,DWORD

        EnterProc
        
        cCall   _Config_Handler, <[Function],[SubFunction],[MyDevNode],[RefData],[Flags]>
        clc
        
        LeaveProc
        return
EndProc Do_Config_Handler


;---------------------------------------------------------------------------;
;
;   VJOYD_PNP_New_DevNode
;
;   DESCRIPTION: This function handles the PNP_NEW_DEVNODE message sent by 
;       mmdevldr.  In response the Do_Config_Handler function is registered with 
;       mmdevldr as the config message handler for this devnode.
;
;   ENTRY: ebx = DEVNODE, edx = DLVXD_LOAD_DRIVER (can be ignored)
;
;   EXIT: eax = return status, ecx = pointer to config handler
;
;   USES:
;       FLAGS
;---------------------------------------------------------------------------;

BeginProc VJOYD_PNP_New_DevNode
	push	ebx				; push devnode 
        mov     eax, ebx
        mov     ebx, offset32 Do_Config_Handler
        VxDcall MMDEVLDR_Register_Device_Driver
	call	_OtherVxDInit@4
        mov     eax, CR_SUCCESS
        clc                                     ; always succeed
        ret
                                                           
EndProc VJOYD_PNP_New_DevNode

;---------------------------------------------------------------------------;
;
;   PROCEDURE: VJOYD_Register_Device_Driver, SERVICE
;
;   DESCRIPTION: This service is used by joystick drivers to register
;		 their callback routine
;
;
;   ENTRY:	eax = polling routine
;		ebx = config manager routine
;		ecx = hw caps routine
;		edx = joystick id routine
;
;   EXIT: NONE
;---------------------------------------------------------------------------;
BeginProc VJOYD_Register_Device_Driver, SERVICE
	push	edx
	push	ecx
	push	ebx
	push	eax
	call	_RegisterDeviceDriver@16
	clc
	ret
EndProc	VJOYD_Register_Device_Driver

;---------------------------------------------------------------------------;
;
;   VJOYDAPI_GetPosEx_Service, SERVICE
;
;   DESCRIPTION: Process a GetPosEx request
;
;   ENTRY:	eax = pointer to JOYINFOEX
;		ebx = joystick id
;
;   EXIT:  	eax = joystick return code
;---------------------------------------------------------------------------;
BeginProc VJOYD_GetPosEx_Service, SERVICE
	push	eax			; pointer to JOYINFOEX
	push	ebx			; contains joystick id
	call	_VJOYD_GetPosEx@8
	clc
	ret
EndProc VJOYD_GetPosEx_Service

;---------------------------------------------------------------------------;
;
;   VJOYDAPI_DeviceIOControl
;
;   DESCRIPTION: This function is called to perform Device IO
;	for a 32 bit process which has opened this device with <f CreateFile>,
;	and is performing IO using <f DeviceIOControl>. Preserves the C32
;	calling registers ESI, EDI, but not EBX, since the DevIOCtl calls
;	preserve that already.
;
;   ENTRY: EBX	DDB
;	   ECX	dwIoControlCode
;	   ESI	ptr to DIOCParams
;
;   EXIT:  As determined by function, or 1 if invalid IOCTL
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_DeviceIOControl
	cmp	ecx, VJOYDAPI_IOCTL_MAX
	jb	@f
	mov	eax, -1
	ret
@@:
	push	edi
	push	esi
	jmp	VJOYDAPI_IOCTL_Table[ecx*4]
EndProc VJOYDAPI_DeviceIOControl


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_IOCTL_Get_Version_Handler
;
;   DESCRIPTION: Get the version of VJOYD
;
;   ENTRY: 
;
;   EXIT:  first DWORD has version
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_IOCTL_Get_Version_Handler
	mov	edi, [esi.lpcbBytesReturned]
	mov	esi, [esi.lpvOutBuffer]
	mov	dword ptr [esi], VJOYD_Ver_Major SHL 8 + VJOYD_Ver_Minor
	or	edi, edi
	jz	short IOCTL_Exit
	mov	dword ptr [edi], 4
IOCTL_Exit:
	pop	esi
	pop	edi
	xor	eax, eax			; Return success
	ret
EndProc VJOYDAPI_IOCTL_Get_Version_Handler


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_IOCTL_GetPosEx_Handler
;
;   DESCRIPTION: Process a GetPosEx request
;
;   ENTRY: 
;
;   EXIT: 
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_IOCTL_GetPosEx_Handler
	mov	edi, [esi.lpvOutBuffer]			; pointer to JOYINFOEX
	push	edi
	push	dword ptr [edi.jinfoex_dwReserved1]	; contains joystick id
	call	_VJOYD_GetPosEx@8
	mov	[edi.jinfoex_dwReserved1], eax		; save return code
	jmp	IOCTL_Exit
EndProc VJOYDAPI_IOCTL_GetPosEx_Handler


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_IOCTL_GetPos_Handler
;
;   DESCRIPTION: Process a GetPos request
;
;   ENTRY: 
;
;   EXIT:
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_IOCTL_GetPos_Handler
	mov	eax, [esi.lpvInBuffer]			
	mov	ecx, [esi.lpvOutBuffer]			; pointer to JOYINFOEX
	
	push	ecx
	push	dword ptr [eax]				; joystick id
	call	_VJOYD_GetPos@8
	mov	edi, [esi.lpcbBytesReturned]
	mov	dword ptr [edi], eax			; save return code
	jmp	IOCTL_Exit
EndProc VJOYDAPI_IOCTL_GetPos_Handler


;---------------------------------------------------------------------------;
;
;   VJOYD_PM_API_Handler
;
;   DESCRIPTION: Entry point for all API requesets
;
;   ENTRY: Client_AX - requested service
;
;   EXIT:  Client_EAX - 0 if fail, non-zero otherwise
;---------------------------------------------------------------------------;
BeginProc VJOYD_PM_API_Handler
	movzx	eax, [ebp.Client_AX]
	cmp	eax, VJOYDAPI_PM_API_MAX
	ja	VJOYDAPI_Handler_Error
	call	VJOYDAPI_PM_API_Table[eax*4]
	ret
VJOYDAPI_Handler_Error:
	xor	eax, eax
	mov	[ebp.Client_EAX], eax
	ret
EndProc VJOYD_PM_API_Handler


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_Get_Version_Handler
;
;   DESCRIPTION: Get the version of VJOYD
;
;   ENTRY: <none>
;
;   EXIT:  Client_AX - version
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_Get_Version_Handler
	mov	ah,VJOYD_Ver_Major
	mov	al,VJOYD_Ver_Minor
	mov	[ebp.Client_AX], ax
	ret
EndProc VJOYDAPI_Get_Version_Handler


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_GetPosEx_Handler
;
;   DESCRIPTION: Get current position information for the joystick
;
;   ENTRY: Client_DX - joystick id
;          Client_ES:Client_BX -> JOYINFOEX struct
;
;   EXIT:  Client_DX:Client_AX - rvalue
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_GetPosEx_Handler
	Client_Ptr_Flat edi, es, bx, USES_EAX
	movzx	edx,[ebp.Client_DX]
	push	edi
	push	edx
	call	_VJOYD_GetPosEx@8
	mov	[edi.jinfoex_dwReserved1], eax		; save return code
	mov	[ebp.Client_AX], ax
	shr	eax,16
	mov	[ebp.Client_DX], ax
	ret
EndProc VJOYDAPI_GetPosEx_Handler

;---------------------------------------------------------------------------;
;
;   VJOYDAPI_GetHWCaps_Handler
;
;   DESCRIPTION: Get the hardware caps
;
;   ENTRY: Client_ES:Client_BX -> JOYHWCAPS struct
;   ENTRY: Client_DX -> joystick id
;
;   EXIT:  Client_DX:Client_AX - rvalue
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_GetHWCaps_Handler
	Client_Ptr_Flat edi, es, bx, USES_EAX
	movzx	edx,[ebp.Client_DX]
	push	edi
	push	edx
	call	_VJOYD_GetHWCaps@8
	mov	[ebp.Client_AX], ax
	shr	eax,16
	mov	[ebp.Client_DX], ax
	ret
EndProc VJOYDAPI_GetHWCaps_Handler

;---------------------------------------------------------------------------;
;
;   VJOYDAPI_GetPos_Handler
;
;   DESCRIPTION: Get current position information for the joystick
;
;   ENTRY: Client_DX - joystick id
;          Client_ES:Client_BX -> JOYINFO struct
;
;   EXIT:  Client_DX:Client_AX - rvalue
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_GetPos_Handler
	Client_Ptr_Flat edi, es, bx, USES_EAX
	movzx	edx,[ebp.Client_DX]
	push	edi
	push	edx
	call	_VJOYD_GetPos@8
	mov	[ebp.Client_AX], ax
	shr	eax,16
	mov	[ebp.Client_DX], ax
	ret
EndProc VJOYDAPI_GetPos_Handler


;---------------------------------------------------------------------------;
;
;   VJOYDAPI_SetData_Handler
;
;   DESCRIPTION: Set up joystick data (passed in by ring 3 driver)
;
;   ENTRY: Client_ES:Client_BX -> joystick data
;
;   EXIT:  Client_AX - success (1)
;---------------------------------------------------------------------------;
BeginProc VJOYDAPI_SetData_Handler
	Client_Ptr_Flat edi, es, bx, USES_EAX
	push	edi
	call	_SetData@4
	mov	[ebp.Client_AX], 1
	ret
EndProc VJOYDAPI_SetData_Handler

VXD_PAGEABLE_CODE_ENDS

VxD_LOCKED_CODE_SEG
;---------------------------------------------------------------------------;
;
;   VJOYD_Control
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
BeginProc VJOYD_Control
	Control_Dispatch W32_DEVICEIOCONTROL,	  VJOYDAPI_DeviceIOControl
        Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, VJOYD_DeviceInit
        Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, VJOYD_DeviceExit
        Control_Dispatch PNP_NEW_DEVNODE,         VJOYD_PNP_New_DevNode
        clc
        ret

EndProc VJOYD_Control


;---------------------------------------------------------------------------;
;
;   VJOYD_DeviceInit
;
;   DESCRIPTION: handle the init of our device
;
;---------------------------------------------------------------------------;
BeginProc VJOYD_DeviceInit
        call   _DeviceInit@0
        clc                                     ; succeed
        ret

EndProc VJOYD_DeviceInit


;---------------------------------------------------------------------------;
;
;   VJOYD_DeviceExit
;
;   DESCRIPTION: handle the exiting of our device
;
;---------------------------------------------------------------------------;
BeginProc VJOYD_DeviceExit
        call   _DeviceExit@0
        clc                                     ; succeed
        ret

EndProc VJOYD_DeviceExit

VXD_LOCKED_CODE_ENDS

end
