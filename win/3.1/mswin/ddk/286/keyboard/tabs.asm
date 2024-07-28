	page	,132
; TABS.ASM -- Translation tables module for keyboard driver
;
; Copyright 1988-1990 by Microsoft Corporation.  All Rights Reserved.
;
;***************************************************************************
;
;	This contains the flags and pointers for a keyboard
;	translation table, and a USA table in a separate
;	segment.  To change to another translation table (in
;	a DynaLink Library) the pointers and other variables in
;	the header are replaced with those for the table in
;	the DLL.
;
;	This also contains the NewTable() function, which loads
;	a keyboard-table DynaLink Library, and calls the DLL's
;	function for copying pointers and data to the driver,
;	thus changing the keyboard layout.
;
;	The MapVirtualKey() function has been added.
;
;	Note: the NOKIA, ICO, and ENHANCE ifdefs are mutually exclusive
;	in this module.
;
;***************************************************************************
;
;	History
;
;	30 may 91	sandeeps	Added mapping VK_NUMPADx to scancodes
;
;	15 jan 90	peterbe		Add ifdif ICO a bit after TypeInRange.
;					Add KeyType range checking near start
;					of NewTable().
;	12 dec 89	peterbe		Read OliType (subtype=) from system.ini
;					even if the keyboard type > 2.
;	11 dec 89	peterbe		(1.53) OliType defaults to 0 if IsOli
;					is 0, in ICO ifdef code.
;	28 nov 89	peterbe		(1.51a) Use OpenFile() to open
;					Oem/Ansi xlat file now, so the system
;					and Windows directory are searched for
;					unqualified paths.
;
;	08 nov 89	peterbe		For 102-key ICO keyboard, Help key
;					now -> VK_F1 (for Help)
;
;	06 sep 89	peterbe		key # 13 -> VK_OEM_PLUS in keyTrTab.
;					Caused problem with KBDUSX.DLL, which
;					doesn't overlay this!
;
;	15 jul 89	peterbe		Type 2 now rated at 12, tho' may have
;					18 in some cases. O well. F17, F18
;					VK's changed VK_OEM_V1x.
;
;	06 jun 89	peterbe		Type 2 kbd only has 10 VIRTUAL function
;					keys (may change later), but adjusted
;					NumKeys...
;
;	30 jun 89	peterbe		Now set iqdNumFunc on basis of KeyType
;
;	13 jun 89	peterbe		Use standard VK_OEM_ names in tables.
;					GetTableSeg() can't preserve AX, so
;					must push/pop AX in MapVirtualKey().
;	12 jun 89	peterbe		CLD after mvkSkipGet.
;
;	09 jun 89	peterbe		Changed VK_OEM_NUMBER to VK_NUMLOCK
;
;	10 may 89	peterbe		Change jump code at start of
;					NewTable() -- need long jump now.
;
;	03 may 89	peterbe		Increased size of filename buffer
;					(NAMESIZE) to 63 -- max. for DOS.
;					Make this dynamic later!
;					Added check for Get..Int() returning
;					0 value for keyboard type -- now
;					causes restore of default value.
;
;	02 may 89	peterbe		Increased size of filename buffer
;					(NAMESIZE).
;
;	28 apr 89	peterbe		Get keyboard type info from SYSTEM.INI
;					now.  Names of KEYBOARD.DLL and
;					OEMANSI.BIN will also come from there.
; 	(up to Win 3.0, 1.20 build)
;	11 mar 89	peterbe		Fixed deadkey search in MapVirtualKey().
;	10 mar 89	peterbe		Fixed comment for case 3 of
;					MapVirtualKey().  Changed JLE to JBE
;					in 3rd case.
;	29 feb 89	peterbe		"sName" no longer forced to C:
;					Now overwrites Oem/Ansi tables with
;					"OemAnsi.bin" file.
;
;	26 jan 89	peterbe		Remove lpXlat parameter to GetKbdTable()
;					call.
;
;	30 nov 88	davidw		Removed GetCSAlias, no longer needed.
;
;	22 nov 88	peterbe		Implemented MapVirtualKey().
;
;	20 sep 88	peterbe		Reformatting first part of table header
;
;	17 sep 88	peterbe		moved TableType and fKeyType
;					to beginning of header for easy overlay.
;
;	31 aug 88	peterbe		Add code data to patch keyTrTab[] to
;					USA when error in changing tables.
;
;	25 aug 88	peterbe		Changing NewTable() to handle fixed
;					file name.  USA table (TABS seg.) is
;					restored if changing DLL fails.
;
;	23 aug 88	peterbe		Added GetCSAlias().
;
;	22 aug 88	peterbe		Changed comments and moved %OUT's.
;					Removed GetCODE(), use CSAlias now.
;
;	19 aug 88	peterbe		Removed offset of Version.
;
;	18 aug 88	peterbe		Changed comments.
;
;	17 aug 88	peterbe		Added 'Not9140'.
;
;	16 aug 88	peterbe		Add code to patch keyTrTab[] for
;					Nokia 6 keyboard.
;
;	14 aug 88	peterbe		Moved keyTrTab here from DATACOM --
;					the overlay for Nokia 6 keyboard is
;					in the segment _NEWTAB.
;
;	12 aug 88	peterbe		Changed 'NotOli' to 'NotM24'.
;					Moved RAMBIOS to keyboard.inc
;
;	11 aug 88	peterbe		Add page directive at beginning.
;					DLL name now defaults to "kbdus.mod".
;
;	08 aug 88	peterbe		KeyType, PCType, etc. moved here
;					from DATACOM.  Keyboard type defaults
;					set before looking at WIN.INI.
;
;	05 aug 88	peterbe		Fixed KeyType parameter to
;					GetKbdTable()
;
;	02 aug 88	peterbe		Moved NewTabld() variables
;					to DATA segment.  GetTableSeg()
;					doesn't check hTables, caller
;					must!
;
;	27 jul 88	peterbe		Moved TableEnd
;					Began NewTable() code.
;
;	26 jul 88	peterbe		Added pMortoCode pointer.
;					Added version etc. at beginning.
;
;	25 jul 88	peterbe		Modified GetTableSeg slightly.
;
;***************************************************************************

if1
%out
%out .. Tabs.Asm
endif

include windows.inc
include keyboard.inc
include vkwin.inc
include vkoem.inc

; macros for translation table entry.
include trans.inc

