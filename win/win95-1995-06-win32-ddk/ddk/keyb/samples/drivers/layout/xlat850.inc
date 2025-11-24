;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;** XLAT850.INC (Nordic) for Olivetti **************************************
;                                                                          *
;***************************************************************************
;   CODE PAGE 850 ANSI/OEM translation tables.
;***************************************************************************
if1
%out XLAT850.INC
endif

;.xlist		; must be included in file
;include	\lib\cmacros.inc
;.list

; This contains the OemToAnsi and AnsiToOem translation tables used
; for countries systems or using CODE Page 850 for DOS applications.

CodePage dw	850	; the 'international' code page

ifdef EXTENDEDANSI

CtrlAnsiToOem label byte

	; Translate control characters from ANSI to OEM character set
	db	00,01,02,03,04,05,06,07
	db	08,09,10,11,12,13,14,15
	db	16,17,18,19,20,21,22,23
	db	24,25,26,27,28,29,30,31

endif ; EXTENDEDANSI


AnsiToOemTable	label	byte	; The following start with ANSI 160d.

ifdef EXTENDEDANSI

; table starts at 128

	db	128,129,02ch,09fh,02ch,05fh,0fdh,0fch
	db	136,025h,053h,03ch,04fh,141,142,143
	db	144,060h,027h,022h,022h,149,02dh,05fh
	db	152,153,073h,03eh,06fh,157,158,059h

endif ; EXTENDEDANSI

