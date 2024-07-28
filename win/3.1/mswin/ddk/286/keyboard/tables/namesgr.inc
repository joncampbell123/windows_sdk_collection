;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NamesGR.INC -- Tables for GetKeyNameText() -- German
;
; Copyright 1989, 1990 by Microsoft Corporation.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; If you put an accented character in a string, place the whole
; string in angle brackets:  <"abc",accent,"def">

; History (last first)
;	18 dec 89	peterbe		Added degree sign for ring accent.
;	10 oct 89	peterbe		Use multiply, divide signs.
;	10 oct 89	thijsg		corrections, additional keys translated
;	15 jul 89	peterbe		Added additional function key support.
;	19 jun 89	thijsg		Sent to Windows SYS development
;	20 jun 89	peterbe		Added "STRG RECHTS"

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
	STAB	flag,	lBS,	nBS,		<"R",220,"CK">
	STAB	flag,	lTAB,	nTab,		"TABULATOR"
	STAB	flag,	lENT,	nEnter, 	"EINGABE"
	STAB	flag,	lCTRL,	nCtrl,		"STRG"
	STAB	flag,	lSH,	nShift, 	"UMSCHALT"
        ; left-hand '/' key has other translations on non-USA keyboards.
        ; the Num Div key is actually an extended key..
	; 247 is divide sign, 215 is multiply sign
        STAB    flag,   lND,    nNumDiv,        <247," (ZEHNERTASTATUR)">
        ; I would rather take the "v" instead of the "/" but it doesn't
        ; exist in the ANSI character set (ASCII code is 37). andreass
        STAB    flag,   lRSh,   nRShift,        "UMSCHALT RECHTS"
	STAB	flag,	lNM,	nNumMult,	<215," (ZEHNERTASTATUR)">
        STAB    flag,   lAlt,   nAlt,           "ALT"
	STAB	flag,	lSP,	nSpace, 	"LEER"
	STAB	flag,	lCaps,	nCapsLock,	"FESTSTELL"
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
  	STAB	flag,	lPause,	nPause,		"PAUSE"
	STAB	flag,	lScr,	nScroll,	"ROLLEN-FESTSTELL"
	STAB	flag,	lNum7,	nNum7,		"7 (ZEHNERTASTATUR)"
	STAB	flag,	lNum8,	nNum8,		"8 (ZEHNERTASTATUR)"
	STAB	flag,	lNum9,	nNum9,		"9 (ZEHNERTASTATUR)"
	STAB	flag,	lNumSb, nNumSub,	"- (ZEHNERTASTATUR)"
	STAB	flag,	lNum4,	nNum4,		"4 (ZEHNERTASTATUR)"
	STAB	flag,	lNum5,	nNum5,		"5 (ZEHNERTASTATUR)"
	STAB	flag,	lNum6,	nNum6,		"6 (ZEHNERTASTATUR)"
	STAB	flag,	lNumPl, nNumPlus,	"+ (ZEHNERTASTATUR)"
	STAB	flag,	lNum1,	nNum1,		"1 (ZEHNERTASTATUR)"
	STAB	flag,	lNum2,	nNum2,		"2 (ZEHNERTASTATUR)"
	STAB	flag,	lNum3,	nNum3,		"3 (ZEHNERTASTATUR)"
	STAB	flag,	lNum0,	nNum0,		"0 (ZEHNERTASTATUR)"
	STAB	flag,	lNumDc, nNumDec,	"KOMMA (ZEHNERTASTATUR)"
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
        STAB    flag,   lhelp,  nhelp,         "HILFE"  ; ICO (extend bit set)
        STAB    flag,   lClear, nClear,   <"L",214,"SCH"> ; ICO (extend bit set)
        STAB    flag,   lBreak, nBreak,         "UNTBR" ; ICO (extend bit set)
	STAB	flag,	l00,	n00		"<00>"	; ICO (extend bit is
							; set on second 0)

; strings for extended keys.
	STAB	flag,	lPrtS,	nPrtScreen,	"DRUCK"
	STAB	flag,	lNumL,	nNumLock,	"NUM-FESTSTELL"
	STAB	flag,	lNumEn, nNumEnter,	"EINGABE (ZEHNERTASTATUR)"
	STAB	flag,	lAltGr, nAltGr, 	"ALT GR"
	STAB	flag,	lHome,	nHome,		"POS1"
        STAB    flag,   lUP,    nUP,            "NACH-OBEN"
        STAB    flag,   lPGUP,  nPGUP,          "BILD-NACH-OBEN"
        STAB    flag,   lLEFT,  nLEFT,          "NACH-LINKS"
        STAB    flag,   lRIGHT, nRIGHT,         "NACH-RECHTS"
	STAB	flag,	lEND,	nEND,		"ENDE"
        STAB    flag,   lDOWN,  nDOWN,          "NACH-UNTEN"
        STAB    flag,   lPGDN,  nPGDOWN,        "BILD-NACH-UNTEN"
	STAB	flag,	lINS,	nINSERT,	"EINFG"
	STAB	flag,	lDEL,	nDELETE,	"ENTF"
        STAB    flag,   lRCTRL, nRCtrl,         "STRG-RECHTS"

; Strings for dead keys.
; NOTE: Translate these even if your keyboard doesn't have these dead keys!

	STAB	flag,	dACUTE, nACUTE, 	"AKUT"
	STAB	flag,	dGRAVE, nGRAVE, 	"GRAVIS"
	STAB	flag,	dCIRCU, nCIRCUMFLEX,	"ZIRKUMFLEX"
        STAB    flag,   dUMLAU, nUMLAUT,        "UMLAUT"
        STAB    flag,   dTILDE, nTILDE,         "TILDE"
	STAB	flag,	dCED,	nCEDILLA,	"CEDILLE"
	STAB	flag,	dRING,	nRING		176	; prob. Iceland only

endm    ; End of IRP