externFP	<OpenFile>
externFP	<AllocCStoDSAlias>
externFP	<FreeSelector>

sBegin DATA

    assumes ds,DATA

;***************************************************************************
;
;	String indicating Windows 3.00 driver.
;	This should begin the data segment.
;
;***************************************************************************

Vers	db	'WinKB3.00 '		; 10 bytes long!

	even

;***************************************************************************
;
;	Some variables indicating system/keyboard type.
;
;***************************************************************************

	public PCType, PCTypeHigh, KeyType
	public IsEri, IsOli, OliType

PCType		db	0			; determines if it's AT or not.
PCTypeHigh	db	0			; secondary ID byte
KeyType		db	0			; 1..6 -- INIT to ZERO!!
IsEri		db	0
IsOli		db	0			; NZ if Olivetti/AT&T ROM BIOS
OliType		db	0			; NZ if Olivetti-protocol kbd.
						; for M24 or AT&T 6300.
						; (= SubType in SYSTEM.INI)

;***************************************************************************
;
;	This value will be >= 32 if the table segment is in
;	a DynaLink Library (DLL).

	public	hTables

hTables	dw	0			; handle of table module

;***************************************************************************
;
;	Segment of tables.  Initialized by a call to GetTableSeg(),
; 	but changed when a DLL's tables are used -- GetKbdTable()
;	returns a pointer to the DLL's data segment.
;
	public TableSeg

TableSeg	dw	0

;***************************************************************************
;
;	Publics for table size and offset variables.
;
;***************************************************************************

    public pCapital, szCapital
    public pMorto, szMorto, pMortoCode
    public szDeadKey, pDeadKeyCode, pDeadChar
    public szSGCaps, pSGCapsVK, pSGTrans

    public VirtAdr, AsciiTab, VKSizes

;***************************************************************************
;
; Everything from here up to TableEnd gets overlaid with variables
; with the same names in the DLL, when a DLL is loaded.
;
; The first 16 bytes of this is then overlaid again, if TableType is not 4.
;
;***************************************************************************

    public TableBeg
TableBeg label byte

;***************************************************************************
;
; Keyboard table type
;
;***************************************************************************

	public TableType

TableType	db	4

;***************************************************************************
;
; This flags special features like caps lock.
;
;	kbAltGr	=	right alt key is ctrl-alt
;	kbShiftLock =	has shift lock instead of caps lock
;
;***************************************************************************

	public fKeyType

fKeyType	db	0


;***************************************************************************
;
; Table sizes (Number of bytes in search table for a particular
; translation).
;
; Order of entries must be maintained!
; First 4 entries are accessed by indexing based on shift state.
;
; Also, the whole list of pointers is transferred to the driver from the DLL
; with one REP MOVSB instruction.
;
;***************************************************************************

VKSizes	label word

szAscTran	dw	TABSoffset AscTranEnd - TABSoffset AscTranVK
szAscControl	dw	TABSoffset AscControlEnd - TABSoffset AscControlVK
szAscCtlAlt	dw	TABSoffset AscCtlAltEnd - TABSoffset AscCtlAltVK
szAscShCtlAlt	dw	TABSoffset AscShCtlAltEnd - TABSoffset AscShCtlAltVK

szMorto		dw	TABSoffset MortoEnd - TABSoffset Morto
szSGCaps	dw	TABSoffset SGCapsEnd - TABSoffset SGCapsVK

szCapital	dw	TABSoffset CapitalEnd - TABSoffset CapitalTable

; Everything up to here is overlaid a second time, for KeyType .ne. 4

szDeadKey	dw	TABSoffset DeadKeyEnd - TABSoffset DeadKeyCode

;***************************************************************************
;
; Addresses of virtual key code arrays for various shifts. 
;
;***************************************************************************

VirtAdr label word
		dw	TABSoffset AscTranVK		; shifted, unshifted
		dw	TABSoffset AscControlVK		; Control
		dw	TABSoffset AscCtlAltVK		; Control-Alt
		dw	TABSoffset AscShCtlAltVK	; Shift-Control-Alt

pMorto		dw	TABSoffset Morto
pDeadKeyCode	dw	TABSoffset DeadKeyCode
pSGCapsVK	dw	TABSoffset SGCapsVK

pCapital	dw	TABSoffset CapitalTable

;***************************************************************************
;
;	Addresses of translated character arrays for various shifts.
;
;***************************************************************************

AsciiTab label word
		dw	TABSoffset AscTran		; shifted, unshifted
		dw	TABSoffset AscControl		; Control
		dw	TABSoffset AscCtlAlt		; Control-Alt
		dw	TABSoffset AscShCtlAlt		; Shift-Control-Alt

pMortoCode	dw	TABSoffset MortoCode
pDeadChar	dw	TABSoffset DeadChar
pSGTrans	dw	TABSoffset SGTrans



;***************************************************************************
;
;	End of pointers and flags in header.
;
;***************************************************************************

	public	TableEnd

TableEnd label byte

;***************************************************************************
;	
;	Scan code to VK-code translation table; updated
;	in NewTable() when a DLL is loaded.
;	
;***************************************************************************

	public KeyTransBase, KeyTransTblSize

KeyTransBase	dw	dataOffset keyTrTab
KeyTransTblSize	dw	KeyTransTblEnd - keyTrTab   ; count of keys

;***************************************************************************

	public keyTrTab
keyTrTab label byte

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

	DB	VK_OEM_MINUS	; 0ch 12	; variable
	DB	VK_OEM_PLUS 	; 0dh 13	; variable
	DB	VK_BACK   	; 0eh 14
	DB	VK_TAB    	; 0fh 15
	DB	VK_Q      	; 010h 16	; variable -- also VK_A
	DB	VK_W      	; 011h 17	; variable -- also VK_Z
	DB	VK_E      	; 012h 18
	DB	VK_R      	; 013h 19
	DB	VK_T      	; 014h 20
	DB	VK_Y      	; 015h 21	; variable -- also VK_Z
	DB	VK_U      	; 016h 22
	DB	VK_I      	; 017h 23
	DB	VK_O      	; 018h 24
	DB	VK_P      	; 019h 25
	DB	VK_OEM_4    	; 01ah 26	; variable
	DB	VK_OEM_6    	; 01bh 27	; variable
	DB	VK_RETURN 	; 01ch 28
	DB	VK_CONTROL	; 01dh 29
	DB	VK_A      	; 01eh 30	; variable -- also VK_Q
	DB	VK_S      	; 01fh 31
	DB	VK_D      	; 020h 32
	DB	VK_F      	; 021h 33
	DB	VK_G      	; 022h 34
	DB	VK_H      	; 023h 35
	DB	VK_J      	; 024h 36
	DB	VK_K      	; 025h 37
	DB	VK_L      	; 026h 38
	DB	VK_OEM_1    	; 027h 39	; variable -- also VK_M
	DB	VK_OEM_7    	; 028h 40	; variable
