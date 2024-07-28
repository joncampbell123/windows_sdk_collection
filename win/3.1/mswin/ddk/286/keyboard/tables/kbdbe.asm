; 'kbdbe.asm' Windows 3.00 keyboard table file based on:
;  'fr1.wk2', 'fr2.wk2', 'fr1.wk2', 'be4.wk2', 'fr5.wk2', 'be6.wk2'
;
; ********************************************************
; Copyright (C) 1989-1990 by Microsoft Corporation.
; ********************************************************
;
; Belgian keyboards (only type 4 differs from KBDFR DLL)

; History
;	08 jan 90	peterbe		Nokia ifdef
;	06 sep 89	peterbe		Commented out VK_EXECUTE translation.
;	31 aug 89	peterbe		Added dead key for Y acute.
;	14 jul 89	peterbe		SHIFT + VK_TAB ==> ascii TAB now.

; Table 3 (for 'AT' 84 or 86-key keyboards) is the same as table 1
; ********************************************************
;
; Keyboard translation table for Windows 3.00
;
; if INDRIVER is set, this is just an include file with the actual
; tables in it, to be included in the TABS.ASM file.
;
; Otherwise, this contains the type 4 (Enhanced keyboard) translation
; tables for some keyboard table DLL, and the patch tables required
; to alter the above tables for all other keyboard types (1..6).
;
; ********************************************************

; ********************************************************
;	These equates determine which dead key table
;	translations are used in any keyboard for this
;	country.
; ********************************************************

; define DGRAVE DACUTE DCIRCUMFLEX DUMLAUT DTILDE DCEDILLA
; to include the translations for those accents.

DGRAVE		equ	1
DACUTE		equ	1
DCIRCUMFLEX	equ	1
DUMLAUT		equ	1
DTILDE		equ	1
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
;	The CODE segment of this file contains information
;	for patching the tables in the DATA segment.
;
; ********************************************************

include keyboard.inc
include vkwin.inc
include vkoem.inc

; macros for translation table entry.
include trans.inc

if1
%out
%out .. KBDBE.ASM (Belgian)
endif


sBegin CODE

    assumes ds,DATA


; ********************************************************
;
; Tables to patch the keyTrTab table in the driver.
; These tables are fixed in size.
;
; ********************************************************

; Numbers of entries to patch

	public	nIndices, PatchIndices, TransPatches
	public	X1, X2, Patch102

nIndices dw	CODEoffset IndicesEnd - CODEoffset PatchIndices

; This table maps an index to TransPatchN into the corresponding
; entry in keyTrTab in the driver:

PatchIndices label byte
	db	12,13,16,17,21,26,27,30,39,40
    X1 label byte
	db	41
    X2 label byte
	db	43,44,50,51,52,53
    Patch102 label byte
	db	86				; 98 for Nokia type 6
	db	87,88				; Enhanced VK_F11, VK_F12
IndicesEnd label byte

; patch values for keyTrTab.

TransPatches label word

	dw	CODEoffset TransPatch1		; 1
	dw	CODEoffset TransPatch2		; 2
	dw	CODEoffset TransPatch3		; 3
	dw	CODEoffset TransPatch4		; 4
ifdef NOKIA
	dw	CODEoffset TransPatch5		; 5
	dw	CODEoffset TransPatch6		; 6
endif

; may be all the same -- but usually not!

TransPatch1 label byte 		; This is for type 1
	; Same as next
TransPatch3 label byte		; This is for type 3
	db	VK_OEM_4    	; 0ch 12
	db	VK_OEM_MINUS	; 0dh 13
	db	VK_A      	; 010h 16
	db	VK_Z      	; 011h 17
	db	VK_Y      	; 015h 21
	db	VK_OEM_6    	; 01ah 26
	db	VK_OEM_1    	; 01bh 27
	db	VK_Q      	; 01eh 30
	db	VK_M      	; 027h 39
	db	VK_OEM_3    	; 028h 40
	db	VK_OEM_7    	; 029h 41
	db	VK_OEM_102	; 02bh 43
	db	VK_W      	; 02ch 44
	db	VK_OEM_COMMA	; 032h 50
	db	VK_OEM_PERIOD	; 033h 51
	db	VK_OEM_2    	; 034h 52
	db	VK_OEM_PLUS 	; 035h 53
	db	VK_OEM_102	; 056h 86
	db	VK_F11	; 057h 87
	db	VK_F12	; 058h 88

TransPatch2 label byte		; This is for type 2 (ICO)
	db	VK_OEM_4    	; 0ch 12
	db	VK_OEM_PLUS 	; 0dh 13
	db	VK_A      	; 010h 16
	db	VK_Z      	; 011h 17
	db	VK_Y      	; 015h 21
	db	VK_OEM_6    	; 01ah 26
	db	VK_OEM_1    	; 01bh 27
	db	VK_Q      	; 01eh 30
	db	VK_M      	; 027h 39
	db	VK_OEM_3    	; 028h 40
	db	VK_OEM_7    	; 029h 41
	db	VK_OEM_5    	; 02bh 43
	db	VK_W      	; 02ch 44
	db	VK_OEM_COMMA	; 032h 50
	db	VK_OEM_PERIOD	; 033h 51
	db	VK_OEM_2    	; 034h 52
	db	VK_OEM_8    	; 035h 53
	db	VK_ICO_HELP 	; 056h 86
	db	VK_RETURN 	; 057h 87
	db	VK_LEFT   	; 058h 88

TransPatch4 label byte		; This is for type 4
	db	VK_OEM_4    	; 0ch 12
	db	VK_OEM_MINUS	; 0dh 13
	db	VK_A      	; 010h 16
	db	VK_Z      	; 011h 17
	db	VK_Y      	; 015h 21
	db	VK_OEM_6    	; 01ah 26
	db	VK_OEM_1    	; 01bh 27
	db	VK_Q      	; 01eh 30
	db	VK_M      	; 027h 39
	db	VK_OEM_3    	; 028h 40
	db	VK_OEM_7    	; 029h 41
	db	VK_OEM_5    	; 02bh 43
	db	VK_W      	; 02ch 44
	db	VK_OEM_COMMA	; 032h 50
	db	VK_OEM_PERIOD	; 033h 51
	db	VK_OEM_2    	; 034h 52
	db	VK_OEM_PLUS 	; 035h 53
	db	VK_OEM_102	; 056h 86
	db	VK_F11	; 057h 87
	db	VK_F12	; 058h 88

