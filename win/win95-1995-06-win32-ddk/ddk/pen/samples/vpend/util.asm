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
TITLE VpenD - Virtual Pen Driver's basic shell file.  Contains entry points
;******************************************************************************
;
;	Title:	VpenD.ASM
;
;	Version:	4.00
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE VPICD.Inc
;	INCLUDE vpend.inc
	INCLUDE pendrv.inc
	INCLUDE ins8250.inc
	INCLUDE vcd.inc
	.LIST

	; there's a bug here...
	EXTRN _ActiveDevice:DWORD
;	EXTRN _ddPM_API_ebp:DWORD

	PUBLIC _CallPenDriverInVM
;	PUBLIC _ConvertToFlat
;	PUBLIC _VpenD_GlobalizePort
	PUBLIC VpenD_Register
	PUBLIC _VpenD_AcquirePort

VxD_LOCKED_DATA_SEG
gddPortData dd ?
VxD_LOCKED_DATA_ENDS

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;						Utility Functions
;
;******************************************************************************


BeginDoc
;******************************************************************************
;
;	_VpenD_CommVersion
;
;   DESCRIPTION:
;
;   ENTRY:
;
;	EXIT:
;	   eax contains 0 on failure and positive number on success
;
;   USES:
;
;==============================================================================
EndDoc

;BeginProc _VpenD_CommVersion
;
;	push ebp				;
;	mov ebp,esp				;
;	VxDCall VCD_Get_Version			;
;	jnc CV_Done				; Is there an error? jmp if not
;	xor eax,eax				; return 0 on failure
;CV_Done:					; return Number on success
;	pop ebp 				;
;	ret					;
;EndProc _VpenD_CommVersion


BeginDoc
;******************************************************************************
;
;	_VpenD_AcquirePort
;
;   DESCRIPTION:
;
;   ENTRY:
;	   Com port number on stack
;	   called like:
;		 _VpenD_AcquirePort(pName,ComPortNumber);
;
;	EXIT:
;	   eax contains 0 on failure and positive number on success
;
;   USES:
;
;==============================================================================
EndDoc

BeginProc _VpenD_AcquirePort

;;;	int 3
;;;	push ebp					;
;;;	mov ebp,esp 					;
;;;	VMMCall Get_Cur_VM_Handle
;;;	mov eax,[ebp+8]					; grab com port number off stack
;;;	xor ecx,ecx
;;;	mov edi,[ebp+12]				; grab pointer to VpenD name
;;;	xor edx,edx 					;
;;;
;;;;
;;;; Experiment to see if this might work.  esi will get the address to a
;;;; memory buffer on the stack and edi will get zero which will be the "offset"
;;;; to where vcd can write.
;;;;
;;;	mov esi,OFFSET32 gddPortData
;;;	xor edi,edi ; zero is the offset
;;;
;;;
;;;	VxDCall VCD_Acquire_Port_Windows_Style
;;;	jnc AP_Done					; Is there an error? jmp if not
;;;	xor eax,eax 					; return 0 on failure
;;;AP_Done:						; return handle on success
;;;;
;;;; adjust stack
;;;;
;;;	sub esp,4
;;;
;;;
;;;
;;;	pop ebp 					;
;;;	ret 						;
;
; The following code is the working code:
;
;
	push ebp					;
	mov ebp,esp					;
	push edi
	mov eax,[ebp+8]					; grab com port number off stack
	xor ecx,ecx
	mov edi,[ebp+12]				; grab pointer to VpenD name
	xor edx,edx 					;
	VxDCall VCD_Acquire_Port			;
	jnc AP_Done					; Is there an error? jmp if not.
	xor eax,eax 					; return 0 on failure
AP_Done:						; return handle on success
	pop edi
	pop ebp 					;
	ret 						;
EndProc _VpenD_AcquirePort


BeginDoc
;******************************************************************************
;
;	_VpenD_GlobalizePort
;
;   DESCRIPTION:
;
;   ENTRY:
;	   Com port number on stack
;
;	EXIT:
;	   eax contains 0 on failure and positive number on success
;
;   USES:
;
;==============================================================================
EndDoc

