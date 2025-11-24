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

        page    ,132
;*****************************************************************************
;
; MapVirtualKey
;
;*****************************************************************************
;*****************************************************************************

VKFUN = 0

.xlist
include keyboard.inc

include vkwin.inc
include vkoem.inc
.list
include lcid.inc

if1
%out
%out   mapVirt.asm  (Microsoft)
endif
createSeg _MAPVK, MAPVK, BYTE, PUBLIC, CODE

        TITLE   MAPVIRTUALKEYEX - NEW ASCII translation routine

;*****************************************************************************

regptr          esdi,    es,di
by              equ      byte ptr
externFP        GetLCIDtable

;*****************************************************************************

sBegin   DATA

externD CurrentLocale

        ;***
        ;*** These are the values that are (a) always the same regardless of
        ;*** state, and (b) always the same regardless of keyboard.
        ;***
        ;*** Good assumption, but in case it's not true, we check this list
        ;*** AFTER the lcid list.
        ;***

ScanCodes       label   byte
        DB   0eh        ; 14    VK_BACK         
        DB   0fh        ; 15    VK_TAB          
        DB   01ch       ; 28    VK_RETURN       
        DB   039h       ; 57    VK_SPACE        
        DB   01h        ; 1     VK_ESCAPE       

        DB   04ah       ; 74    VK_SUBTRACT     
        DB   04eh       ; 78    VK_ADD          
        DB   053h       ; 83    VK_DELETE       

        DB   00h        ; 0     -1              
        DB   0ch        ; 12    VK_OEM_MINUS    ; variable
        DB   0dh        ; 13    VK_OEM_PLUS     ; variable
        DB   01ah       ; 26    VK_OEM_4        ; variable
        DB   01bh       ; 27    VK_OEM_6        ; variable
        DB   01dh       ; 29    VK_CONTROL      
        DB   027h       ; 39    VK_OEM_1        ; variable -- also VK_M
        DB   028h       ; 40    VK_OEM_7        ; variable
        DB   029h       ; 41    VK_OEM_3        ; variable
        DB   02ah       ; 42    VK_SHIFT        
        DB   02bh       ; 43    VK_OEM_5        ; variable

        DB   033h       ; 51    VK_OEM_COMMA    ; variable
        DB   034h       ; 52    VK_OEM_PERIOD   ; variable
        DB   035h       ; 53    VK_OEM_2        ; variable

        DB   036h       ; 54    VK_SHIFT        
        DB   037h       ; 55    VK_MULTIPLY     
        DB   038h       ; 56    VK_MENU         
        DB   03ah       ; 58    VK_CAPITAL      
        DB   03bh       ; 59    VK_F1           
        DB   03ch       ; 60    VK_F2           
        DB   03dh       ; 61    VK_F3           
        DB   03eh       ; 62    VK_F4           
        DB   03fh       ; 63    VK_F5           
        DB   040h       ; 64    VK_F6           
        DB   041h       ; 65    VK_F7           
        DB   042h       ; 66    VK_F8           
        DB   043h       ; 67    VK_F9           
        DB   044h       ; 68    VK_F10          

        DB   045h       ; 69    VK_NUMLOCK      
        DB   046h       ; 70    VK_OEM_SCROLL   
;
; NOTE: VK value and scan code are the same! (except prefix for DOS)
;
	db	VK_LWIN		; 5b
	db	VK_RWIN		; 5c
	db	VK_APPS		; 5d

        ; This is always in normal IBM-compatible driver:

        DB  056h        ; 86    VK_OEM_102       
        DB  057h        ; 87    VK_F11           
        DB  058h        ; 88    VK_F12           
        ; This is the end of the table for drivers handling Enhanced keyboards!

NumPad  label byte
        public NumPad
        DB   052h       ; 82    VK_INSERT       
        DB   04fh       ; 79    VK_END          
        DB   050h       ; 80    VK_DOWN         
        DB   051h       ; 81    VK_NEXT         
        DB   04bh       ; 75    VK_LEFT         
        DB   04ch       ; 76    VK_CLEAR        
        DB   04dh       ; 77    VK_RIGHT        
        DB   047h       ; 71    VK_HOME         
        DB   048h       ; 72    VK_UP           
        DB   049h       ; 73    VK_PRIOR        

        DB   052h       ; 82    VK_NUMPAD0
        DB   04fh       ; 79    VK_NUMPAD1
        DB   050h       ; 80    VK_NUMPAD2
        DB   051h       ; 81    VK_NUMPAD3
        DB   04bh       ; 75    VK_NUMPAD4
        DB   04ch       ; 76    VK_NUMPAD5
        DB   04dh       ; 77    VK_NUMPAD6
        DB   047h       ; 71    VK_NUMPAD7
        DB   048h       ; 72    VK_NUMPAD8
        DB   049h       ; 73    VK_NUMPAD9
	DB   053h	;	VK_DECIMAL

