;********** XLATCA.INC *****************************************************
;                                                                          *
;   Copyright (C) 1985 by Microsoft Corporation                            *
;                                                                          *
;***************************************************************************
;   Canadian (French) ANSI/OEM translation tables.  To be included in	   *
;   DATA module.							   *
;***************************************************************************
if1
%out  XLATCA.INC  French-Canadian translation tables    28 oct 87
endif
;   Olivetti History
;	28 oct 87	plb	Removed 2 bytes before AnsiToOemTable
;	14 oct 87	plb	Fixing numerous errors in AnsiToOem table.
;	13 oct 87	plb	Created, from XLATUS.INC

; These translation tables are used for IBM-compatible French-Canadian
; keyboards.

; SEE CODE in XLAT.ASM, as modified for Olivetti version.

; This table maps ANSI characters >= 160 to the Canadian ROM/OEM code.

AnsiToOemTable	label	byte	; label points to ANSI character 160
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
      db   'L'	    ; D7h  reserved (L.)
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
      db   'l'	    ; F7h  reserved (l.)
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
      db   0DBh	    ; 9Eh  U circumflex uc	Canadian
      db   066h	    ; 9Fh  florin (dutch)
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
      db   0A6h	    ; B3h Vertical bar
      db   05Fh	    ; B4h
      db   05Fh	    ; B5h
      db   05Fh	    ; B6h
      db   05Fh	    ; B7h
      db   05Fh	    ; B8h
      db   05Fh	    ; B9h
      db   05Fh	    ; BAh
      db   05Fh	    ; BBh
      db   05Fh	    ; BCh
      db   05Fh	    ; BDh
      db   05Fh	    ; BEh
      db   05Fh	    ; BFh
		; OEM graphics second column
      db   05Fh	    ; C0h
      db   05Fh	    ; C1h
      db   05Fh	    ; C2h
      db   05Fh	    ; C3h
      db   05Fh	    ; C4h
      db   05Fh	    ; C5h
      db   05Fh	    ; C6h
      db   05Fh	    ; C7h
      db   05Fh	    ; C8h
      db   05Fh	    ; C9h
      db   05Fh	    ; CAh
      db   05Fh	    ; CBh
      db   05Fh	    ; CCh
      db   05Fh	    ; CDh
      db   05Fh	    ; CEh
      db   05Fh	    ; CFh
		; OEM graphics third column
      db   05Fh	    ; D0h
      db   05Fh	    ; D1h
      db   05Fh	    ; D2h
      db   05Fh	    ; D3h
      db   05Fh	    ; D4h
      db   05Fh	    ; D5h
      db   05Fh	    ; D6h
      db   05Fh	    ; D7h
      db   05Fh	    ; D8h
      db   05Fh	    ; D9h
      db   05Fh	    ; DAh
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
      db   05Fh	    ; F6h  math divide
      db   05Fh	    ; F7h  math approximately (~)
      db   0B0h	    ; F8h  degree
      db   0B7h	    ; F9h  period accent (bold)
      db   0B7h	    ; FAh  period accent
      db   05Fh	    ; FBh  math root
      db   06Eh	    ; FCh  n superscript
      db   0B2h	    ; FDh  2 superscript
      db   0A8h	    ; FEh  black box (or umlaut)
      db   05Fh	    ; FFh  blank

; end of include file
