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

;*********************************Module*Header********************************
;This module contains the routines that perform the RGB to physical index
;color matching.
;
;******************************************************************************

.xlist
include cmacros.inc
.list

SCALEFACTOR	    equ     128
COLOR_THRESHOLD     equ     40

sBegin	Data
externB nSymmetryIndex			;in DITHERS.ASM
externB wRGB				;in DITHERS.ASM
sEnd	Data

sBegin	Code

externB PAccelTable			;defined in ?GA.ASM
externB PColorTable			;defined in ?GA.ASM
externB PMatchTable1			;defined in ?GA.ASM
externB PMatchTable2
externB PMatchTable3
externB PIndexTable1
externB PIndexTable2
externB PIndexTable3
externW _cstods

externA NUMBER_CL1_COLORS
externA NUMBER_CL2_COLORS
externA NUMBER_CL3_COLORS

externNP    ComputeSymmetry		;in DITHERS.ASM
externNP    ComputePColor		;in DITHERS.ASM

assumes cs, Code
assumes ds, nothing
assumes es, nothing

AxisHashTable	label	byte
	db	0
	db	2
	db	0
	db	0
	db	4
	db	2
	db	4
	db	0

ParameterOffsetTable	label	word
	dw	CodeOFFSET PMatchTable1
	dw	CodeOFFSET PIndexTable1
	dw	NUMBER_CL1_COLORS
	dw	CodeOFFSET PMatchTable2
	dw	CodeOFFSET PIndexTable2
	dw	NUMBER_CL2_COLORS
	dw	CodeOFFSET PMatchTable3
	dw	CodeOFFSET PIndexTable3
	dw	NUMBER_CL3_COLORS

cProc	rgb_to_ipc, <NEAR, PUBLIC>, <bx, cx, si, di>
cBegin
	mov	dh, ah
	sub	ah, ah
	mov	bx, ax			;BX: red
	mov	al, dh
	mov	cx, ax			;CX: green
	mov	dh, ah			;DX: blue
	cCall	ComputeSymmetry 	;make R >= G >= B
	mov	ds, _cstods
	assumes ds, Data
	mov	[nSymmetryIndex], al	;save original position in RGB space
	mov	byte ptr [wRGB], bl	;save this for possible color dithering
	mov	word ptr [wRGB][1], cx	;later on BL: red, CL: green, CH: blue
	or	bl, bl			;is it black we're dealing with?
	jnz	DoPreProcessing 	;no. Do normal stuff
	sub	ax, ax
	jmp	short Blackness 	;yes.
DoPreProcessing:
	mov	di, bx			;DI: red
	mov	bl, cl
	mov	si, bx			;SI: green
	mov	bl, ch			;BX: blue transformed (and un-Xformed)
	mov	cx, si
	sub	cx, bx			;CX: green transformed
	mov	dx, di
	sub	dx, si			;DX: red transformed

	xchg	bx, dx			;BX: R, CX: Y, DX: W
	cCall	ComputeSymmetry
	cbw
	mov	si, ax
	mov	al, cs:[si].AxisHashTable
	mov	si, ax
	shl	ax, 1
	add	si, ax
	add	si, CodeOFFSET ParameterOffsetTable
	lods	word ptr cs:[si]	;get P match table
	mov	dx, ax
	lods	word ptr cs:[si]	;get P index table
	mov	di, ax
	lods	word ptr cs:[si]	;get P match table item count
	mov	cx, ax
	mov	ch, al
	mov	si, dx			;CS:SI-->P match table
	mov	dx, -1			;DX: min. error
	mov	bl, byte ptr [wRGB]
SearchLoop:
	lods	byte ptr cs:[si]
	sub	al, bl
	sbb	ah, ah
	xor	al, ah
	sub	al, ah
	jz	ExactMatchFound
	cmp	al, dh
	jb	SetNewMinStuff
	dec	cl
	jnz	SearchLoop
	jmp	short MatchFound
ExactMatchFound:
	mov	dl, cl
	jmp	short MatchFound
SetNewMinStuff:
	mov	dl, cl
	mov	dh, al
	dec	cl
	jnz	SearchLoop
MatchFound:
	mov	bx, di			;CS:BX-->physical index table
	mov	ah, ch
	sub	ah, dl
	push	ax
	mov	si, sp
	mov	al, [nSymmetryIndex]
	mov	cx, 1
	cCall	ComputePColor
	pop	ax
