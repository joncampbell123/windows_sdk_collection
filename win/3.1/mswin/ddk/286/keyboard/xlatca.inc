;********** XLATCA.INC *****************************************************
;                                                                          *
;  Copyright (C) 1987,1988,1989,1990 by Microsoft Corporation              *
;                                                                          *
;***************************************************************************
;   Canadian (French) ANSI/OEM translation tables.  To be included in	   *
;   DATA module.							   *
;***************************************************************************
if1
%out  XLATCA.INC  French-Canadian translation tables    28 oct 87
endif
;   Microsoft history
;	24 jun 89	peterbe	Fixed EXTENDEDANSI stuff.
;	29 apr 89	peterbe	Added support for extended ANSI (in ifdef).
;	27 feb 89	peterbe	Added code page entry.
;   Olivetti History
;	28 oct 87	plb	Removed 2 bytes before AnsiToOemTable
;	14 oct 87	plb	Fixing numerous errors in AnsiToOem table.
;	13 oct 87	plb	Created, from XLATUS.INC

CodePage dw	863	; French Canada

; These translation tables are used for IBM-compatible French-Canadian
; keyboards.

; SEE CODE in XLAT.ASM

ifdef EXTENDEDANSI

CtrlAnsiToOem label byte

	; Translate control characters from ANSI to OEM character set
	db	00,01,02,03,04,05,06,07
	db	08,09,10,11,12,13,14,15
	db	16,17,18,19,20,21,22,23
	db	24,25,26,27,28,29,30,31

endif ; EXTENDEDANSI

; This table maps ANSI characters >= 160 to the Canadian ROM/OEM code.

AnsiToOemTable	label	byte	; label points to ANSI character 160

ifdef EXTENDEDANSI

; table starts at 128

	db	128,129,02ch,09fh,02ch,05fh,0fdh,0fch
	db	136,025h,053h,03ch,04fh,141,142,143
	db	144,060h,027h,022h,022h,0f9h,02dh,05fh
	db	152,153,073h,03eh,06fh,157,158,059h

endif ; EXTENDEDANSI

		; ANSI punctuation, first row
      db   020h	    ; non breaking space (NBSP)
      db   '!'	    ; A1h  inverted point		Canadian
      db   09Bh	    ; A2h  cent
      db   09Ch	    ; A3h  british pound
      db   098h	    ; A4h  sun				Canadian
      db   'Y'	    ; A5h  yen				Canadian
      db   0A0H	    ; A6h  vertical bar (cut) 		Canadian
      db   08Fh	    ; A7h  section mark			Canadian
      db   0A4h	    ; A8h  umlaut sign			Canadian
      db   063h	    ; A9h  copyright sign
      db   'a'	    ; AAh  a underlined (superscript)	Canadian
      db   0AEh	    ; ABh  <<
      db   0AAh	    ; ACh  logical NOT
      db   08Dh	    ; ADh  syllabic hyphenation (SHY)	Canadian
      db   'r'	    ; AEh  registered mark
      db   0A7h	    ; AFh  top bar			Canadian
		; ANSI punctuation, second row
      db   0F8h	    ; B0h  degree
      db   0F1h	    ; B1h  + underlined
      db   0FDh	    ; B2h  2 (superscript)
      db   0A6h	    ; B3h  3 (superscript)		Canadian
      db   0A1h	    ; B4h  acute accent			Canadian
      db   0E6h	    ; B5h  greek mu
      db   086h	    ; B6h  paragraph sign		Canadian
      db   0FAh	    ; B7h  middle dot
      db   0A5h	    ; B8h  cedilla			Canadian
      db   031h	    ; B9h  1 (superscript)
      db   'o'	    ; BAh  o underlined (superscript)	Canadian
      db   0AFh	    ; BBh  >>
      db   0ACh	    ; BCh  1/4
      db   0ABh	    ; BDh  1/2
      db   0ADh	    ; BEh  3/4				Canadian
      db   '?'	    ; BFh  inverted question mark	Canadian
		; ANSI uppercase first row
      db   08Eh	    ; C0h  A grave uc			Canadian
      db   'A'	    ; C1h  A acute uc
      db   084h	    ; C2h  A circumflex uc		Canadian
      db   'A'	    ; C3h  A tilde uc
      db   'A'	    ; C4h  A umlaut uc			Canadian
      db   'A'	    ; C5h  A ring uc			Canadian
      db   'E'	    ; C6h  AE uc			Canadian
      db   080h	    ; C7h  C cedilla uc
      db   091h	    ; C8h  E grave uc			Canadian
      db   090h	    ; C9h  E acute uc
      db   092h	    ; CAh  E circumflex uc		Canadian
      db   094h	    ; CBh  E umlaut uc			Canadian
      db   'I'	    ; CCh  I grave uc
      db   'I'	    ; CDh  I acute uc
      db   0A8h	    ; CEh  I circumflex uc		Canadian
      db   095h	    ; CFh  I umlaut uc			Canadian
		; ANSI uppercase second row
      db   'D'	    ; D0h  D striked uc
      db   'N'	    ; D1h  N tilde uc			Canadian
      db   'O'	    ; D2h  O grave uc
      db   'O'	    ; D3h  O acute uc
      db   099h	    ; D4h  O circumflex uc		Canadian
      db   'O'	    ; D5h  O tilde uc
      db   'O'	    ; D6h  O umlaut uc			Canadian
      db   078h	    ; D7h  OLD:reserved (L.)
      db   'O'	    ; D8h  O slashed uc			Canadian
      db   09Dh	    ; D9h  U grave uc			Canadian
      db   'U'	    ; DAh  U acute uc
      db   09Eh	    ; DBh  U circumflex uc		Canadian
      db   09Ah	    ; DCh  U umlaut uc
      db   'Y'	    ; DDh  Y acute uc
      db   'T'	    ; DEH  iceland Eth
      db   0E1h	    ; DFh  german sharp S or greek beta

		; ANSI lowercase first row
      db   085h	    ; E0h  a grave lc
      db   'a'	    ; E1h  a acute lc			Canadian
      db   083h	    ; E2h  a circumflex lc
      db   'a'	    ; E3h  a tilde lc			Canadian
      db   'a'	    ; E4h  a umlaut lc			Canadian
      db   'a'	    ; E5h  a ring lc			Canadian
      db   'e'	    ; E6h  ae lc			Canadian
      db   087h	    ; E7h  c cedilla lc
      db   08Ah	    ; E8h  e grave lc
      db   082h	    ; E9h  e acute lc
      db   088h	    ; EAh  e circumflex lc
      db   089h	    ; EBh  e umlaut lc
      db   'i'	    ; ECh  i grave lc			Canadian
      db   'i'	    ; EDh  i acute lc			Canadian
      db   08Ch	    ; EEh  i circumflex lc
      db   08Bh	    ; EFh  i umlaut lc
		; ANSI lowercase second row
      db   'd'	    ; F0h  d striked lc
      db   'n'	    ; F1h  n tilde lc			Canadian
      db   'o'	    ; F2h  o grave lc			Canadian
      db   0A2h	    ; F3h  o acute lc
      db   093h	    ; F4h  o circumflex lc
      db   'o'	    ; F5h  o tilde lc
      db   'o'	    ; F6h  o umlaut lc			Canadian
      db   0F6h	    ; F7h  old:reserved (l.)
      db   'o'	    ; F8h  o slashed lc
      db   097h	    ; F9h  u grave lc
      db   0A3h	    ; FAh  u acute lc
      db   096h	    ; FBh  u circumflex lc
      db   081h	    ; FCh  u umlaut lc
      db   'y'	    ; FDh  y acute lc
      db   't'	    ; FEH  iceland eth lc
      db   'y'	    ; FFh  y umlaut lc			Canadian