ifdef NOKIA
TransPatch5 label byte
	db	VK_OEM_4    	; 0ch 12
	db	VK_OEM_MINUS	; 0dh 13
	db	VK_A      	; 010h 16
	db	VK_Z      	; 011h 17
	db	VK_Y      	; 015h 21
	db	VK_OEM_6    	; 01ah 26
	db	VK_OEM_1    	; 01bh 27
	db	VK_Q      	; 01eh 30
	db	VK_M      	; 027h 39
	db	VK_OEM_3    	; 028h 40
	db	VK_OEM_7    	; 029h 41
	db	VK_OEM_102	; 02bh 43
	db	VK_W      	; 02ch 44
	db	VK_OEM_COMMA	; 032h 50
	db	VK_OEM_PERIOD	; 033h 51
	db	VK_OEM_2    	; 034h 52
	db	VK_OEM_PLUS 	; 035h 53
	db	0ffh	; 056h 86
	db	0ffh	; 057h 87
	db	0ffh	; 058h 88

TransPatch6 label byte		; This is for type 6 (NOKIA)
	db	VK_OEM_4    	; 0ch 12
	db	VK_OEM_2    	; 0dh 13
	db	VK_A      	; 010h 16
	db	VK_Z      	; 011h 17
	db	VK_Y      	; 015h 21
	db	VK_OEM_6    	; 01ah 26
	db	VK_OEM_1    	; 01bh 27
	db	VK_Q      	; 01eh 30
	db	VK_OEM_8    	; 027h 39
	db	VK_OEM_3    	; 028h 40
	db	VK_OEM_7    	; 029h 41
	db	VK_OEM_5    	; 02bh 43
	db	VK_W      	; 02ch 44
	db	VK_M      	; 032h 50
	db	VK_OEM_COMMA	; 033h 51
	db	VK_OEM_PERIOD	; 034h 52
	db	VK_OEM_MINUS	; 035h 53
	db	0ech	; 056h 86
	db	VK_HELP   	; 057h 87
	db	0ffh	; 058h 88
endif

; End of keyTrTab patches.

; ********************************************************
;
; Everything from HeaderBeg to HeaderEnd gets copied to variables
; with the same names in the driver.
;
; ********************************************************

    public szHeader, HeaderBeg
    public szAscTran, szAscControl, szAscCtlAlt, szAscShCtlAlt

    public pCapital, szCapital
    public pMorto, szMorto, pMortoCode
    public szDeadKey, pDeadKeyCode, pDeadChar
    public szSGCaps, pSGCapsVK, pSGTrans

    public VirtAdr, AsciiTab

szHeader dw	CODEoffset HeaderEnd - CODEoffset HeaderBeg

HeaderBeg label byte

; ********************************************************
;
; Keyboard table type
;
;	This value is patched after the header is copied.
;
; ********************************************************

	public TableType

TableType	db	4


; ********************************************************
;
; This flags special features like caps lock.
;
;	kbAltGr	=	right alt key is ctrl-alt
;	kbShiftLock =	has shift lock instead of caps lock
;
;	This value is patched after the header
;	is copied to the driver.
;
; ********************************************************

	public fKeyType

fKeyType label byte	; db	kbAltGr+kbShiftLock
	db	0 + kbAltGr + kbShiftLock

; ********************************************************
;
; Table sizes (Number of bytes in search table for a particular
; translation).
;
; Order of entries must be maintained!
; First 4 entries are accessed by indexing based on shift state.
;
; Also, the whole list of pointers is transferred to the code module
; with one REP MOVSB instruction.
;
; If the table type is not 4, the sizes at the beginning, and the
; table type and flags, are updated after the header is copied.
;
; ********************************************************

; These sizes may vary depending on the keyboard type.

szAscTran	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
szAscControl	dw	DATAoffset AscControlEnd - DATAoffset AscControlVK
szAscCtlAlt	dw	DATAoffset AscCtlAltEnd - DATAoffset AscCtlAltVK
szAscShCtlAlt	dw	DATAoffset AscShCtlAltEnd - DATAoffset AscShCtlAltVK

szMorto		dw	DATAoffset MortoEnd - DATAoffset Morto
szSGCaps	dw	DATAoffset SGCapsEnd - DATAoffset SGCapsVK

szCapital	dw	DATAoffset CapitalEnd - DATAoffset CapitalTable

; These sizes are fixed for a particular country.
szDeadKey	dw	DATAoffset DeadKeyEnd - DATAoffset DeadKeyCode

; ********************************************************
;
; Addresses of virtual key code arrays for various shifts. 
;
; ********************************************************

VirtAdr label word
		dw	DATAoffset AscTranVK		; shifted, unshifted
		dw	DATAoffset AscControlVK		; Control
		dw	DATAoffset AscCtlAltVK		; Control-Alt
		dw	DATAoffset AscShCtlAltVK	; Shift-Control-Alt

pMorto		dw	DATAoffset Morto
pDeadKeyCode	dw	DATAoffset DeadKeyCode
pSGCapsVK	dw	DATAoffset SGCapsVK

pCapital	dw	DATAoffset CapitalTable

; ********************************************************
;
; addresses of translated character arrays for various shifts.
;
; ********************************************************

AsciiTab label word
		dw	DATAoffset AscTran		; shifted, unshifted
		dw	DATAoffset AscControl		; Control
		dw	DATAoffset AscCtlAlt		; Control-Alt
		dw	DATAoffset AscShCtlAlt		; Shift-Control-Alt

