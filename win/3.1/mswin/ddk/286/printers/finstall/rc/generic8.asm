; Generic8.asm
; Copyright (c) 1989-1990, Microsoft Corporation.

; History
; 06 nov 89	peterbe		Added copyright

DATA SEGMENT

	DB	080h,	00h	; 080h
	DB	081h,	00h	; 081h
	DB	082h,	00h	; 082h
	DB	083h,	00h	; 083h
	DB	084h,	00h	; 084h
	DB	085h,	00h	; 085h
	DB	086h,	00h	; 086h
	DB	087h,	00h	; 087h
	DB	088h,	00h	; 088h
	DB	089h,	00h	; 089h
	DB	08ah,	00h	; 08ah
	DB	08bh,	00h	; 08bh
	DB	08ch,	00h	; 08ch
	DB	08dh,	00h	; 08dh
	DB	08eh,	00h	; 08eh
	DB	08fh,	00h	; 08fh
	DB	090h,	00h	; 090h
	DB	091h,	00h	; 091h
	DB	092h,	00h	; 092h
	DB	093h,	00h	; 093h
	DB	094h,	00h	; 094h
	DB	095h,	00h	; 095h
	DB	096h,	00h	; 096h
	DB	097h,	00h	; 097h
	DB	098h,	00h	; 098h
	DB	099h,	00h	; 099h
	DB	09ah,	00h	; 09ah
	DB	09bh,	00h	; 09bh
	DB	09ch,	00h	; 09ch
	DB	09dh,	00h	; 09dh
	DB	09eh,	00h	; 09eh
	DB	09fh,	00h	; 09fh
	DB	0a0h,	00h	; 0a0h
	DB	0a1h,	00h	; 0a1h
	DB	0a2h,	00h	; 0a2h
	DB	0a3h,	00h	; 0a3h
	DB	0a4h,	00h	; 0a4h
	DB	0a5h,	00h	; 0a5h
	DB	0a6h,	00h	; 0a6h
	DB	0a7h,	00h	; 0a7h
	DB	0a8h,	00h	; 0a8h
	DB	0a9h,	00h	; 0a9h
	DB	0aah,	00h	; 0aah
	DB	0abh,	00h	; 0abh
	DB	0ach,	00h	; 0ach
	DB	0adh,	00h	; 0adh
	DB	0aeh,	00h	; 0aeh
	DB	0afh,	00h	; 0afh
	DB	0b0h,	00h	; 0b0h
	DB	0b1h,	00h	; 0b1h
	DB	0b2h,	00h	; 0b2h
	DB	0b3h,	00h	; 0b3h
	DB	0b4h,	00h	; 0b4h
	DB	0b5h,	00h	; 0b5h
	DB	0b6h,	00h	; 0b6h
	DB	0b7h,	00h	; 0b7h
	DB	0b8h,	00h	; 0b8h
	DB	0b9h,	00h	; 0b9h
	DB	0bah,	00h	; 0bah
	DB	0bbh,	00h	; 0bbh
	DB	0bch,	00h	; 0bch
	DB	0bdh,	00h	; 0bdh
	DB	0beh,	00h	; 0beh
	DB	0bfh,	00h	; 0bfh
	DB	0c0h,	00h	; 0c0h
	DB	0c1h,	00h	; 0c1h
	DB	0c2h,	00h	; 0c2h
	DB	0c3h,	00h	; 0c3h
	DB	0c4h,	00h	; 0c4h
	DB	0c5h,	00h	; 0c5h
	DB	0c6h,	00h	; 0c6h
	DB	0c7h,	00h	; 0c7h
	DB	0c8h,	00h	; 0c8h
	DB	0c9h,	00h	; 0c9h
	DB	0cah,	00h	; 0cah
	DB	0cbh,	00h	; 0cbh
	DB	0cch,	00h	; 0cch
	DB	0cdh,	00h	; 0cdh
	DB	0ceh,	00h	; 0ceh
	DB	0cfh,	00h	; 0cfh
	DB	0d0h,	00h	; 0d0h
	DB	0d1h,	00h	; 0d1h
	DB	0d2h,	00h	; 0d2h
	DB	0d3h,	00h	; 0d3h
	DB	0d4h,	00h	; 0d4h
	DB	0d5h,	00h	; 0d5h
	DB	0d6h,	00h	; 0d6h
	DB	0d7h,	00h	; 0d7h
	DB	0d8h,	00h	; 0d8h
	DB	0d9h,	00h	; 0d9h
	DB	0dah,	00h	; 0dah
	DB	0dbh,	00h	; 0dbh
	DB	0dch,	00h	; 0dch
	DB	0ddh,	00h	; 0ddh
	DB	0deh,	00h	; 0deh
	DB	0dfh,	00h	; 0dfh
	DB	0e0h,	00h	; 0e0h
	DB	0e1h,	00h	; 0e1h
	DB	0e2h,	00h	; 0e2h
	DB	0e3h,	00h	; 0e3h
	DB	0e4h,	00h	; 0e4h
	DB	0e5h,	00h	; 0e5h
	DB	0e6h,	00h	; 0e6h
	DB	0e7h,	00h	; 0e7h
	DB	0e8h,	00h	; 0e8h
	DB	0e9h,	00h	; 0e9h
	DB	0eah,	00h	; 0eah
	DB	0ebh,	00h	; 0ebh
	DB	0ech,	00h	; 0ech
	DB	0edh,	00h	; 0edh
	DB	0eeh,	00h	; 0eeh
	DB	0efh,	00h	; 0efh
	DB	0f0h,	00h	; 0f0h
	DB	0f1h,	00h	; 0f1h
	DB	0f2h,	00h	; 0f2h
	DB	0f3h,	00h	; 0f3h
	DB	0f4h,	00h	; 0f4h
	DB	0f5h,	00h	; 0f5h
	DB	0f6h,	00h	; 0f6h
	DB	0f7h,	00h	; 0f7h
	DB	0f8h,	00h	; 0f8h
	DB	0f9h,	00h	; 0f9h
	DB	0fah,	00h	; 0fah
	DB	0fbh,	00h	; 0fbh
	DB	0fch,	00h	; 0fch
	DB	0fdh,	00h	; 0fdh
	DB	0feh,	00h	; 0feh
	DB	0ffh,	00h	; 0ffh
DATA ENDS
    END
