; 'patch_it.inc' keyboard table patch file based on:
;  'it1.wk2', 'it2.wk2', 'it1.wk2', 'it4.wk2', 'it5.wk2', 'it6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < Italian 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	11,	022H	; Count, starting offset
	DB	073H,	02H,	075H,	02H,	087H,	02H
	DB	08aH,	02H,	096H,	02H,	08bH

	DW	5,	03bH	; Count, starting offset
	DB	0bdH,	08H,	09H,	051H,	05aH

	DW	2,	048H	; Count, starting offset
	DB	0ddH,	0baH

	DW	6,	055H	; Count, starting offset
	DB	04dH,	0c0H,	0deH,	010H,	0dcH,	057H

	DW	4,	060H	; Count, starting offset
	DB	0bcH,	0beH,	0bfH,	0dfH

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	38,	0158H	; Count, starting offset
	DB	0e0H,	030H,	0ffH,	031H,	0a3H,	031H
	DB	0ffH,	032H,	0e9H,	032H,	0ffH,	033H
	DB	022H,	033H,	0ffH,	034H,	027H,	034H
	DB	0ffH,	035H,	028H,	035H,	0ffH,	036H
	DB	05fH,	036H,	09eH,	037H,	0e8H,	037H
	DB	0ffH,	038H,	05eH,	038H,	0ffH,	039H
	DB	0e7H,	039H

	DW	46,	0224H	; Count, starting offset
	DB	024H,	026H,	01dH,	0bfH,	03aH,	02fH
	DB	0ffH,	0c0H,	0f9H,	025H,	0ffH,	0dbH
	DB	029H,	0b0H,	01fH,	0dcH,	03cH,	03eH
	DB	01cH,	0ddH,	0ecH,	03dH,	01bH,	0deH
	DB	02aH,	0a7H,	0ffH,	0dfH,	0f2H,	021H
	DB	0ffH,	0bbH,	0ffH,	0ffH,	0ffH,	0bcH
	DB	02cH,	03fH,	0ffH,	0bdH,	02dH,	02bH
	DB	01fH,	0beH,	03bH,	02eH

	DW	32,	025bH	; Count, starting offset
	DB	031H,	00H,	032H,	00H,	033H,	00H
	DB	034H,	00H,	035H,	00H,	036H,	00H
	DB	037H,	00H,	038H,	00H,	039H,	00H
	DB	030H,	00H,	0dbH,	00H,	0bdH,	00H
	DB	0ffH,	0ffH,	033H,	023H,	0ffH,	0bdH
	DB	07eH,	0ffH

	DW	27,	027bH	; Count, starting offset
	DB	0ddH,	040H,	060H,	0baH,	05bH,	07bH
	DB	0deH,	05dH,	07dH,	0dcH,	05cH,	07cH
	DB	0ffH,	0ffH,	0ffH,	00H,	020H,	049H
	DB	074H,	061H,	06cH,	069H,	061H,	06eH
	DB	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Italian 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	7,	026H	; Count, starting offset
	DB	069H,	02H,	06cH,	02H,	078H,	02H
	DB	06dH

	DW	6,	0228H	; Count, starting offset
	DB	0f9H,	0a7H,	01cH,	0c0H,	0f2H,	0e7H

	DW	2,	0234H	; Count, starting offset
	DB	05cH,	07cH

	DW	1,	023dH	; Count, starting offset
	DB	0b0H

	DW	3,	0254H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	25,	025fH	; Count, starting offset
	DB	07bH,	0bbH,	05dH,	07dH,	0c0H,	040H
	DB	0ffH,	0deH,	023H,	0ffH,	0ffH,	0ffH
	DB	0ffH,	00H,	020H,	049H,	074H,	061H
	DB	06cH,	069H,	061H,	06eH,	020H,	034H
	DB	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Italian 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	7,	026H	; Count, starting offset
	DB	093H,	02H,	096H,	02H,	0a2H,	02H
	DB	097H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	30,	025dH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H,	0ffH,	05eH
	DB	038H,	0ffH,	07bH,	039H,	0ffH,	07dH
	DB	030H,	0ffH,	0ffH,	0bbH,	0ffH,	05dH
	DB	0dbH,	0ffH,	0ffH,	0ddH,	0ffH,	0ffH

	DW	39,	027bH	; Count, starting offset
	DB	0baH,	0ffH,	05bH,	0c0H,	0ffH,	0ffH
	DB	0deH,	0ffH,	0acH,	0dcH,	0ffH,	07eH
	DB	0bfH,	05cH,	07cH,	0bcH,	0ffH,	0ffH
	DB	0beH,	0ffH,	0ffH,	0bdH,	0ffH,	0ffH
	DB	0ffH,	0ffH,	0ffH,	00H,	020H,	049H
	DB	074H,	061H,	06cH,	069H,	061H,	06eH
	DB	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Italian 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	7,	026H	; Count, starting offset
	DB	093H,	02H,	096H,	02H,	0a2H,	02H
	DB	097H

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

	DW	30,	025dH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H,	0ffH,	05eH
	DB	038H,	0ffH,	07bH,	039H,	0ffH,	07dH
	DB	030H,	0ffH,	0ffH,	0bbH,	0ffH,	05dH
	DB	0dbH,	0ffH,	0ffH,	0ddH,	0ffH,	0ffH

	DW	39,	027bH	; Count, starting offset
	DB	0baH,	0ffH,	05bH,	0c0H,	0ffH,	040H
	DB	0deH,	0ffH,	023H,	0dcH,	0ffH,	0acH
	DB	0bfH,	05cH,	07cH,	0bcH,	0ffH,	0ffH
	DB	0beH,	0ffH,	0ffH,	0bdH,	0ffH,	0ffH
	DB	0ffH,	0ffH,	0ffH,	00H,	020H,	049H
	DB	074H,	061H,	06cH,	069H,	061H,	06eH
	DB	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 238
	DW	(offset PatchBase) + 318
	DW	(offset PatchBase) + 443
	DW	-1

