; 'patch_ca.inc' keyboard table patch file based on:
;  'ca1.wk2', 'fr2.wk2', 'ca3.wk2', 'ca4.wk2', 'fr5.wk2', 'fr6.wk2'



PatchBase label byte

; Patches for < French 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	12,	022H	; Count, starting offset
	DB	0d3H,	02H,	0d5H,	02H,	0edH,	02H
	DB	0f0H,	02H,	01H,	03H,	0f7H,	02H

	DW	6,	03aH	; Count, starting offset
	DB	0dbH,	0bbH,	08H,	09H,	041H,	05aH

	DW	5,	048H	; Count, starting offset
	DB	0ddH,	0baH,	0dH,	011H,	051H

	DW	1,	055H	; Count, starting offset
	DB	04dH

	DW	1,	05aH	; Count, starting offset
	DB	057H

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
	DB	02dH,	036H,	09eH,	037H,	0e8H,	037H
	DB	0ffH,	038H,	05fH,	038H,	09fH,	039H
	DB	0e7H,	039H

	DW	30,	0224H	; Count, starting offset
	DB	024H,	02aH,	01dH,	0bfH,	03aH,	02fH
	DB	0ffH,	0c0H,	0f9H,	025H,	0ffH,	0dbH
	DB	029H,	0b0H,	0ffH,	0dcH,	03cH,	03eH
	DB	01cH,	0ddH,	05eH,	0a8H,	01bH,	0deH
	DB	060H,	026H,	0ffH,	0dfH,	021H,	0a7H

	DW	11,	0246H	; Count, starting offset
	DB	01fH,	0bcH,	02cH,	03fH,	0ffH,	0bdH
	DB	0ffH,	0ffH,	0ffH,	0beH,	03bH

	DW	1,	02b3H	; Count, starting offset
	DB	0b7H

	DW	70,	02bbH	; Count, starting offset
	DB	031H,	00H,	032H,	00H,	033H,	00H
	DB	034H,	00H,	035H,	00H,	036H,	00H
	DB	037H,	00H,	038H,	00H,	039H,	00H
	DB	030H,	00H,	0dbH,	00H,	0bbH,	00H
	DB	0ffH,	0ffH,	033H,	023H,	0ffH,	0dbH
	DB	05eH,	0ffH,	0bbH,	07eH,	0ffH,	0ddH
	DB	040H,	0ffH,	0baH,	05bH,	07bH,	0c0H
	DB	0b5H,	0ffH,	0deH,	05dH,	07dH,	0dcH
	DB	05cH,	07cH,	0ffH,	0ffH,	0ffH,	0ddH
	DB	01H,	05eH,	0ddH,	02H,	0a8H,	00H
	DB	020H,	046H,	072H,	065H,	06eH,	063H
	DB	068H,	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Canada_French 3>

	DW	1,	09H	; Count, starting offset
	DB	03H

	DW	4,	02f0H	; Count, starting offset
	DB	0baH,	07bH,	0ffH,	0c0H

	DW	1,	0308H	; Count, starting offset
	DB	04cH

	DW	1,	032dH	; Count, starting offset
	DB	033H

	DW	0		; Zero count terminates patch list.