Blackness:
	mov	dh, ah
	mov	al, ah
	cbw
	mov	bx, ax			    ;BX: physical color index
	or	dh, cs:[bx].PAccelTable     ;DH: phy. color index with accel.
	mov	ax, bx
	shl	bx, 1
	add	bx, ax
	mov	ax, word ptr cs:[bx].PColorTable[0] ;actual R and G in AX
	mov	dl, cs:[bx].PColorTable[2]  ;actual B in DL
cEnd

cProc	IndexToColor, <FAR, PUBLIC>, <si>
cBegin	nogen
	push	si			    ;save SI
	mov	al, dh			    ;get index into AL
	and	ax, 0fh 		    ;Modulo 16
	mov	si, ax			    ;SI: color index mod 16
	or	dh, cs:[si].PAccelTable     ;add accel. bits into dh
	shl	ax, 1
	add	si, ax			    ;SI: 3*color index mod 16
	mov	ax, word ptr cs:[si].PColorTable[0] ;AX: red, green
	mov	dl, cs:[si].PColorTable[2]  ;DL: blue
	pop	si			    ;restore SI
	retf				    ;return
cEnd	nogen



ifdef   EUCLIDEAN_COLOR_MATCH

;-----------------------------------------------------------------------------
; VgaInverseMap                                                              ;
;                                                                            ;
; Subdividing the RGB color cube twice along each axis yields 64 smaller     ;
; cubes.  A maximum of three VGA colors, and often only one VGA color,       ;
; match (Euclidean distance) into each of the subdivided cubes.  Therefore,  ;
; this adaptive Eudclidean match is must faster than the traditional         ;
; Euclidean match.                                                           ;
;                                                                            ;
; Note:  This table was built according to the VGA palette.  The             ;
; indices it returns will not be appropriate for all devices.  Use a         ;
; VgaTranslateMap to produce the final physical color index.                 ;
; (Example: GDI has indices 7 and 8 reversed.  To use this code in GDI,      ;
; enable the VgaTranslateMap and swap indices 7 and 8.)                      ;
;                                                                            ;
; The index to this map is computed as follows given a 24-bit RGB.           ;
;                                                                            ;
;       index = ((Red & 0xC0) >> 6)     |                                    ;
;               ((Green & 0xC0) >> 4)   |                                    ;
;               ((Blue & 0xC0) >> 2);                                        ;
;                                                                            ;
; Each entry is a word made up of four nibbles.  The first nibble always     ;
; contains a valid GDI VGA color index.  The second and third nibbles        ;
; contain valid GDI VGA color indices if they are non-zero.                  ;
; The fourth nibble is an optimization for sub-cubes 42 and 63.              ;
;                                                                            ;
; Note:  This table was built assuming that Euclidean distance ties went     ;
; to the higher index.  (It helps the subdivision.)  Therefore, it is        ;
; important that the first nibble has the lowest index, the second           ;
; nibble has the middle index... so EuclideanMatch will handle ties the      ;
; same way as the table.                                                     ;
;                                                                            ;
;-----------------------------------------------------------------------------

public VgaInverseMap
VgaInverseMap	label	byte
        db      (0  + 0  * 16), 0       ; Index 0
        db      (4  + 0  * 16), 0
        db      (4  + 0  * 16), 0
        db      (12 + 0  * 16), 0
        db      (2  + 0  * 16), 0
        db      (6  + 0  * 16), 0
        db      (6  + 0  * 16), 0
        db      (6  + 12 * 16), 0

        db      (2  + 0  * 16), 0       ; Index 8
        db      (6  + 0  * 16), 0
        db      (6  + 0  * 16), 0
        db      (6  + 14 * 16), 0
        db      (10 + 0  * 16), 0
        db      (6  + 10 * 16), 0
        db      (6  + 14 * 16), 0
        db      (14 + 0  * 16), 0

        db      (1  + 0  * 16), 0       ; Index 16
        db      (5  + 0  * 16), 0
        db      (5  + 0  * 16), 0
        db      (5  + 12 * 16), 0
        db      (3  + 0  * 16), 0
        db      (7  + 0  * 16), 0
        db      (7  + 0  * 16), 0
        db      (7  + 8  * 16), 12

        db      (3  + 0  * 16), 0       ; Index 24
        db      (7  + 0  * 16), 0
        db      (7  + 8  * 16), 0
        db      (7  + 8  * 16), 14
        db      (3  + 10 * 16), 0
        db      (7  + 8  * 16), 10
        db      (7  + 8  * 16), 14
        db      (7  + 8  * 16), 14

        db      (1  + 0  * 16), 0       ; Index 32
        db      (5  + 0  * 16), 0
        db      (5  + 0  * 16), 0
        db      (5  + 13 * 16), 0
        db      (3  + 0  * 16), 0
        db      (7  + 0  * 16), 0
        db      (7  + 8  * 16), 0
        db      (7  + 8  * 16), 13

        db      (3  + 0  * 16), 0       ; Index 40
        db      (7  + 8  * 16), 0
        db      (7  + 8  * 16), (0 + 1 * 16)
        db      (7  + 8  * 16), 0
        db      (3  + 11 * 16), 0
        db      (7  + 8  * 16), 11
        db      (7  + 8  * 16), 0
        db      (8  + 15 * 16), 0

        db      (9  + 0  * 16), 0       ; Index 48
        db      (5  + 9  * 16), 0
        db      (5  + 13 * 16), 0
        db      (13 + 0  * 16), 0
        db      (3  + 9  * 16), 0
        db      (7  + 8  * 16), 9
        db      (7  + 8  * 16), 13
        db      (7  + 8  * 16), 13

        db      (3  + 11 * 16), 0       ; Index 56
        db      (7  + 8  * 16), 11
        db      (7  + 8  * 16), 0
        db      (8  + 15 * 16), 0
        db      (11 + 0  * 16), 0
        db      (7  + 8  * 16), 11
        db      (8  + 15 * 16), 0
        db      (8  + 15 * 16), (0 + 1 * 16)