pMortoCode	dw	DATAoffset MortoCode
pDeadChar	dw	DATAoffset DeadChar
pSGTrans	dw	DATAoffset SGTrans


; ********************************************************
;
;	End of Header.
; ********************************************************

	public	HeaderEnd

HeaderEnd label byte

; ********************************************************
;	Patching tables
; ********************************************************

; ********************************************************
;	Patches for flags and sizes at beginning of header.
;
;	These tables overlay the beginning of the 'header'
;	in the driver, after the header has been copied,
;	if the keyboard type is not 4.
; ********************************************************

	public BegPatches

BegPatches label word
	dw	CODEoffset sz1
	dw	CODEoffset sz2
	dw	CODEoffset sz3
	dw	0
ifdef NOKIA
	dw	CODEoffset sz5
	dw	CODEoffset sz6
endif

; Each one of these tables is 8 words long.

sz1 label word
	db	1	; overlays TableType
	db	0
	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
	dw	6 + CODEoffset PatchACtrl1End - CODEoffset PatchACtrl1VK
	dw	CODEoffset PatchACtlAlt1End - CODEoffset PatchACtlAlt1VK
	dw	CODEoffset PatchAShCtlAlt1End - CODEoffset PatchAShCtlAlt1VK
	dw	CODEoffset PatchMortoEnd1 - CODEoffset PatchMorto1
	dw	0	; SGCaps size = 0
	dw	CODEoffset PatchCapEnd1 - CODEoffset PatchCap1

sz2 label word
	db	2
	db	0
	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
	dw	6 + CODEoffset PatchACtrl2End - CODEoffset PatchACtrl2VK
	dw	CODEoffset PatchACtlAlt2End - CODEoffset PatchACtlAlt2VK
	dw	CODEoffset PatchAShCtlAlt2End - CODEoffset PatchAShCtlAlt2VK
	dw	CODEoffset PatchMortoEnd2 - CODEoffset PatchMorto2
	dw	0	; SGCaps size = 0
	dw	CODEoffset PatchCapEnd2 - CODEoffset PatchCap2

sz3 label word
	db	3
	db	0
	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
	dw	6 + CODEoffset PatchACtrl3End - CODEoffset PatchACtrl3VK
	dw	CODEoffset PatchACtlAlt3End - CODEoffset PatchACtlAlt3VK
	dw	CODEoffset PatchAShCtlAlt3End - CODEoffset PatchAShCtlAlt3VK
	dw	CODEoffset PatchMortoEnd3 - CODEoffset PatchMorto3
	dw	0	; SGCaps size = 0
	dw	CODEoffset PatchCapEnd3 - CODEoffset PatchCap3

ifdef NOKIA
sz5 label word
	db	5
	db	0
	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
	dw	6 + CODEoffset PatchACtrl5End - CODEoffset PatchACtrl5VK
	dw	CODEoffset PatchACtlAlt5End - CODEoffset PatchACtlAlt5VK
	dw	CODEoffset PatchAShCtlAlt5End - CODEoffset PatchAShCtlAlt5VK
	dw	CODEoffset PatchMortoEnd5 - CODEoffset PatchMorto5
	dw	0	; SGCaps size = 0
	dw	CODEoffset PatchCapEnd5 - CODEoffset PatchCap5

sz6 label word
	db	6
	db	0
	dw	DATAoffset AscTranEnd - DATAoffset AscTranVK
	dw	6 + CODEoffset PatchACtrl6End - CODEoffset PatchACtrl6VK
	dw	CODEoffset PatchACtlAlt6End - CODEoffset PatchACtlAlt6VK
	dw	CODEoffset PatchAShCtlAlt6End - CODEoffset PatchAShCtlAlt6VK
	dw	CODEoffset PatchMortoEnd6 - CODEoffset PatchMorto6
	dw	0	; SGCaps size = 0
	dw	CODEoffset PatchCapEnd6 - CODEoffset PatchCap6
endif

; ********************************************************
;
;	Patches to 'normal' AsciiTran.
;
;	This translation table is fixed in size, so no size
;	adjustment is necessary.
;
;	The patches are just an overlay of the translations
;	of VK_0 .. VK_DECIMAL in AscTran.
;
;	One of the following arrays is just copied to
;	PatchATran4.
;
;	These may seem excessively large and redundant, but
;	it's all disposable!
;
; ********************************************************

	public	szPatchATran
	public	PatchATran

szPatchATran label word
	; all the same size, but repeat for code simplicity.
	dw	CODEoffset PatchATran1End - CODEoffset PatchATran1
	dw	CODEoffset PatchATran1End - CODEoffset PatchATran1
	dw	CODEoffset PatchATran1End - CODEoffset PatchATran1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchATran1End - CODEoffset PatchATran1
	dw	CODEoffset PatchATran1End - CODEoffset PatchATran1
endif

PatchATran label word

	dw	CODEoffset PatchATran1
	dw	CODEoffset PatchATran2
	dw	CODEoffset PatchATran3
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchATran5
	dw	CODEoffset PatchATran6
endif


PatchATran1 label word		; XT table

ifdef ATSAME
PatchATran3 label word
endif

	db	0e0h,	'0'	; VK_0      
	db	'&',	'1'	; VK_1      
	db	0e9h,	'2'	; VK_2      
	db	'"',	'3'	; VK_3      
	db	027h,	'4'	; VK_4      
	db	'(',	'5'	; VK_5      
	db	0a7h,	'6'	; VK_6      
	db	0e8h,	'7'	; VK_7      
	db	'!',	'8'	; VK_8      
	db	0e7h,	'9'	; VK_9      
	db	',',	'?'	; VK_OEM_COMMA
	db	';',	'.'	; VK_OEM_PERIOD
	db	'-',	'_'	; VK_OEM_MINUS
	db	'=',	'+'	; VK_OEM_PLUS 
	db	'$',	'*'	; VK_OEM_1    
	db	':',	'/'	; VK_OEM_2    
	db	0f9h,	'%'	; VK_OEM_3    
	db	')',	0b0h	; VK_OEM_4    
	db	'#',	0b5h	; VK_OEM_5    
	; Unshifted is dead key
	; Shifted is dead key
	db	-1,	-1	; VK_OEM_6    
	db	0b5h,	0a3h	; VK_OEM_7    
	db	'!',	0a7h	; VK_OEM_8    
	db	'<',	'>'	; VK_OEM_102
	db	'.',	'.'	; VK_DECIMAL

