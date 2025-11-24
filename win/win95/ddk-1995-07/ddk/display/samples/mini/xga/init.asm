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

;-----------------------------------------------------------------------
; INIT.ASM
;-----------------------------------------------------------------------
        .xlist
        include cmacros.inc
	include macros.inc
        include minivdd.inc
        .list
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
VDD		        equ     000Ah           ;id of Virtual Display Driver.

;----------------------------------------------------------------------------
; E X T E R N S  and  P U B L I C S
;----------------------------------------------------------------------------
externFP GetPrivateProfileInt   ; Kernel!GetProfileInt
externFP GlobalSmartPageLock

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
globalB bReEnable,0                     ;only set for re-enable call
globalW wResolution,255                 ;Contains the resolution setting.
globalW wDpi,96                         ;Contains the dpi setting.
globalW wBpp,8                          ;Contains the bpp setting.
globalW wPalettized,0                   ;0 = not palettized, 1 = palettized
globalD VDDEntryPoint,0
globalW OurVMHandle,0
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386
assumes ds,Data
public	szSection, szSystem
szSection           db      "display",0         ;section name
szSystem            db      "system.ini",0      ;file name
szPalettized        db      "palettized",0      ;key name
szx                 db      "x_resolution",0    ;key name
szy                 db      "y_resolution",0    ;key name
szDpi               db      "dpi",0             ;key name
szBpp               db      "bpp",0             ;key name
szIgnoreRegistry    db      "IgnoreRegistry",0  ;key name

;-----------------------------------------------------------------------
; DriverInit
;
; Windows display driver initialization.  Called at driver load time.
; Entry:
;       CX = size of heap
;       DI = module handle
;       DS = automatic data segment
;       ES:SI = address of command line (not used)
;-----------------------------------------------------------------------
cProc   DriverInit,<FAR,PUBLIC,WIN>,<si,di>
cBegin
	push	_TEXT
	call	GlobalSmartPageLock

        mov     ax,1684h                ;Int 2FH: GET DEVICE API ENTRY POINT
        mov     bx,VDD                  ;this is VxD ID for VDD
        int     2fh                     ;get the API Entry Point
        mov     word ptr VDDEntryPoint,di
        mov     word ptr VDDEntryPoint+2,es
        mov     ax,1683h
        int     2fh
        mov     OurVMHandle,bx

	call	GetScreenSettings
        mov     ax,1                    ;initialization successful.
cEnd


;-----------------------------------------------------------------------
; GetScreenSettings
; Entry:
; Exit:
;  wResolution = 1,2,3,4
;  wBpp = 1,4,8,16,24,32
;  wDpi = 96,120
;  wPalettizes = 0 or 1
;-----------------------------------------------------------------------
cProc	GetScreenSettings,<NEAR,PUBLIC>
	localV	DispInfo,%(SIZE DISPLAYINFO)
cBegin

; Get dpi=<> setting from system.ini

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szDpi]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,96,cs,cx>
	mov	wDpi,ax

; get the resolution values from system.ini.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szx]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        mov     si,ax                   ;save x resolution in si
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szy]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
        shl     esi,16                  ;x resolution in esi.hi
        mov     si,ax                   ;y resolution in esi.lo

	call	XYtoRes			;get matching int for x,y resolution
	mov	wResolution,ax		;

; get the bpp value from system.ini.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szBpp]
        mov     cx,offset cs:[szSystem]
	cCall	GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
	mov	wBpp,ax 		;save return value from call

; Get the DisplayInfo structure from the VDD.  This structure contains
; the resolution, color depth, etc. from the registry.  Update wBpp and
; wResolution with data from this structure.
; The calling convention for this is:
; entry:
;   eax = VDD_GET_DISPLAY_CONFIG
;   ebx = OurVMHandle
;   ecx = size of DISPLAYINFO structure + minivdd specific additions
;   es:edi--> DISPLAYINFO structure
; exit:
;   eax =  VDD_GET_DISPLAY_CONFIG means vdd not present.  Assume some
;           basic settings.
;   eax = 0 means success.
;  eax = -1 means error reading registry.  Assume basic settings.

