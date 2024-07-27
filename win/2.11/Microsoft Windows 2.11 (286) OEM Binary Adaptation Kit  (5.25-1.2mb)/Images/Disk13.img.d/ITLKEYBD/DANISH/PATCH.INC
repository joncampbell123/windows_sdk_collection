; 'patch_da.inc' keyboard table patch file based on:
;  'da1.wk2', 'da2.wk2', 'da1.wk2', 'da4.wk2', 'da5.wk2', 'da6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < Danish 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	12,	022H	; Count, starting offset
	DB	0c1H,	02H,	0c3H,	02H,	0dbH,	02H
	DB	0deH,	02H,	0f5H,	02H,	0ebH,	02H

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	5,	0165H	; Count, starting offset
	DB	0a3H,	0ffH,	034H,	034H,	024H

	DW	56,	02bdH	; Count, starting offset
	DB	0c0H,	00H,	0deH,	00H,	0ffH,	0ffH
	DB	033H,	023H,	0ffH,	034H,	0a4H,	0ffH
	DB	037H,	027H,	0ffH,	0dbH,	05eH,	07eH
	DB	0ddH,	040H,	060H,	0baH,	05bH,	07bH
	DB	0dcH,	05dH,	07dH,	0bfH,	05cH,	07cH
	DB	0ffH,	0ffH,	0ffH,	0dbH,	01H,	0b4H
	DB	0dbH,	02H,	060H,	0baH,	02H,	05eH
	DB	0baH,	01H,	0a8H,	00H,	020H,	044H
	DB	061H,	06eH,	069H,	073H,	068H,	020H
	DB	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Danish 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	12,	022H	; Count, starting offset
	DB	0c1H,	02H,	0c3H,	02H,	0e1H,	02H
	DB	0e4H,	02H,	0feH,	02H,	0f4H,	02H

	DW	2,	021cH	; Count, starting offset
	DB	02cH,	02cH

	DW	3,	0228H	; Count, starting offset
	DB	027H,	02aH,	0ffH

	DW	3,	0234H	; Count, starting offset
	DB	0bdH,	0a7H,	01cH

	DW	3,	0254H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	65,	02bdH	; Count, starting offset
	DB	0c0H,	00H,	0deH,	00H,	0ffH,	0ffH
	DB	032H,	040H,	0ffH,	033H,	0a3H,	0ffH
	DB	034H,	024H,	0ffH,	037H,	07bH,	0ffH
	DB	038H,	05bH,	0ffH,	039H,	05dH,	0ffH
	DB	030H,	07dH,	0ffH,	0baH,	07eH,	0ffH
	DB	04dH,	0b5H,	0ffH,	0e2H,	05cH,	0ffH
	DB	0ffH,	0ffH,	0ffH,	0dbH,	01H,	0b4H
	DB	0dbH,	02H,	060H,	0baH,	02H,	05eH
	DB	0baH,	01H,	0a8H,	0baH,	040H,	07eH
	DB	00H,	020H,	044H,	061H,	06eH,	069H
	DB	073H,	068H,	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Danish 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	0169H	; Count, starting offset
	DB	024H

	DW	1,	0224H	; Count, starting offset
	DB	0a8H

	DW	40,	02c6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	0a4H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0ffH,	0deH
	DB	0ffH,	0acH,	0dcH,	0ffH

	DW	8,	02f3H	; Count, starting offset
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH

	DW	1,	0313H	; Count, starting offset
	DB	035H

	DW	0		; Zero count terminates patch list.


; Patches for < Danish 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	44,	073H	; Count, starting offset
	DB	0e9H,	0ffH,	024H,	026H,	0ebH,	0ffH
	DB	025H,	0ffH,	027H,	0ffH,	0edH,	028H
	DB	0f5H,	02dH,	02eH,	0eaH,	012H,	0ecH
	DB	02fH,	0ffH,	0eeH,	0cH,	0efH,	0f0H
	DB	0f1H,	0f2H,	02aH,	0f3H,	0f4H,	0ffH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	080H,	081H,	082H,	083H,	084H,	085H
	DB	086H,	087H

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	1,	0afH	; Count, starting offset
	DB	01bH

	DW	1,	0bdH	; Count, starting offset
	DB	09H

	DW	7,	0e1H	; Count, starting offset
	DB	0bcH,	0ffH,	0ffH,	0ffH,	06aH,	0ffH
	DB	020H

	DW	18,	0f3H	; Count, starting offset
	DB	090H,	091H,	024H,	026H,	021H,	06dH
	DB	025H,	0cH,	027H,	06bH,	023H,	028H
	DB	022H,	02dH,	02eH,	0ffH,	0ffH,	0f5H

	DW	1,	0126H	; Count, starting offset
	DB	0dH

	DW	1,	0169H	; Count, starting offset
	DB	024H

	DW	1,	0224H	; Count, starting offset
	DB	0a8H

	DW	40,	02c6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	0a4H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0c3H,	0deH
	DB	0ffH,	0acH,	0dcH,	0ffH

	DW	8,	02f3H	; Count, starting offset
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH

	DW	1,	0313H	; Count, starting offset
	DB	036H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 116
	DW	(offset PatchBase) + 240
	DW	(offset PatchBase) + 348
	DW	-1

