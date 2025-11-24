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
;****************************************************************************
TITLE MOUSE.ASM Microsoft Windows VMD 4.0 Mouse Driver
;****************************************************************************
;
;  Title:      MOUSE.ASM
;
;****************************************************************************

;****************************************************************************
;				 I N C L U D E S
;****************************************************************************

	memS = 1

	.xlist
	INCLUDE CMACROS.INC
	INCLUDE WINDEFS.INC
	INCLUDE INT2FAPI.INC
	INCLUDE	VMD.INC
	.list

	externFP	WritePrivateProfileString


;  This is extracted from VMM.INC
VMD_Device_ID			EQU	0000Ch

assumes cs, CODE
assumes ds, DATA

sBegin DATA

VMD_API_Address 	dd	0	; PM break point for VMD API access

ms MOUSEINFO <00h, 01h, 02h, 34, 2, 2, 0, 0>

comm_port	dw	0

IntVect		dw	0
Mouse_Type	db	0
Mouse_Port	dw	0

MouseIni	db	"system.ini",0

MouseSection	db	"Mouse",0
MouseType	db	"MouseType",0
PortNumber	db	"PortNumber",0

port		db	"0",0

BusMouse	db	"Bus",0
SerialMouse	db	"Serial",0
InportMouse	db	"Inport",0
PS2Mouse	db	"PS2",0

sEnd DATA

sBegin CODE

;*******************************************************************************
;
;  WORD Inquire(LPMOUSEINFO lpMouseInfo)
;
;  DESCRIPTION:
;	This function returns information about the mouse hardware, such as
;	the number of buttons, interrupt rates, communications ports, etc.
;
;	NOTE:  The export ordinal for this function must be 1.
;
;  ENTRY:
;	lpMouseInfo, pointer to the MOUSEINFO structure.
;
;  EXIT:
;	AX, number of bytes written to the MOUSEINFO structure.
;
;  USES:
;
;*******************************************************************************

cProc	Inquire, <FAR, PUBLIC, WIN, PASCAL>, <si, di>

parmD	lpMouseInfo

cBegin

	mov	ax, SIZE MOUSEINFO
	add	ax, 2				; for mouseinfo

	mov	si, OFFSET ms
	les	di, lpMouseInfo

	cld
	mov	cx, ax
	rep	movsb

cEnd

;*******************************************************************************
;
;  void Enable(void)
;
;  DESCRIPTION:
;	This function is called when the interrupt hook should be enabled and
;	gives the Mouse_Event callback address for posting mouse messages.
;
;	NOTE:  The export ordinal for this function must be 2.
;
;  ENTRY:
;	lpEventProc, pointer to the Mouse_Event callback function.
;
;  EXIT:
;	None.
;
;  USES:
;
;*******************************************************************************

cProc	Enable, <FAR, PUBLIC, WIN, PASCAL>

parmD	lpEventProc

cBegin

	cmp	[ms.msExists], 0	; Q: Is VMD 4.0 loaded?
	je	Enable_Exit		;    N: Don't bother then...

	mov	ax, VMDAPI_SET_MOUSE_EVENT_CALLBACK
	mov	cx, WORD PTR [lpEventProc + 2]
	mov	dx, WORD PTR [lpEventProc]
	call	DWORD PTR [VMD_API_Address]

Enable_Exit:

cEnd

;*******************************************************************************
;
;  void Disable(void)
;
;  DESCRIPTION:
;	This function is called when the interrupt hook should be disabled.
;	The Mouse_Event callback function should not be called after this
;	point.
;
;	NOTE:  The export ordinal for this function must be 3.
;
;  ENTRY:
;	None.
;
;  EXIT:
;	None.
;
;  USES:
;
;*******************************************************************************

cProc	Disable, <FAR, PUBLIC, WIN, PASCAL>

cBegin

	cmp	[ms.msExists], 0
	je	Enable_Exit

	mov	ax, VMDAPI_SET_MOUSE_EVENT_CALLBACK
	xor	cx, cx
	xor	dx,dx
	call	DWORD PTR [VMD_API_Address]

Disable_Exit:

cEnd

;*******************************************************************************
;
;  WORD MouseGetIntVect(void)
;
;  DESCRIPTION:
;	Returns the interrupt-vector number used by the mouse.	If no mouse
;	was found, then -1 is returned.
;
;  ENTRY:
;	None.
;
;  EXIT:
;	AX, interrupt-vector number used by the interrupt hook.
;
;  USES:
;
;*******************************************************************************

