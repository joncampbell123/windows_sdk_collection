; Deskjet8.asm
; Copyright (c) 1989-1990, Microsoft Corporation.

; History
; 06 nov 89	peterbe		Added copyright

DATA SEGMENT

	DB	0ffh,	00h	; 080h
	DB	0ffh,	00h	; 081h
	DB	0ffh,	00h	; 082h
	DB	0ffh,	00h	; 083h
	DB	0ffh,	00h	; 084h
	DB	0ffh,	00h	; 085h
	DB	0ffh,	00h	; 086h
	DB	0ffh,	00h	; 087h
	DB	0ffh,	00h	; 088h
	DB	0ffh,	00h	; 089h
	DB	0ffh,	00h	; 08ah
	DB	0ffh,	00h	; 08bh
	DB	0ffh,	00h	; 08ch
	DB	0ffh,	00h	; 08dh
	DB	0ffh,	00h	; 08eh
	DB	0ffh,	00h	; 08fh
	DB	0ffh,	00h	; 090h
	DB	0ffh,	00h	; 091h
	DB	0ffh,	00h	; 092h
	DB	0ffh,	00h	; 093h
	DB	0ffh,	00h	; 094h
	DB	0ffh,	00h	; 095h
	DB	0ffh,	00h	; 096h
	DB	0ffh,	00h	; 097h
	DB	0ffh,	00h	; 098h
	DB	0ffh,	00h	; 099h
	DB	0ffh,	00h	; 09ah
	DB	0ffh,	00h	; 09bh
	DB	0ffh,	00h	; 09ch
	DB	0ffh,	00h	; 09dh
	DB	0ffh,	00h	; 09eh
	DB	0ffh,	00h	; 09fh
	DB	00h,	00h	; 0a0h
	DB	01h,	00h	; 0a1h
	DB	02h,	00h	; 0a2h
	DB	03h,	00h	; 0a3h
	DB	04h,	00h	; 0a4h
	DB	05h,	00h	; 0a5h
	DB	06h,	00h	; 0a6h
	DB	07h,	00h	; 0a7h
	DB	08h,	00h	; 0a8h
	DB	09h,	00h	; 0a9h
	DB	0ah,	00h	; 0aah
	DB	0bh,	00h	; 0abh
	DB	0ch,	00h	; 0ach
	DB	0dh,	00h	; 0adh
	DB	0eh,	00h	; 0aeh
	DB	0fh,	00h	; 0afh
	DB	010h,	00h	; 0b0h
	DB	011h,	00h	; 0b1h
	DB	012h,	00h	; 0b2h
	DB	013h,	00h	; 0b3h
	DB	014h,	00h	; 0b4h
	DB	015h,	00h	; 0b5h
	DB	016h,	00h	; 0b6h
	DB	017h,	00h	; 0b7h
	DB	018h,	00h	; 0b8h
	DB	019h,	00h	; 0b9h
	DB	01ah,	00h	; 0bah
	DB	01bh,	00h	; 0bbh
	DB	01ch,	00h	; 0bch
	DB	01dh,	00h	; 0bdh
	DB	01eh,	00h	; 0beh
	DB	01fh,	00h	; 0bfh
	DB	020h,	00h	; 0c0h
	DB	'!',	00h	; 0c1h
	DB	'"',	00h	; 0c2h
	DB	'#',	00h	; 0c3h
	DB	'$',	00h	; 0c4h
	DB	'%',	00h	; 0c5h
	DB	'&',	00h	; 0c6h
	DB	027h,	00h	; 0c7h
	DB	'(',	00h	; 0c8h
	DB	')',	00h	; 0c9h
	DB	'*',	00h	; 0cah
	DB	'+',	00h	; 0cbh
	DB	',',	00h	; 0cch
	DB	'-',	00h	; 0cdh
	DB	'.',	00h	; 0ceh
	DB	'/',	00h	; 0cfh
	DB	'0',	00h	; 0d0h
	DB	'1',	00h	; 0d1h
	DB	'2',	00h	; 0d2h
	DB	'3',	00h	; 0d3h
	DB	'4',	00h	; 0d4h
	DB	'5',	00h	; 0d5h
	DB	'6',	00h	; 0d6h
	DB	'7',	00h	; 0d7h
	DB	'8',	00h	; 0d8h
	DB	'9',	00h	; 0d9h
	DB	':',	00h	; 0dah
	DB	';',	00h	; 0dbh
	DB	'<',	00h	; 0dch
	DB	'=',	00h	; 0ddh
	DB	'>',	00h	; 0deh
	DB	'?',	00h	; 0dfh
	DB	'@',	00h	; 0e0h
	DB	'A',	00h	; 0e1h
	DB	'B',	00h	; 0e2h
	DB	'C',	00h	; 0e3h
	DB	'D',	00h	; 0e4h
	DB	'E',	00h	; 0e5h
	DB	'F',	00h	; 0e6h
	DB	'G',	00h	; 0e7h
	DB	'H',	00h	; 0e8h
	DB	'I',	00h	; 0e9h
	DB	'J',	00h	; 0eah
	DB	'K',	00h	; 0ebh
	DB	'L',	00h	; 0ech
	DB	'M',	00h	; 0edh
	DB	'N',	00h	; 0eeh
	DB	'O',	00h	; 0efh
	DB	'P',	00h	; 0f0h
	DB	'Q',	00h	; 0f1h
	DB	'R',	00h	; 0f2h
	DB	'S',	00h	; 0f3h
	DB	'T',	00h	; 0f4h
	DB	'U',	00h	; 0f5h
	DB	'V',	00h	; 0f6h
	DB	'W',	00h	; 0f7h
	DB	'X',	00h	; 0f8h
	DB	'Y',	00h	; 0f9h
	DB	'Z',	00h	; 0fah
	DB	'[',	00h	; 0fbh
	DB	'\',	00h	; 0fch
	DB	']',	00h	; 0fdh
	DB	'^',	00h	; 0feh
	DB	'_',	00h	; 0ffh
	DB	0ffh,	00h	; 0100h
	DB	0ffh,	00h	; 0101h
	DB	0ffh,	00h	; 0102h
	DB	0ffh,	00h	; 0103h
	DB	0ffh,	00h	; 0104h
	DB	0ffh,	00h	; 0105h
	DB	0ffh,	00h	; 0106h
	DB	0ffh,	00h	; 0107h
	DB	0ffh,	00h	; 0108h
	DB	0ffh,	00h	; 0109h
	DB	0ffh,	00h	; 010ah
	DB	0ffh,	00h	; 010bh
	DB	0ffh,	00h	; 010ch
	DB	0ffh,	00h	; 010dh
	DB	0ffh,	00h	; 010eh
	DB	0ffh,	00h	; 010fh
	DB	0ffh,	00h	; 0110h
	DB	'@',	00h	; 0111h
	DB	07h,	00h	; 0112h
	DB	0ffh,	00h	; 0113h
	DB	0ffh,	00h	; 0114h
	DB	0ffh,	00h	; 0115h
	DB	0ffh,	00h	; 0116h
	DB	0ffh,	00h	; 0117h
	DB	0ffh,	00h	; 0118h
	DB	0ffh,	00h	; 0119h
	DB	0ffh,	00h	; 011ah
	DB	0ffh,	00h	; 011bh
	DB	0ffh,	00h	; 011ch
	DB	0ffh,	00h	; 011dh
	DB	0ffh,	00h	; 011eh
	DB	0ffh,	00h	; 011fh
	DB	00h,	00h	; 0120h
	DB	'h',	00h	; 0121h
	DB	'o',	00h	; 0122h
	DB	'k',	00h	; 0123h
	DB	'j',	00h	; 0124h
	DB	'l',	00h	; 0125h
	DB	0aah,	00h	; 0126h
	DB	'm',	00h	; 0127h
	DB	'c',	00h	; 0128h
	DB	0f9h,	00h	; 0129h
	DB	083h,	00h	; 012ah
	DB	085h,	00h	; 012bh
	DB	0adh,	00h	; 012ch
	DB	0dh,	00h	; 012dh
	DB	0fah,	00h	; 012eh
	DB	'f',	00h	; 012fh
	DB	'g',	00h	; 0130h
	DB	088h,	00h	; 0131h
	DB	0f2h,	00h	; 0132h
	DB	0f7h,	00h	; 0133h
	DB	'`',	00h	; 0134h
	DB	'}',	00h	; 0135h
	DB	'~',	00h	; 0136h
	DB	'|',	00h	; 0137h
	DB	089h,	00h	; 0138h
	DB	0fbh,	00h	; 0139h
	DB	084h,	00h	; 013ah
	DB	087h,	00h	; 013bh
	DB	081h,	00h	; 013ch
	DB	082h,	00h	; 013dh
	DB	07fh,	00h	; 013eh
	DB	'i',	00h	; 013fh
	DB	015h,	'a'	; 0140h
	DB	'!',	'`'	; 0141h
	DB	'!',	'b'	; 0142h
	DB	'!',	'd'	; 0143h
	DB	'!',	'c'	; 0144h
	DB	'!',	'g'	; 0145h
	DB	'q',	00h	; 0146h
	DB	'#',	089h	; 0147h
	DB	'%',	'a'	; 0148h
	DB	'%',	'`'	; 0149h
	DB	'%',	'b'	; 014ah
	DB	'%',	'c'	; 014bh
	DB	')',	'a'	; 014ch
	DB	')',	'`'	; 014dh
	DB	')',	'b'	; 014eh
	DB	')',	'c'	; 014fh
	DB	'v',	00h	; 0150h
	DB	'.',	'd'	; 0151h
	DB	'/',	'a'	; 0152h
	DB	'/',	'`'	; 0153h
	DB	'/',	'b'	; 0154h
	DB	'/',	'd'	; 0155h
	DB	'/',	'c'	; 0156h
	DB	'e',	00h	; 0157h
	DB	'p',	00h	; 0158h
	DB	'5',	'a'	; 0159h
	DB	'5',	'`'	; 015ah
	DB	'5',	'b'	; 015bh
	DB	'5',	'c'	; 015ch
	DB	'9',	'`'	; 015dh
	DB	'z',	00h	; 015eh
	DB	'u',	00h	; 015fh
	DB	'A',	08bh	; 0160h
	DB	'A',	08ah	; 0161h
	DB	'A',	08ch	; 0162h
	DB	'A',	08eh	; 0163h
	DB	'A',	08dh	; 0164h
	DB	'r',	00h	; 0165h
	DB	't',	00h	; 0166h
	DB	'C',	089h	; 0167h
	DB	'E',	08bh	; 0168h
	DB	'E',	08ah	; 0169h
	DB	'E',	08ch	; 016ah
	DB	'E',	08dh	; 016bh
	DB	08fh,	08bh	; 016ch
	DB	08fh,	08ah	; 016dh
	DB	08fh,	08ch	; 016eh
	DB	08fh,	08dh	; 016fh
	DB	'w',	00h	; 0170h
	DB	'N',	08eh	; 0171h
	DB	'O',	08bh	; 0172h
	DB	'O',	08ah	; 0173h
	DB	'O',	08ch	; 0174h
	DB	'O',	08eh	; 0175h
	DB	'O',	08dh	; 0176h
	DB	0edh,	00h	; 0177h
	DB	's',	00h	; 0178h
	DB	'U',	08bh	; 0179h
	DB	'U',	08ah	; 017ah
	DB	'U',	08ch	; 017bh
	DB	'U',	08dh	; 017ch
	DB	'Y',	08ah	; 017dh
	DB	'{',	00h	; 017eh
	DB	'Y',	08dh	; 017fh
DATA ENDS
    END