; X1	label byte	; swap for AT (no swap for USA version)
	DB	VK_OEM_3    	; 029h 41	; variable
	DB	VK_SHIFT  	; 02ah 42
; X2	label byte	; swap for AT
	DB	VK_OEM_5    	; 02bh 43	; variable
	DB	VK_Z      	; 02ch 44	; variable -- also VK_Y
	DB	VK_X      	; 02dh 45
	DB	VK_C      	; 02eh 46
	DB	VK_V      	; 02fh 47
	DB	VK_B      	; 030h 48
	DB	VK_N      	; 031h 49
	DB	VK_M      	; 032h 50	; variable --

	DB	VK_OEM_COMMA	; 033h 51	; variable
	DB	VK_OEM_PERIOD	; 034h 52	; variable
	DB	VK_OEM_2    	; 035h 53	; variable

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

	; (scancodes 69..83 for most keyboards -- overlaid for Nokia type 6)

LabNok6 label byte
	DB	VK_NUMLOCK	; 045h 69
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

	; follow with Enhanced, ICO or Nokia 1050 keys

ifdef ICO

	; Olivetti 'ICO' 102-key extended keyboard for M24
	; These scan codes won't be generated with the 83-key
	; keyboard, so we put this in for all M24/AT&T 6300 keyboards.
	; It is assumed that the VK codes for 86..88 may be overlaid
	; with Enhanced keyboard VK codes.

	DB	VK_ICO_00   	; 054h 84
	DB	VK_MULTIPLY	; 055h 85
	DB	VK_F1	 	; 056h 86	; ICO HELP key: was VK_ICO_HELP
	DB	VK_RETURN 	; 057h 87	; will be overlaid if type 4
	DB	VK_LEFT   	; 058h 88	; will be overlaid if type 4
				;
	DB	VK_DOWN   	; 059h 89
	DB	VK_RIGHT  	; 05ah 90
	DB	VK_UP     	; 05bh 91
	DB	VK_ICO_CLEAR	; 05ch 92
	DB	VK_OEM_SCROLL	; 05dh 93
	DB	-1		; 05eh 94
	DB	VK_DIVIDE 	; 05fh 95
	DB	VK_F11    	; 060h 96
	DB	VK_F12    	; 061h 97
	DB	VK_F13    	; 062h 98
	DB	VK_F14    	; 063h 99
	DB	VK_F15    	; 064h 100
	DB	VK_F16    	; 065h 101
	DB	VK_OEM_F17     	; 066h 102
	DB	VK_OEM_F18     	; 067h 103

	; This is the end of the table for the Olivetti M24 driver.

else	; .. not Olivetti M24 ICO keyboard

	; This is always in normal IBM-compatible driver:

ifdef ENHANCE ; & not ICO
	DB	-1		; 054h 84
	DB	-1		; 055h 85
	DB	VK_OEM_102	; 056h 86
	DB	VK_F11    	; 057h 87
	DB	VK_F12    	; 058h 88
	; This is the end of the table for drivers handling Enhanced keyboards!
endif	; ENHANCE

endif	; not ICO

ifdef NOKIA
	; Extension for Nokia 5 (1050, etc.) keyboard
	; This is always in the Nokia driver, and may be overlaid with the
	; PatchNok6 table.

	DB	-1		; 054h 84
	DB	-1		; 055h 85
	DB	-1		; 056h 86
	DB	-1    		; 057h 87
	DB	-1    		; 058h 88
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
	DB	VK_RETURN	; 078h 120
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
	DB	VK_UP		; 0c8h 200
	DB	-1		; 0c9h 201
	DB	VK_SUBTRACT	; 0cah 202
	DB	VK_LEFT		; 0cbh 203
	DB	-1		; 0cch 204
	DB	VK_RIGHT	; 0cdh 205
	DB	VK_ADD		; 0ceh 206
	DB	-1		; 0cfh 207
	DB	VK_DOWN		; 0d0h 208
	DB	-1		; 0d1h 209
	DB	VK_INSERT	; 0d2h 210
	DB	VK_DELETE	; 0d3h 211
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

endif	; NOKIA

 
keyTransTblEnd label byte


;***************************************************************************
;
;	Variables for NewTab()
;

wCount		dw	0			; byte count from OEMANSI.BIN

NAMESIZE	equ	63			; size of filename buffer
public sNameBuf
sNameBuf	db	(NAMESIZE + 1) dup (0)	; filename buffer.

externW		keyNumBase			; for VK_NUMPADx
externW		iqdNumFunc			; number of function keys in
						; KBINFO structure.
NumKeys	label byte				; Table of no. of virtual
	db	0, 10, 12, 10, 12, 10, 24	; function keys, indexed by
	;	    1   2   3   4   5   6	; <-- KeyType.

sEnd DATA


; ********************************************************
;
sBegin CODE

externB CodePage	; 258 bytes starting here is overwritten.

sEnd CODE

; ********************************************************
;
; The actual tables are in a separate segment which may
; be freed, if the above tables are replaced with those
; from an installed keyboard table library.
;
; ********************************************************


createSeg _TABS, TABS, BYTE, PUBLIC, CODE
sBegin TABS
assumes CS,TABS
assumes DS,DATA

; ********************************************************
;
; Include a table file from .\tables directory.
; Segment begin and end directives, the pointer and count
; DW statements, and include statements are enclosed in IFNDEF INDRIVER
; blocks, and are thus not included.
;
; ********************************************************

INDRIVER = 0		; this will cause many statements in TAB4.INC
			; to be supressed.

include tab4.inc

; ** GetTableSeg() ***************************************
;
; This finds the paragraph of the TABS segment and stores
; it in TableSeg.
;
; Calling this will force the segment to be loaded.
;
; This segment isn't written to.
;
; REMEMBER that AX is TRASHED !!


cProc	GetTableSeg,<PUBLIC,FAR>,<si,di>
cBegin	GetTableSeg

	mov	ax,cs
	mov	TableSeg,ax

