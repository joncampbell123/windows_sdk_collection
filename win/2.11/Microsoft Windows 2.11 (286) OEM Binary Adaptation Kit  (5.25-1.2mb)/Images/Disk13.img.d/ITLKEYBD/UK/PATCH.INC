; 'patch_uk.inc' keyboard table patch file based on:
;  'uk1.wk2', 'uk2.wk2', 'uk1.wk2', 'uk4.wk2', 'uk5.wk2', 'uk6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < UK 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	7,	026H	; Count, starting offset
	DB	060H,	02H,	063H,	02H,	06aH,	02H
	DB	064H

	DW	1,	03bH	; Count, starting offset
	DB	0dbH

	DW	2,	048H	; Count, starting offset
	DB	0ddH,	0baH

	DW	1,	055H	; Count, starting offset
	DB	0bbH

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	2,	0159H	; Count, starting offset
	DB	05fH,	09fH

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	027H,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	3,	0224H	; Count, starting offset
	DB	05bH,	07bH,	01dH

	DW	7,	022cH	; Count, starting offset
	DB	03aH,	02aH,	0ffH,	0dbH,	05eH,	07eH
	DB	0ffH

	DW	6,	0238H	; Count, starting offset
	DB	040H,	060H,	01bH,	0deH,	05dH,	07dH

	DW	1,	0244H	; Count, starting offset
	DB	03bH

	DW	2,	024dH	; Count, starting offset
	DB	03dH,	01fH

	DW	10,	025dH	; Count, starting offset
	DB	033H,	023H,	0ffH,	0ffH,	0ffH,	0ffH
	DB	00H,	020H,	055H,	04bH

	DW	3,	0267H	; Count, starting offset
	DB	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < UK 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	7,	026H	; Count, starting offset
	DB	060H,	02H,	063H,	02H,	06aH,	02H
	DB	064H

	DW	3,	057H	; Count, starting offset
	DB	0dfH,	010H,	0deH

	DW	1,	084H	; Count, starting offset
	DB	0dcH

	DW	3,	0233H	; Count, starting offset
	DB	0deH,	023H,	07eH

	DW	8,	023bH	; Count, starting offset
	DB	0dfH,	060H,	0acH,	0ffH,	0dcH,	05cH
	DB	07cH,	01cH

	DW	10,	025dH	; Count, starting offset
	DB	0dfH,	0a6H,	0ffH,	0ffH,	0ffH,	0ffH
	DB	00H,	020H,	055H,	04bH

	DW	3,	0267H	; Count, starting offset
	DB	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < UK 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	7,	026H	; Count, starting offset
	DB	093H,	02H,	096H,	02H,	09dH,	02H
	DB	097H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	022eH	; Count, starting offset
	DB	0ffH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	10,	025dH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H

	DW	54,	0267H	; Count, starting offset
	DB	0ffH,	05eH,	038H,	0ffH,	0ffH,	039H
	DB	0ffH,	07bH,	030H,	0ffH,	07dH,	0bbH
	DB	0ffH,	0ffH,	0dbH,	0ffH,	05bH,	0ddH
	DB	0ffH,	05dH,	0baH,	0ffH,	0ffH,	0c0H
	DB	0ffH,	0acH,	0deH,	0ffH,	07eH,	0dcH
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	0ffH,	00H
	DB	020H,	055H,	04bH,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < UK 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	7,	026H	; Count, starting offset
	DB	093H,	02H,	096H,	02H,	09dH,	02H
	DB	097H

	DW	1,	03bH	; Count, starting offset
	DB	0baH

	DW	1,	055H	; Count, starting offset
	DB	0bbH

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

	DW	1,	0159H	; Count, starting offset
	DB	023H

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	027H,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	2,	0224H	; Count, starting offset
	DB	05eH,	07eH

	DW	18,	022cH	; Count, starting offset
	DB	03aH,	02aH,	0ffH,	0dbH,	040H,	060H
	DB	01bH,	0dcH,	0a6H,	05fH,	01cH,	0ddH
	DB	05bH,	07bH,	01dH,	0deH,	05dH,	07dH

	DW	1,	0244H	; Count, starting offset
	DB	03bH

	DW	2,	024dH	; Count, starting offset
	DB	03dH,	01fH

	DW	2,	0254H	; Count, starting offset
	DB	05cH,	05bH

	DW	10,	025dH	; Count, starting offset
	DB	032H,	0ffH,	040H,	033H,	0ffH,	023H
	DB	034H,	0ffH,	024H,	036H

	DW	54,	0267H	; Count, starting offset
	DB	0ffH,	05eH,	038H,	0ffH,	0ffH,	039H
	DB	0ffH,	07bH,	030H,	0ffH,	07dH,	0bbH
	DB	0ffH,	0ffH,	0dbH,	0ffH,	0ffH,	0ddH
	DB	05bH,	05bH,	0baH,	0ffH,	07eH,	0c0H
	DB	0ffH,	0acH,	0deH,	05dH,	05dH,	0dcH
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	0ffH,	00H
	DB	020H,	055H,	04bH,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 141
	DW	(offset PatchBase) + 216
	DW	(offset PatchBase) + 346
	DW	-1

