;** XLATNO.INC (Nordic) for Olivetti ***************************************
;                                                                          *
;   Copyright (C) 1985 by Microsoft Corporation                            *
;                                                                          *
;***************************************************************************
;   Nordic ANSI/OEM translation tables.  To be included in DATA module	   *
;   or <somename>.kbt source file for keyboard/trans. data file.	   *
;***************************************************************************
;	History
if1
%out XLATNO.INC (Nordic OEM/ANSI tables)    28 oct 87
endif
;	28 oct 87	plb	Removed 2 bytes before AnsiToOemTable
;	 6 mar 87	plb	ANSI a6 = OEM B3 = vert. bar

;.xlist		; must be included in file
;include	\lib\cmacros.inc
;.list

; This contains the OemToAnsi and AnsiToOem translation tables
; used by XLAT.ASM for the Nordic countries.


; A line with --- indicates a change from the standard XLAT.ASM

; This table maps ANSI characters >= 160 to the Norway/Denmark ROM/OEM code.

AnsiToOemTable	label	byte	; The following start with ANSI 160d.
      db   020h	    ; non breaking space (NBSP)
      db   0ADh	    ; A1h  inverted point
      db   063h	    ; A2h  cent ---
      db   09Ch	    ; A3h  british pound
      db   0AFh	    ; A4h  sun ---
      db   079h	    ; A5h  yen ---
      db   0B3H	    ; A6h  vertical bar (cut)
      db   015h	    ; A7h  section mark
      db   022h	    ; A8h  umlaut
      db   063h	    ; A9h  copyright sign
      db   061h	    ; AAh  a underlined (superscript) ---
      db   03Ch	    ; ABh  << ---
      db   0AAh	    ; ACh  logical NOT
      db   02Dh	    ; ADh  syllabic hyphenation (SHY)
      db   072h	    ; AEh  registered mark
      db   05Fh	    ; AFh  top bar

      db   0F8h	    ; B0h  degree
      db   0F1h	    ; B1h  + underlined
      db   0FDh	    ; B2h  2 (superscript)
      db   0AEh	    ; B3h  3 (superscript) ---
      db   027h	    ; B4h  acute
      db   0E6h	    ; B5h  greek mu
      db   014h	    ; B6h  paragraph sign
      db   0FAh	    ; B7h  middle dot
      db   02Ch	    ; B8h  weird comma
      db   031h	    ; B9h  1 (superscript)
      db   06Fh	    ; BAh  o underlined (superscript) ---
      db   03Eh	    ; BBh  >> ---
      db   05Fh	    ; BCh  1/4 ---
      db   05Fh	    ; BDh  1/2 ---
      db   05Fh	    ; BEh  3/4 ---
      db   0A8h	    ; BFh  inverted question mark

      db   041h	    ; C0h  A grave uppercase
      db   041h	    ; C1h  A acute uc
      db   041h	    ; C2h  A circumflex uc
      db   0AAh	    ; C3h  A tilde uc ---
      db   08Eh	    ; C4h  A umlaut uc
      db   08Fh	    ; C5h  A ring uc
      db   092h	    ; C6h  AE uc
      db   080h	    ; C7h  C cedilla uc
      db   045h	    ; C8h  E grave uc
      db   090h	    ; C9h  E acute uc
      db   045h	    ; CAh  E circumflex uc
      db   045h	    ; CBh  E umlaut uc
      db   049h	    ; CCh  I grave uc
      db   049h	    ; CDh  I acute uc
      db   049h	    ; CEh  I circumflex uc
      db   049h	    ; CFh  I umlaut uc

      db   044h	    ; D0h  D striked uc
      db   0A5h	    ; D1h  N tilde uc
      db   04Fh	    ; D2h  O grave uc
      db   04Fh	    ; D3h  O acute uc
      db   04Fh	    ; D4h  O circumflex uc
      db   0A7h	    ; D5h  O tilde uc ---
      db   099h	    ; D6h  O umlaut uc
      db   09eh	    ; D7h  (reserved) L.  (NORDIC)
      db   09Dh	    ; D8h  O slashed uc ---
      db   055h	    ; D9h  U grave uc
      db   055h	    ; DAh  U acute uc
      db   055h	    ; DBh  U circumflex uc
      db   09Ah	    ; DCh  U umlaut uc
      db   059h	    ; DDh  Y acute uc
      db   05Fh	    ; DEH  iceland Eth uc
      db   0E1h	    ; DFh  german sharp S or greek beta


      db   085h	    ; E0h  a grave lowercase
      db   0A0h	    ; E1h  a acute lc
      db   083h	    ; E2h  a circumflex lc
      db   0A9h	    ; E3h  a tilde lc ---
      db   084h	    ; E4h  a umlaut lc
      db   086h	    ; E5h  a ring lc
      db   091h	    ; E6h  ae lc
      db   087h	    ; E7h  c cedilla lc
      db   08Ah	    ; E8h  e grave lc
      db   082h	    ; E9h  e acute lc
      db   088h	    ; EAh  e circumflex lc
      db   089h	    ; EBh  e umlaut lc
      db   08Dh	    ; ECh  i grave lc
      db   0A1h	    ; EDh  i acute lc
      db   08Ch	    ; EEh  i circumflex lc
      db   08Bh	    ; EFh  i umlaut lc

      db   064h	    ; F0h  d striked lc
      db   0A4h	    ; F1h  n tilde lc
      db   095h	    ; F2h  o grave lc
      db   0A2h	    ; F3h  o acute lc
      db   093h	    ; F4h  o circumflex lc
      db   0A6h	    ; F5h  o tilde lc ---
      db   094h	    ; F6h  o umlaut lc
      db   09fh	    ; F7h  (reserved)  l. (NORDIC)
      db   09Bh	    ; F8h  o slashed lc ---
      db   097h	    ; F9h  u grave lc
      db   0A3h	    ; FAh  u acute lc
      db   096h	    ; FBh  u circumflex lc
      db   081h	    ; FCh  u umlaut lc
      db   079h	    ; FDh  y acute lc
      db   05Fh	    ; FEH  iceland eth lc
      db   098h	    ; FFh  y umlaut lc
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
      db   0C7H	    ; 80h  C cedilla
      db   0FCh	    ; 81h  u umlaut
      db   0E9h	    ; 82h  e acute
      db   0E2h	    ; 83h  a circumflex
      db   0E4h	    ; 84h  a umlaut
      db   0E0h	    ; 85h  a grave
      db   0E5h	    ; 86h  a ring
      db   0E7h	    ; 87h  c cedilla
      db   0EAh	    ; 88h  e circumflex
      db   0EBh	    ; 89h  e umlaut
      db   0E8h	    ; 8Ah  e grave
      db   0EFh	    ; 8Bh  i umlaut
      db   0EEh	    ; 8Ch  i circumflex
      db   0ECh	    ; 8Dh  i grave
      db   0C4h	    ; 8Eh  A umlaut
      db   0C5h	    ; 8Fh  A ring

      db   0C9h	    ; 90h  E acute
      db   0E6h	    ; 91h  ae
      db   0C6h	    ; 92h  AE
      db   0F4h	    ; 93h  o circumflex
      db   0F6h	    ; 94h  o umlaut
      db   0F2h	    ; 95h  o grave
      db   0FBh	    ; 96h  u circumflex
      db   0F9h	    ; 97h  u grave
      db   0FFh	    ; 98h  y umlaut
      db   0D6h	    ; 99h  O umlaut
      db   0DCh	    ; 9Ah  U umlaut
      db   0F8h	    ; 9Bh  o slashed ---
      db   0A3h	    ; 9Ch  british pound
      db   0D8h	    ; 9Dh  O slashed ---
      db   215d	    ; 9Eh  L period ---		Olivetti extended ANSI
      db   247d	    ; 9Fh  l period ---		Olivetti extended ANSI

      db   0E1h	    ; A0h  a acute
      db   0EDh	    ; A1h  i acute
      db   0F3h	    ; A2h  o acute
      db   0FAh	    ; A3h  u acute
      db   0F1h	    ; A4h  n tilde
      db   0D1h	    ; A5h  N tilde
      db   0F5h	    ; A6h  o tilde ---
      db   0D5h	    ; A7h  O tilde ---
      db   0BFh	    ; A8h  inverted question mark
      db   0E3h	    ; A9h  a tilde ---
      db   0C3h	    ; AAh  A tilde ---
      db   06Ch	    ; ABh  l script ---
      db   06Eh	    ; ACh  quote n ---
      db   0A1h	    ; ADh  inverted point
      db   0B3h	    ; AEh  3 superscript ---
      db   0A4h	    ; AFh  sun ---

      db   ' '      ; B0h  here begins semigraphic characters
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