; this table is used for translating OEM codes in the range 0..31.
; Only sunburst, paragraph, and section signs are translated.
; this table MUST START just before OemToAnsiTable...

CtrlOemToAnsi label byte
	db	00,01,02,03,04,05,06,07
	db	08,09,10,11,12,13,14,0a4h
	db	16,17,18,19,0b6h,0a7h,22,23
	db	24,25,26,27,28,29,30,31

; this translates Oem codes >= 128 to ANSI.
; there are 128 entries.

OemToAnsiTable	 label	 byte
		; OEM letters first column
      db   0C7H	    ; 80h  C cedilla uc
      db   0FCh	    ; 81h  u umlaut lc
      db   0E9h	    ; 82h  e acute lc
      db   0E2h	    ; 83h  a circumflex lc
      db   0C2h	    ; 84h  A circumflex uc	Canadian
      db   0E0h	    ; 85h  a grave lc
      db   0B6h	    ; 86h  paragraph sign	Canadian
      db   0E7h	    ; 87h  c cedilla lc
      db   0EAh	    ; 88h  e circumflex lc
      db   0EBh	    ; 89h  e umlaut lc
      db   0E8h	    ; 8Ah  e grave lc
      db   0EFh	    ; 8Bh  i umlaut lc
      db   0EEh	    ; 8Ch  i circumflex
      db   0ADh	    ; 8Dh  syllabic hyphenation	Canadian  double underscore
      db   0C0h	    ; 8Eh  A grave uc		Canadian
      db   0A7h	    ; 8Fh  section mark		Canadian
		; OEM letters second column
      db   0C9h	    ; 90h  E acute  uc
      db   0C8h	    ; 91h  E grave uc		Canadian
      db   0CAh	    ; 92h  E circumflex uc	Canadian
      db   0F4h	    ; 93h  o circumflex lc
      db   0CBh	    ; 94h  E umlaut uc		Canadian
      db   0CFh	    ; 95h  I umlaut uc		Canadian
      db   0FBh	    ; 96h  u circumflex lc
      db   0F9h	    ; 97h  u grave lc		prob. not special: Canadian ??
      db   0A4h	    ; 98h  sun			Canadian
      db   0D4h	    ; 99h  O circumflex uc	Canadian
      db   0DCh	    ; 9Ah  U umlaut uc
      db   0A2h	    ; 9Bh  cent
      db   0A3h	    ; 9Ch  british pound
      db   0D9h	    ; 9Dh  U grave uc		Canadian
      db   0DBh	    ; 9Eh  OLD:U circumflex uc	Canadian
      db   083h	    ; 9Fh  florin (dutch)
		; OEM letters/punctuation third column
      db   0A6h	    ; A0h  vertical bar		Canadian
      db   0B4h	    ; A1h  acute accent		Canadian
      db   0F3h	    ; A2h  o acute lc
      db   0FAh	    ; A3h  u acute lc
      db   0A8h	    ; A4h  umlaut sign		Canadian (dierisis)
      db   0B8h	    ; A5h  cedilla		Canadian
      db   0B3h	    ; A6h  3 (superscript)	Canadian
      db   0AFh	    ; A7h  top bar ??		Canadian (overscore?)
      db   0CEh	    ; A8h  I circumflex uc	Canadian
      db   05Fh	    ; A9h  left top corner
      db   0ACh	    ; AAh  right top corner
      db   0BDh	    ; ABh  1/2
      db   0BCh	    ; ACh  1/4
      db   0BEh	    ; ADh  3/4			Canadian
      db   0ABh	    ; AEh  <<
      db   0BBh	    ; AFh  >>
		; OEM graphics first column
      db   05Fh	    ; B0h  here begins semigraphic characters
      db   05Fh	    ; B1h
      db   05Fh	    ; B2h
		    ; In the following, '-' and vertical bar are used for
		    ; horiz. and vert. single or double lines, and 1-sided
		    ; connectors, and '+' for all other connectors.
      db   0A6h	    ; B3h Vertical bar
      db   0a6h	    ; B4h 
      db   0a6h	    ; B5h
      db   0a6h	    ; B6h
      db   '+'	    ; B7h
      db   '+'	    ; B8h
      db   0a6h	    ; B9h
      db   0a6h	    ; BAh Vertical bar (double)
      db   '+'	    ; BBh
      db   '+'	    ; BCh
      db   '+'	    ; BDh
      db   '+'	    ; BEh
      db   '+'	    ; BFh

      db   '+'	    ; C0h
      db   '-'	    ; C1h
      db   '-'	    ; C2h
      db   '+'	    ; C3h
      db   '-'	    ; C4h
      db   '+'	    ; C5h
      db   0a6h	    ; C6h
      db   0a6h	    ; C7h
      db   '+'	    ; C8h
      db   '+'	    ; C9h
      db   '-'	    ; CAh
      db   '-'	    ; CBh
      db   0a6h	    ; CCh
      db   '-'	    ; CDh
      db   '+'	    ; CEh
      db   '-'	    ; CFh

      db   '-'	    ; D0h
      db   '-'	    ; D1h
      db   '-'	    ; D2h
      db   '+'	    ; D3h
      db   '+'	    ; D4h
      db   '+'	    ; D5h
      db   '+'	    ; D6h
      db   '+'	    ; D7h
      db   '+'	    ; D8h
      db   '+'	    ; D9h
      db   '+'	    ; DAh

      db   05Fh	    ; DBh
      db   05Fh	    ; DCh
      db   0A6h	    ; DDh 
      db   05Fh	    ; DEh
      db   05Fh	    ; DFh  end of semigraphic characters
		; OEM Greek column
      db   05Fh	    ; E0h  alpha
      db   0DFh	    ; E1h  german sharp S or greek beta
      db   05Fh	    ; E2h  lambda
      db   0B6h	    ; E3h  pi
      db   05Fh	    ; E4h  sigma uc
      db   05Fh	    ; E5h  sigma lc
      db   0B5h	    ; E6h  mu
      db   05Fh	    ; E7h  tau
      db   05Fh	    ; E8h  phi uc
      db   05Fh	    ; E9h  theta
      db   05Fh	    ; EAh  omega
      db   05Fh	    ; EBh  delta
      db   05Fh	    ; ECh  infinite
      db   05Fh	    ; EDh  math empty set or phi lc
      db   05Fh	    ; EEh  math own sign
      db   05Fh	    ; EFh  math include sign
		; OEM math symbols, last column
      db   05Fh	    ; F0h  math equivalence sign
      db   0B1h	    ; F1h  + underlined
      db   05Fh	    ; F2h  greater equal
      db   05Fh	    ; F3h  less equal
      db   05Fh	    ; F4h  math integral upper part
      db   05Fh	    ; F5h  math integral lower part
      db   0F7h	    ; F6h  OLD:math divide
      db   05Fh	    ; F7h  math approximately (~)
      db   0B0h	    ; F8h  degree
      db   149	    ; F9h  period accent (bold)
      db   0B7h	    ; FAh  period accent
      db   05Fh	    ; FBh  math root
      db   06Eh	    ; FCh  n superscript
      db   0B2h	    ; FDh  2 superscript
      db   05Fh	    ; FEh  black box (or umlaut)
      db   05Fh	    ; FFh  blank

; end of include file