cEnd GetTableSeg

sEnd TABS

;***************************************************************************
;	Code for changing keyboard table, based on SYSTEM.INI entries
;	and tables in a DynaLink Library.
;***************************************************************************

;***************************************************************************
; Some Windows functions used here.  '...' means the name of this function.

			    ; Get SYSTEM.INI variables:
			    ; (lpFilename -> "SYSTEM.INI")
externFP  GetPrivateProfileInt		; int = Get..Int(lpAppName, lpKey,
					;		nDefault, lpFileName)
externFP  GetPrivateProfileString	; len = Get..String(lpAppName,
					;  lpKey, lpDefault, lpReturnedString,
					;  nSize, lpFileName)

			    ; use to access functions in DynaLink Libraries:
externFP  LoadLibrary		; HANDLE = ...(lpFileName) [ >= 32 if good ]
externFP  GetProcAddress	; LPFARPROC = ...(hModule, lpProcName)
externFP  FreeLibrary		; VOID = ...(hModule)

			    ; Used here to access function in Kernel
; externFP  GetModuleHandle	; HANDLE = ...(lpModuleName)

;***************************************************************************
; This is all in a PRELOAD DISCARDABLE segment.
;
; Anything that needs to stay around should be saved in the DATA segment.
; Temporary variables and constants can be in the NEWTAB segment.
;

createSeg _NEWTAB, NEWTAB, BYTE, PUBLIC, CODE
sBegin NEWTAB
assumes CS,NEWTAB
assumes DS,DATA

;***************************************************************************
;
; 'backup' copy of header
;
;	This is used when a DLL fails to load properly after the old
;	one is unloaded!
;
;***************************************************************************

public USTableBeg
USTableBeg label byte

	db	4	; table type
	db	0	; flags

	; trans. table sizes
	dw	TABSoffset AscTranEnd - TABSoffset AscTranVK
	dw	TABSoffset AscControlEnd - TABSoffset AscControlVK
	dw	TABSoffset AscCtlAltEnd - TABSoffset AscCtlAltVK
	dw	TABSoffset AscShCtlAltEnd - TABSoffset AscShCtlAltVK

	dw	TABSoffset MortoEnd - TABSoffset Morto
	dw	TABSoffset SGCapsEnd - TABSoffset SGCapsVK
	dw	TABSoffset CapitalEnd - TABSoffset CapitalTable

	dw	TABSoffset DeadKeyEnd - TABSoffset DeadKeyCode

	dw	TABSoffset AscTranVK		; shifted, unshifted
	dw	TABSoffset AscControlVK		; Control
	dw	TABSoffset AscCtlAltVK		; Control-Alt
	dw	TABSoffset AscShCtlAltVK	; Shift-Control-Alt

	dw	TABSoffset Morto
	dw	TABSoffset DeadKeyCode
	dw	TABSoffset SGCapsVK

	dw	TABSoffset CapitalTable

	dw	TABSoffset AscTran		; shifted, unshifted
	dw	TABSoffset AscControl		; Control
	dw	TABSoffset AscCtlAlt		; Control-Alt
	dw	TABSoffset AscShCtlAlt		; Shift-Control-Alt

	dw	TABSoffset MortoCode
	dw	TABSoffset DeadChar
	dw	TABSoffset SGTrans
USTableEnd label byte

szTable	= (DATAoffset TableEnd) - (DATAoffset TableBeg)
szUSTable = (NEWTABoffset USTableEnd) - (NEWTABoffset USTableBeg)

	.errnz szTable - szUSTable

; ********************************************************
;	keyTrTab patches to restore USA 4 table
; ********************************************************

USTransPatch label byte

	DB	VK_OEM_MINUS	; 0ch 12	; variable
	DB	VK_OEM_PLUS 	; 0dh 13	; variable
	DB	VK_BACK   	; 0eh 14
	DB	VK_TAB    	; 0fh 15
	DB	VK_Q      	; 010h 16	; variable -- also VK_A
	DB	VK_W      	; 011h 17	; variable -- also VK_Z
	DB	VK_E      	; 012h 18
	DB	VK_R      	; 013h 19
	DB	VK_T      	; 014h 20
	DB	VK_Y      	; 015h 21	; variable -- also VK_Z
	DB	VK_U      	; 016h 22
	DB	VK_I      	; 017h 23
	DB	VK_O      	; 018h 24
	DB	VK_P      	; 019h 25
	DB	VK_OEM_4    	; 01ah 26	; variable
	DB	VK_OEM_6    	; 01bh 27	; variable
	DB	VK_RETURN 	; 01ch 28
	DB	VK_CONTROL	; 01dh 29
	DB	VK_A      	; 01eh 30	; variable -- also VK_Q
	DB	VK_S      	; 01fh 31
	DB	VK_D      	; 020h 32
	DB	VK_F      	; 021h 33
	DB	VK_G      	; 022h 34
	DB	VK_H      	; 023h 35
	DB	VK_J      	; 024h 36
	DB	VK_K      	; 025h 37
	DB	VK_L      	; 026h 38
	DB	VK_OEM_1    	; 027h 39	; variable -- also VK_M
	DB	VK_OEM_7    	; 028h 40	; variable
; X1	label byte	; swap for AT (no swap for USA version)
	DB	VK_OEM_3    	; 029h 41	; variable
	DB	VK_SHIFT  	; 02ah 42
; X2	label byte	; swap for AT
	DB	VK_OEM_5    	; 02bh 43	; variable
	DB	VK_Z      	; 02ch 44	; variable -- also VK_Y
	DB	VK_X      	; 02dh 45
	DB	VK_C      	; 02eh 46
	DB	VK_V      	; 02fh 47
	DB	VK_B      	; 030h 48
	DB	VK_N      	; 031h 49
	DB	VK_M      	; 032h 50	; variable --

	DB	VK_OEM_COMMA	; 033h 51	; variable
	DB	VK_OEM_PERIOD	; 034h 52	; variable
	DB	VK_OEM_2    	; 035h 53	; variable


USTransPatchEnd label byte

szTransPatch = (NEWTABoffset USTransPatchEnd) - (NEWTABoffset USTransPatch)

; ********************************************************
;	keyTrTab patches for Nokia type 6 keyboard.
; ********************************************************


ifdef NOKIA

