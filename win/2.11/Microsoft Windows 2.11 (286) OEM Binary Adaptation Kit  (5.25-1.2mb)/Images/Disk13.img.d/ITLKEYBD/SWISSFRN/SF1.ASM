 
; ****** Keyboard Translation table for Olivetti Windows 2.0 *************
;
; Copyright 1986, 1987 by Olivetti.
; ************************************************************************
;
; history
; 23 jun 88	peterbe	Changed version string at beginning of data segment.
; 15 jun 88	peterbe	Commented out deadkey entries in CtrlAltTable
;			Changed VK_ assignment of '{' and '}'.
; 12 jan 88	plb	Add 80h to some control codes
;
; Country and keyboard table number:  *********** end insert 1

IF1
%OUT SwissFrench 1
ENDIF
 
; *********************************************** begin insert 2
include keyboard.inc
include olikbd.inc
include vkwin.inc
include vkoem.inc

ifdef BINTABLE
DATASEG segment public
	assume ds:DATASEG
endif

ifdef INDRIVER
sBegin DATA
	assumes ds,DATA
KeyStart label byte
endif

	public	TableType
	public	KeyType
	public	fKbRt
	public	OliType
	public	IsOli
	public	IsEri
	public	PCType
	public	PCTypeHigh
	public	RTFlags

db	'MSWin2.10'	; key to identify table -- DON'T CHANGE THIS !!

; ************************************************************************
; The TableType byte identifies whether the this source file is
; for:	1: 83-key keyboard for M24, etc.
;	2: Olivetti (ICO) extended 102-key keyboard for M24
;	3: 86-key keyboard -- usually same as (1), but is sometimes
;	   like (2)
;	4: Compatible 101-102 key keyboard.
;
; (At offset 09:)
;
TableType label byte ; *************************** end insert 2

	DB	1
 
; ************************************* begin insert 3
; The KeyType byte is set by the driver when the physical keyboard
; type is determined.  It can be 1, 2, 3 or 4, as with TableType.
; If KeyType is 3, TableType (after modification) may be 1, 2, or
; 3; for other values of KeyType, TableType will match.
;
; (At offset 0A:)
;
KeyType		db	0

; This flag is set if this is an Olivetti computer.
; for a non-Olivetti RT keyboard, word ptr [KeyType] will be 0.
;
; (At offset 0B:)
;
IsOli		db	0	; NZ if Olivetti system
IsEri		db	0	; NZ if Ericsson system

; This flag indicates whether the attached keyboard is an RT 101/102
; keyboard, whether Olivetti protocol, or XT or AT protocol.  It is
; set just if KeyType is set to 4.
;
; (At offset 0D:)
;
fKbRt		db	0

; If an Olivetti(or AT&T)-protocol keyboard is attached, this byte contains
; the keyboard ID.
;
; (At offset 0E)
;
OliType		db	0


; These two bytes contain the system identification bytes from the ROM.
;
; (At offsets 0FH, 10H:)
;
PCType		db	0
PCTypeHigh	db	0

; The RTFlags flag is set from an input parameter to MSW.
; It is used only for type 4 (RT) keyboards.
; The bits mean:
;	08h	this keyboard interprets AltGr key as CTRL ALT
;	20h	this keyboard uses Swiss-German ShiftLockTab
;	40h	this keyboard has shift lock instead of caps lock
;	80h	this is an RT keyboard.
;
; (At offset 11H:)
;
RTFlags label byte		; ***************** end insert 3

	DB	0
 
; ************************************* begin insert 4


	public	KeyTransBase, KeyTransTblSize
	public	X1, X2
	public	keyNumBase
	public	AsciiTabBase,VirtualDefault
	public	DeadKeyBase,DeadKeyDefault
	public	CapitalTabBase,CapitalDefault
	public	CtrlAltBase
	public	CtrlAltDefault
	public	MortoBase
	public	KeyTabEnd

; The following is a table of offsets to the various tables in this
; module.  These are in the nearly the same order as the order of
; the tables they point to in the file.

KeyTransBase	dw	dataOffset keyTranslationTable
KeyTransTblSize	dw	KeyTransTblEnd - KeyTranslationTable   ; count of keys

keyNumBase	dw	dataOffset KeyNumTrans

AsciiTabBase	dw	dataOffset AsciiTable
VirtualDefault	dw	dataOffset AsciiDefLabel

DeadKeyBase	dw	dataOffset DeadKeyTable
DeadKeyDefault	dw	dataOffset DeadKeyDefLabel