PatchATran1End label word

ifndef ATSAME
PatchATran3 label word
; (Same as XT)

endif

PatchATran2 label word	; ICO table

	db	0e0h,	'0'	; VK_0      
	db	0a3h,	'1'	; VK_1      
	db	0e9h,	'2'	; VK_2      
	db	'"',	'3'	; VK_3      
	db	027h,	'4'	; VK_4      
	db	'(',	'5'	; VK_5      
	db	'-',	'6'	; VK_6      
	db	0e8h,	'7'	; VK_7      
	db	'_',	'8'	; VK_8      
	db	0e7h,	'9'	; VK_9      
	db	',',	'?'	; VK_OEM_COMMA
	db	';',	'.'	; VK_OEM_PERIOD
	db	-1,	-1	; VK_OEM_MINUS
	db	'=',	'+'	; VK_OEM_PLUS 
	db	'$',	'*'	; VK_OEM_1    
	db	':',	'/'	; VK_OEM_2    
	db	0f9h,	'%'	; VK_OEM_3    
	db	')',	0b0h	; VK_OEM_4    
	db	'<',	'>'	; VK_OEM_5    
	; Unshifted is dead key
	; Shifted is dead key
	db	-1,	-1	; VK_OEM_6    
	db	'`',	'&'	; VK_OEM_7    
	db	'!',	0a7h	; VK_OEM_8    
	db	-1,	-1	; VK_OEM_102
	db	'.',	'.'	; VK_DECIMAL

ifdef NOKIA
PatchATran5 label word

	db	0e0h,	'0'	; VK_0      
	db	'&',	'1'	; VK_1      
	db	0e9h,	'2'	; VK_2      
	db	'"',	'3'	; VK_3      
	db	027h,	'4'	; VK_4      
	db	'(',	'5'	; VK_5      
	db	0a7h,	'6'	; VK_6      
	db	0e8h,	'7'	; VK_7      
	db	'!',	'8'	; VK_8      
	db	0e7h,	'9'	; VK_9      
	db	',',	'?'	; VK_OEM_COMMA
	db	';',	'.'	; VK_OEM_PERIOD
	db	'-',	'_'	; VK_OEM_MINUS
	db	'=',	'+'	; VK_OEM_PLUS 
	db	'$',	'*'	; VK_OEM_1    
	db	':',	'/'	; VK_OEM_2    
	db	0f9h,	'%'	; VK_OEM_3    
	db	')',	0b0h	; VK_OEM_4    
	db	'#',	0b5h	; VK_OEM_5    
	; Unshifted is dead key
	; Shifted is dead key
	db	-1,	-1	; VK_OEM_6    
	db	0b5h,	0a3h	; VK_OEM_7    
	db	'!',	0a7h	; VK_OEM_8    
	db	'<',	'>'	; VK_OEM_102
	db	'.',	'.'	; VK_DECIMAL

PatchATran6 label word

	db	'0',	'='	; VK_0      
	db	'1',	'!'	; VK_1      
	db	'2',	'"'	; VK_2      
	db	'3',	'#'	; VK_3      
	db	'4',	'$'	; VK_4      
	db	'5',	'%'	; VK_5      
	db	'6',	'+'	; VK_6      
	db	'7',	'/'	; VK_7      
	db	'8',	'('	; VK_8      
	db	'9',	')'	; VK_9      
	db	',',	';'	; VK_OEM_COMMA
	db	'.',	':'	; VK_OEM_PERIOD
	db	'-',	'_'	; VK_OEM_MINUS
	db	-1,	-1	; VK_OEM_PLUS 
	db	'&',	'*'	; VK_OEM_1    
	; Unshifted is dead key
	; Shifted is dead key
	db	-1,	-1	; VK_OEM_2    
	db	0e0h,	0f9h	; VK_OEM_3    
	db	027h,	'?'	; VK_OEM_4    
	db	'<',	'>'	; VK_OEM_5    
	db	0e8h,	'['	; VK_OEM_6    
	db	'`',	']'	; VK_OEM_7    
	db	0e9h,	0e7h	; VK_OEM_8    
	db	-1,	-1	; VK_OEM_102
	db	'.',	'.'	; VK_DECIMAL

endif

; End of patches for AsciiTran.

; ********************************************************
;
;	For the other tables, the number of entries varies
;	among the various keyboard types, so the size
;	table szXXXXX entry must be adjusted in the
;	header before the header is copied.
;
;	If any table is longer than the corresponding table
;	for the Type 4 keyboard, padding must be put at the
;	end of that table in the DATA segment to allow
;	for overwriting with additional entries.
;
; ********************************************************


; ********************************************************
;
;	Patches to 'AscControl' table.
;
;	The overlays for this table are in both the 'key'
;	list AscControlVK, and the translated Ascii list
;	'AscControl'.  Also, they first part of AscControlVK
;	and AscControl are fixed -- the changeable part
;	of each array is labeled.
;
; ********************************************************

	public	szPatchACtrl
	public	PatchACtrlVK
	public	PatchACtrl

szPatchACtrl label word
	dw CODEoffset PatchACtrl1End - CODEoffset PatchACtrl1VK
	dw CODEoffset PatchACtrl2End - CODEoffset PatchACtrl2VK
	dw CODEoffset PatchACtrl3End - CODEoffset PatchACtrl3VK
	dw	0
ifdef NOKIA
	dw CODEoffset PatchACtrl5End - CODEoffset PatchACtrl5VK
	dw CODEoffset PatchACtrl6End - CODEoffset PatchACtrl6VK
