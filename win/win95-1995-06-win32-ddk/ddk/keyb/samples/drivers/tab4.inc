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

; ** TAB4.INC ********************************************
;
; Keyboard translation table for Windows 4.00
;
; Otherwise, this contains the type 4 (Enhanced keyboard) translation
; tables for some keyboard table DLL, and the patch tables required
; to alter the above tables for all other keyboard types (1..6).
;
; ********************************************************

    if1
    %out  .. include TAB4.INC
    endif

; ********************************************************
;	These equates determine which dead key table
;	translations are used in any keyboard for this
;	country.
; ********************************************************

; define DGRAVE DACUTE DCIRCUMFLEX DUMLAUT DTILDE DCEDILLA
; to include the translations for those accents.

	; No grave deadkeys
	; No acute deadkeys
	; No circumflex deadkeys
	; No umlaut deadkeys
	; No tilde deadkeys
	; No cedilla deadkeys

; ********************************************************
; define whether XT (type 1) and AT (type 3) tables are the same:
; ********************************************************

ATSAME	equ	1

; ********************************************************
;
; definitions of deadkey accent characters for this country.
; Tilde is always '~', circumflex is always '^'.
; Umlaut and acute may vary for different countries.
;
; ********************************************************

umlaut		equ	0a8h
acute		equ	0b4h
grave		equ	060h
circumflex	equ	'^'
tilde		equ	'~'
cedilla		equ	0b8h

; ********************************************************
;
; AscTranVK is an array of virtual keycodes, used as keys to
; search the WORD array AscTran for pairs of bytes (unshifted
; and shifted ASCII).
;
; ********************************************************

	public PatchATran4

IRP VKFLAG, <0, 1>

    klabels AscTranVK, AscTran, VKFLAG

	; This group is common to all keyboards.
	ktrans2	VK_SPACE  ,	' ',  	' ',	VKFLAG
	ktrans2	VK_TAB    ,	09h,	09h,	VKFLAG
	ktrans2	VK_RETURN ,	0dh,	0dh,	VKFLAG
	ktrans2	VK_BACK   ,	08h,	08h,	VKFLAG
	ktrans2	VK_ESCAPE ,	01bh,	01bh,	VKFLAG
	ktrans2	VK_CANCEL ,	03h,	03h,	VKFLAG

	; Variable keys.  These are:
	; VK_0..VK_9,  VK_OEM_COMMA, VK_OEM_PERIOD VK_OEM_MINUS, VK_OEM_PLUS,
	; VK_OEM_1..VK_OEM_8, VK_OEM_102, and VK_DECIMAL

    klabels PatchATranVK, PatchATran4, VKFLAG

;	Variable entries in AscTab[]:
	ktrans2	VK_0      ,	'0',	')',	VKFLAG
	ktrans2	VK_1      ,	'1',	'!',	VKFLAG
	ktrans2	VK_2      ,	'2',	'@',	VKFLAG
	ktrans2	VK_3      ,	'3',	'#',	VKFLAG
	ktrans2	VK_4      ,	'4',	'$',	VKFLAG
	ktrans2	VK_5      ,	'5',	'%',	VKFLAG
	ktrans2	VK_6      ,	'6',	'^',	VKFLAG
	ktrans2	VK_7      ,	'7',	'&',	VKFLAG
	ktrans2	VK_8      ,	'8',	'*',	VKFLAG
	ktrans2	VK_9      ,	'9',	'(',	VKFLAG
	ktrans2	VK_OEM_COMMA,	',',	'<',	VKFLAG
	ktrans2	VK_OEM_PERIOD,	'.',	'>',	VKFLAG
	ktrans2	VK_OEM_MINUS,	'-',	'_',	VKFLAG
	ktrans2	VK_OEM_PLUS ,	'=',	'+',	VKFLAG
	ktrans2	VK_OEM_1    ,	';',	':',	VKFLAG
	ktrans2	VK_OEM_2    ,	'/',	'?',	VKFLAG
	ktrans2	VK_OEM_3    ,	'`',	'~',	VKFLAG
	ktrans2	VK_OEM_4    ,	'[',	'{',	VKFLAG
	ktrans2	VK_OEM_5    ,	'\',	'|',	VKFLAG
	ktrans2	VK_OEM_6    ,	']',	'}',	VKFLAG
	ktrans2	VK_OEM_7    ,	027h,	'"',	VKFLAG
	ktrans2	VK_OEM_8    ,	-1,	-1,	VKFLAG
	ktrans2	VK_OEM_102,	'\',	'|',	VKFLAG
	ktrans2	VK_DECIMAL,	'.',	'.',	VKFLAG


    ; The keypad translations MUST be AFTER the VK_OEM_*,
    ; to make VkKeyScan() work properly!
    ; VK_DECIMAL is first, since it sometimes (Danish, Finnish/Swedish,
    ; German) translates to comma.

	ktrans2	VK_MULTIPLY,	'*',  	'*',	VKFLAG
	ktrans2	VK_SUBTRACT,	'-',  	'-',	VKFLAG
	ktrans2	VK_ADD    ,	'+',  	'+',	VKFLAG
	ktrans2	VK_DIVIDE ,	'/',  	'/',	VKFLAG

    klabdef AscTranEnd, VKFLAG

