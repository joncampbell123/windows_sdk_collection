; 'patch_sp.inc' keyboard table patch file based on:
;  'sp1.wk2', 'sp2.wk2', 'sp1.wk2', 'sp4.wk2', 'sp5.wk2', 'sp6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < Spanish 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	7,	026H	; Count, starting offset
	DB	0dcH,	02H,	0dfH,	02H,	0f7H,	02H
	DB	0ecH

	DW	2,	03aH	; Count, starting offset
	DB	0dbH,	0ddH

	DW	2,	048H	; Count, starting offset
	DB	0baH,	0bbH

	DW	5,	055H	; Count, starting offset
	DB	0c0H,	0deH,	0dcH,	010H,	0bfH

	DW	1,	063H	; Count, starting offset
	DB	0bdH

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	13,	0159H	; Count, starting offset
	DB	03dH,	0ffH,	031H,	031H,	021H,	0ffH
	DB	032H,	032H,	022H,	0ffH,	033H,	033H
	DB	0a3H

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	02fH,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	1,	0210H	; Count, starting offset
	DB	02aH

	DW	26,	0224H	; Count, starting offset
	DB	0e7H,	0c7H,	01bH,	0bfH,	03cH,	03eH
	DB	01cH,	0c0H,	0f1H,	0d1H,	0ffH,	0dbH
	DB	027H,	03fH,	01fH,	0dcH,	07eH,	0a7H
	DB	0ffH,	0ddH,	060H,	05eH,	0ffH,	0deH
	DB	0b0H,	0a1H

	DW	6,	0244H	; Count, starting offset
	DB	02bH,	02aH,	0ffH,	0bcH,	02cH,	03bH

	DW	1,	0251H	; Count, starting offset
	DB	03aH

	DW	43,	02bdH	; Count, starting offset
	DB	0c0H,	00H,	0ffH,	0ffH,	033H,	023H
	DB	0ffH,	037H,	027H,	0ffH,	0dbH,	05eH
	DB	0ffH,	0ddH,	0a8H,	0ffH,	050H,	060H
	DB	0ffH,	0baH,	040H,	0bfH,	0bbH,	05bH
	DB	07bH,	0dcH,	05dH,	07dH,	0bfH,	05cH
	DB	07cH,	0ffH,	0ffH,	0ffH,	0dbH,	01H
	DB	0b4H,	0ddH,	01H,	060H,	0ddH,	02H
	DB	05eH

	DW	15,	02e8H	; Count, starting offset
	DB	0ddH,	04H,	0a8H,	00H,	020H,	053H
	DB	070H,	061H,	06eH,	069H,	073H,	068H
	DB	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Spanish 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	7,	026H	; Count, starting offset
	DB	0dcH,	02H,	0dfH,	02H,	0f7H,	02H
	DB	0ecH

	DW	2,	03aH	; Count, starting offset
	DB	0dbH,	0ddH

	DW	2,	048H	; Count, starting offset
	DB	0baH,	0bbH

	DW	5,	055H	; Count, starting offset
	DB	0c0H,	0deH,	0dcH,	010H,	0bfH

	DW	1,	063H	; Count, starting offset
	DB	0bdH

	DW	13,	0159H	; Count, starting offset
	DB	03dH,	0ffH,	031H,	031H,	021H,	0ffH
	DB	032H,	032H,	022H,	0ffH,	033H,	033H
	DB	0b7H

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	02fH,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	1,	0210H	; Count, starting offset
	DB	02aH

	DW	26,	0224H	; Count, starting offset
	DB	060H,	05eH,	01bH,	0bfH,	0e7H,	0c7H
	DB	01cH,	0c0H,	0f1H,	0d1H,	0ffH,	0dbH
	DB	027H,	03fH,	0ffH,	0dcH,	0baH,	0aaH
	DB	0ffH,	0ddH,	0a1H,	0bfH,	0ffH,	0deH
	DB	027H,	0b7H

	DW	6,	0244H	; Count, starting offset
	DB	02bH,	02aH,	01dH,	0bcH,	02cH,	03bH

	DW	9,	024eH	; Count, starting offset
	DB	0ffH,	0beH,	02eH,	03aH,	0ffH,	0e2H
	DB	03cH,	03eH,	01cH

	DW	45,	02bbH	; Count, starting offset
	DB	0c0H,	00H,	0bfH,	00H,	0ffH,	0ffH
	DB	031H,	07cH,	0ffH,	032H,	040H,	0ffH
	DB	033H,	023H,	0ffH,	036H,	0acH,	0ffH
	DB	0baH,	05bH,	0ffH,	0bbH,	05dH,	0ffH
	DB	0deH,	07bH,	0ffH,	0dcH,	05cH,	0ffH
	DB	0bfH,	07dH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0deH,	01H,	0b4H,	0baH,	01H,	060H
	DB	0baH,	02H,	05eH

	DW	15,	02e8H	; Count, starting offset
	DB	0deH,	02H,	0b7H,	00H,	020H,	053H
	DB	070H,	061H,	06eH,	069H,	073H,	068H
	DB	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Spanish 5>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	8,	026H	; Count, starting offset
	DB	0f7H,	02H,	0faH,	02H,	012H,	03H
	DB	07H,	03H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	02b3H	; Count, starting offset
	DB	0a8H

	DW	38,	02c2H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	0ffH,	039H,	0ffH,	07bH,	030H
	DB	0ffH,	07dH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	05bH,	0ddH,	0ffH,	05dH,	0baH
	DB	0ffH,	0ffH,	0c0H,	0ffH,	0acH,	0deH
	DB	0ffH,	07eH

	DW	42,	02e8H	; Count, starting offset
	DB	0dcH,	05cH,	07cH,	0bfH,	0ffH,	0ffH
	DB	0bcH,	0ffH,	0ffH,	0beH,	0ffH,	0ffH
	DB	0bdH,	0ffH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0dbH,	01H,	0b4H,	0ddH,	01H,	060H
	DB	0ddH,	02H,	05eH,	0dbH,	02H,	0a8H
	DB	00H,	020H,	053H,	070H,	061H,	06eH
	DB	069H,	073H,	068H,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Spanish 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	8,	026H	; Count, starting offset
	DB	0f7H,	02H,	0faH,	02H,	012H,	03H
	DB	07H,	03H

	DW	2,	03aH	; Count, starting offset
	DB	0bfH,	0ddH

	DW	1,	049H	; Count, starting offset
	DB	0bbH

	DW	1,	063H	; Count, starting offset
	DB	0bdH

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

	DW	17,	0159H	; Count, starting offset
	DB	03dH,	0ffH,	031H,	031H,	0a6H,	0ffH
	DB	032H,	032H,	022H,	0ffH,	033H,	033H
	DB	040H,	0ffH,	034H,	034H,	070H

	DW	13,	0171H	; Count, starting offset
	DB	026H,	09eH,	037H,	037H,	02fH,	0ffH
	DB	038H,	038H,	028H,	0ffH,	039H,	039H
	DB	029H

	DW	9,	0229H	; Count, starting offset
	DB	03fH,	0ffH,	0c0H,	027H,	0b7H,	0ffH
	DB	0dbH,	0b0H,	060H

	DW	6,	0238H	; Count, starting offset
	DB	0a8H,	0bfH,	01dH,	0deH,	0a1H,	021H

	DW	6,	0244H	; Count, starting offset
	DB	02bH,	02aH,	0ffH,	0bcH,	02cH,	03bH

	DW	5,	0251H	; Count, starting offset
	DB	03aH,	0ffH,	0e2H,	0e7H,	0c7H

	DW	1,	02b3H	; Count, starting offset
	DB	0a8H

	DW	1,	02bdH	; Count, starting offset
	DB	0e2H

	DW	38,	02c2H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	0acH,	038H
	DB	0ffH,	0ffH,	039H,	0ffH,	07bH,	030H
	DB	0ffH,	07dH,	0bbH,	05dH,	05dH,	0dbH
	DB	05bH,	05eH,	0ddH,	0ffH,	0ffH,	0baH
	DB	0ffH,	0ffH,	0c0H,	07bH,	07bH,	0deH
	DB	07dH,	07dH

	DW	42,	02e8H	; Count, starting offset
	DB	0dcH,	05cH,	07cH,	0bfH,	0ffH,	0ffH
	DB	0bcH,	0ffH,	0ffH,	0beH,	0ffH,	0ffH
	DB	0bdH,	0ffH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0c0H,	01H,	0b4H,	0dbH,	02H,	060H
	DB	0dbH,	080H,	05eH,	0ddH,	01H,	0a8H
	DB	00H,	020H,	053H,	070H,	061H,	06eH
	DB	069H,	073H,	068H,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 218
	DW	(offset PatchBase) + 427
	DW	(offset PatchBase) + 569
	DW	-1