endif

PatchACtrlVK label word

	dw	CODEoffset PatchACtrl1VK
	dw	CODEoffset PatchACtrl2VK
	dw	CODEoffset PatchACtrl1VK
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchACtrl5VK
	dw	CODEoffset PatchACtrl6VK
endif

PatchACtrl label word

	dw	CODEoffset PatchACtrl1
	dw	CODEoffset PatchACtrl2
	dw	CODEoffset PatchACtrl1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchACtrl5
	dw	CODEoffset PatchACtrl6
endif


; type 1
; type 3
IRP VKFLAG, <0, 1>

    klabels PatchACtrl1VK, PatchACtrl1, VKFLAG
ifdef ATSAME
    klabels PatchACtrl3VK, PatchACtrl3, VKFLAG
endif

	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_OEM_1    ,	01dh,	VKFLAG
	ktrans	VK_OEM_6    ,	09bh,	VKFLAG
	ktrans	VK_OEM_MINUS,	09fh,	VKFLAG
	ktrans	VK_OEM_102,	01ch,	VKFLAG

    klabdef PatchACtrl1End, VKFLAG
ifdef ATSAME
    klabdef PatchACtrl3End, VKFLAG
endif

ENDM

ifndef ATSAME
; type 3
IRP VKFLAG, <0, 1>

    klabels PatchACtrl3VK, PatchACtrl3, VKFLAG

; (Same as XT)

    klabdef PatchACtrl3End, VKFLAG

ENDM
endif

; type 2
IRP VKFLAG, <0, 1>

    klabels PatchACtrl2VK, PatchACtrl2, VKFLAG

	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_8      ,	09fh,	VKFLAG
	ktrans	VK_OEM_1    ,	01dh,	VKFLAG
	ktrans	VK_OEM_5    ,	01ch,	VKFLAG
	ktrans	VK_OEM_6    ,	01bh,	VKFLAG
	ktrans	VK_OEM_PLUS ,	01fh,	VKFLAG

    klabdef PatchACtrl2End, VKFLAG

ENDM

ifdef NOKIA
; type 5
IRP VKFLAG, <0, 1>

    klabels PatchACtrl5VK, PatchACtrl5, VKFLAG

	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_OEM_1    ,	01dh,	VKFLAG
	ktrans	VK_OEM_6    ,	01bh,	VKFLAG
	ktrans	VK_OEM_MINUS,	09fh,	VKFLAG
	ktrans	VK_OEM_102,	01ch,	VKFLAG

    klabdef PatchACtrl5End, VKFLAG

ENDM

; type 6
IRP VKFLAG, <0, 1>

    klabels PatchACtrl6VK, PatchACtrl6, VKFLAG

	ktrans	VK_2      ,	080h,	VKFLAG
	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_OEM_1    ,	01dh,	VKFLAG
	ktrans	VK_OEM_5    ,	01ch,	VKFLAG
	ktrans	VK_OEM_6    ,	01bh,	VKFLAG
	ktrans	VK_OEM_MINUS,	01fh,	VKFLAG

    klabdef PatchACtrl6End, VKFLAG

ENDM
endif


; ********************************************************
;
;	Patches to 'AscCtlAlt' table.
;
;	For this and following tables, the whole table is
;	copied when a keyboard type is selected.
;
; ********************************************************

	public	szPatchACtlAlt
	public	PatchACtlAltVK
	public	PatchACtlAlt

szPatchACtlAlt label word
	dw CODEoffset PatchACtlAlt1End - CODEoffset PatchACtlAlt1VK
	dw CODEoffset PatchACtlAlt2End - CODEoffset PatchACtlAlt2VK
	dw CODEoffset PatchACtlAlt3End - CODEoffset PatchACtlAlt3VK
	dw	0
ifdef NOKIA
	dw CODEoffset PatchACtlAlt5End - CODEoffset PatchACtlAlt5VK
	dw CODEoffset PatchACtlAlt6End - CODEoffset PatchACtlAlt6VK
endif

PatchACtlAltVK label word

	dw	CODEoffset PatchACtlAlt1VK
	dw	CODEoffset PatchACtlAlt2VK
	dw	CODEoffset PatchACtlAlt3VK
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchACtlAlt5VK
	dw	CODEoffset PatchACtlAlt6VK
endif

PatchACtlAlt label word

	dw	CODEoffset PatchACtlAlt1
	dw	CODEoffset PatchACtlAlt2
	dw	CODEoffset PatchACtlAlt1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchACtlAlt5
	dw	CODEoffset PatchACtlAlt6
endif


; type 1
; type 3
IRP VKFLAG, <0, 1>

    klabels PatchACtlAlt1VK, PatchACtlAlt1, VKFLAG
ifdef ATSAME
    klabels PatchACtlAlt3VK, PatchACtlAlt3, VKFLAG
endif

; Patch Control-Alt table for type 1
	ktrans	VK_2      ,	'@',	VKFLAG
	ktrans	VK_3      ,	'#',	VKFLAG
	ktrans	VK_6      ,	'^',	VKFLAG
	ktrans	VK_OEM_6    ,	'[',	VKFLAG
	ktrans	VK_OEM_1    ,	']',	VKFLAG
	ktrans	VK_OEM_102,	'\',	VKFLAG

    klabdef PatchACtlAlt1End, VKFLAG
ifdef ATSAME
    klabdef PatchACtlAlt3End, VKFLAG
endif

ENDM

ifndef ATSAME
; type 2
IRP VKFLAG, <0, 1>

    klabels PatchACtlAlt3VK, PatchACtlAlt3, VKFLAG

; (Same as XT)

    klabdef PatchACtlAlt3End, VKFLAG

ENDM
endif

; type 2
IRP VKFLAG, <0, 1>

    klabels PatchACtlAlt2VK, PatchACtlAlt2, VKFLAG