CapitalTabBase	dw	dataOffset CapitalTable
CapitalDefault	dw	dataOffset CapDefLabel

CtrlAltBase	dw	dataOffset CtrlAltTable
CtrlAltDefault	dw	dataOffset CtrlAltDefLabel

MortoBase	dw	dataOffset TavoloMorto

; Pointer to end

KeyTabEnd	dw	dataOffset KeyEnd

; Pointer to country name
pCountryName	dw	dataOffset CountryName

;
; The Oem2Ansi and Ansi2Oem tables WERE included here. No Longer.
;
; ************************************* end insert 4
 
; ************************************* begin insert 5
;
; This is the scan-code to Windows virtual keycode translation table.
; Translations past 83 decimal depend on the keyboard type.
; Translations for some other keys (punctuation, some letters)
; will vary from keyboard to keyboard.

keyTranslationTable label byte		; ****** end insert 5

	; keyTranslationTable
	DB	-1		; 00h 0
	DB	VK_ESCAPE 	; 01h 1
	DB	VK_1      	; 02h 2
	DB	VK_2      	; 03h 3
	DB	VK_3      	; 04h 4
	DB	VK_4      	; 05h 5
	DB	VK_5      	; 06h 6
	DB	VK_6      	; 07h 7
	DB	VK_7      	; 08h 8
	DB	VK_8      	; 09h 9
	DB	VK_9      	; 0ah 10
	DB	VK_0      	; 0bh 11
	DB	VK_OEM_4    	; 0ch 12
	DB	VK_OEM_6    	; 0dh 13
	DB	VK_BACK   	; 0eh 14
	DB	VK_TAB    	; 0fh 15
	DB	VK_Q      	; 010h 16
	DB	VK_W      	; 011h 17
	DB	VK_E      	; 012h 18
	DB	VK_R      	; 013h 19
	DB	VK_T      	; 014h 20
	DB	VK_Z      	; 015h 21
	DB	VK_U      	; 016h 22
	DB	VK_I      	; 017h 23
	DB	VK_O      	; 018h 24
	DB	VK_P      	; 019h 25
	DB	VK_OEM_1    	; 01ah 26
	DB	VK_OEM_3    	; 01bh 27
	DB	VK_RETURN 	; 01ch 28
	DB	VK_CONTROL	; 01dh 29
	DB	VK_A      	; 01eh 30
	DB	VK_S      	; 01fh 31
	DB	VK_D      	; 020h 32
	DB	VK_F      	; 021h 33
	DB	VK_G      	; 022h 34
	DB	VK_H      	; 023h 35
	DB	VK_J      	; 024h 36
	DB	VK_K      	; 025h 37
	DB	VK_L      	; 026h 38
	DB	VK_OEM_7    	; 027h 39
	DB	VK_OEM_5    	; 028h 40
X1	label byte	; swap for AT
	DB	VK_OEM_2    	; 029h 41
	DB	VK_SHIFT  	; 02ah 42
X2	label byte	; swap for AT
	DB	VK_OEM_8    	; 02bh 43
	DB	VK_Y      	; 02ch 44
	DB	VK_X      	; 02dh 45
	DB	VK_C      	; 02eh 46
	DB	VK_V      	; 02fh 47
	DB	VK_B      	; 030h 48
	DB	VK_N      	; 031h 49
	DB	VK_M      	; 032h 50
	DB	VK_OEM_COMMA	; 033h 51
	DB	VK_OEM_PERIOD	; 034h 52
	DB	VK_OEM_MINUS	; 035h 53
	DB	VK_SHIFT  	; 036h 54
	DB	VK_MULTIPLY	; 037h 55
	DB	VK_MENU   	; 038h 56
	DB	VK_SPACE  	; 039h 57
	DB	VK_CAPITAL	; 03ah 58
	DB	VK_F1     	; 03bh 59
	DB	VK_F2     	; 03ch 60
	DB	VK_F3     	; 03dh 61
	DB	VK_F4     	; 03eh 62
	DB	VK_F5     	; 03fh 63
	DB	VK_F6     	; 040h 64
	DB	VK_F7     	; 041h 65
	DB	VK_F8     	; 042h 66
	DB	VK_F9     	; 043h 67
	DB	VK_F10    	; 044h 68
	DB	VK_OEM_NUMBER	; 045h 69
	DB	VK_OEM_SCROLL	; 046h 70
	DB	VK_HOME   	; 047h 71
	DB	VK_UP     	; 048h 72
	DB	VK_PRIOR  	; 049h 73
	DB	VK_SUBTRACT	; 04ah 74
	DB	VK_LEFT   	; 04bh 75
	DB	VK_CLEAR  	; 04ch 76
	DB	VK_RIGHT  	; 04dh 77
	DB	VK_ADD    	; 04eh 78
	DB	VK_END    	; 04fh 79
	DB	VK_DOWN   	; 050h 80
	DB	VK_NEXT   	; 051h 81
	DB	VK_INSERT 	; 052h 82
	DB	VK_DELETE 	; 053h 83
	DB	-1		; 054h 84
	DB	-1		; 055h 85
	DB	VK_OEM_102	; 056h 86
	DB	VK_F11    	; 057h 87
	DB	VK_F12    	; 058h 88
	DB	-1		; 059h 89
	DB	-1		; 05ah 90
	DB	-1		; 05bh 91
	DB	-1		; 05ch 92
	DB	-1		; 05dh 93
	DB	-1		; 05eh 94
	DB	-1		; 05fh 95
	DB	-1		; 060h 96
	DB	-1		; 061h 97
	DB	-1		; 062h 98
	DB	-1		; 063h 99
	DB	-1		; 064h 100
	DB	-1		; 065h 101
	DB	-1		; 066h 102
	DB	-1		; 067h 103

