; 'patch_po.inc' keyboard table patch file based on:
;  'po1.wk2', 'po2.wk2', 'po1.wk2', 'po4.wk2', 'po1.wk2', 'po1.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < Portugese 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	1,	02f4H	; Count, starting offset
	DB	032H

	DW	0		; Zero count terminates patch list.


; Patches for < Portugese 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	11,	022H	; Count, starting offset
	DB	0bdH,	02H,	0bfH,	02H,	0daH,	02H
	DB	0ddH,	02H,	0faH,	02H,	0edH

	DW	5,	0226H	; Count, starting offset
	DB	0ffH,	0bfH,	07eH,	05eH,	0ffH

	DW	2,	0234H	; Count, starting offset
	DB	05cH,	07cH

	DW	1,	0246H	; Count, starting offset
	DB	0ffH

	DW	2,	0254H	; Count, starting offset
	DB	03cH,	03eH

	DW	57,	02bdH	; Count, starting offset
	DB	0ffH,	0ffH,	032H,	040H,	0ffH,	033H
	DB	0a3H,	0ffH,	034H,	0a7H,	0ffH,	037H
	DB	07bH,	0ffH,	038H,	05bH,	0ffH,	039H
	DB	05dH,	0ffH,	030H,	07dH,	0ffH,	0bbH
	DB	05fH,	0ffH,	0baH,	05dH,	0ffH,	0ffH
	DB	0ffH,	0ffH,	0baH,	01H,	0b4H,	0baH
	DB	02H,	060H,	0bfH,	02H,	05eH,	0bbH
	DB	040H,	0a8H,	0bfH,	01H,	07eH,	00H
	DB	020H,	050H,	06fH,	072H,	074H,	075H
	DB	067H,	065H,	073H

	DW	4,	02f6H	; Count, starting offset
	DB	065H,	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Portugese 1>

	DW	1,	09H	; Count, starting offset
	DB	01H

	DW	0		; Zero count terminates patch list.


; Patches for < Portugese 1>

	DW	1,	09H	; Count, starting offset
	DB	01H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 36
	DW	(offset PatchBase) + 158
	DW	(offset PatchBase) + 165
	DW	-1