; Patch Control-Alt table for type 2
	ktrans	VK_3      ,	'#',	VKFLAG
	ktrans	VK_OEM_4    ,	'^',	VKFLAG
	ktrans	VK_OEM_PLUS ,	'~',	VKFLAG
	ktrans	VK_OEM_6    ,	'@',	VKFLAG
	ktrans	VK_OEM_1    ,	'[',	VKFLAG
	ktrans	VK_OEM_3    ,	0b5h,	VKFLAG
	ktrans	VK_OEM_7    ,	']',	VKFLAG
	ktrans	VK_OEM_5    ,	'\',	VKFLAG

    klabdef PatchACtlAlt2End, VKFLAG

ENDM

ifdef NOKIA
; type 5
IRP VKFLAG, <0, 1>

    klabels PatchACtlAlt5VK, PatchACtlAlt5, VKFLAG

; Patch Control-Alt table for type 5
	ktrans	VK_OEM_102,	'\',	VKFLAG

    klabdef PatchACtlAlt5End, VKFLAG

ENDM

; type 6
IRP VKFLAG, <0, 1>

    klabels PatchACtlAlt6VK, PatchACtlAlt6, VKFLAG

; Patch Control-Alt table for type 6
	ktrans	VK_OEM_7    ,	0b5h,	VKFLAG
	ktrans	VK_OEM_5    ,	'\',	VKFLAG

    klabdef PatchACtlAlt6End, VKFLAG

ENDM
endif


; ********************************************************
;
;	Patches to 'AscShCtlAlt' table.
;
; ********************************************************

	public	szPatchAShCtlAlt
	public	PatchAShCtlAltVK
	public	PatchAShCtlAlt

szPatchAShCtlAlt label word
	dw CODEoffset PatchAShCtlAlt1End - CODEoffset PatchAShCtlAlt1VK
	dw CODEoffset PatchAShCtlAlt2End - CODEoffset PatchAShCtlAlt2VK
	dw CODEoffset PatchAShCtlAlt3End - CODEoffset PatchAShCtlAlt3VK
	dw	0
ifdef NOKIA
	dw CODEoffset PatchAShCtlAlt5End - CODEoffset PatchAShCtlAlt5VK
	dw CODEoffset PatchAShCtlAlt6End - CODEoffset PatchAShCtlAlt6VK
endif

PatchAShCtlAltVK label word

	dw	CODEoffset PatchAShCtlAlt1VK
	dw	CODEoffset PatchAShCtlAlt2VK
	dw	CODEoffset PatchAShCtlAlt1VK
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchAShCtlAlt5VK
	dw	CODEoffset PatchAShCtlAlt6VK
endif

PatchAShCtlAlt label word

	dw	CODEoffset PatchAShCtlAlt1
	dw	CODEoffset PatchAShCtlAlt2
	dw	CODEoffset PatchAShCtlAlt1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchAShCtlAlt5
	dw	CODEoffset PatchAShCtlAlt6
endif


; type 1
; type 3
IRP VKFLAG, <0, 1>

    klabels PatchAShCtlAlt1VK, PatchAShCtlAlt1, VKFLAG
ifdef ATSAME
    klabels PatchAShCtlAlt3VK, PatchAShCtlAlt3, VKFLAG
endif

; Patch Shift-Control-Alt table for type 1

    klabdef PatchAShCtlAlt1End, VKFLAG
ifdef ATSAME
    klabdef PatchAShCtlAlt3End, VKFLAG
endif

ENDM

ifndef ATSAME
; type 2
IRP VKFLAG, <0, 1>

    klabels PatchAShCtlAlt3VK, PatchAShCtlAlt3, VKFLAG

; (Same as XT)

    klabdef PatchAShCtlAlt3End, VKFLAG

ENDM
endif


; type 2
IRP VKFLAG, <0, 1>

    klabels PatchAShCtlAlt2VK, PatchAShCtlAlt2, VKFLAG

; Patch Shift-Control-Alt table for type 2
	ktrans	VK_OEM_1    ,	'{',	VKFLAG
	ktrans	VK_OEM_7    ,	'}',	VKFLAG
	ktrans	VK_OEM_5    ,	'|',	VKFLAG

    klabdef PatchAShCtlAlt2End, VKFLAG

ENDM

ifdef NOKIA
; type 5
IRP VKFLAG, <0, 1>

    klabels PatchAShCtlAlt5VK, PatchAShCtlAlt5, VKFLAG

; Patch Shift-Control-Alt table for type 5
	ktrans	VK_2      ,	'@',	VKFLAG
	ktrans	VK_3      ,	'#',	VKFLAG
	ktrans	VK_4      ,	'$',	VKFLAG
	ktrans	VK_6      ,	'^',	VKFLAG
	ktrans	VK_8      ,	'{',	VKFLAG
	ktrans	VK_9      ,	'}',	VKFLAG
	ktrans	VK_OEM_6    ,	'[',	VKFLAG
	ktrans	VK_OEM_1    ,	']',	VKFLAG
	ktrans	VK_OEM_3    ,	0ach,	VKFLAG
	ktrans	VK_OEM_7    ,	'~',	VKFLAG
	ktrans	VK_OEM_102,	'|',	VKFLAG

    klabdef PatchAShCtlAlt5End, VKFLAG

ENDM

; type 6
IRP VKFLAG, <0, 1>

    klabels PatchAShCtlAlt6VK, PatchAShCtlAlt6, VKFLAG

; Patch Shift-Control-Alt table for type 6
	ktrans	VK_2      ,	'@',	VKFLAG
	ktrans	VK_3      ,	0a7h,	VKFLAG
	ktrans	VK_4      ,	0a3h,	VKFLAG
	ktrans	VK_6      ,	'^',	VKFLAG
	ktrans	VK_8      ,	'{',	VKFLAG
	ktrans	VK_9      ,	'}',	VKFLAG
	ktrans	VK_OEM_6    ,	'[',	VKFLAG
	ktrans	VK_OEM_1    ,	']',	VKFLAG
	ktrans	VK_OEM_3    ,	0ach,	VKFLAG
	ktrans	VK_OEM_7    ,	0b5h,	VKFLAG
	ktrans	VK_OEM_5    ,	'|',	VKFLAG

    klabdef PatchAShCtlAlt6End, VKFLAG