ENDM



; ********************************************************
;
; This table associates a combination of ASCII code and accent
; with an ANSI accented character.
;
; There is a section below for each dead key, in an IFDEF.
; This table will be the same for all keyboards, for a particular
; DLL, even if some keyboards for a country have fewer dead keys
; than others.
;
; ********************************************************


IRP VKFLAG, <0, 1>

    klabels DeadKeyCode, DeadChar, VKFLAG

    ; grave
    ifdef DGRAVE
	deadtrans	'a', grave,	0E0h,	VKFLAG
	deadtrans	'e', grave,	0E8h,	VKFLAG
	deadtrans	'i', grave,	0ECh,	VKFLAG
	deadtrans	'o', grave,	0F2h,	VKFLAG
	deadtrans	'u', grave,	0F9h,	VKFLAG
	deadtrans	'A', grave,	0C0h,	VKFLAG
	deadtrans	'E', grave,	0C8h,	VKFLAG
	deadtrans	'I', grave,	0CCh,	VKFLAG
	deadtrans	'O', grave,	0D2h,	VKFLAG
	deadtrans	'U', grave,	0D9h,	VKFLAG
	deadtrans	' ', grave,	060h,	VKFLAG
    endif

    ; acute
    ifdef DACUTE

	deadtrans	'a', acute,	0E1h,	VKFLAG
	deadtrans	'e', acute,	0E9h,	VKFLAG
	deadtrans	'i', acute,	0EDh,	VKFLAG
	deadtrans	'o', acute,	0F3h,	VKFLAG
	deadtrans	'u', acute,	0FAh,	VKFLAG
	deadtrans	'y', acute,	0FDh,	VKFLAG
	deadtrans	'A', acute,	0C1h,	VKFLAG
	deadtrans	'E', acute,	0C9h,	VKFLAG
	deadtrans	'I', acute,	0CDh,	VKFLAG
	deadtrans	'O', acute,	0D3h,	VKFLAG
	deadtrans	'U', acute,	0DAh,	VKFLAG
	deadtrans	' ', acute,	0B4h,	VKFLAG
    endif

    ; circumflex
    ifdef DCIRCUMFLEX
	deadtrans	'a', circumflex,	0E2h,	VKFLAG
	deadtrans	'e', circumflex,	0EAh,	VKFLAG
	deadtrans	'i', circumflex,	0EEh,	VKFLAG
	deadtrans	'o', circumflex,	0F4h,	VKFLAG
	deadtrans	'u', circumflex,	0FBh,	VKFLAG
	deadtrans	'A', circumflex,	0C2h,	VKFLAG
	deadtrans	'E', circumflex,	0CAh,	VKFLAG
	deadtrans	'I', circumflex,	0CEh,	VKFLAG
	deadtrans	'O', circumflex,	0D4h,	VKFLAG
	deadtrans	'U', circumflex,	0DBh,	VKFLAG
	deadtrans	' ', circumflex,	'^',	VKFLAG
    endif

    ; umlaut
    ifdef DUMLAUT
	deadtrans	'a', umlaut,	0E4h,	VKFLAG
	deadtrans	'e', umlaut,	0EBh,	VKFLAG
	deadtrans	'i', umlaut,	0EFh,	VKFLAG
	deadtrans	'o', umlaut,	0F6h,	VKFLAG
	deadtrans	'u', umlaut,	0FCh,	VKFLAG
	deadtrans	'y', umlaut,	0FFh,	VKFLAG
	deadtrans	'A', umlaut,	0C4h,	VKFLAG
	deadtrans	'E', umlaut,	0CBh,	VKFLAG
	deadtrans	'I', umlaut,	0CFh,	VKFLAG
	deadtrans	'O', umlaut,	0D6h,	VKFLAG
	deadtrans	'U', umlaut,	0DCh,	VKFLAG
	deadtrans	' ', umlaut,	'"',	VKFLAG
    endif

    ; tilde
    ifdef DTILDE
	deadtrans	'a', tilde,	0e3h,	VKFLAG
	deadtrans	'o', tilde,	0f5h,	VKFLAG
	deadtrans	'n', tilde,	0F1h,	VKFLAG
	deadtrans	'A', tilde,	0c3h,	VKFLAG
	deadtrans	'O', tilde,	0d5h,	VKFLAG
	deadtrans	'N', tilde,	0D1h,	VKFLAG
	deadtrans	' ', tilde,	'~' ,	VKFLAG
    endif

    ; cedilla
    ifdef DCEDILLA
	deadtrans	'c', cedilla,	231,	VKFLAG
	deadtrans	'C', cedilla,	199,	VKFLAG
    endif

    klabdef DeadKeyEnd, VKFLAG