KBD_SCANS 	equ $ - ScanCodes

;---------------------------------------------------------------------------

BaseAnsi label byte
        db      '0123456789'
VkBaseAnsi label byte
        db      '/*-+.', 3
        db      8,9,0dH, 020H, 01bH

BaseAnsi_Len    equ     $-BaseAnsi
VkBaseAnsi_Len  equ     $-VkBaseAnsi
;---------------------------------------------------------------------------

AsciiVirtKeys   label   byte
        db      060h, 061h, 062h, 063h, 064h, 065h, 066h, 067h
        db      068h, 069h

AsciiVirtKeysNoNumpad label byte
        public AsciiVirtKeys, BaseAnsi, BaseAnsi_Len

VkVirtKeys        label   byte
        db      06fh, 06ah, 06dh, 06bh, 06eh, 3

VirtKeys        label   byte
        DB      VK_BACK         ; 0eh 14
        DB      VK_TAB          ; 0fh 15
        DB      VK_RETURN       ; 01ch 28
        DB      VK_SPACE        ; 039h 57
        DB      VK_ESCAPE       ; 01h 1

        DB      VK_SUBTRACT     ; 04ah 74
        DB      VK_ADD          ; 04eh 78
        DB      VK_DELETE       ; 053h 83

        DB      -1              ; 00h 0
        DB      VK_OEM_MINUS    ; 0ch 12        ; variable
        DB      VK_OEM_PLUS     ; 0dh 13        ; variable
        DB      VK_OEM_4        ; 01ah 26       ; variable
        DB      VK_OEM_6        ; 01bh 27       ; variable
        DB      VK_CONTROL      ; 01dh 29
        DB      VK_OEM_1        ; 027h 39       ; variable -- also VK_M
        DB      VK_OEM_7        ; 028h 40       ; variable
        DB      VK_OEM_3        ; 029h 41       ; variable
        DB      VK_SHIFT        ; 02ah 42
        DB      VK_OEM_5        ; 02bh 43       ; variable

        DB      VK_OEM_COMMA    ; 033h 51       ; variable
        DB      VK_OEM_PERIOD   ; 034h 52       ; variable
        DB      VK_OEM_2        ; 035h 53       ; variable

        DB      VK_SHIFT        ; 036h 54
        DB      VK_MULTIPLY     ; 037h 55
        DB      VK_MENU         ; 038h 56
        DB      VK_CAPITAL      ; 03ah 58
        DB      VK_F1           ; 03bh 59
        DB      VK_F2           ; 03ch 60
        DB      VK_F3           ; 03dh 61
        DB      VK_F4           ; 03eh 62
        DB      VK_F5           ; 03fh 63
        DB      VK_F6           ; 040h 64
        DB      VK_F7           ; 041h 65
        DB      VK_F8           ; 042h 66
        DB      VK_F9           ; 043h 67
        DB      VK_F10          ; 044h 68

        DB      VK_NUMLOCK      ; 045h 69
        DB      VK_OEM_SCROLL   ; 046h 70

	db	VK_LWIN		; 5b
	db	VK_RWIN		; 5c
	db	VK_APPS		; 5d

        ; This is always in normal IBM-compatible driver:

        DB      VK_OEM_102      ; 056h 86
        DB      VK_F11          ; 057h 87
        DB      VK_F12          ; 058h 88
        ; This is the end of the table for drivers handling Enhanced keyboards!

        DB      VK_INSERT       ; 052h 82
        DB      VK_END          ; 04fh 79
        DB      VK_DOWN         ; 050h 80
        DB      VK_NEXT         ; 051h 81
        DB      VK_LEFT         ; 04bh 75
        DB      VK_CLEAR        ; 04ch 76
        DB      VK_RIGHT        ; 04dh 77
        DB      VK_HOME         ; 047h 71
        DB      VK_UP           ; 048h 72
        DB      VK_PRIOR        ; 049h 73

        DB      VK_NUMPAD0      ; 052h 82
        DB      VK_NUMPAD1      ; 04fh 79
        DB      VK_NUMPAD2      ; 050h 80
        DB      VK_NUMPAD3      ; 051h 81
        DB      VK_NUMPAD4      ; 04bh 75
        DB      VK_NUMPAD5      ; 04ch 76
        DB      VK_NUMPAD6      ; 04dh 77
        DB      VK_NUMPAD7      ; 047h 71
        DB      VK_NUMPAD8      ; 048h 72
        DB      VK_NUMPAD9      ; 049h 73
        DB      VK_DECIMAL      ; 053h 

KBD_VIRTKEYS 	equ $ - VirtKeys