BeginProc _VpenD_GlobalizePort
	push ebp					;
	mov ebp,esp 					;
	mov eax,[ebp+8]					; grab com port number off stack
	xor edx,edx 					;
	VxDCall VCD_Set_Port_Global 			;
	jnc GP_Done 					; Is there an error? jmp if not.
	xor eax,eax 					; return 0 on failure
	jmp GP_Leave					;
GP_Done:						;
	mov eax,1					; return 1 on success
GP_Leave:						;
	pop ebp 					;
	ret 						;
EndProc _VpenD_GlobalizePort


BeginDoc
;******************************************************************************
;
;	VpenD_Register
;
;   DESCRIPTION:
;
;   ENTRY:
;	esi : Points to the start of a vpend_init structure to be read.
;
;   EXIT:
;
;	USES:  eax,
;
;==============================================================================
EndDoc

BeginProc VpenD_Register
	push eax
	push ecx
	push edi

	lea eax, _ActiveDevice				; .pDataType
	mov ebx,[eax]
	lea edi,[ebx.pDataType]
       ;mov edi,[edi].pDataType  			; This didn't work, remove
       ;mov edi,OFFSET32 _ActiveDevice.pDataType	; Starting address of destination location
       ;mov edi,OFFSET32 _ActiveDevice.pDataType	; Starting address of destination location
	mov ecx,7					; Seven DWORDs of information

Reg_loop:
	mov	ax,word ptr [esi][2]			; get Selector
	mov	[ebp.Client_es],ax			; place in client structure
	mov	ax,word ptr [esi][0]			; get offset
	mov	[ebp.Client_di],ax			; place in client structure
	Client_Ptr_Flat eax, es, di 			; map flat
	mov	[edi],eax				; move result to destination location
							;
	add esi,4					; advance pointers
	add edi,4					;
	dec ecx 					; moved one
	cmp ecx,0					; check ending condition
	jg Reg_loop 					; loop
;
; Don't forget the entry point that should remain in selector:offset form.
;
	mov	eax,[esi]
	mov	[edi],eax

	pop edi
	pop ecx
	pop eax
	ret

EndProc VpenD_Register


;******************************************************************************
;
;	_CallPenDriverInVM
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;******************************************************************************
BeginProc _CallPenDriverInVM

	Push_Client_State			; process info

	VMMcall Begin_Nest_Exec			; call VpenD_Packet
	push ebx

	lea ebx, _ActiveDevice
	mov eax,[ebx]
	mov ecx,[eax.pfnEntryPoint]
       ;mov ebx,_ActiveDevice
       ;mov ebx,OFFSET32 _ActiveDevice
       ;mov ecx,[ebx+pfnEntryPoint]
       ;mov ecx, _ActiveDevice.pfnEntryPoint
	movzx	edx, cx
	shr	ecx, 16 			; CX:EDX = Seg:Offset of proc
	pop 	ebx
	VMMcall Build_Int_Stack_Frame
	VMMcall Resume_Exec
	VMMcall Enable_VM_Ints
	VMMcall End_Nest_Exec

	Pop_Client_State
	ret
EndProc _CallPenDriverInVM


;******************************************************************************
;
;	_ConvertToFlat
;
;	DESCRIPTION: Converts a Selector:Offset pointer in the calling VM to a
;		     32-bit flat pointer.
;
;	ENTRY:
;		Pointer to convert on stack.
;
;   EXIT:
;
;   USES:
;
;******************************************************************************
;BeginProc _ConvertToFlat
;;
;; Need to retrieve the pointer off the stack before changing ebp!!!
;	push ebp
;	mov ebp,esp
;	mov eax,[ebp+8]			; grab pointer off stack
;;
;	push ebp			; save this new ebp
;	mov ebp,_ddPM_API_ebp		; Get it a ptr to the client structure
;					;
;	mov [ebp.Client_es],ax		;
;	shr eax,16			;
;	mov [ebp.Client_di],ax		;
;	Client_Ptr_Flat eax, es, di 	; convert the pointer
;					;
;	pop ebp 			; restore ebp
;;
;	pop ebp
;	ret 				; eax contains flat pointer
;EndProc _ConvertToFlat



VxD_LOCKED_CODE_ENDS

END