ENDM


; ********************************************************
;
; This string identifies the table type (nationality).
;
;
; ********************************************************


CountryName label byte

	db	'kbdus.asm'
	db	0


; ********************************************************
;
; This table lists the virtual scancodes of the dead keys.
; Each entry lists the scan code and a shift state for which
; this key is a dead key, and the translated dead key.
;
;
; ********************************************************

    public Morto, MortoCode

IRP VKFLAG, <0, 1>

    klabels Morto, MortoCode, VKFLAG


    klabdef MortoEnd, VKFLAG

	; No Padding Needed

ENDM


; ********************************************************
;
; translations for control characters.
;
; ********************************************************

    public VarAscCtrlVK, VarAscCtrl

IRP VKFLAG, <0, 1>

    klabels AscControlVK, AscControl, VKFLAG

    ; this part is FIXED

	ktrans	VK_CANCEL ,	03h,	VKFLAG
	ktrans	VK_BACK   ,	07fh,	VKFLAG
	ktrans	VK_RETURN ,	0ah,	VKFLAG
	ktrans	VK_ESCAPE ,	01bh,	VKFLAG
	ktrans	VK_SPACE  ,  	020h,	VKFLAG
;;	ktrans	VK_EXECUTE,	0ah,	VKFLAG

    ; starting here, these tables may be variable.

    klabels VarAscCtrlVK, VarAscCtrl, VKFLAG

	ktrans	VK_2      ,	080h,	VKFLAG
	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_OEM_4    ,	01bh,	VKFLAG
	ktrans	VK_OEM_5    ,	01ch,	VKFLAG
	ktrans	VK_OEM_6    ,	01dh,	VKFLAG
	ktrans	VK_OEM_MINUS,	09fh,	VKFLAG
	ktrans	VK_OEM_102,	01ch,	VKFLAG

    klabdef AscControlEnd, VKFLAG

	; No Padding Needed

ENDM


; ********************************************************
;
; These list translations for keys with CTRL-ALT and SHIFT-CTRL-ALT.
;
; ********************************************************

    public AscCtlAltVK, AscCtlAlt

IRP VKFLAG, <0, 1>

    klabels AscCtlAltVK, AscCtlAlt, VKFLAG

; Control-Alt insertions for type 4 

    klabdef AscCtlAltEnd, VKFLAG

	; Padding
;;	ktrans	000h,	00h,	VKFLAG ;; comment out for TAB4.INC

ENDM


; ********************************************************
;
; Shift-Control-Alt
;
; ********************************************************

    public AscShCtlAltVK, AscShCtlAlt

IRP VKFLAG, <0, 1>

    klabels AscShCtlAltVK, AscShCtlAlt, VKFLAG

; Shift-Ctrl-Alt insertions for type 4 

    klabdef AscShCtlAltEnd, VKFLAG

	; Padding
;;	ktrans	000h,	00h,	VKFLAG	;; comment out!
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG
;;	ktrans	000h,	00h,	VKFLAG

ENDM


; ********************************************************
;
; This table lists the virtual keycodes of keys with caps lock
; or shift lock, other than letters VK_A..VK_Z.
;
; ********************************************************

    public CapitalTable

CapitalTable label byte

	; db	VK_code

; Capital table, type 4

CapitalEnd label byte

	; No Padding Needed



; ********************************************************
;
; This table handles shiftlock translation on the Swiss-German
; keyboard only.
;
; ********************************************************


IRP VKFLAG, <0, 1>

    klabels SGCapsVK, SGTrans, VKFLAG

ifdef SGCAPS
	ktrans	VK_OEM_1,	0dch, VKFLAG	; U umlaut
	ktrans	VK_OEM_7,	0d6h, VKFLAG	; O umlaut
	ktrans	VK_OEM_5,	0c4h, VKFLAG	; A umlaut
endif

    klabdef SGCapsEnd, VKFLAG

ENDM


KeyEnd label byte



    if1
    %out  .. end TAB4.INC
    endif

