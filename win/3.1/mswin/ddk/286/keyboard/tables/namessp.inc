;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NamesSP.INC -- Tables for GetKeyNameText() -- Spanish
;
; Copyright 1989, 1990 by Microsoft Corporation.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; If you put an accented character in a string, place the whole
; string in angle brackets:  <"abc",accent,"def">

; History (latest first)
;	18 dec 89	peterbe		Added degree sign for ring accent.
;	10 oct 89	thijsg		keys (re-) translated
;	15 jul 89	peterbe		Added additional function key support.
;	20 jun 89	thijsg		Sent to Windows SYS development
;	21 jun 89	peterbe		Checked in

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

	STAB	flag,	lESC,	nEsc,		"ESC"
	STAB	flag,	lBS,	nBS,		"RETROCESO"
	STAB	flag,	lTAB,	nTab,		"TABULACION"
	STAB	flag,	lENT,	nEnter, 	"ENTRAR"
	STAB	flag,	lCTRL,	nCtrl,		"CTRL"
	STAB	flag,	lSH,	nShift, 	"MAYUSCULAS"
        ; left-hand '/' key has other translations on non-USA keyboards.
        ; the Num Div key is actually an extended key..
	STAB	flag,	lND,	nNumDiv,	"TECLA DE DIVISION"
	STAB	flag,	lRSh,	nRShift,	"MAYUSCULAS DERECHA"
	STAB	flag,	lNM,	nNumMult,	"TECLA DE MULTIPLICACION"
	STAB	flag,	lAlt,	nAlt,		"ALT"
	STAB	flag,	lSP,	nSpace, 	"BARRA ESPACIADORA"
	STAB	flag,	lCaps,	nCapsLock,	"BLOQ MAYUS"
	STAB	flag,	lF1,	nF1,		"F1"
	STAB	flag,	lF2,	nF2,		"F2"
	STAB	flag,	lF3,	nF3,		"F3"
	STAB	flag,	lF4,	nF4,		"F4"
	STAB	flag,	lF5,	nF5,		"F5"
	STAB	flag,	lF6	nF6,		"F6"
	STAB	flag,	lF7,	nF7,		"F7"
	STAB	flag,	lF8,	nF8,		"F8"
	STAB	flag,	lF9,	nF9,		"F9"
	STAB	flag,	lF10,	nF10,		"F10"
	STAB	flag,	lPause,	nPause,		"PAUSA"
	STAB	flag,	lScr,	nScroll,	"BLOQ DESPL"
	STAB	flag,	lNum7,	nNum7,		"NUMERO 7"
	STAB	flag,	lNum8,	nNum8,		"NUMERO 8"
	STAB	flag,	lNum9,	nNum9,		"NUMERO 9"
	STAB	flag,	lNumSb, nNumSub,	"TECLA DE SUSTRACCION"
	STAB	flag,	lNum4,	nNum4,		"NUMERO 4"
	STAB	flag,	lNum5,	nNum5,		"NUMERO 5"
	STAB	flag,	lNum6,	nNum6,		"NUMERO 6"
	STAB	flag,	lNumPl, nNumPlus,	"TECLA DE ADICION"
	STAB	flag,	lNum1,	nNum1,		"NUMERO 1"
	STAB	flag,	lNum2,	nNum2,		"NUMERO 2"
	STAB	flag,	lNum3,	nNum3,		"NUMERO 3"
	STAB	flag,	lNum0,	nNum0,		"NUMERO 0"
	STAB	flag,	lNumDc, nNumDec,	"TECLA DECIMAL"
	STAB	flag,	lF11,	nF11,		"F11"
	STAB	flag,	lF12,	nF12,		"F12"

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
	STAB	flag,	lhelp,	nhelp,		"AYUDA"  ; ICO (extend bit set)
	STAB	flag,	lClear, nClear, 	"BORRAR" ; ICO (extend bit set)
	STAB	flag,	lBreak, nBreak,    "INTERRUMPIR" ; ICO (extend bit set)
	STAB	flag,	l00,	n00		"<00>"	; ICO (extend bit is
							; set on second 0)

; strings for extended keys.
	STAB	flag,	lPrtS,	nPrtScreen,	"IMP PANT"
	STAB	flag,	lNumL,	nNumLock,	"BLOQ NUM"
	STAB	flag,	lNumEn, nNumEnter,	"INTRO"
	STAB	flag,	lAltGr, nAltGr, 	"ALT DERECHA"
	STAB	flag,	lHome,	nHome,		"INICIO"
	STAB	flag,	lUP,	nUP,		"FLECHA ARRIBA"
	STAB	flag,	lPGUP,	nPGUP,		"RE PAG"
	STAB	flag,	lLEFT,	nLEFT,		"FLECHA IZQUIERDA"
	STAB	flag,	lRIGHT, nRIGHT, 	"FLECHA DERECHA"
	STAB	flag,	lEND,	nEND,		"FIN"
	STAB	flag,	lDOWN,	nDOWN,		"FLECHA ABAJO"
	STAB	flag,	lPGDN,	nPGDOWN,	"AV PAG"
	STAB	flag,	lINS,	nINSERT,	"INSERT"
	STAB	flag,	lDEL,	nDELETE,	"SUPR"
	STAB	flag,	lRCTRL, nRCtrl, 	"CTRL DERECHA"


; Strings for dead keys.
; NOTE: Translate these even if your keyboard doesn't have these dead keys!

	STAB	flag,	dACUTE, nACUTE, 	"AGUDO"
        STAB    flag,   dGRAVE, nGRAVE,         "GRAVE"
	STAB	flag,	dCIRCU, nCIRCUMFLEX,	"CIRCUNFLEJO"
	STAB	flag,	dUMLAU, nUMLAUT,	"DIERERIS"
        STAB    flag,   dTILDE, nTILDE,         "TILDE"
	STAB	flag,	dCED,	nCEDILLA,	"CEDILLA"
	STAB	flag,	dRING,	nRING		176	; prob. Iceland only

endm    ; End of IRP