; This table maps ANSI characters >= 160 to the CP 850 ROM/OEM code.

      db   020h	    ; non breaking space (NBSP)
      db   0ADh	    ; A1h  inverted point
      db   0bdh	    ; A2h  cent ---
      db   09Ch	    ; A3h  british pound
      db   0cfh	    ; A4h  sun ---
      db   0beh	    ; A5h  yen ---
      db   0DDH	    ; A6h  vertical bar (cut) ---
      db   0f5h	    ; A7h  section mark ---
      db   0f9h	    ; A8h  umlaut ---
      db   0b8h	    ; A9h  copyright sign ---
      db   0a6h	    ; AAh  a underlined (superscript) ---
      db   0aeh	    ; ABh  << ---
      db   0AAh	    ; ACh  logical NOT
      db   0f0h	    ; ADh  syllabic hyphenation (SHY) ---
      db   0a9h	    ; AEh  registered mark
      db   0eeh	    ; AFh  top bar ---

      db   0F8h	    ; B0h  degree
      db   0F1h	    ; B1h  + underlined (plus or minus)
      db   0FDh	    ; B2h  2 (superscript)
      db   0FCh	    ; B3h  3 (superscript) ---
      db   0efh	    ; B4h  acute ---
      db   0E6h	    ; B5h  greek mu
      db   0f4h	    ; B6h  paragraph sign ---
      db   0FAh	    ; B7h  middle dot
      db   0f7h	    ; B8h  cedilla for deadkey ---
      db   0fbh	    ; B9h  1 (superscript) ---
      db   0a7h	    ; BAh  o underlined (superscript) ---
      db   0afh	    ; BBh  >> ---
      db   0ach	    ; BCh  1/4 ---
      db   0abh	    ; BDh  1/2 ---
      db   0f3h	    ; BEh  3/4 ---
      db   0A8h	    ; BFh  inverted question mark

      db   0b7h	    ; C0h  A grave uppercase --
      db   0b5h	    ; C1h  A acute uc --
      db   0b6h	    ; C2h  A circumflex uc --
      db   0c7h	    ; C3h  A tilde uc ---
      db   08Eh	    ; C4h  A umlaut uc
      db   08Fh	    ; C5h  A ring uc
      db   092h	    ; C6h  AE uc
      db   080h	    ; C7h  C cedilla uc
      db   0d4h	    ; C8h  E grave uc ---
      db   090h	    ; C9h  E acute uc
      db   0d2h	    ; CAh  E circumflex uc ---
      db   0d3h	    ; CBh  E umlaut uc ---
      db   0deh	    ; CCh  I grave uc ---
      db   0d6h	    ; CDh  I acute uc ---
      db   0d7h	    ; CEh  I circumflex uc ---
      db   0d8h	    ; CFh  I umlaut uc ---

      db   0d1h	    ; D0h  Icelandic eth (D striked) uc ---
      db   0A5h	    ; D1h  N tilde uc
      db   0e3h	    ; D2h  O grave uc ---
      db   0e0h	    ; D3h  O acute uc ---
      db   0e2h	    ; D4h  O circumflex uc ---
      db   0e5h	    ; D5h  O tilde uc ---
      db   099h	    ; D6h  O umlaut uc
      db   09eh	    ; D7h  Multiply sign ---
      db   09Dh	    ; D8h  O slashed uc ---
      db   0ebh	    ; D9h  U grave uc ---
      db   0e9h	    ; DAh  U acute uc ---
      db   0eah	    ; DBh  U circumflex uc ---
      db   09Ah	    ; DCh  U umlaut uc
      db   0edh	    ; DDh  Y acute uc ---
      db   0e8h	    ; DEH  Icelandic thorn uc ---
      db   0E1h	    ; DFh  german sharp S


      db   085h	    ; E0h  a grave lowercase
      db   0A0h	    ; E1h  a acute lc
      db   083h	    ; E2h  a circumflex lc
      db   0c6h	    ; E3h  a tilde lc ---
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

      db   0d0h	    ; F0h  Icelandic eth (d striked) lc ---
      db   0A4h	    ; F1h  n tilde lc
      db   095h	    ; F2h  o grave lc
      db   0A2h	    ; F3h  o acute lc
      db   093h	    ; F4h  o circumflex lc
      db   0e4h	    ; F5h  o tilde lc ---
      db   094h	    ; F6h  o umlaut lc
      db   0f6h	    ; F7h  divide sign ---
      db   09Bh	    ; F8h  o slashed lc ---
      db   097h	    ; F9h  u grave lc
      db   0A3h	    ; FAh  u acute lc
      db   096h	    ; FBh  u circumflex lc
      db   081h	    ; FCh  u umlaut lc
      db   0ech	    ; FDh  y acute lc
      db   0e7h	    ; FEH  icelandic thorn lc
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
      db   0d7h	    ; 9Eh  multiply ---
      db   083h	    ; 9Fh  florin --- (not in ANSI)

      db   0E1h	    ; A0h  a acute
      db   0EDh	    ; A1h  i acute
      db   0F3h	    ; A2h  o acute
      db   0FAh	    ; A3h  u acute
      db   0F1h	    ; A4h  n tilde
      db   0D1h	    ; A5h  N tilde
      db   0aah	    ; A6h  a underscore ---
      db   0bah	    ; A7h  o underscore ---
      db   0BFh	    ; A8h  inverted question mark
      db   0aeh	    ; A9h  Registered trademark ---
      db   0ach	    ; AAh  Not sign ---
      db   0bdh	    ; ABh  1/2 ---
      db   0bch	    ; ACh  1/4 ---
      db   0A1h	    ; ADh  inverted point
      db   0abh	    ; AEh  << ---
      db   0bbh	    ; AFh  >> ---

      db   '_'      ; B0h  here begins semigraphic characters
      db   '_'      ; B1h
      db   '_'      ; B2h
		    ; In the following, '-' and vertical bar are used for
		    ; horiz. and vert. single or double lines, and 1-sided
		    ; connectors, and '+' for all other connectors.
		    ;
		    ; IN CP 850, some of the characters in this group are
		    ; alphabetic (accented) characters, or special characters.
		    ;
      db   0A6h	    ; B3h  graphics (not in ANSI)
      db   0a6h	    ; B4h  graphics (not in ANSI)
      db   0c1h	    ; B5h  A acute ---
      db   0c2h	    ; B6h  A circumflex ---
      db   0c0h	    ; B7h  A grave ---
      db   0a9h	    ; B8h  Copyright ---
      db   0a6h	    ; B9h  graphics (not in ANSI)
      db   0a6h	    ; BAh  Vertical bar (double)
      db   '+'	    ; BBh  graphics (not in ANSI)
      db   '+'	    ; BCh  graphics (not in ANSI)
      db   0a2h	    ; BDh  Cent sign ---
      db   0a5h	    ; BEh  Yen sign ---
      db   '+'	    ; BFh  graphics (not in ANSI)

      db   '+'	    ; C0h  graphics (not in ANSI)
      db   '-'	    ; C1h  graphics (not in ANSI)
      db   '-'	    ; C2h  graphics (not in ANSI)
      db   '+'	    ; C3h  graphics (not in ANSI)
      db   '-'	    ; C4h  graphics (not in ANSI)
      db   '+'	    ; C5h  graphics (not in ANSI)
      db   0e3h	    ; C6h  a tilde ---
      db   0c3h	    ; C7h  A tilde ---
      db   '+'	    ; C8h  graphics (not in ANSI)
      db   '+'	    ; C9h  graphics (not in ANSI)
      db   '-'	    ; CAh  graphics (not in ANSI)
      db   '-'	    ; CBh  graphics (not in ANSI)
      db   0a6h	    ; CCh  graphics (not in ANSI)
      db   '-'	    ; CDh  graphics (not in ANSI)
      db   '+'	    ; CEh  graphics (not in ANSI)
      db   0a4h	    ; CFh  Sunburst currency sign ---

      db   0f0h	    ; D0h  Icelandic eth lc (slashed d) ---
      db   0d0h	    ; D1h  Icelandic eth uc (slashed D) ---
      db   0cah	    ; D2h  E circumflex ---
      db   0cbh	    ; D3h  E umlaut ---
      db   0c8h	    ; D4h  E grave ---
      db   'i'	    ; D5h  undotted lc 'i' (Turkish?) --- not in ANSI
      db   0cdh	    ; D6h  I acute ---
      db   0ceh	    ; D7h  I circumflex ---
      db   0cfh	    ; D8h  I umlaut ---
      db   '+'	    ; D9h  graphics (not in ANSI)
      db   '+'	    ; DAh  graphics (not in ANSI)

      db   '_'      ; DBh  graphics (not in ANSI)
      db   '_'      ; DCh  graphics (not in ANSI)
      db   0a6h     ; DDh  vertical bar ---
      db   0cch     ; DEh  I grave ---
      db   '_'      ; DFh  end of semigraphic characters

      db   0d3h	    ; E0h  O acute ---
      db   0DFh	    ; E1h  german sharp S
      db   0d4h	    ; E2h  O circumflex ---
      db   0d2h	    ; E3h  O grave ---
      db   0f5h	    ; E4h  o tilde ---
      db   0d5h	    ; E5h  O tilde ---
      db   0B5h	    ; E6h  mu
      db   0feh	    ; E7h  icelandic thorn lc ---
      db   0deh	    ; E8h  icelandic thorn uc ---
      db   0dah	    ; E9h  U acute ---
      db   0dbh	    ; EAh  U circumflex ---
      db   0d9h	    ; EBh  U grave ---
      db   0fdh	    ; ECh  y acute ---
      db   0ddh	    ; EDh  Y acute ---
      db   0afh	    ; EEh  overscore ---
      db   0b4h	    ; EFh  acute deadkey ---

      db   0adh	    ; F0h  hyphen ---
      db   0B1h	    ; F1h  + underlined (plus or minus)
      db   '_'	    ; F2h  Double underscore --- (not in ANSI)
      db   0beh	    ; F3h  3/4 ---
      db   0b6h	    ; F4h  paragraph sign ---
      db   0a7h	    ; F5h  section sign ---
      db   0f7h	    ; F6h  math divide
      db   0b8h	    ; F7h  cedilla (deadkey) ---
      db   0B0h	    ; F8h  degree
      db   0A8h	    ; F9h  umlaut (deadkey) ---
      db   0B7h	    ; FAh  period accent (raised dot)
      db   0b9h	    ; FBh  1 superscript ---
      db   0b3h	    ; FCh  3 superscript ---
      db   0B2h	    ; FDh  2 superscript
      db   05Fh	    ; FEh  black box (or umlaut)
      db   ' '	    ; FFh  blank