;-----------------------------------------------------------------------------
; VgaTranslateMap                                                            ;
;                                                                            ;
;       The VgaInverseMap was built from the VGA palette so the              ;
; VgaTranslateMap is not needed in the VGA driver.  Modify this for 8 bpp    ;
; devices or GDI.                                                            ;
;                                                                            ;
;-----------------------------------------------------------------------------

public VgaTranslateMap
VgaTranslateMap	label	byte
        db      0
        db      1
        db      2
        db      3
        db      4
        db      5
        db      6
        db      7       ; Note: GDI has 7 and 8 switched.
        db      8
        db      9
        db      10
        db      11
        db      12
        db      13
        db      14
        db      15

;-----------------------------------------------------------------------------
; VgaStaticMap                                                               ;
;                                                                            ;
;       The VgaStaticMap is used to speed up the matching of exact static    ;
; colors.  Three VGA palette indices fall into sub-cubes where a Euclidean   ;
; match is required.  We can bisect the sub-cubes and quickly return the     ;
; static color.                                                              ;
;       0 = perform the Eucliean match                                       ;
;       1 = use the palette index in CL                                      ;
;       2 = use the palette index in CH                                      ;
;                                                                            ;
;-----------------------------------------------------------------------------

public VgaStaticMap
VgaStaticMap     label   byte
        db      1
        db      0
        db      0
        db      0
        db      0
        db      0
        db      0
        db      2

;-----------------------------------------------------------------------------
; SolidColorMatch							     ;
;       converts a RGB triplet into a a color byte for VGA adapters          ;
;									     ;
;  ENTRY:								     ;
;        DL : (R)ed value (0,255)                                            ;
;        AH : (G)reen (0,255)						     ;
;        AL : (B)lue  (0,255)						     ;
;									     ;
;  RETURN:								     ;
;        DH : the converted color byte, bit following bit significance:	     ;
;             bit 0 (LSB)   :    the red plane color                         ;
;             bit 1         :    the green plane color                       ;
;             bit 2         :    the blue plane color                        ;
;             bit 3         :    the intensity bit (4 plane only)	     ;
;             bit 4         :    (R+G+B > BW_THRESHOLD):1 ? 0 monochrome bit ;
;             bit 5         :    (if bits 0,1,2,3 are all 1 or 0) : 1 ? 0    ;
;             bits 6,7(MSB) :    0                                           ;
;                                                                            ;
;        DL, AH, AL : the physical color triplet (same order as entry.)      ;
;                                                                            ;
;  PRESERVES:                                                                ;
;       BX, CX                                                               ;
;                                                                            ;
;  CALLS:                                                                    ;
;       EuclideanMatch                                                       ;
;                                                                            ;
;-----------------------------------------------------------------------------

cProc	SolidColorMatch, <NEAR, PUBLIC>, <bx, cx>
cBegin

; Compute InverseMap index in BX.  Account for the table being two bytes
; wide.  (Red => bits 1,2; Green => bits 3,4; Blue => bits 5,6)

        mov     bl, dl
        and     bx, 0C0H
        shr     bl, 5
        mov     cx, ax
        and     cx, 0C0C0H
        shr     cx, 1
        or      bl, cl
        shr     cx, 2
        or      bl, ch