cProc	MouseGetIntVect, <FAR, PUBLIC, WIN, PASCAL>

cBegin

;  BUGBUG  This is an entirely arbitrary number.  Needs to be a VMD API that
;  returns the interrupt number.

	mov ax, 65h

cEnd

;******************************************************************************
;
; WORD MouseRedetect(void)
;
; DESCRIPTION:
;	calls down into VMD to re-detect the mouse
;
; ENTRY:
;	none
; Exit:
;	AX = 0 if mouse not found, else 1
;==============================================================================

cProc MouseRedetect, <FAR,PUBLIC,WIN,PASCAL>

cBegin
	xor	ax,ax
	cmp	WORD PTR [VMD_API_Address+2],0
	je	MR_Done
	mov	ax,VMDAPI_DETECT_MOUSE
	call	[VMD_API_Address]
	call	Initialize
	xor	ax,ax
	mov	al,[ms.msExists]
MR_Done:

cEnd

;*******************************************************************************
;
;  WORD WEP(WORD nParameter)
;
;  DESCRIPTION:
;	Windows Exit Procedure.
;
;  ENTRY:
;
;  EXIT:
;
;  USES:
;
;*******************************************************************************

cProc	WEP, <FAR, PUBLIC, WIN, PASCAL>

parmW	nParameter

cBegin

	mov ax, 1

cEnd

;*******************************************************************************
;
;  LibMain
;
;  DESCRIPTION:
;
;  ENTRY:
;
;  EXIT:
;
;  USES:
;
;*******************************************************************************

cProc	Initialize, <FAR, PUBLIC, WIN, PASCAL>, <es,di>

cBegin

	mov	bx, VMD_Device_ID
	mov	ax, (W386_Int_Multiplex * 100h) + W386_Get_Device_API
	int	W386_API_Int

	mov	ax, es
	or	ax, di			; Q: Did we get a NULL pointer back?
	jz	Initialize_Exit 	;    Y: No API address then!

	mov	WORD PTR [VMD_API_Address], di
	mov	WORD PTR [VMD_API_Address + 2], es

	xor	ax, ax			; Get Version API
	call	DWORD PTR [VMD_API_Address]
	cmp	ax, ((VMD_Major_Version SHL 8)+VMD_Minor_Version)
					; Q: Is it at least version 4.0?
	jb	Initialize_Exit 	;    N: Can't use this VMD!

	mov	ax, VMDAPI_GET_MOUSE_INFO
	call	DWORD PTR [VMD_API_Address]

	mov	[Mouse_Type],al
	mov	[Mouse_Port],cx

	cmp	al,1			; Q: Is mouse present ?
	jb	Initialize_Exit		;  N: Undefined mouse type
	ja	Initialize_NotBus
	mov	ax, offset BusMouse
	jmp	Initialize_Write_Info

Initialize_NotBus:
	cmp	al,3
	jb	Initialize_Serial
	je	Initialize_Inport
	cmp	al,4			; Q: Do we recognize mouse type ?
	ja	Initialize_Exit1	;    N:, but there is a mouse.
	mov	ax, offset PS2Mouse
	jmp	Initialize_Write_Info

Initialize_Serial:
	mov	bx, offset MouseSection
	mov	cx, offset PortNumber
	mov	dx, offset MouseIni
	mov	ax, 1			; assume on com1
	cmp	[Mouse_Port],3F8h
	je	Serial_Comx		; COM1
	inc	al
	cmp	[Mouse_Port],2F8h
	je	Serial_Comx		; COM2
	inc	al
	cmp	[Mouse_Port],3E8h
	je	Serial_Comx		; COM3
	inc	al			; COM4
Serial_Comx:
	mov	[comm_port],ax		; save for windows
	add	byte ptr [port],al
	mov	ax, offset port
	cCall	WritePrivateProfileString,<ds,bx,ds,cx,ds,ax,ds,dx>

	mov	ax, offset SerialMouse
	jmp	Initialize_Write_Info

Initialize_Inport:
	mov	ax, offset InportMouse

Initialize_Write_Info:
	mov	bx, offset MouseSection
	mov	cx, offset MouseType
	mov	dx, offset MouseIni

	cCall	WritePrivateProfileString,<ds,bx,ds,cx,ds,ax,ds,dx>

Initialize_Exit1:
	inc	[ms.msExists]		; Show that mouse exists

Initialize_Exit:
	mov	ax, 1

cEnd

sEnd CODE

	END	Initialize