ifdef ERICSSON	; Special scan codes

	DB	-1		; 068h 104
	DB	-1		; 069h 105
	DB	-1		; 06ah 106
	DB	-1		; 06bh 107
	DB	-1		; 06ch 108
	DB	-1		; 06dh 109
	DB	-1		; 06eh 110
	DB	-1		; 06fh 111
	DB	-1		; 070h 112
	DB	-1		; 071h 113
	DB	-1		; 072h 114
	DB	-1		; 073h 115
	DB	-1		; 074h 116
	DB	-1		; 075h 117
	DB	-1		; 076h 118
	DB	-1		; 077h 119
	DB	-1		; 078h 120
	DB	-1		; 079h 121
	DB	-1		; 07ah 122
	DB	-1		; 07bh 123
	DB	-1		; 07ch 124
	DB	-1		; 07dh 125
	DB	-1		; 07eh 126
	DB	-1		; 07fh 127
	DB	-1		; 080h 128
	DB	-1		; 081h 129
	DB	-1		; 082h 130
	DB	-1		; 083h 131
	DB	-1		; 084h 132
	DB	-1		; 085h 133
	DB	-1		; 086h 134
	DB	-1		; 087h 135
	DB	-1		; 088h 136
	DB	-1		; 089h 137
	DB	-1		; 08ah 138
	DB	-1		; 08bh 139
	DB	-1		; 08ch 140
	DB	-1		; 08dh 141
	DB	-1		; 08eh 142
	DB	-1		; 08fh 143
	DB	-1		; 090h 144
	DB	-1		; 091h 145
	DB	-1		; 092h 146
	DB	-1		; 093h 147
	DB	-1		; 094h 148
	DB	-1		; 095h 149
	DB	-1		; 096h 150
	DB	-1		; 097h 151
	DB	-1		; 098h 152
	DB	-1		; 099h 153
	DB	-1		; 09ah 154
	DB	-1		; 09bh 155
	DB	-1		; 09ch 156
	DB	-1		; 09dh 157
	DB	-1		; 09eh 158
	DB	-1		; 09fh 159
	DB	-1		; 0a0h 160
	DB	-1		; 0a1h 161
	DB	-1		; 0a2h 162
	DB	-1		; 0a3h 163
	DB	-1		; 0a4h 164
	DB	-1		; 0a5h 165
	DB	-1		; 0a6h 166
	DB	-1		; 0a7h 167
	DB	-1		; 0a8h 168
	DB	-1		; 0a9h 169
	DB	-1		; 0aah 170
	DB	-1		; 0abh 171
	DB	-1		; 0ach 172
	DB	-1		; 0adh 173
	DB	-1		; 0aeh 174
	DB	-1		; 0afh 175
	DB	-1		; 0b0h 176
	DB	-1		; 0b1h 177
	DB	-1		; 0b2h 178
	DB	-1		; 0b3h 179
	DB	-1		; 0b4h 180
	DB	-1		; 0b5h 181
	DB	-1		; 0b6h 182
	DB	-1		; 0b7h 183
	DB	-1		; 0b8h 184
	DB	-1		; 0b9h 185
	DB	-1		; 0bah 186
	DB	-1		; 0bbh 187
	DB	-1		; 0bch 188
	DB	-1		; 0bdh 189
	DB	-1		; 0beh 190
	DB	-1		; 0bfh 191
	DB	-1		; 0c0h 192
	DB	-1		; 0c1h 193
	DB	-1		; 0c2h 194
	DB	-1		; 0c3h 195
	DB	-1		; 0c4h 196
	DB	-1		; 0c5h 197
	DB	-1		; 0c6h 198
	DB	-1		; 0c7h 199
	DB	-1		; 0c8h 200
	DB	-1		; 0c9h 201
	DB	-1		; 0cah 202
	DB	-1		; 0cbh 203
	DB	-1		; 0cch 204
	DB	-1		; 0cdh 205
	DB	-1		; 0ceh 206
	DB	-1		; 0cfh 207
	DB	-1		; 0d0h 208
	DB	-1		; 0d1h 209
	DB	-1		; 0d2h 210
	DB	-1		; 0d3h 211
	DB	-1		; 0d4h 212
	DB	-1		; 0d5h 213
	DB	-1		; 0d6h 214
	DB	-1		; 0d7h 215
	DB	-1		; 0d8h 216
	DB	-1		; 0d9h 217
	DB	-1		; 0dah 218
	DB	-1		; 0dbh 219
	DB	-1		; 0dch 220
	DB	-1		; 0ddh 221
	DB	-1		; 0deh 222
	DB	-1		; 0dfh 223
	DB	-1		; 0e0h 224
	DB	-1		; 0e1h 225
	DB	-1		; 0e2h 226
	DB	-1		; 0e3h 227
	DB	-1		; 0e4h 228
	DB	-1		; 0e5h 229
	DB	-1		; 0e6h 230
	DB	-1		; 0e7h 231
	DB	-1		; 0e8h 232
	DB	-1		; 0e9h 233
	DB	-1		; 0eah 234
	DB	-1		; 0ebh 235
	DB	-1		; 0ech 236
	DB	-1		; 0edh 237
	DB	-1		; 0eeh 238
	DB	-1		; 0efh 239
	DB	-1		; 0f0h 240
	DB	-1		; 0f1h 241
	DB	-1		; 0f2h 242
	DB	-1		; 0f3h 243
	DB	-1		; 0f4h 244
	DB	-1		; 0f5h 245
	DB	-1		; 0f6h 246
	DB	-1		; 0f7h 247
	DB	-1		; 0f8h 248
	DB	-1		; 0f9h 249
	DB	-1		; 0fah 250
	DB	-1		; 0fbh 251
	DB	-1		; 0fch 252
	DB	-1		; 0fdh 253
	DB	-1		; 0feh 254
	DB	-1		; 0ffh 255

