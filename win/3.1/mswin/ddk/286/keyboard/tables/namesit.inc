;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NamesIT.INC -- Tables for GetKeyNameText() -- Italian
;
; Copyright 1989, 1990 by Microsoft Corporation.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; If you put an accented character in a string, place the whole
; string in angle brackets:  <"abc",accent,"def">

; History (latest first)
;	18 dec 89	peterbe		Added degree sign for ring accent.
;	10 oct 89	thijsg		additional keys translated
;	15 jul 89	peterbe		Added additional function key support.
;	20 jun 89	thijsg		Sent to Windows SYS development
;	21 jun 89	peterbe		Checked in, use 217 for 'U`'

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
	STAB	flag,	lENT,	nEnter, 	"INVIO"
        STAB    flag,   lCTRL,  nCtrl,          "CTRL"
	STAB	flag,	lSH,	nShift, 	"MAIUSC"
        ; left-hand '/' key has other translations on non-USA keyboards.
        ; the Num Div key is actually an extended key..
	STAB	flag,	lND,	nNumDiv,	"/ (Tn)"
	STAB	flag,	lRSh,	nRShift,	"MAIUSC (DESTRA)"
	STAB	flag,	lNM,	nNumMult,	"* (Tn)"
        STAB    flag,   lAlt,   nAlt,           "ALT"
	STAB	flag,	lSP,	nSpace, 	"BARRA SPAZIATRICE"
	STAB	flag,	lCaps,	nCapsLock,	"BLOC MAIUS"
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
	STAB	flag,	lPause,	nPause,		"PAUSA"
	STAB	flag,	lScr,	nScroll,	"BLOC SCORR"
	STAB	flag,	lNum7,	nNum7,		"7 (Tn)"
	STAB	flag,	lNum8,	nNum8,		"8 (Tn)"
	STAB	flag,	lNum9,	nNum9,		"9 (Tn)"
	STAB	flag,	lNumSb, nNumSub,	"- (Tn)"
	STAB	flag,	lNum4,	nNum4,		"4 (Tn)"
	STAB	flag,	lNum5,	nNum5,		"5 (Tn)"
	STAB	flag,	lNum6,	nNum6,		"6 (Tn)"
	STAB	flag,	lNumPl, nNumPlus,	<"PI",217," (Tn)">
	STAB	flag,	lNum1,	nNum1,		"1 (Tn)"
	STAB	flag,	lNum2,	nNum2,		"2 (Tn)"
	STAB	flag,	lNum3,	nNum3,		"3 (Tn)"
	STAB	flag,	lNum0,	nNum0,		"0 (Tn)"
	STAB	flag,	lNumDc, nNumDec,	". (Tn)"
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
	STAB	flag,	lhelp,	nhelp,		"AIUTO"	; ICO (extend bit set)
	STAB	flag,	lClear,	nClear,		"CANCELLA" ; ICO (extend bit set)
	STAB	flag,	lBreak,	nBreak,		"INTERR" ; ICO (extend bit set)
	STAB	flag,	l00,	n00		"<00>"	; ICO (extend bit is
							; set on second 0)

; strings for extended keys.
	STAB	flag,	lPrtS,	nPrtScreen,	"STAMPA"
	STAB	flag,	lNumL,	nNumLock,	"BLOC NUM"
	STAB	flag,	lNumEn, nNumEnter,	"INVIO (TN)"
	STAB	flag,	lAltGr, nAltGr, 	"ALT GR"
        STAB    flag,   lHome,  nHome,          "HOME"
	STAB	flag,	lUP,	nUP,		"Freccia SU"
	STAB	flag,	lPGUP,	nPGUP,		"PGSU"
	STAB	flag,	lLEFT,	nLEFT,		"Freccia SINISTRA"
	STAB	flag,	lRIGHT, nRIGHT, 	"Freccia DESTRA"
	STAB	flag,	lEND,	nEND,		"FINE"
	STAB	flag,	lDOWN,	nDOWN,		<"GI",217>
	STAB	flag,	lPGDN,	nPGDOWN,	<"PGGI",217>
        STAB    flag,   lINS,   nINSERT,        "INS"
        STAB    flag,   lDEL,   nDELETE,        "RITORNO"
	STAB	flag,	lRCTRL, nRCtrl, 	"CTRL (DESTRA)"


; Strings for dead keys.
; NOTE: Translate these even if your keyboard doesn't have these dead keys!

	STAB	flag,	dACUTE, nACUTE, 	"ACUTO"
        STAB    flag,   dGRAVE, nGRAVE,         "GRAVE"
	STAB	flag,	dCIRCU, nCIRCUMFLEX,	"CIRCONFLESSO"
	STAB	flag,	dUMLAU, nUMLAUT,	"DIERESI"
        STAB    flag,   dTILDE, nTILDE,         "TILDE"
	STAB	flag,	dCED,	nCEDILLA,	"CEDIGLIA"
	STAB	flag,	dRING,	nRING		176	; prob. Iceland only

endm    ; End of IRP
