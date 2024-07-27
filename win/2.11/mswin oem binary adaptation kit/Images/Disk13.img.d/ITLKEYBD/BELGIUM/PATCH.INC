; 'patch_be.inc' keyboard table patch file based on:
;  'fr1.wk2', 'fr2.wk2', 'fr1.wk2', 'be4.wk2', 'fr5.wk2', 'be6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < French 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	7,	026H	; Count, starting offset
	DB	0edH,	02H,	0f0H,	02H,	01H,	03H
	DB	0f7H

	DW	1,	03bH	; Count, starting offset
	DB	0bbH

	DW	1,	059H	; Count, starting offset
	DB	0dcH

	DW	1,	063H	; Count, starting offset
	DB	0dfH

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	1,	015cH	; Count, starting offset
	DB	0a3H

	DW	1,	0170H	; Count, starting offset
	DB	02dH

	DW	3,	0178H	; Count, starting offset
	DB	05fH,	038H,	09fH

	DW	10,	0234H	; Count, starting offset
	DB	03cH,	03eH,	01cH,	0ddH,	05eH,	0a8H
	DB	01bH,	0deH,	060H,	026H

	DW	1,	0246H	; Count, starting offset
	DB	01fH

	DW	3,	024cH	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	3,	0254H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	42,	02d1H	; Count, starting offset
	DB	0bbH,	00H,	0ffH,	0ffH,	033H,	023H
	DB	0ffH,	0dbH,	05eH,	0ffH,	0bbH,	07eH
	DB	0ffH,	0ddH,	040H,	0ffH,	0baH,	05bH
	DB	07bH,	0c0H,	0b5H,	0ffH,	0deH,	05dH
	DB	07dH,	0dcH,	05cH,	07cH,	0ffH,	0ffH
	DB	0ffH,	0ddH,	01H,	05eH,	0ddH,	02H
	DB	0a8H,	00H,	020H,	046H,	072H,	065H

	DW	6,	02fbH	; Count, starting offset
	DB	06eH,	063H,	068H,	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Belgium 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	048H

	DW	12,	022H	; Count, starting offset
	DB	0e3H,	02H,	0e5H,	02H,	0fH,	03H
	DB	012H,	03H,	02dH,	03H,	022H,	03H

	DW	1,	059H	; Count, starting offset
	DB	0dcH

	DW	14,	0234H	; Count, starting offset
	DB	0b5H,	0a3H,	01cH,	0ddH,	05eH,	0a8H
	DB	01bH,	0deH,	0b2H,	0b3H,	0ffH,	0dfH
	DB	0ffH,	0ffH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	1,	02b3H	; Count, starting offset
	DB	0a8H

	DW	40,	02d3H	; Count, starting offset
	DB	0ddH,	00H,	0baH,	00H,	0c0H,	00H
	DB	0dcH,	00H,	0bcH,	00H,	0beH,	00H
	DB	0bfH,	00H,	0bbH,	00H,	0ffH,	0ffH
	DB	031H,	07cH,	0ffH,	032H,	040H,	0ffH
	DB	033H,	023H,	0ffH,	034H,	07bH,	0ffH
	DB	035H,	05bH,	0ffH,	036H,	05eH,	0ffH
	DB	039H,	07bH,	0ffH,	030H

	DW	50,	02fbH	; Count, starting offset
	DB	07dH,	0ffH,	0ddH,	05bH,	0ffH,	0baH
	DB	05dH,	0ffH,	0c0H,	027H,	0ffH,	0dcH
	DB	060H,	0ffH,	0bbH,	07eH,	0ffH,	0e2H
	DB	05cH,	0ffH,	0ffH,	0ffH,	0ffH,	0c0H
	DB	040H,	0b4H,	0dcH,	040H,	060H,	0ddH
	DB	01H,	05eH,	0ddH,	02H,	0a8H,	0bbH
	DB	040H,	07eH,	00H,	020H,	042H,	065H
	DB	06cH,	067H,	069H,	075H,	06dH,	020H
	DB	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < French 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	8,	026H	; Count, starting offset
	DB	0bH,	03H,	0eH,	03H,	01fH,	03H
	DB	015H,	03H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	023aH	; Count, starting offset
	DB	01bH

	DW	37,	02d6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0acH,	0deH
	DB	0ffH

	DW	36,	02fbH	; Count, starting offset
	DB	07eH,	0e2H,	05cH,	07cH,	0bfH,	0ffH
	DB	0ffH,	0bcH,	0ffH,	0ffH,	0beH,	0ffH
	DB	0ffH,	0bdH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0ffH,	0ddH,	01H,	05eH,	0ddH,	02H
	DB	0a8H,	00H,	020H,	046H,	072H,	065H
	DB	06eH,	063H,	068H,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < Belgium 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	1,	011H	; Count, starting offset
	DB	048H

	DW	11,	022H	; Count, starting offset
	DB	0bbH,	02H,	0bdH,	02H,	0f3H,	02H
	DB	0f6H,	02H,	08H,	03H,	0fdH

	DW	1,	03bH	; Count, starting offset
	DB	0bfH

	DW	5,	055H	; Count, starting offset
	DB	0dfH,	0c0H,	0deH,	010H,	0dcH

	DW	4,	060H	; Count, starting offset
	DB	04dH,	0bcH,	0beH,	0bdH

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
	DB	030H,	03dH,	0ffH,	031H,	031H,	021H
	DB	0ffH,	032H,	032H,	022H,	080H,	033H
	DB	033H,	023H,	0ffH,	034H,	034H,	024H
	DB	0ffH,	035H,	035H,	025H,	0ffH,	036H
	DB	036H,	02bH,	09eH,	037H,	037H,	02fH
	DB	0ffH,	038H,	038H,	028H,	0ffH,	039H
	DB	039H,	029H

	DW	38,	0224H	; Count, starting offset
	DB	026H,	02aH,	01dH,	0bfH,	05eH,	0a8H
	DB	0ffH,	0c0H,	0e0H,	0f9H,	0ffH,	0dbH
	DB	027H,	03fH,	0ffH,	0dcH,	03cH,	03eH
	DB	01cH,	0ddH,	0e8H,	05bH,	01bH,	0deH
	DB	060H,	05dH,	0ffH,	0dfH,	0e9H,	0e7H
	DB	0ffH,	0bbH,	0ffH,	0ffH,	0ffH,	0bcH
	DB	02cH,	03bH

	DW	9,	024eH	; Count, starting offset
	DB	01fH,	0beH,	02eH,	03aH,	0ffH,	0e2H
	DB	0ffH,	0ffH,	0ffH

	DW	1,	02b3H	; Count, starting offset
	DB	0a8H

	DW	64,	02bbH	; Count, starting offset
	DB	0ffH,	0ffH,	032H,	0ffH,	040H,	033H
	DB	0ffH,	0a7H,	034H,	0ffH,	0a3H,	036H
	DB	0ffH,	05eH,	038H,	0ffH,	07bH,	039H
	DB	0ffH,	07dH,	030H,	0ffH,	0ffH,	0bbH
	DB	0ffH,	0ffH,	0dbH,	0ffH,	0ffH,	0ddH
	DB	0ffH,	05bH,	0baH,	0ffH,	05dH,	0c0H
	DB	0ffH,	0acH,	0deH,	0b5H,	0b5H,	0dcH
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	0ffH,	0bfH
	DB	01H,	05eH,	0bfH,	02H

	DW	13,	02fbH	; Count, starting offset
	DB	0a8H,	00H,	020H,	042H,	065H,	06cH
	DB	067H,	069H,	075H,	06dH,	020H,	036H
	DB	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 163
	DW	(offset PatchBase) + 322
	DW	(offset PatchBase) + 457
	DW	-1