endif
 
keyTransTblEnd label byte		; ****** begin insert 6

; This is the translation for the numeric pad when NUMLOCK is set.
; This is the same for all keyboards, and is fixed in length.

KeyNumTrans	LABEL	BYTE
	DB	VK_NUMPAD7	; numpad-7
	DB	VK_NUMPAD8	; numpad-8
	DB	VK_NUMPAD9	; numpad-9
	DB	VK_SUBTRACT	; numpad-minus
	DB	VK_NUMPAD4	; numpad-4
	DB	VK_NUMPAD5	; numpad-5
	DB	VK_NUMPAD6	; numpad-6
	DB	VK_ADD		; numpad-plus
	DB	VK_NUMPAD1	; numpad-1
	DB	VK_NUMPAD2	; numpad-2
	DB	VK_NUMPAD3	; numpad-3
	DB	VK_NUMPAD0	; numpad-0
	DB	VK_DECIMAL	; numpad-period

; ********************************************************
;
; This is the virtual keycode to ASCII (ANSI) translation
; table for unshifted keys, and keys with just the SHIFT
; just the CONTROL keys depressed.
;
AsciiTable label byte		; *********** end insert 6

	; AsciiTable
	DB	VK_CANCEL ,	03h,	03h,	03h
	DB	VK_BACK   ,	08h,	08h,	07fh
	DB	VK_TAB    ,	09h,	-1,   	-1
	DB	VK_RETURN ,	0dh,	0dh,	0ah
	DB	VK_ESCAPE ,	01bh,	01bh,	01bh
	DB	VK_SPACE  ,	' ',  	' ',  	020h
	DB	VK_EXECUTE,	0dh,	0dh,	0ah
	DB	VK_0      ,	'0',  	'=',  	-1
	DB	VK_1      ,	'1',  	'+',  	-1
	DB	VK_2      ,	'2',  	'"',  	-1
	DB	VK_3      ,	'3',  	'*',  	-1
	DB	VK_4      ,	'4',  	0e7h,	-1
	DB	VK_5      ,	'5',  	025h,	-1
	DB	VK_6      ,	'6',  	'&',  	01eh + 80h
	DB	VK_7      ,	'7',  	'/',  	-1
	DB	VK_8      ,	'8',  	'(',  	-1
	DB	VK_9      ,	'9',  	')',  	-1
	DB	VK_A      ,	'a',  	'A',  	01h
	DB	VK_B      ,	'b',  	'B',  	02h
	DB	VK_C      ,	'c',  	'C',  	03h
	DB	VK_D      ,	'd',  	'D',  	04h
	DB	VK_E      ,	'e',  	'E',  	05h
	DB	VK_F      ,	'f',  	'F',  	06h
	DB	VK_G      ,	'g',  	'G',  	07h
	DB	VK_H      ,	'h',  	'H',  	08h
	DB	VK_I      ,	'i',  	'I',  	09h
	DB	VK_J      ,	'j',  	'J',  	0ah
	DB	VK_K      ,	'k',  	'K',  	0bh
	DB	VK_L      ,	'l',  	'L',  	0ch
	DB	VK_M      ,	'm',  	'M',  	0dh
	DB	VK_N      ,	'n',  	'N',  	0eh
	DB	VK_O      ,	'o',  	'O',  	0fh
	DB	VK_P      ,	'p',  	'P',  	010h
	DB	VK_Q      ,	'q',  	'Q',  	011h
	DB	VK_R      ,	'r',  	'R',  	012h
	DB	VK_S      ,	's',  	'S',  	013h
	DB	VK_T      ,	't',  	'T',  	014h
	DB	VK_U      ,	'u',  	'U',  	015h
	DB	VK_V      ,	'v',  	'V',  	016h
	DB	VK_W      ,	'w',  	'W',  	017h
	DB	VK_X      ,	'x',  	'X',  	018h
	DB	VK_Y      ,	'y',  	'Y',  	019h
	DB	VK_Z      ,	'z',  	'Z',  	01ah
	DB	VK_NUMPAD0,	'0',  	'0',  	-1
	DB	VK_NUMPAD1,	'1',  	'1',  	-1
	DB	VK_NUMPAD2,	'2',  	'2',  	-1
	DB	VK_NUMPAD3,	'3',  	'3',  	-1
	DB	VK_NUMPAD4,	'4',  	'4',  	-1
	DB	VK_NUMPAD5,	'5',  	'5',  	-1
	DB	VK_NUMPAD6,	'6',  	'6',  	-1
	DB	VK_NUMPAD7,	'7',  	'7',  	-1
	DB	VK_NUMPAD8,	'8',  	'8',  	-1
	DB	VK_NUMPAD9,	'9',  	'9',  	-1
	DB	VK_MULTIPLY,	'*',  	'*',  	-1
	DB	VK_SUBTRACT,	'-',  	'-',  	-1
	DB	VK_ADD    ,	'+',  	'+',  	-1
	DB	VK_DECIMAL,	'.',  	'.',  	-1
	DB	VK_DIVIDE ,	'/',  	'/',  	-1
	DB	VK_OEM_1    ,	0e8h,	0fch,	01bh
	DB	VK_OEM_2    ,	'$',  	0a3h,	-1
	DB	VK_OEM_3    ,	0a8h,	'!',  	01dh
	DB	VK_OEM_4    ,	027h,	'?',  	-1
	DB	VK_OEM_5    ,	0e0h,	0e4h,	-1
	DB	VK_OEM_6    ,	'^',  	'`',  	-1
	DB	VK_OEM_7    ,	0e9h,	0f6h,	-1
	DB	VK_OEM_8    ,	'<',  	'>',  	01ch
	DB	VK_OEM_PLUS ,	-1,   	-1,   	-1
	DB	VK_OEM_COMMA,	',',  	';',  	-1
	DB	VK_OEM_MINUS,	'-',  	'_',  	01fh
	DB	VK_OEM_PERIOD,	'.',  	':',  	-1
	DB	VK_OEM_102,	-1,   	-1,   	-1
 
