;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NamesDU.INC -- Tables for GetKeyNameText() -- Dutch
;
; Copyright 1989, 1990 by Microsoft Corporation.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; If you put an accented character in a string, place the whole
; string in angle brackets:  <"abc",accent,"def">

; History (latest first)
;	18 dec 89	peterbe		Added degree sign for ring accent.
;	13 oct 89	peterbe		NUM-DECIMAAL shows comma, not '.'
;	13 oct 89	thijsg		one translation updated
;	10 oct 89	thijsg		additional keys translated
;	15 jul 89	peterbe		Added additional function key support.
;	21 jun 89	thijsg		Sent to Windows SYS development

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; StringTab[]:
;
; Table of key names.  StringTab[] is an array of offsets to the strings,
; indiced by the numeric values nEsc, nBS, ... nDELETE, which are
; consecutive integers, defined by the macros used here.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        temp    =       0       ; init. variable for defining indices.

IRP flag, <0,1>

    if  flag eq 1
            StringTab   label   word
    endif

; Strings -- normal.

        ;       select  label   index           string

        STAB    flag,   lESC,   nEsc,           "ESC"
        STAB    flag,   lBS,    nBS,            "BACKSPACE"
        STAB    flag,   lTAB,   nTab,           "TAB"
        STAB    flag,   lENT,   nEnter,         "ENTER"
        STAB    flag,   lCTRL,  nCtrl,          "CTRL"
        STAB    flag,   lSH,    nShift,         "SHIFT"
        ; left-hand '/' key has other translations on non-USA keyboards.
        ; the Num Div key is actually an extended key..
	STAB	flag,	lND,	nNumDiv,	"NUM-SLASH (/)"
	STAB	flag,	lRSh,	nRShift,	"RECHTS-SHIFT"
	STAB	flag,	lNM,	nNumMult,	"NUM-STERRETJE (*)"
        STAB    flag,   lAlt,   nAlt,           "ALT"
	STAB	flag,	lSP,	nSpace, 	"SPATIEBALK"
        STAB    flag,   lCaps,  nCapsLock,      "CAPS-LOCK"
        STAB    flag,   lF1,    nF1,            "F1"
        STAB    flag,   lF2,    nF2,            "F2"
        STAB    flag,   lF3,    nF3,            "F3"
        STAB    flag,   lF4,    nF4,            "F4"
        STAB    flag,   lF5,    nF5,            "F5"
        STAB    flag,   lF6     nF6,            "F6"
        STAB    flag,   lF7,    nF7,            "F7"
        STAB    flag,   lF8,    nF8,            "F8"
        STAB    flag,   lF9,    nF9,            "F9"
        STAB    flag,   lF10,   nF10,           "F10"
	STAB	flag,	lPause,	nPause,		"Pause"
	STAB	flag,	lScr,	nScroll,	"SCROLL-LOCK"
	STAB	flag,	lNum7,	nNum7,		"NUM-7"
	STAB	flag,	lNum8,	nNum8,		"NUM-8"
	STAB	flag,	lNum9,	nNum9,		"NUM-9"
	STAB	flag,	lNumSb, nNumSub,	"NUM-MINTEKEN (-)"
	STAB	flag,	lNum4,	nNum4,		"NUM-4"
	STAB	flag,	lNum5,	nNum5,		"NUM-5"
	STAB	flag,	lNum6,	nNum6,		"NUM-6"
	STAB	flag,	lNumPl, nNumPlus,	"NUM-PLUSTEKEN (+)"
	STAB	flag,	lNum1,	nNum1,		"NUM-1"
	STAB	flag,	lNum2,	nNum2,		"NUM-2"
	STAB	flag,	lNum3,	nNum3,		"NUM-3"
	STAB	flag,	lNum0,	nNum0,		"NUM-0"
	STAB	flag,	lNumDc, nNumDec,	"NUM-DECIMAAL (,)"
        STAB    flag,   lF11,   nF11,           "F11"
        STAB    flag,   lF12,   nF12,           "F12"

	STAB	flag,	lF13,	nF13,		"F13"	; ICO, NOKIA, others
	STAB	flag,	lF14,	nF14,		"F14"
	STAB	flag,	lF15,	nF15,		"F15"
	STAB	flag,	lF16,	nF16,		"F16"
	STAB	flag,	lF17,	nF17,		"F17"
	STAB	flag,	lF18,	nF18,		"F18"
	STAB	flag,	lF19,	nF19,		"F19"
	STAB	flag,	lF20,	nF20,		"F20"
	STAB	flag,	lF21,	nF21,		"F21"
	STAB	flag,	lF22,	nF22,		"F22"
	STAB	flag,	lF23,	nF23,		"F23"
	STAB	flag,	lF24,	nF24,		"F24"
	STAB	flag,	lhelp,	nhelp,		"HELP"	; ICO (extend bit set)
	STAB	flag,	lClear,	nClear,		"CLEAR"	; ICO (extend bit set)
	STAB	flag,	lBreak,	nBreak,		"BREAK"	; ICO (extend bit set)
	STAB	flag,	l00,	n00		"<00>"	; ICO (extend bit is
							; set on second 0)

; strings for extended keys.
	STAB	flag,	lPrtS,	nPrtScreen,	"PRINT-SCRN"
	STAB	flag,	lNumL,	nNumLock,	"NUM-LOCK"
	STAB	flag,	lNumEn, nNumEnter,	"NUM-ENTER"
	STAB	flag,	lAltGr, nAltGr, 	"ALT-GR"
        STAB    flag,   lHome,  nHome,          "HOME"
	STAB	flag,	lUP,	nUP,		"PIJL-OMHOOG"
        STAB    flag,   lPGUP,  nPGUP,          "PGUP"
	STAB	flag,	lLEFT,	nLEFT,		"PIJL-LINKS"
	STAB	flag,	lRIGHT, nRIGHT, 	"PIJL-RECHTS"
        STAB    flag,   lEND,   nEND,           "END"
	STAB	flag,	lDOWN,	nDOWN,		"PIJL-OMLAAG"
	STAB	flag,	lPGDN,	nPGDOWN,	"PGDN"
	STAB	flag,	lINS,	nINSERT,	"INS"
	STAB	flag,	lDEL,	nDELETE,	"DEL"
	STAB	flag,	lRCTRL, nRCtrl, 	"RECHTS-CTRL"

; Strings for dead keys.
; NOTE: Translate these even if your keyboard doesn't have these dead keys!

	STAB	flag,	dACUTE, nACUTE, 	<"ACCENT-AIGU (",180,")">
	STAB	flag,	dGRAVE, nGRAVE, 	<"ACCENT-GRAVE (",96,")">
	STAB	flag,	dCIRCU, nCIRCUMFLEX,	<"CIRCUMFLEX (",94,")">
	STAB	flag,	dUMLAU, nUMLAUT,	<"TREMA (",168,")">
	STAB	flag,	dTILDE, nTILDE, 	<"TILDE (",126,")">
	STAB	flag,	dCED,	nCEDILLA,	<"CEDILLE (",184,")">
	STAB	flag,	dRING,	nRING		176	; prob. Iceland only

endm    ; End of IRP