PatchNok6 label byte

	; 9140 translations for scan codes >= 45h

	DB	VK_OEM_RESET	; 045h 69
	DB	-1		; 046h 70
	DB	VK_HOME   	; 047h 71
	DB	VK_UP     	; 048h 72
	DB	VK_OEM_PA1  	; 049h 73
	DB	-1		; 04ah 74
	DB	VK_LEFT   	; 04bh 75
	DB	-1	  	; 04ch 76
	DB	VK_RIGHT  	; 04dh 77
	DB	-1	  	; 04eh 78
	DB	VK_OEM_PA3  	; 04fh 79
	DB	VK_DOWN   	; 050h 80
	DB	VK_OEM_BACKTAB	; 051h 81
	DB	VK_INSERT 	; 052h 82
	DB	VK_DELETE 	; 053h 83
	DB	VK_OEM_JUMP	; 054h 84
	DB	VK_MENU		; 055h 85
	DB	VK_OEM_PA2	; 056h 86
	DB	VK_HELP 	; 057h 87
	DB	-1	   	; 058h 88
	DB	VK_OEM_WSCTRL	; 059h 89
	DB	VK_CLEAR	; 05ah 90
	DB	VK_OEM_CUSEL	; 5Bh 91
	DB	VK_OEM_ATTN	; 5Ch 92
	DB	VK_OEM_FINNISH	; 5Dh 93
	DB	VK_OEM_COPY	; 5Eh 94
	DB	VK_PRINT	; 5Fh 95
	DB	VK_OEM_AUTO	; 60h 96
	DB	VK_OEM_ENLW	; 61h 97
	DB	VK_OEM_102	; 62h 98 -- overlaid with -1 for some keyboards
	DB	VK_F11		; 63h 99
	DB	VK_F12		; 64h 100
	DB	VK_F13		; 65h 106
	DB	VK_F14		; 66h 102
	DB	VK_F15		; 67h 105
	DB	VK_F16		; 68h 104

	DB	VK_OEM_F17	; 69h 105
	DB	VK_OEM_F18	; 6Ah 106
	DB	VK_OEM_F19	; 6Bh 107
	DB	VK_OEM_F20	; 6Ch 108
	DB	VK_OEM_F21	; 6Dh 109
	DB	VK_OEM_F22	; 6Eh 110
	DB	VK_OEM_F23	; 6Fh 111
	DB	VK_OEM_F24	; 70h 112
	DB	-1		; 071h 113
	DB	-1		; 072h 114
	DB	-1		; 073h 115
	DB	-1		; 074h 116
	DB	-1		; 075h 117
	DB	-1		; 076h 118
	DB	-1		; 077h 119
	DB	VK_RETURN	; 078h 120
	DB	-1		; 079h 121
	DB	-1		; 07ah 122
	DB	-1		; 07bh 123
	DB	-1		; 07ch 124
	DB	-1		; 07dh 125
	DB	-1		; 07eh 126
	DB	-1		; 07fh 127
	DB	-1		; 080h 128
	DB	VK_ESCAPE	; 081h 129
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
	DB	VK_TAB		; 08fh 143
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
	DB	VK_OEM_COMMA	; 0b3h 179
	DB	-1		; 0b4h 180
	DB	-1		; 0b5h 181
	DB	-1		; 0b6h 182
	DB	VK_MULTIPLY	; 0b7h 183
	DB	-1		; 0b8h 184
	DB	VK_SPACE	; 0b9h 185
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
	DB	VK_NUMLOCK	; 0c5h 197
	DB	VK_OEM_SCROLL	; 0c6h 198
	DB	VK_HOME   	; 0c7h 199
	DB	VK_UP     	; 0c8h 200
	DB	VK_PRIOR  	; 0c9h 201
	DB	VK_SUBTRACT	; 0cah 202
	DB	VK_LEFT   	; 0cbh 203
	DB	VK_CLEAR  	; 0cch 204
	DB	VK_RIGHT  	; 0cdh 205
	DB	VK_ADD    	; 0ceh 206
	DB	VK_END    	; 0cfh 207
	DB	VK_DOWN   	; 0d0h 208
	DB	VK_NEXT   	; 0d1h 209
	DB	VK_INSERT 	; 0d2h 210
	DB	VK_DELETE 	; 0d3h 211
	DB	-1		; 0d4h 212
	DB	-1		; 0d5h 213
	DB	VK_OEM_BACKTAB	; 0d6h 214
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
	DB	VK_RETURN	; 0f8h 248
	DB	-1		; 0f9h 249
	DB	-1		; 0fah 250
	DB	-1		; 0fbh 251
	DB	-1		; 0fch 252
	DB	-1		; 0fdh 253
	DB	-1		; 0feh 254
	DB	-1		; 0ffh 255

PatchNok6End label byte

endif	; NOKIA...

;***************************************************************************
;
; Constants for NewTable(),  in the code segment 'NEWTAB'.

; These are names in SYSTEM.INI
sSysIni		db	"system.ini",0		; name of file.
sKeyboard	db	"keyboard",0		; [keyboard] section.
sType		db	"type", 0		; type 1..6
sSubType	db	"subtype", 0		; For AT&T/Oli kbd's, mainly
sDllName	db	"keyboard.dll",0	; Name of keyboard DLL file.
sOemName	db	"oemansi.bin",0		; name of Oem/Ansi binary file
sNULL		db	0			; NULL string for default

; use this for initializing DLL. 
sProcName	db	"GetKbdTable",0		; name of function in DLL.


;***************************************************************************
;	GetSysFileName()
;
;	This is a local function whose purpose is to get a filename
;	from the [keyboard] section of SYSTEM.INI
;	It returns the filename in DATA:sNameBuf[] and the length
;	in AX.  It's a NEAR PASCAL function.
;	The only parameter is lpKey, the key name in [keyboard].
;	The default is ALWAYS a null string.
;***************************************************************************

cProc	GetSysFileName,<PUBLIC,NEAR>
	
	parmD	lpKey

cBegin
	; load up and define the parameters to GetPrivateProfileString()

	mov	bx,NEWTABoffset sKeyboard
	 regptr	csbx,cs,bx		; app. name = "keyboard"
	mov	cx,NEWTABoffset sNULL
	 regptr	cscx,cs,cx		; default is NULL string
	mov	ax, NAMESIZE		; how big our buffer is..
	mov	di,DATAoffset sNameBuf
	 regptr	dsdi,ds,di		; output string
	mov	dx,NEWTABoffset sSysIni
	 regptr	csdx,cs,dx		; file name = "SYSTEM.INI"

					;app   key   def.  ret.  siz  file
	cCall	GetPrivateProfileString,<csbx, lpKey, cscx, dsdi, ax, csdx>
