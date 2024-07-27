;** XLATPO.INC (Portugal) for Olivetti *************************************
;                                                                          *
;   Copyright (C) 1985 by Microsoft Corporation                            *
;                                                                          *
;***************************************************************************
;   Portugese ANSI/OEM translation tables.  To be included in DATA module  *
;   or <somename>.kbt source file for keyboard/trans. data file.	   *
;									   *
;   This corresponds to IBM Portugese character set CODE PAGE 860.	   *
;***************************************************************************
;	History
if1
%out XLATPO2.INC (Portugal Code Page 860 OEM/ANSI tables)  28 October 1987
endif
;	28 oct 87	plb	Removed 2 bytes before AnsiToOemTable
;	13 oct 87	plb	Modified from Nordic

; This contains the OemToAnsi and AnsiToOem translation tables
; used by XLAT.ASM for Portugal.

; A line with --- indicates a change from the standard XLAT.ASM

; This table maps ANSI characters >= 160 to the Portugese ROM/OEM code.

AnsiToOemTable	label	byte	; The following start with ANSI 160d.
      db   020h	    ; non breaking space (NBSP)
      db   0ADh	    ; A1h  inverted point
      db   09bh	    ; A2h  cent			Portugese
      db   09Ch	    ; A3h  british pound
      db   00Fh	    ; A4h  sun			Portugese
      db   059h	    ; A5h  yen 'Y'		Portugese
      db   0B3H	    ; A6h  vertical bar (cut)
      db   015h	    ; A7h  section mark
      db   022h	    ; A8h  umlaut
      db   063h	    ; A9h  copyright sign
      db   0A6h	    ; AAh  a underlined (superscript) Portugese
      db   0AEh	    ; ABh  <<
      db   0AAh	    ; ACh  logical NOT
      db   02Dh	    ; ADh  syllabic hyphenation (SHY)
      db   072h	    ; AEh  registered mark
      db   05Fh	    ; AFh  top bar

      db   0F8h	    ; B0h  degree
      db   0F1h	    ; B1h  + underlined
      db   0FDh	    ; B2h  2 (superscript)
      db   0FCh	    ; B3h  3 (superscript) 	Portugese (n superscript)
      db   027h	    ; B4h  acute
      db   0E6h	    ; B5h  greek mu
      db   014h	    ; B6h  paragraph sign
      db   0FAh	    ; B7h  middle dot
      db   02Ch	    ; B8h  weird comma
      db   031h	    ; B9h  1 (superscript)
      db   0A7h	    ; BAh  o underlined (superscript) Portugese
      db   0AFh	    ; BBh  >>
      db   0ACh	    ; BCh  1/4
      db   0ABh	    ; BDh  1/2
      db   05Fh	    ; BEh  3/4 xxxx
      db   0A8h	    ; BFh  inverted question mark

		    ; AnsiToOem UC letters:

      db   091h	    ; C0h  A grave uc		Portugese
      db   086h	    ; C1h  A acute uc		Portugese
      db   08fh	    ; C2h  A circumflex uc	Portugese
      db   08Eh	    ; C3h  A tilde uc		Portugese
      db   'A'	    ; C4h  A umlaut uc		Portugese
      db   'A'	    ; C5h  A ring uc		Portugese
      db   'E'	    ; C6h  AE uc		Portugese
      db   080h	    ; C7h  C cedilla uc
      db   092h	    ; C8h  E grave uc		Portugese
      db   090h	    ; C9h  E acute uc
      db   089h	    ; CAh  E circumflex uc	Portugese
      db   'E'	    ; CBh  E umlaut uc		Portugese
      db   098h	    ; CCh  I grave uc		Portugese
      db   08Bh	    ; CDh  I acute uc		Portugese
      db   049h	    ; CEh  I circumflex uc
      db   049h	    ; CFh  I umlaut uc

      db   'D'	    ; D0h  D striked uc icelandic
      db   0A5h	    ; D1h  N tilde uc
      db   0A9h	    ; D2h  O grave uc		Portugese
      db   09Fh	    ; D3h  O acute uc		Portugese
      db   08Ch	    ; D4h  O circumflex uc
      db   099h	    ; D5h  O tilde uc		Portugese
      db   'O'	    ; D6h  O umlaut uc		Portugese
      db   'L'	    ; D7h 
      db   'O'	    ; D8h  O slashed uc		Portugese
      db   09Dh	    ; D9h  U grave uc		Portugese
      db   096h	    ; DAh  U acute uc		Portugese
      db   055h	    ; DBh  U circumflex uc
      db   09Ah	    ; DCh  U umlaut uc
      db   'Y'	    ; DDh  Y acute uc		Portugese
      db   'T'	    ; DEH  iceland Eth uc
      db   0E1h	    ; DFh  german sharp S or greek beta

		    ; AnsiToOem LC letters:

      db   085h	    ; E0h  a grave lowercase
      db   0A0h	    ; E1h  a acute lc
      db   083h	    ; E2h  a circumflex lc
      db   084h	    ; E3h  a tilde lc		Portugese
      db   'a'	    ; E4h  a umlaut lc		Portugese
      db   'a'	    ; E5h  a ring lc		Portugese
      db   'e'	    ; E6h  ae lc		Portugese
      db   087h	    ; E7h  c cedilla lc
      db   08Ah	    ; E8h  e grave lc
      db   082h	    ; E9h  e acute lc
      db   088h	    ; EAh  e circumflex lc
      db   'e'	    ; EBh  e umlaut lc
      db   08Dh	    ; ECh  i grave lc
      db   0A1h	    ; EDh  i acute lc
      db   'i'	    ; EEh  i circumflex lc	Portugese
      db   'i'	    ; EFh  i umlaut lc		Portugese

      db   'd'	    ; F0h  d striked lc icelandic
      db   0A4h	    ; F1h  n tilde lc
      db   095h	    ; F2h  o grave lc
      db   0A2h	    ; F3h  o acute lc
      db   093h	    ; F4h  o circumflex lc
      db   094h	    ; F5h  o tilde lc		Portugese
      db   'o'	    ; F6h  o umlaut lc		Portugese
      db   'l'	    ; F7h  
      db   'o'	    ; F8h  o slashed lc		Portugese
      db   097h	    ; F9h  u grave lc
      db   0A3h	    ; FAh  u acute lc
      db   'u'	    ; FBh  u circumflex lc	Portugese
      db   081h	    ; FCh  u umlaut lc
      db   'y'	    ; FDh  y acute lc		Portugese
      db   't'	    ; FEH  iceland eth lc
      db   'y'	    ; FFh  y umlaut lc		Portugese
			; end of AnsiToOemTable