AsciiDefLabel label byte	; *********** begin insert 7
	db	-1,	-1,	-1,	-1


; ********************************************************
;
; This table translates dead keys.  The rows represent the
; character which was typed, and the columns represent the
; accent type.  The first column contains the character which
; was typed, and the remaining columns contain the translations
; for the character for grave, acute, circumflex, umlaut (dierisis),
; and tilde accents, respectively.

; Note: 1) The fact that acute or TILDE is anywhere valid in a row is used
;           to determine if the code is valid or not.
;       2) As the 'y umlaut' has the value '0FFh' (-1) we are testing
;           this special case in the TOASCII routine.
;       3) There are two differences among countries in this table:
;          (a) the value returned by 'acute+space', normally it
;              is the acute accent code, in Germany it is the quote (')
;              because that's the only easy way to get this last code.
;	   (b) 'umlaut+space' gives a large dot in some countries, and
;	       a double-quote in others.

DeadKeyTable label byte
ifndef NODEAD
;               character, grave, acute, circumflex, umlaut, TILDE
	db	'a', 0E0h, 0E1h, 0E2h, 0E4h , 0e3h
	db	'e', 0E8h, 0E9h, 0EAh, 0EBh , -1
	db	'i', 0ECh, 0EDh, 0EEh, 0EFh , -1
	db	'o', 0F2h, 0F3h, 0F4h, 0F6h , 0f5h
	db	'u', 0F9h, 0FAh, 0FBh, 0FCh , -1
	db	'y', -1  , 0FDh, -1  , 0FFh , -1
	db	'n', -1  , -1  , -1  , -1   , 0F1h
	db	'A', 0C0h, 0C1h, 0C2h, 0C4h , 0c3h
	db	'E', 0C8h, 0C9h, 0CAh, 0CBh , -1
	db	'I', 0CCh, 0CDh, 0CEh, 0CFh , -1
	db	'O', 0D2h, 0D3h, 0D4h, 0D6h , 0d5h
	db	'U', 0D9h, 0DAh, 0DBh, 0DCh , -1
	db	'Y', -1  , 0DDh, -1  , -1   , -1
	db	'N', -1  , -1  , -1  , -1   , 0D1h
		; Translation for space+accent:		**** end insert 7
	DB	' ' , 060h, 0B4h, 05Eh, 0A8h, '~' 
							; ** begin insert 8
endif
DeadKeyDefLabel label byte
ifndef NODEAD
	db	-1,  -1  , -1  , -1  , -1  , -1
endif


; ********************************************************
;
; This table lists the virtual keycodes of keys with caps lock
; or shift lock.

CapitalTable label byte		; ***************** end insert 8

	; CapitalTable
 
CapDeflabel label byte		; ***************** begin insert 9
	db	-1,	-1

; ********************************************************
;
; This lists translations for keys with CTRL-ALT and SHIFT-CTRL-ALT.

CtrlAltTable label byte		; **************** end insert 9

	; CtrlAltTable
	DB	VK_2      ,	'@',	-1
	DB	VK_3      ,	'#',	-1
	DB	VK_4      ,	0b0h,	-1
	DB	VK_5      ,	0a7h,	-1
	DB	VK_7      ,	'|',	-1
;;	DB	VK_OEM_4    ,	027h,	-1	; deadkey
;;	DB	VK_OEM_6    ,	'~',	-1	; deadkey
	DB	VK_OEM_1    ,	'[',	-1
	DB	VK_OEM_3    ,	']',	-1
	DB	VK_OEM_7    ,	'{',	-1	; changed 15jun88
	DB	VK_OEM_5    ,	'}',	-1	; changed 15jun88
	DB	VK_OEM_8    ,	'\',	-1
 
CtrlAltDefLabel label byte	; **************** begin insert 10
	db	-1,	-1,	-1

; ********************************************************
;
; This table lists the scancodes of the dead keys.
; Each entry lists the shift states for which the listed
; accent code is valid.
;
TavoloMorto label byte		; ****************** end insert 10

	; TavoloMorto
	DB	VK_OEM_4    ,	shift_5, acute
	DB	VK_OEM_6    ,	shift_2, grave
	DB	VK_OEM_6    ,	shift_1, circumflex
	DB	VK_OEM_3    ,	shift_1, umlaut
	DB	VK_OEM_6    ,	shift_5, tilde

	db	0
 
; ************************************* begin insert 10
;
; This string is copied when a keyboard table is overlaid.
;
CountryName label byte		; ***** end insert 11

	DB	' SwissFrench 1', 0
 

; (end of file)			; begin insert 12

KeyEnd label byte		; end of all this stuff

ifdef BINTABLE
DATASEG ends
endif

ifdef INDRIVER
include kpad.inc
sEnd DATA
endif

end ; *********************************** end insert 12