cEnd

;***************************************************************************
;
;	NewTable()
;
;	Change keyboard tables, if a keyboard table DLL is defined in
;	SYSTEM.INI and the function GetKbdTable() exists and returns
;	successfully.
;
;	This function is passed no parameters by the caller -- it obtains
;	the following from SYSTEM.INI:
;
;	      [keyboard]
;		TYPE = 4			; 1..6.  4 is enhanced kbd.
;		SUBTYPE = 0			; 0 for all but Olivetti
;						; 8086 systems & AT&T 6300+
;		KEYBOARD.DLL = kbdus.dll	; name of DLL file
;		OEMANSI.BIN = XLATNO.BIN	; oem/ansi tables file
;
;	The module name of the DLL is expected to be the root of the DLL's
;	file name.  In any event, the module name must be different for
;	each keyboard-table DLL!
;
;***************************************************************************

cProc	NewTable,<PUBLIC,FAR>,<si,di>
						; LOCAL variables on stack:
localD	pProc					; pointer to GetKbdTable

localW	selCS2DS

localV	OFStruc,%(size OPENSTRUC)		; buffer for OPENFILE struc

cBegin	NewTable

	mov	selCS2DS, 0

	; Get new keyboard table type.
	; This is done only the FIRST time this is called!

	cmp	KeyType,0
	jz	SetDefaultKeyType
ifdef NOKIA
	cmp	KeyType,6
else
	cmp	KeyType,6
endif
	ja	SetDefaultKeyType
	mov	KeyType, 4

jKeyTypeFound:
	jmp	KeyTypeFound

SetDefaultKeyType:
	; Set default keyboard type, in case entry isn't in WIN.INI.
ifdef ICO
	cmp	IsOli, 0			; is it REALLY an Oli. system?
	jnz	IsOliType			; if not, 
	mov	KeyType, 4			; Default keyboard type to 4
	mov	OliType, 0			; clear OliType.
	jmp	short NoMoreOliTypes
IsOliType:					; Default for Olivetti M24 or
	mov	KeyType,1			; AT&T 6300 is type 1 -- 
	mov	OliType,KB83			; hardware ID is 2
	cmp	PCType, IDM240			; M240?
	jne	NotM240				; The M240 probably
	mov	KeyType,4			;  has enhanced keyboard.
	mov	OliType,KB102RT			;
NotM240:
	cmp	word ptr PCType, IBMATID	; 6300 Plus?
	jne	Not6300P
	mov	KeyType,1			; Probably a Plus: XT keyboard
	mov	OliType,KB302			; with 3 lights.

Not6300P:
NoMoreOliTypes:

else ; NOT ICO...
						; IBM compatible (or possibly
						; Nokia)
    ifdef NOKIA

	if1
	%out .. NOKIA
	endif
						; Default for Nokia/Ericsson
	mov	KeyType,5			; default to 1050 type
;;	jmp	NotAT
;;	cmp	IsEri, 0			; Is it an Ericsson system?
;;	jnz	NotAT				; if so, try WIN.INI
						; else set IBM-type default.

    else ; it's not NOKIA ...
						; Probably IBM-compatible:
	mov	KeyType,1			; First set to XT keyboard.
	cmp	PCType,IBMATID			; If it's an AT-type system,
	jne	NotAT
	mov	KeyType,3			; default to type 3.
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
    assumes DS,RAMBIOS
	test	byte ptr [KB_type],10h		; is Enhanced keyboard flag set?
	pop	ds
    assumes DS,DATA
	jz	notEnhanced			; skip if not
	mov	KeyType,4			; it's an enhanced keyboard
notEnhanced:

NotAT:

    endif ; not NOKIA


endif	; ICO not defined.


	; Get keyboard table type from WIN.INI.
	mov	si,NEWTABoffset sKeyboard	
	mov	di,NEWTABoffset sType	
	mov	bx,NEWTABoffset sSysIni	
	regptr	cssi,cs,si			; lpAppName = "keyboard"
	regptr	csdi,cs,di			; lpKeyName = "type"
	regptr	csbx,cs,bx			; lpFile = "SYSTEM.INI"
	xor	ah,ah
	mov	al,KeyType			; nDefault = KeyType
	cCall	GetPrivateProfileInt,<cssi,csdi,ax,csbx>
	or	al,al				; was returned value 0?
	jnz	KeyTypeNZ
	mov	al,KeyType			; yes. restore orig. value.
KeyTypeNZ:

ifdef NOKIA
	if1
	%out .. NOKIA range check
	endif

	cmp	al,6
else
	cmp	al,4				; only allow 5, 6 for Nokia
endif	; NOKIA

	jbe	TypeInRange
	jmp	LoadOemAnsi

TypeInRange:
	mov	KeyType,al			; save result

	; Get Olivetti/AT&T keyboard type from WIN.INI (should be 0
	; if the system is NOT an Olivetti system: M24 series
	; or AT&T 6300 series).  This value is used in the case of
	; 80286 systems to determine whether the M24 LED protocol
	; should be used IF olikbd.drv is accidentally installed.
ifdef ICO
	cmp	IsOli,0				; Olivetti ROM?
	jz	NotOliRom			;  skip if not.

	mov	si,NEWTABoffset sKeyboard	
	mov	di,NEWTABoffset sSubType	
	mov	bx,NEWTABoffset sSysIni	
	regptr	cssi,cs,si			; lpAppName = "keyboard"
	regptr	csdi,cs,di			; lpKeyName = "subtype"
	regptr	csbx,cs,bx			; lpFile = "SYSTEM.INI"
	xor	ah,ah
	mov	al,OliType			; nDefault
	cCall	GetPrivateProfileInt,<cssi,csdi,ax,csbx>
	mov	OliType,al			; save result
NotOliRom:
endif ; ICO

KeyTypeFound:

	; Set number of function keys
	mov	bl, KeyType			; get keyboard type
	xor	bh,bh				;  (1..6)
	mov	bl, NumKeys[bx]			; look up
	mov	iqdNumFunc, bx			; save in KBINFO

	; Is a keyboard DLL already loaded?

	cmp	hTables, 32
	jb	LoadNewDll

	; handle looks valid, so unload old library first

	mov	ax,hTables			; handle of existing DLL
	cCall	FreeLibrary,<ax>		; .. ignore result value