SCAN_TO_VKEY_OFFS	equ ((dataOFFSET VirtKeys)-(1+(dataOFFSET ScanCodes)))

        public VirtKeys, ScanCodes, KBD_SCANS, SCAN_TO_VKEY_OFFS

sEnd     DATA

;*****************************************************************************


;  The MapVirtualKey function is used by the PIF editor and
;  Winoldap to get information about keyboard mapping.
;
;  This maps the value in wCode into the returned value in AX,
;  according to the value of wMapType:
;
;  wMapType == 0: Map VK to scan code
;
;  wMapType == 1: Map scan code to VK
;
;  wMapType == 2: Map VK to Ascii.
;
;  (Larger values reserved for future expansion)
;
;***************************************************************************


sBegin MAPVK
assumes CS,MAPVK
assumes DS,DATA

.386

MapVirtualKey label far
        public MapVirtualKey
        mov     ax,     ds      ; prolgue
        nop
        mov     es,     ax
        pop     ebx             ; get back the return address
        mov     ecx,    es:CurrentLocale
        push    ecx
        push    ebx
        jmp     MapVirtualKeyEx+3


;***************************************************************************

cProc MapVirtualKeyEx,<PUBLIC,FAR, PASCAL, LOADDS>

        parmW   wCode                   ; value to be translated
        parmW   wMapType
        parmD   lcid

	localW	iDiacritics
cBegin		
        push    esi
        push    edi

        mov     edx, lcid
        call    GetLCIDtable
        or      bx, bx
        jz      BadMap

	xor	ax, ax
	mov	iDiacritics, ax		; cases 0 and 1
        mov     al, by wCode
        mov     cx, wMapType
        or      cx, cx
        jz      VirtKeyToScanCode
        dec     cx
        jz      ScanCodeToVirtKey
        dec     cx
        jnz     BadMap

;-----------------------------------------------------------------------------
VirtKeyToAnsi:

	mov	cx, [bx+NUM_DEAD]		; if we have diacritics
	mov	iDiacritics, cx

        mov     cx,  VkBaseAnsi_Len
	shl	ecx, 16

        mov     di,  dataOFFSET AsciiVirtKeysNoNumpad
        shl     edi, 16
        mov     di,  [bx+VKEYTOIDX]

        mov     si,  dataOFFSET VkBaseAnsi
	shl     esi, 16
        mov     si,  [bx+VKEYTOANSI]

        jmp     DoTheScan

;-----------------------------------------------------------------------------
ScanCodeToVirtKey:

        mov     cx,  KBD_SCANS
	shl	ecx, 16

        mov     si,  dataOFFSET VirtKeys
	shl     esi, 16
	mov     si,  [bx+VKEYTOIDX]

        mov     di,  dataOFFSET ScanCodes
        shl     edi, 16
        mov     di,  [bx+SCANTOIDX]
        jmp     DoTheScan

;-----------------------------------------------------------------------------
BadMap:
        xor     ax,     ax
        jmp     MapVirtDone

;-----------------------------------------------------------------------------
VirtKeyToScanCode:

        mov     cx,  KBD_SCANS
	shl	ecx, 16
        mov     di,  dataOFFSET VirtKeys
	shl     edi, 16
        mov     di,  [bx+VKEYTOIDX]
        mov     si,  dataOFFSET ScanCodes
        shl     esi, 16
        mov     si,  [bx+SCANTOIDX]
DoTheScan:
        mov     cx,  [bx+SCAN_SIZE]
Translate:
        mov     dx,  di
        repnz   scasb
        jz      GetOtherValue

TryStdKbdScan:
        ;
        ; test the base values that stay the same across every language, ie 
        ; VK_LEFT. 
        ;
        ror     esi, 16
        ror     edi, 16
	ror	ecx, 16
        mov     dx,  di
        repnz   scasb
        jnz     BadMap
GetOtherValue:
        dec     di
        sub     di,  dx
        add     si,  di
        lodsb
        xor     ah,  ah

;-----------------------------------------------------------------------------
;
; this will only occur for Vk to Ansi, and only for deadkey'ed tabled
;
	mov	cx, iDiacritics
	jcxz	MapVirtDone
	
	mov	di, [bx+DEAD_KEYS]
	test	[bx+LCID_FLAGS], DEADCOMBOS
	jz	NormalDead
	
	mov	bx, 4			; bump di by this value
@@:
	cmp	al, es:[di]
	jz	FoundDead
	add	di, bx
	loop	@B
	jmp	MapVirtDone		; didn't find it.

NormalDead:
	repnz	scasb
	jnz	MapVirtDone
FoundDead:
	mov	ah, 80H			; dead key!
;-----------------------------------------------------------------------------
MapVirtDone:
        pop     edi
        pop     esi
cEnd
.286p

sEnd MAPVK

end