; this table is used for translating OEM codes in the range 0..31.
; Only sunburst, paragraph, and section signs are translated.
; this table MUST START just before OemToAnsiTable...

CtrlOemToAnsi label byte
	db	00,01,02,03,04,05,06,07
	db	08,09,10,11,12,13,14,0a4h
	db	16,17,18,19,0b6h,0a7h,22,23
	db	24,25,26,27,28,29,30,31

OemToAnsiTable	 label	 byte
		; OemToAnsi letters, 1st column
      db   0C7H	    ; 80h  C cedilla uc
      db   0FCh	    ; 81h  u umlaut lc
      db   0E9h	    ; 82h  e acute lc
      db   0E2h	    ; 83h  a circumflex lc
      db   0E3h	    ; 84h  a tilde lc		Portugese
      db   0E0h	    ; 85h  a grave lc
      db   0C1h	    ; 86h  A acute uc		Portugese
      db   0E7h	    ; 87h  c cedilla lc
      db   0EAh	    ; 88h  e circumflex lc
      db   0CAh	    ; 89h  E circumflex	 uc	Portugese
      db   0E8h	    ; 8Ah  e grave lc
      db   0CDh	    ; 8Bh  I acute uc		Portugese
      db   0D4h	    ; 8Ch  O circumflex uc	Portugese
      db   0ECh	    ; 8Dh  i grave lc
      db   0C3h	    ; 8Eh  A tilde uc		Portugese
      db   0C2h	    ; 8Fh  A circumflex uc	Portugese
		; OemToAnsi letters, 2st column
      db   0C9h	    ; 90h  E acute uc
      db   0C0h	    ; 91h  A grave uc		Portugese
      db   0C8h	    ; 92h  E grave uc		Portugese
      db   0F4h	    ; 93h  o circumflex lc
      db   0F5h	    ; 94h  o tilde lc		Portugese
      db   0F2h	    ; 95h  o grave lc
      db   0DAh	    ; 96h  U acute uc		Portugese
      db   0F9h	    ; 97h  u grave lc
      db   0CCh	    ; 98h  I grave uc		Portugese
      db   0D5h	    ; 99h  O tilde uc		Portugese
      db   0DCh	    ; 9Ah  U umlaut uc
      db   0A2h	    ; 9Bh  cent
      db   0A3h	    ; 9Ch  british pound
      db   0D9h	    ; 9Dh  U grave uc		Portugese
      db   'P'	    ; 9Eh  peseta
      db   0D3h	    ; 9Fh  O acute uc		Portugese
		; OemToAnsi letters, 3rd column
      db   0E1h	    ; A0h  a acute lc
      db   0EDh	    ; A1h  i acute lc
      db   0F3h	    ; A2h  o acute lc
      db   0FAh	    ; A3h  u acute lc
      db   0F1h	    ; A4h  n tilde lc
      db   0D1h	    ; A5h  N tilde uc
      db   0AAh	    ; A6h  a underscore		Portugese
      db   0BAh	    ; A7h  o underscore		Portugese
      db   0BFh	    ; A8h  inverted question mark
      db   0D2h	    ; A9h  O grave uc		Portugese
      db   0ACh	    ; AAh  right top corner
      db   0BDh	    ; ABh  1/2
      db   0BCh	    ; ACh  1/4
      db   0A1h	    ; ADh  inverted excl. point
      db   0ABh	    ; AEh  <<
      db   0BBh	    ; AFh  >>
		; OemToAnsi  semigraphic characters 1
      db   ' '      ; B0h
      db   ' '      ; B1h
      db   ' '      ; B2h
      db   0A6H      ; B3h	vertical bar
      db   '+'      ; B4h
      db   '+'      ; B5h
      db   '+'      ; B6h
      db   '+'      ; B7h
      db   '+'      ; B8h
      db   '+'      ; B9h
      db   '|'      ; BAh
      db   '+'      ; BBh
      db   '+'      ; BCh
      db   '+'      ; BDh
      db   '+'      ; BEh
      db   '+'      ; BFh
		; OemToAnsi  semigraphic characters 2
      db   '+'      ; C0h
      db   '+'      ; C1h
      db   '+'      ; C2h
      db   '+'      ; C3h
      db   '-'      ; C4h
      db   '+'      ; C5h
      db   '+'      ; C6h
      db   '+'      ; C7h
      db   '+'      ; C8h
      db   '+'      ; C9h
      db   '+'      ; CAh
      db   '+'      ; CBh
      db   '+'      ; CCh
      db   '='      ; CDh
      db   '+'      ; CEh
      db   '+'      ; CFh
		; OemToAnsi  semigraphic characters 3
      db   '+'      ; D0h
      db   '+'      ; D1h
      db   '+'      ; D2h
      db   '+'      ; D3h
      db   '+'      ; D4h
      db   '+'      ; D5h
      db   '+'      ; D6h
      db   '+'      ; D7h
      db   '+'      ; D8h
      db   '+'      ; D9h
      db   '+'      ; DAh
      db   ' '      ; DBh
      db   ' '      ; DCh
      db   ' '      ; DDh
      db   ' '      ; DEh
      db   ' '      ; DFh  end of semigraphic characters
		; OemToAnsi  Greek characters
      db   05Fh	    ; E0h  alpha
      db   0DFh	    ; E1h  german sharp S or greek beta
      db   05Fh	    ; E2h  lambda
      db   0B6h	    ; E3h  pi
      db   05Fh	    ; E4h  sigma uc
      db   05Fh	    ; E5h  sigma lc
      db   0B5h	    ; E6h  mu
      db   05Fh	    ; E7h  tau
      db   05Fh	    ; E8h  phi uc
      db   05Fh	    ; E9h  teta
      db   05Fh	    ; EAh  omega
      db   05Fh	    ; EBh  delta
      db   05Fh	    ; ECh  infinite
      db   05Fh	    ; EDh  math empty set or phi lc
      db   05Fh	    ; EEh  math own sign
      db   05Fh	    ; EFh  math include sign
		; OemToAnsi  Math characters
      db   05Fh	    ; F0h  math equivalence sign
      db   0B1h	    ; F1h  + underlined
      db   05Fh	    ; F2h  greater equal
      db   05Fh	    ; F3h  less equal
      db   05Fh	    ; F4h  math integral upper part
      db   05Fh	    ; F5h  math integral lower part
      db   05Fh	    ; F6h  math divide
      db   05Fh	    ; F7h  math approximately (~)
      db   0B0h	    ; F8h  degree
      db   0B7h	    ; F9h  period accent
      db   0B7h	    ; FAh  period accent
      db   05Fh	    ; FBh  math root
      db   06Eh	    ; FCh  n superscript or eta ????
      db   0B2h	    ; FDh  2 superscript
      db   05Fh	    ; FEh  black box (or umlaut)
      db   05Fh	    ; FFh  blank