ENDM
endif

; ********************************************************
;
;	Patches to 'Morto' (dead key table)
;
;	Note: there are 2 size tables, since the key array
;	contains words, and the translated array contains
;	bytes.
;
; ********************************************************

	public szPatchMortoVK
	public PatchMortoVK

	public szPatchMortoCode
	public PatchMortoCode

szPatchMortoVK label	word
	dw	CODEoffset PatchMortoEnd1 - CODEoffset PatchMorto1
	dw	CODEoffset PatchMortoEnd2 - CODEoffset PatchMorto2
	dw	CODEoffset PatchMortoEnd1 - CODEoffset PatchMorto1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchMortoEnd5 - CODEoffset PatchMorto5
	dw	CODEoffset PatchMortoEnd6 - CODEoffset PatchMorto6
endif

PatchMortoVK label	word
	dw	CODEoffset PatchMorto1
	dw	CODEoffset PatchMorto2
	dw	CODEoffset PatchMorto1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchMorto5
	dw	CODEoffset PatchMorto6
endif

szPatchMortoCode label	word
	dw	CODEoffset PatchMortoCodeEnd1 - CODEoffset PatchMortoCode1
	dw	CODEoffset PatchMortoCodeEnd2 - CODEoffset PatchMortoCode2
	dw	CODEoffset PatchMortoCodeEnd3 - CODEoffset PatchMortoCode3
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchMortoCodeEnd5 - CODEoffset PatchMortoCode5
	dw	CODEoffset PatchMortoCodeEnd6 - CODEoffset PatchMortoCode6
endif

PatchMortoCode label	word
	dw	CODEoffset PatchMortoCode1
	dw	CODEoffset PatchMortoCode2
	dw	CODEoffset PatchMortoCode1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchMortoCode5
	dw	CODEoffset PatchMortoCode6
endif

IRP VKFLAG, <0, 1>

    klabels PatchMorto1, PatchMortoCode1, VKFLAG
ifdef ATSAME
    klabels PatchMorto3, PatchMortoCode3, VKFLAG
endif

	deadtrans	VK_OEM_6    ,	0,	circumflex,	VKFLAG
	deadtrans	VK_OEM_6    ,	1,	umlaut,	VKFLAG

    klabels PatchMortoEnd1, PatchMortoCodeEnd1, VKFLAG
ifdef ATSAME
    klabels PatchMortoEnd3, PatchMortoCodeEnd3, VKFLAG
endif

ENDM

ifndef ATSAME
IRP VKFLAG, <0, 1>

    klabels PatchMorto3, PatchMortoCode3, VKFLAG

; (Same as XT)

    klabels PatchMortoEnd3, PatchMortoCodeEnd3, VKFLAG

ENDM
endif


IRP VKFLAG, <0, 1>

    klabels PatchMorto2, PatchMortoCode2, VKFLAG

	deadtrans	VK_OEM_6    ,	0,	circumflex,	VKFLAG
	deadtrans	VK_OEM_6    ,	1,	umlaut,	VKFLAG

    klabels PatchMortoEnd2, PatchMortoCodeEnd2, VKFLAG

ENDM

ifdef NOKIA
IRP VKFLAG, <0, 1>

    klabels PatchMorto5, PatchMortoCode5, VKFLAG

	deadtrans	VK_OEM_6    ,	0,	circumflex,	VKFLAG
	deadtrans	VK_OEM_6    ,	1,	umlaut,	VKFLAG

    klabels PatchMortoEnd5, PatchMortoCodeEnd5, VKFLAG

ENDM

IRP VKFLAG, <0, 1>

    klabels PatchMorto6, PatchMortoCode6, VKFLAG


	deadtrans	VK_OEM_2    ,	0,	circumflex,	VKFLAG
	deadtrans	VK_OEM_2    ,	1,	umlaut,	VKFLAG

    klabels PatchMortoEnd6, PatchMortoCodeEnd6, VKFLAG

ENDM
endif

; ********************************************************
;
;	Patches to 'CapitalTable'
;
; ********************************************************

	public szPatchCapital
	public PatchCapital

szPatchCapital label word
	dw	CODEoffset PatchCapEnd1 - CODEoffset PatchCap1
	dw	CODEoffset PatchCapEnd2 - CODEoffset PatchCap2
	dw	CODEoffset PatchCapEnd3 - CODEoffset PatchCap3
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchCapEnd5 - CODEoffset PatchCap5
	dw	CODEoffset PatchCapEnd6 - CODEoffset PatchCap6
endif

PatchCapital label word
	dw	CODEoffset PatchCap1
	dw	CODEoffset PatchCap2
	dw	CODEoffset PatchCap1
	dw	0
ifdef NOKIA
	dw	CODEoffset PatchCap5
	dw	CODEoffset PatchCap6
endif

PatchCap1 label byte
ifdef ATSAME
PatchCap3 label byte
endif
; Capital table, type 1
	db	VK_1      
	db	VK_2      
	db	VK_3      
	db	VK_4      
	db	VK_5      
	db	VK_6      
	db	VK_7      
	db	VK_8      
	db	VK_9      
	db	VK_0      
	db	VK_OEM_4    
	db	VK_OEM_MINUS
PatchCapEnd1 label byte
ifdef ATSAME
PatchCapEnd3 label byte

else
PatchCap3 label byte
; (Same as XT)
PatchCapEnd3 label byte
endif

PatchCap2 label byte
; Capital table, type 2
	db	VK_1      
	db	VK_2      
	db	VK_3      
	db	VK_4      
	db	VK_5      
	db	VK_6      
	db	VK_7      
	db	VK_8      
	db	VK_9      
	db	VK_0      
	db	VK_OEM_4    
	db	VK_OEM_PLUS 
PatchCapEnd2 label byte