; Patches for < Canada_French 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	7,	026H	; Count, starting offset
	DB	05H,	03H,	08H,	03H,	029H,	03H
	DB	018H

	DW	3,	0234H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	3,	023cH	; Count, starting offset
	DB	023H,	07cH,	0ffH

	DW	3,	0254H	; Count, starting offset
	DB	0abH,	0bbH,	01cH

	DW	2,	02bdH	; Count, starting offset
	DB	0deH,	05cH

	DW	5,	02f3H	; Count, starting offset
	DB	0dcH,	07dH,	0ffH,	0e2H,	0b0H

	DW	39,	0302H	; Count, starting offset
	DB	0baH,	07eH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0dbH,	03H,	05eH,	0ddH,	01H,	0b8H
	DB	0ddH,	02H,	0a8H,	0c0H,	03H,	060H
	DB	0bfH,	040H,	0b4H,	00H,	020H,	043H
	DB	061H,	06eH,	061H,	064H,	061H,	05fH
	DB	046H,	072H,	065H,	06eH,	063H,	068H
	DB	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < French 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	3,	022H	; Count, starting offset
	DB	0d3H,	02H,	0d5H

	DW	3,	02aH	; Count, starting offset
	DB	01fH,	03H,	015H

	DW	6,	03aH	; Count, starting offset
	DB	0dbH,	0bdH,	08H,	09H,	041H,	05aH

	DW	5,	048H	; Count, starting offset
	DB	0ddH,	0baH,	0dH,	011H,	051H

	DW	6,	055H	; Count, starting offset
	DB	04dH,	0c0H,	0deH,	010H,	0e2H,	057H

	DW	4,	060H	; Count, starting offset
	DB	0bcH,	0beH,	0bfH,	0bbH

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	38,	0158H	; Count, starting offset
	DB	0e0H,	030H,	0ffH,	031H,	026H,	031H
	DB	0ffH,	032H,	0e9H,	032H,	0ffH,	033H
	DB	022H,	033H,	0ffH,	034H,	027H,	034H
	DB	0ffH,	035H,	028H,	035H,	0ffH,	036H
	DB	0a7H,	036H,	09eH,	037H,	0e8H,	037H
	DB	0ffH,	038H,	021H,	038H,	0ffH,	039H
	DB	0e7H,	039H

	DW	30,	0224H	; Count, starting offset
	DB	024H,	02aH,	01dH,	0bfH,	03aH,	02fH
	DB	0ffH,	0c0H,	0f9H,	025H,	0ffH,	0dbH
	DB	029H,	0b0H,	0ffH,	0dcH,	023H,	0b5H
	DB	0ffH,	0ddH,	05eH,	0a8H,	01bH,	0deH
	DB	0b5H,	0a3H,	0ffH,	0dfH,	021H,	0a7H

	DW	1,	0249H	; Count, starting offset
	DB	03fH

	DW	9,	024eH	; Count, starting offset
	DB	09fH,	0beH,	03bH,	02eH,	0ffH,	0e2H
	DB	03cH,	03eH,	01cH

	DW	1,	02b3H	; Count, starting offset
	DB	0b7H

	DW	79,	02bbH	; Count, starting offset
	DB	031H,	00H,	032H,	00H,	033H,	00H
	DB	034H,	00H,	035H,	00H,	036H,	00H
	DB	037H,	00H,	038H,	00H,	039H,	00H
	DB	030H,	00H,	0dbH,	00H,	0bdH,	00H
	DB	0ffH,	0ffH,	032H,	0ffH,	040H,	033H
	DB	0ffH,	023H,	034H,	0ffH,	024H,	036H
	DB	0ffH,	05eH,	038H,	0ffH,	07bH,	039H
	DB	0ffH,	07dH,	030H,	0ffH,	0ffH,	0bbH
	DB	0ffH,	0ffH,	0dbH,	0ffH,	0ffH,	0ddH
	DB	0ffH,	05bH,	0baH,	0ffH,	05dH,	0c0H
	DB	0ffH,	0acH,	0deH,	0ffH,	07eH,	0e2H
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH

	DW	17,	030eH	; Count, starting offset
	DB	0ddH,	01H,	05eH,	0ddH,	02H,	0a8H
	DB	00H,	020H,	046H,	072H,	065H,	06eH
	DB	063H,	068H,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < French 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	3,	022H	; Count, starting offset
	DB	0d3H,	02H,	0d5H

	DW	3,	02aH	; Count, starting offset
	DB	01fH,	03H,	015H

	DW	6,	03aH	; Count, starting offset
	DB	0dbH,	0bdH,	08H,	09H,	041H,	05aH

	DW	5,	048H	; Count, starting offset
	DB	0ddH,	0baH,	0dH,	011H,	051H

	DW	1,	055H	; Count, starting offset
	DB	04dH

	DW	1,	05aH	; Count, starting offset
	DB	057H

	DW	4,	060H	; Count, starting offset
	DB	0bcH,	0beH,	0bfH,	0bbH

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

	DW	38,	0158H	; Count, starting offset
	DB	0e0H,	030H,	0ffH,	031H,	026H,	031H
	DB	0ffH,	032H,	0e9H,	032H,	0ffH,	033H
	DB	022H,	033H,	0ffH,	034H,	027H,	034H
	DB	0ffH,	035H,	028H,	035H,	0ffH,	036H
	DB	0a7H,	036H,	09eH,	037H,	0e8H,	037H
	DB	0ffH,	038H,	021H,	038H,	0ffH,	039H
	DB	0e7H,	039H

	DW	27,	0224H	; Count, starting offset
	DB	024H,	02aH,	01dH,	0bfH,	03aH,	02fH
	DB	0ffH,	0c0H,	0f9H,	025H,	0ffH,	0dbH
	DB	029H,	0b0H,	0ffH,	0dcH,	03cH,	03eH
	DB	01cH,	0ddH,	05eH,	0a8H,	01bH,	0deH
	DB	060H,	0a3H,	0ffH

	DW	1,	0249H	; Count, starting offset
	DB	03fH

	DW	1,	0250H	; Count, starting offset
	DB	03bH

	DW	1,	02b3H	; Count, starting offset
	DB	0b7H

	DW	79,	02bbH	; Count, starting offset
	DB	031H,	00H,	032H,	00H,	033H,	00H
	DB	034H,	00H,	035H,	00H,	036H,	00H
	DB	037H,	00H,	038H,	00H,	039H,	00H
	DB	030H,	00H,	0dbH,	00H,	0bdH,	00H
	DB	0ffH,	0ffH,	032H,	0ffH,	040H,	033H
	DB	0ffH,	023H,	034H,	0ffH,	024H,	036H
	DB	0ffH,	05eH,	038H,	0ffH,	07bH,	039H
	DB	0ffH,	07dH,	030H,	0ffH,	0ffH,	0bbH
	DB	0ffH,	0ffH,	0dbH,	0ffH,	0ffH,	0ddH
	DB	0ffH,	05bH,	0baH,	0ffH,	05dH,	0c0H
	DB	0ffH,	0acH,	0deH,	0b5H,	0b5H,	0dcH
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH

	DW	17,	030eH	; Count, starting offset
	DB	0ddH,	01H,	05eH,	0ddH,	02H,	0a8H
	DB	00H,	020H,	046H,	072H,	065H,	06eH
	DB	063H,	068H,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	(offset PatchBase) + 254
	DW	(offset PatchBase) + 279
	DW	(offset PatchBase) + 381
	DW	(offset PatchBase) + 672
	DW	-1