LoadNewDll:

; regardless of whether we can load a new library, we restore translation
; table keytrtab[] to USA translations.

	push	es
	push	ds

	push	ds
	pop	es				; ES = DATA
	push	cs
	pop	ds				; DS = NEWTAB
						; restore US keyTrTab[]
	mov	cx, szTransPatch		; size of patch
	mov	si, NEWTABoffset USTransPatch	; source offset
	mov	di, (DATAoffset keyTrTab) + 12	; dest. offset
	rep movsb				; overlay keyTrTab[]

	pop	ds
	pop	es

	; Get the library file's path from SYSTEM.INI
	mov	di,NEWTABoffset sDllName	; looking for
	regptr	csdi,cs,di			;  "KEYBOARD.DLL=" 
	cCall	GetSysFileName,<csdi>		;   in SYSTEM.INI
	cmp	ax,3				; look at filename length
	jb	CannotLoadDll			; if < 3, name is bogus

	; Load the new library.

	mov	si,DATAoffset sNameBuf		; filename is in here.
	regptr	dssi,ds,si
	cCall	LoadLibrary,<dssi>		; load it.
	cmp	ax,32
	jae	SaveHandle

CannotLoadDll:

	; could not load new library, so we need to point stuff in the
	; header table back to USA table, and clear hTables, so that
	; ToAscii will load the TABS segment.
	; We also need to restore keyTrTab[] to the USA translations.

	push	ds				; save DS
	push	ds
	pop	es				; ES = DATA
	push	cs
	pop	ds				; DS = NEWTAB

						; Restore header
	mov	cx, szTable			; size of header
	mov	si, NEWTABoffset USTableBeg	; offset of patch
	mov	di, DATAoffset TableBeg		; offset of header
	rep movsb				; copy header

;; This is not required because this has been shifted earlier.
;;
;;						; restore US keyTrTab[]
;;	mov	cx, szTransPatch		; size of patch
;;	mov	si, NEWTABoffset USTransPatch	; source offset
;;	mov	di, (DATAoffset keyTrTab) + 12	; dest. offset
;;	rep movsb				; overlay keyTrTab[]

	pop	ds				; restore ds
	mov	hTables, 0			; this will cause TABS to be
	jmp	LoadOemAnsi			; loaded!

SaveHandle:
	mov	hTables,ax			; Module is loaded

ifdef NOKIA
	; Nokia 9140 keyboard?
	cmp	KeyType, 6
	jne	Not9140

	; Yes, copy big patch to keyTrTab[] for type 6 keyboard.
	; The entry for scan code 98D (VK_OEM_102 or -1) will be
	; patched later.

	mov	cx,NEWTABoffset PatchNok6End - NEWTABoffset PatchNok6
	mov	si,NEWTABoffset PatchNok6
	mov	di,DATAoffset LabNok6
	push	ds				; save DS
	push	ds
	pop	es				; ES = DS
	push	cs
	pop	ds				; DS = CS
	cld
	rep movsb
	pop	ds				; restore DS

Not9140:

endif ; NOKIA

	;  get address of GetKbdTable()

	mov	ax,hTables
	mov	si,NEWTABoffset sProcName
	regptr	cssi,cs,si
	cCall	GetProcAddress,<ax, cssi>	; DX,AX -> routine

	mov	SEG_pProc, dx			; save segment
	mov	OFF_pProc, ax			; save offset

	; We have the address, call it.
	; Parameters of GetKbdTable():

;    ParmW nKeyType		; keyboard type 1..6
;    ParmD lpKeyTrans		; pointer to keyTrTab[] in driver.
;    ParmD lpHeader		; pointer to set of pointers/sizes in

	mov	cl,KeyType				; nKeyType
	xor	ch,ch

	mov	bx,DATAoffset keyTrTab
	regptr	dsbx,ds,bx				; lpKeyTrans

	mov	di,DATAoffset TableBeg
	regptr	dsdi,ds,di				; lpHeader

	cCall	pProc,<cx, dsbx,  dsdi>			; call GetKbdTable(..)
	mov	TableSeg, dx				; set segment

; Try to open the Oem/ansi table file (whose name is in SYSTEM.INI
; [keyboard] OEMANSI.BIN), check byte count, and overwrite Oem/Ansi tables.

LoadOemAnsi:
	; Get the oem/ansi data file's path from SYSTEM.INI
	mov	di,NEWTABoffset sOemName	; looking for
	regptr	csdi,cs,di			;  "OEMANSI.BIN=" 
	cCall	GetSysFileName,<csdi>		;   in SYSTEM.INI
	cmp	ax,3				; look at filename length
	jb	NewTableExit			; if < 3, name is bogus

	mov	si,DATAoffset sNameBuf  ; file name from "OEMANSI.BIN= "
	regptr	lpFileName,ds,si	;  far pointer to file name
	lea	di,OFStruc		; buffer for open-file structure.
	regptr	lpReOpen,ss,di		;  far pointer to open-file struct.
	mov	ax, OF_READ		; style
	cCall	OpenFile,<lpFileName, lpReOpen, ax>	; open the file.
	cmp	ax, 0			; check handle
	jl	NewTableExit		; if negative, we didn't open it.
if1
	%out ROM WARNING: Impure Code, _TEXT must RAM loaded!!!
	%out _TEXT must be fixed in real mode!!!
endif

        push    ax			; Save file handle
	mov	ax, _TEXT
	cCall	AllocCStoDSAlias, <ax>

        pop     bx			; restore file handle
	or	ax, ax

	jz	NTOemClose2

	mov	selCS2DS, ax

	mov	wCount,0		; clear byte count, then read from file.
	mov	dx,DATAoffset wCount
	mov	cx,2			; 1st 2 bytes of file is count
	mov	ah,3fh
	int	21h
	jc	NTOemClose		; if error, give up and close file
	cmp	ax,2			; must have read 2 bytes
	jne	NTOemClose
	mov	cx,wCount		; load byte count for rest of file
	cmp	cx,322			; for extended ansi .bins
	je	SHORT newansioembin	; .. will be 322 for new bin
	cmp	cx,258			; .. must be 258
	jne	NTOemClose		; old (not extended ansi)
					; CX is now byte count for next read..
					; BX is still handle