; Check if the user wants to override the registry.

        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szIgnoreRegistry]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
	test	ax,ax			   ;Registry override?
	jnz	GSS_ValidateBpp		   ;yes. Skip over vdd call.

	mov	ax,ss
	mov	es,ax
	lea	edi,DispInfo		   ;es:edi-->DispInfo structure
	mov	eax,VDD_GET_DISPLAY_CONFIG
	movzx	ebx,OurVMHandle
	mov	ecx,size DISPLAYINFO
	call	dword ptr VDDEntryPoint
	cmp	eax,VDD_GET_DISPLAY_CONFIG ;Did the call work?
	je	GSS_ValidateBpp		   ;no.
	inc	eax			   ;yes. Could vdd read the registry?
	jz	GSS_ValidateBpp		   ;no.
	mov	si,DispInfo.diXRes	   ;yes. si = xRes
	shl	esi,16			   ;esi.hi = xRes
	mov	si,DispInfo.diYRes	   ;esi.lo = yRes
        call    XYtoRes                    ;get matching int for x,y resolution
        mov     wResolution,ax             ;update wResolution
	movzx	ax,DispInfo.diBpp          ;ax = Bpp
	mov	wBpp,ax                    ;update wBpp
	mov	ax,DispInfo.diDPI          ;ax = DPI
	test	ax,ax			   ;is Dpi valid?
	jz	GSS_ValidateBpp		   ;no.
	mov	wDpi,ax                    ;yes. update wDpi

PLABEL GSS_ValidateBpp

; validate the Bpp.

	mov	ax,wBpp
        cmp     ax,1                    ;valid bpp ?
        jz      short @f                ;yes.
        cmp     ax,4                    ;valid bpp ?
        jz      short @f                ;yes.
        cmp     ax,8                    ;valid bpp ?
        jz      short @f                ;yes.
        cmp     ax,16                   ;valid bpp ?
        jz      short @f                ;yes.
	mov	ax,8			;nothing matched, default to 8.
@@:	mov	wBpp,ax

; Get palettized=<> setting from system.ini

	cmp	ax,8
	jne	short @f
        mov     ax,offset cs:[szSection]
        mov     bx,offset cs:[szPalettized]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,1,cs,cx>
	mov	wPalettized,ax
@@:
cEnd


;----------------------------------------------------------------------------
; XYtoRes
; Entry:
;   esi = XY res (x in msw, y in lsw).
; Exit:
;   ax = resolution index (defaults to 1 if unknown).
;----------------------------------------------------------------------------
PPROC	XYtoRes	near
        xor     ax,ax                   ;assume 320x200
        cmp     esi,((320 SHL 16)+200)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 640x480
        cmp     esi,((640 SHL 16)+480)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 800x600
        cmp     esi,((800 SHL 16)+600)  ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1024x768
        cmp     esi,((1024 SHL 16)+768) ;does it match ?
        jz      short @f                ;yes
	inc	ax			;assume 1280x1024
;
;For the AGX, we need to only compare the X-resolution, since we run
;either 1280x960x8 or 1280x1024x4 and both are the same resolution number.
;
	push	esi			;save original resolution value
	and	esi,0ffff0000h		;get rid of Y-resolution for compare
	cmp	esi,(1280 SHL 16)	;does X-resolution match ?
	pop	esi			;restore saved resolution value
        jz      short @f                ;yes
        inc     ax                      ;assume 1152x864
        cmp     esi,((1152 SHL 16)+864) ;does it match ?
        jz      short @f                ;yes
        inc     ax                      ;assume 1600x1200
        cmp     esi,((1600 SHL 16)+1200);does it match ?
        jz      short @f                ;yes
	mov	ax,1			;nothing matched, default to 640x480
@@:	ret
XYtoRes	endp


sEnd    InitSeg
end     DriverInit