ifdef NOKIA
PatchCap5 label byte
; Capital table, type 5
	db	VK_1      
	db	VK_2      
	db	VK_3      
	db	VK_4      
	db	VK_5      
	db	VK_6      
	db	VK_7      
	db	VK_8      
	db	VK_9      
	db	VK_0      
	db	VK_OEM_4    
	db	VK_OEM_MINUS
PatchCapEnd5 label byte

PatchCap6 label byte
; Capital table, type 6
PatchCapEnd6 label byte
endif


sEnd CODE

; ********************************************************
;
; Data segment -- this is FIXED
;
; ********************************************************

sBegin DATA

; ********************************************************
;
; This string identifies the table type (nationality).
;
; It is zero-terminated.
;
; ********************************************************


public CountryName
CountryName label byte

	db	' Belgium '
	db	0

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
	ktrans2	VK_0      ,	0e0h,	'0',	VKFLAG
	ktrans2	VK_1      ,	'&',	'1',	VKFLAG
	ktrans2	VK_2      ,	0e9h,	'2',	VKFLAG
	ktrans2	VK_3      ,	'"',	'3',	VKFLAG
	ktrans2	VK_4      ,	027h,	'4',	VKFLAG
	ktrans2	VK_5      ,	'(',	'5',	VKFLAG
	ktrans2	VK_6      ,	0a7h,	'6',	VKFLAG
	ktrans2	VK_7      ,	0e8h,	'7',	VKFLAG
	ktrans2	VK_8      ,	'!',	'8',	VKFLAG
	ktrans2	VK_9      ,	0e7h,	'9',	VKFLAG
	ktrans2	VK_OEM_COMMA,	',',	'?',	VKFLAG
	ktrans2	VK_OEM_PERIOD,	';',	'.',	VKFLAG
	ktrans2	VK_OEM_MINUS,	'-',	'_',	VKFLAG
	ktrans2	VK_OEM_PLUS ,	'=',	'+',	VKFLAG
	ktrans2	VK_OEM_1    ,	'$',	'*',	VKFLAG
	ktrans2	VK_OEM_2    ,	':',	'/',	VKFLAG
	ktrans2	VK_OEM_3    ,	0f9h,	'%',	VKFLAG
	ktrans2	VK_OEM_4    ,	')',	0b0h,	VKFLAG
	ktrans2	VK_OEM_5    ,	0b5h,	0a3h,	VKFLAG
	; Unshifted is dead key
	; Shifted is dead key
	ktrans2	VK_OEM_6    ,	-1,	-1,	VKFLAG
	ktrans2	VK_OEM_7    ,	0b2h,	0b3h,	VKFLAG
	ktrans2	VK_OEM_8    ,	-1,	-1,	VKFLAG
	ktrans2	VK_OEM_102,	'<',	'>',	VKFLAG
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
	deadtrans	'Y', acute,	0DDh,	VKFLAG
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
	deadtrans	' ', umlaut,	umlaut,	VKFLAG
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
; This table lists the virtual scancodes of the dead keys.
; Each entry lists the scan code and a shift state for which
; this key is a dead key, and the translated dead key.
;
;
; ********************************************************

    public Morto, MortoCode

IRP VKFLAG, <0, 1>

    klabels Morto, MortoCode, VKFLAG

	deadtrans	VK_OEM_3    ,	6,	acute,	VKFLAG
	deadtrans	VK_OEM_3    ,	7,	acute,	VKFLAG
	deadtrans	VK_OEM_5    ,	6,	grave,	VKFLAG
	deadtrans	VK_OEM_5    ,	7,	grave,	VKFLAG
	deadtrans	VK_OEM_6    ,	0,	circumflex,	VKFLAG
	deadtrans	VK_OEM_6    ,	1,	umlaut,	VKFLAG
	deadtrans	VK_OEM_PLUS ,	6,	tilde,	VKFLAG
	deadtrans	VK_OEM_PLUS ,	7,	tilde,	VKFLAG

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

	ktrans	VK_6      ,	09eh,	VKFLAG
	ktrans	VK_OEM_1    ,	01dh,	VKFLAG
	ktrans	VK_OEM_5    ,	01ch,	VKFLAG
	ktrans	VK_OEM_6    ,	01bh,	VKFLAG
	ktrans	VK_OEM_MINUS,	01fh,	VKFLAG
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
	ktrans	VK_1      ,	'|',	VKFLAG
	ktrans	VK_2      ,	'@',	VKFLAG
	ktrans	VK_3      ,	'#',	VKFLAG
	ktrans	VK_4      ,	'{',	VKFLAG
	ktrans	VK_5      ,	'[',	VKFLAG
	ktrans	VK_6      ,	'^',	VKFLAG
	ktrans	VK_9      ,	'{',	VKFLAG
	ktrans	VK_0      ,	'}',	VKFLAG
	ktrans	VK_OEM_6    ,	'[',	VKFLAG
	ktrans	VK_OEM_1    ,	']',	VKFLAG
	; Dead key: VK_OEM_3    
	; Dead key: VK_OEM_5    
	; Dead key: VK_OEM_PLUS 
	ktrans	VK_OEM_102,	'\',	VKFLAG

    klabdef AscCtlAltEnd, VKFLAG

	; No Padding Needed

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
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG
	ktrans	000h,	00h,	VKFLAG

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
	db	VK_1      
	db	VK_2      
	db	VK_3      
	db	VK_4      
	db	VK_5      
	db	VK_6      
	db	VK_7      
	db	VK_8      
	db	VK_9      
	db	VK_0      
	db	VK_OEM_4    
	db	VK_OEM_MINUS
	db	VK_OEM_6    
	db	VK_OEM_1    
	db	VK_OEM_3    
	db	VK_OEM_5    
	db	VK_OEM_COMMA
	db	VK_OEM_PERIOD
	db	VK_OEM_2    
	db	VK_OEM_PLUS 

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


sEnd DATA

    if2
    %out  .. end KBDBE.asm
    %out
    endif

end

;