newansioembin:

	mov	dx,offset CodePage	; read starting at Code Page entry
	mov	ax,selCS2DS		; get writable alias for CS
	push	ds			; save DS
	mov	ds,ax			; set DS = CS alias
	mov	ah,3fh			; now read rest of file
	int	21h			; do it..
	pop	ds

NTOemClose:
        mov     si,bx			; save file handle
	cCall	FreeSelector, <selCS2DS>
        mov     bx,si

NTOemClose2:
	mov	ah,3eh			; close file, bx = handle, still.
	int	21h

NewTableExit:

cEnd	NewTable

sEnd NEWTAB

;***************************************************************************
;	The MapVirtualKey function is used by the PIF editor and
;	Winoldap to get information about keyboard mapping.
;
;	This maps the value in wCode into the returned value in AX,
;	according to the value of wMapType:
;
;	wMapType == 0:	Map VK to scan code
;
;	wMapType == 1:	Map scan code to VK
;
;	wMapType == 2:	Map VK to Ascii.
;
;	(Larger values reserved for future expansion)
;
;***************************************************************************

createSeg _MAPVK, MAPVK, BYTE, PUBLIC, CODE
sBegin MAPVK
assumes CS,MAPVK
assumes DS,DATA

cProc	MapVirtualKey,<PUBLIC,FAR>,<si,di>

parmW	wCode					; value to be translated
parmW	wMapType

cBegin	MapVirtualKey

	mov	ax,wMapType			; which kind of mapping?
	or	al,al
	jnz	MapScanToVK

    ; *********** wMapType == 0 means map VK to scan code ***********

	cld					; search UP!!
	push	ds				; set ES to DATA seg.
	pop	es
	mov	ax,wCode			; get VK to search for.
	mov	cx,KeyTransTblSize		; size of table
;;	jcxz	MapVKNotFound			; (always nonzero, in fact)
	mov	di,KeyTransBase			; offset of table
	push	di				; save base offset
	repne scasb				; search keyTrTab[] for VK code
	pop	cx				; get base back
	jne	CheckNumPad			; earlier code jne MapVKNotFound
	dec	di				; found, back up pointer 1 byte
	sub	di,cx				; subtract table base offset.
	mov	ax,di				; and that's the result.
	jmp	MapVKExit

CheckNumPad:

	mov	di,KeyNumBase			; VK_NUMPAD table
	mov	cx,13				; size
	push	di				; save base offset
	repne	scasb				; search KeyNumBase[] for VK
	pop	cx				; get back base
	jne	MapVKNotFound			; not a NUMPAD VK either
	dec	di				; found, back up pointer 1 byte
	sub	di,cx				; table base offset
	mov	ax,di				; get the offset
ifdef	NOKIA
	add	ax,71+80h			; the result.
else
	add	ax,71				; the result.
endif
	jmp	MapVKExit

MapVKNotFound:
	xor	ax,ax				; not found, so return 0
	jmp	MapVKExit


    ; *********** wMapType == 1 means map scan code to VK ***********

MapScanToVK:
	cmp	al,1
	ja	MapVKtoAscii
	xor	ax,ax				; clear result.
	mov	bx,wCode			; get scan code
	cmp	bx,KeyTransTblSize		; check range
	ja	MapVkExit			; exit with [AX] = 0 if too big
	mov	si, KeyTransBase
	mov	al,[bx+si]			; get VK code from table
	jmp	short MapVKExit


    ; *********** wMapType > 1 means map VK code to ASCII *********

    ; This is used as the first step in assigning a name to a key
    ; when it is typed in the PIF editor.

MapVKtoAscii:

	mov	ax,wCode			; get VK code.
	cmp	al,VK_0				; if VK_0 .. VK_9, this
	jb	mvkNotDigit			; is '0' .. '9', so just
	cmp	al,VK_9				; exit with Ascii in AX!
	jbe	MapVKExit
mvkNotDigit:
	cmp	al,VK_A				; likewise, VK_A..VK_Z are
	jb	mvkNotLetter			; just the upper-case letters.
	cmp	al,VK_Z				; so return those.
	jbe	MapVKExit
mvkNotLetter:					; look up in ASCII table
	cmp	hTables,32			; is DLL loaded?
	jae	mvkSkipGet			; .. handle must be >= 32
	push	ax				; must save VK value
	cCall	GetTableSeg			; load default table, if no DLL.
	pop	ax				; get VK back
	cld					; must scan UP
mvkSkipGet:
	mov	es,TableSeg			; get segment of tables.
	mov	cx,VKSizes			; get size of AscTranVK
;;	jcxz	mvkNotNormal			; .. always nonzero ..
	mov	di,VirtAdr			; get base of AscTranVK
	push	di
	repne scasb				; search
	pop	cx
	jne	mvkNotNormal			; if not found, try dead key
	dec	di				; it's found, so compute
	sub	di,cx				; index ...
	add	di,di				; AscTran is word table
	mov	bx,AsciiTab			; offset of AscTran
	mov	al,es:[bx+di]			; get translation.
	cmp	al,-1				; -1 indicates no translation.
	jne	MapVKExit			; AX is ASCII translation!

mvkNotNormal:					; look up in deadkey table
	; Look for unshifted char only, since we know it's not in normal table.
	mov	ax,wCode			; get VK code again.
	xor	ah,ah				; shift byte is 0 = UNshifted
	mov	cx,szMorto			; get size of table
	shr	cx,1				; change size to word count
	jcxz	mvkNotDead			; any dead keys, this keyboard?
	mov	di,pMorto			; get offset of VK search table
	push	di
	repne scasw				; search
	pop	cx
	jne	mvkNotDead			; found?
	dec	di				; yes, compute index
	sub	di,cx
	shr	di,1				; change to BYTE offset.
	mov	bx,pMortoCode			; get offset of translation.
	mov	al,es:[di+bx]			; get translation.
	mov	ah,80h				; set hi bit of AX --
	jmp	MapVKExit			; to indicate it's a deadkey.

	; Note: 3 different characters can represent umlaut accent deadkey --
	;  and this may be called Umlaut or Dierisis, depending on language!
	; 2 different characters MAY be used for acute accent deadkey!

mvkNotDead:
	xor	ax,ax				; Can't find translation.

MapVKExit:					; return with translation in AX

cEnd MapVirtualKey

sEnd MAPVK

if2
%out  .. end Tabs.Asm
%out
endif

end
