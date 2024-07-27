; 'patch_ne.inc' keyboard table patch file based on:
;  'ne1.wk2', 'ne1.wk2', 'ne1.wk2', 'ne4.wk2', 'ne5.wk2', 'ne6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < USA 1>

	DW	1,	09H	; Count, starting offset
	DB	01H

	DW	0		; Zero count terminates patch list.


; Patches for < Netherlands 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	088H

	DW	8,	026H	; Count, starting offset
	DB	0f6H,	02H,	0f9H,	02H,	01bH,	03H
	DB	0cH,	03H

	DW	2,	03aH	; Count, starting offset
	DB	0dbH,	0bfH

	DW	2,	048H	; Count, starting offset
	DB	0ddH,	0baH

	DW	3,	055H	; Count, starting offset
	DB	0bbH,	0c0H,	0deH

	DW	1,	063H	; Count, starting offset
	DB	0bdH

	DW	1,	0159H	; Count, starting offset
	DB	027H

	DW	2,	0161H	; Count, starting offset
	DB	022H,	0ffH

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	05fH,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	26,	0224H	; Count, starting offset
	DB	02aH,	07cH,	0ffH,	0bfH,	0b0H,	07eH
	DB	01dH,	0c0H,	0b4H,	060H,	0ffH,	0dbH
	DB	02fH,	03fH,	01cH,	0dcH,	03cH,	03eH
	DB	0ffH,	0ddH,	0a8H,	05eH,	0ffH,	0deH
	DB	040H,	0a7H

	DW	19,	0244H	; Count, starting offset
	DB	02bH,	0b1H,	0ffH,	0bcH,	02cH,	03bH
	DB	0ffH,	0bdH,	02dH,	03dH,	01fH,	0beH
	DB	02eH,	03aH,	0ffH,	0e2H,	05dH,	05bH
	DB	01bH

	DW	1,	02b3H	; Count, starting offset
	DB	0b7H

	DW	11,	02bdH	; Count, starting offset
	DB	0deH,	0acH,	0ffH,	031H,	0b9H,	0ffH
	DB	032H,	0b2H,	0ffH,	033H,	0b3H

	DW	83,	02c8H	; Count, starting offset
	DB	0ffH,	034H,	0bcH,	0ffH,	035H,	0bdH
	DB	0ffH,	036H,	0beH,	0ffH,	037H,	0a3H
	DB	0ffH,	038H,	07bH,	0ffH,	039H,	07dH
	DB	0ffH,	0dbH,	05cH,	0ffH,	053H,	0dfH
	DB	0ffH,	0e2H,	0a6H,	0ffH,	05aH,	0abH
	DB	0ffH,	058H,	0bbH,	0ffH,	043H,	0a2H
	DB	0ffH,	04dH,	0b5H,	0ffH,	052H,	0b6H
	DB	0ffH,	0beH,	0b7H,	0ffH,	0ffH,	0ffH
	DB	0ffH,	0bfH,	02H,	07eH,	0bfH,	040H
	DB	0b8H,	0ddH,	01H,	0a8H,	0ddH,	02H
	DB	05eH,	0c0H,	01H,	0b4H,	0c0H,	02H
	DB	060H,	00H,	020H,	04eH,	065H,	074H
	DB	068H,	065H,	072H,	06cH,	061H,	06eH
	DB	064H,	073H,	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < USA 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	7,	026H	; Count, starting offset
	DB	0f3H,	02H,	0f6H,	02H,	0feH,	02H
	DB	0f7H

	DW	2,	056H	; Count, starting offset
	DB	0c0H,	0deH

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	0162H	; Count, starting offset
	DB	0ffH

	DW	1,	022bH	; Count, starting offset
	DB	0c0H

	DW	1,	023bH	; Count, starting offset
	DB	0deH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	11,	02bdH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H,	0ffH

	DW	54,	02c8H	; Count, starting offset
	DB	05eH,	038H,	0ffH,	0ffH,	039H,	0ffH
	DB	07bH,	030H,	0ffH,	07dH,	0bbH,	0ffH
	DB	0ffH,	0dbH,	0ffH,	05bH,	0ddH,	0ffH
	DB	05dH,	0baH,	0ffH,	0ffH,	0c0H,	0ffH
	DB	0acH,	0deH,	0ffH,	07eH,	0dcH,	05cH
	DB	07cH,	0bfH,	0ffH,	0ffH,	0bcH,	0ffH
	DB	0ffH,	0beH,	0ffH,	0ffH,	0bdH,	0ffH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	00H,	020H
	DB	055H,	053H,	041H,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < USA 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	7,	026H	; Count, starting offset
	DB	0f3H,	02H,	0f6H,	02H,	0feH,	02H
	DB	0f7H

	DW	2,	056H	; Count, starting offset
	DB	0c0H,	0deH

	DW	44,	073H	; Count, starting offset
	DB	0e9H,	0ffH,	024H,	026H,	0ebH,	0ffH
	DB	025H,	0ffH,	027H,	0ffH,	0edH,	028H
	DB	0f5H,	02dH,	02eH,	0eaH,	012H,	0ecH
	DB	02fH,	0ffH,	0eeH,	0cH,	0efH,	0f0H
	DB	0f1H,	0f2H,	02aH,	0f3H,	0f4H,	0e2H
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

	DW	1,	0162H	; Count, starting offset
	DB	0ffH

	DW	1,	022bH	; Count, starting offset
	DB	0c0H

	DW	13,	0231H	; Count, starting offset
	DB	05dH,	01bH,	0dcH,	03cH,	03eH,	0ffH
	DB	0ddH,	05cH,	07cH,	01cH,	0deH,	07bH
	DB	07dH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	3,	0254H	; Count, starting offset
	DB	060H,	07eH,	01dH

	DW	11,	02bdH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H,	0ffH

	DW	54,	02c8H	; Count, starting offset
	DB	05eH,	038H,	0ffH,	0ffH,	039H,	0ffH
	DB	07bH,	030H,	0ffH,	07dH,	0bbH,	0ffH
	DB	0ffH,	0dbH,	0ffH,	05bH,	0ddH,	0ffH
	DB	05dH,	0baH,	0ffH,	0ffH,	0c0H,	0ffH
	DB	0acH,	0deH,	0ffH,	07eH,	0dcH,	05cH
	DB	07cH,	0bfH,	0ffH,	0ffH,	0bcH,	0ffH
	DB	03cH,	0beH,	0ffH,	03eH,	0bdH,	0ffH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	00H,	020H
	DB	055H,	053H,	041H,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 7
	DW	(offset PatchBase) + 243
	DW	(offset PatchBase) + 390
	DW	-1