; If the second nibble of the InverseMap is zero, we're done.

        mov     cx, word ptr cs:[bx].VgaInverseMap
        test    cl, 0f0h
        jz      SolidColorMatch_Translate

; If the fourth nibble is non-zero, there is an optimization for this sub-cube.

        test    ch, 0f0h
        jz      SolidColorMatch_Euclidean

        push    cx
        mov     bl, dl                  ; Compute index to VgaStaticMap
        and     bx, 20H
        shr     bl, 5
        mov     cx, ax
        and     cx, 2020H
        shr     cx, 3
        or      bl, cl
        shr     cx, 1
        or      bl, ch
        pop     cx

        mov     bl, cs:[bx].VgaStaticMap
        or      bl, bl
        jz      SolidColorMatch_Euclidean
        dec     bl
        jnz     @F
        and     cl, 0fh
        jmp     SolidColorMatch_Translate
@@:
        shr     cl, 4
        jmp     SolidColorMatch_Translate

; We must perform a reduced Euclidean match.

SolidColorMatch_Euclidean:
        cCall   EuclideanMatch
        jmp     SolidColorMatch_Done

; Translate the VGA palette index to the appropriate index for the device.

SolidColorMatch_Translate:
        mov     bl, cl
        mov     bl, cs:[bx].VgaTranslateMap

; Return the translate VGA palette index in DH.  Return RGB components
; of the physical color chosen in DL, AH, AL.

SolidColorMatch_Done:
        mov     dh, bl
	or	dh, cs:[bx].PAccelTable
	mov	ax, bx
        add     bx, bx
	add	bx, ax
	mov	ax, word ptr cs:[bx].PColorTable[0]
	mov	dl, cs:[bx].PColorTable[2]

cEnd

;-----------------------------------------------------------------------------
; EuclideanMatch							     ;
;       chooses the closer VGA palette index from an RGB triplet             ;
;									     ;
;  ENTRY:								     ;
;       DL : (R)ed value (0,255)                                             ;
;       AH : (G)reen (0,255)						     ;
;       AL : (B)lue  (0,255)						     ;
;                                                                            ;
;       CLL : First VGA palette index                                        ;
;       CLH : Second VGA palette index                                       ;
;       CHL : Third VGA palette index or zero                                ;
;       CHH : unused                                                         ;
;                                                                            ;
;       BH : 0                                                               ;
;									     ;
;  RETURN:								     ;
;       BL : the converted color byte                                        ;
;       BH : 0                                                               ;
;                                                                            ;
;-----------------------------------------------------------------------------

cProc	EuclideanMatch, <NEAR, PUBLIC>, <si, di>

        localW  Red
        localW  Green
        localW  Blue
        localB  VgaPal4         ; always zero
        localB  VgaPal3
        localB  VgaPal2
        localB  VgaPal1

cBegin

; First, save all our register parameters.

        mov     bl, al
        mov     Blue, bx
        mov     bl, ah
        mov     Green, bx
        mov     bl, dl
        mov     Red, bx

        mov     bl, cl
        and     bl, 0fh
        mov     VgaPal1, bl
        shr     cl, 4
        mov     bl, cl
        mov     VgaPal2, bl
        mov     bl, ch
        and     bl, 0fh
        mov     VgaPal3, bl
        mov     VgaPal4, 0

; Initialize the loop.

        mov     si, 0ffffh              ; SI = smallest unsigned distance
        lea     di, VgaPal1             ; DI = VgaPal to use

; Loop through the possible VGA palette indices.

EuclideanMatch_loop:
        mov     bl, ss:[di]
        or      bl, bl
        jz      EuclideanMatch_done

        mov     bl, cs:[bx].VgaTranslateMap
        mov     cl, bl
        add     bl, bl
        add     bl, cl
        push    cx                      ; store the translated index

        xor     ax, ax
        mov     al, cs:[bx].PColorTable[0]
        sub     ax, Blue
        imul    ax
        mov     cx, ax

        xor     ax, ax
        mov     al, cs:[bx].PColorTable[1]
        sub     ax, Green
        imul    ax
        add     cx, ax

        xor     ax, ax
        mov     al, cs:[bx].PColorTable[2]
        sub     ax, Red
        imul    ax
        add     cx, ax

        inc     di
        pop     dx                      ; restore the translated index
        cmp     cx, si
        jae     EuclideanMatch_loop
        mov     si, cx
        mov     VgaPal1, dl
        jmp     EuclideanMatch_loop

EuclideanMatch_done:
        mov     bl, VgaPal1

cEnd

;-----------------------------------------------------------------------------


endif   ;EUCLIDEAN_COLOR_MATCH


sEnd	Code
end
