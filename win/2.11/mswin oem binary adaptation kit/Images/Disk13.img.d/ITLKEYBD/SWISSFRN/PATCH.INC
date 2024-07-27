; 'patch_sf.inc' keyboard table patch file based on:
;  'sf1.wk2', 'sf2.wk2', 'sf1.wk2', 'sf4.wk2', 'fr5.wk2', 'sg6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < SwissFrench 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	7,	026H	; Count, starting offset
	DB	0d5H,	02H,	0d8H,	02H,	0f4H,	02H
	DB	0e5H

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	1,	022dH	; Count, starting offset
	DB	027H

	DW	55,	02bdH	; Count, starting offset
	DB	031H,	0a7H,	0b0H,	033H,	023H,	0ffH
	DB	0dbH,	021H,	0ffH,	0ddH,	07eH,	0ffH
	DB	0baH,	040H,	060H,	0c0H,	05bH,	07bH
	DB	0bfH,	05dH,	07dH,	0dfH,	05cH,	07cH
	DB	0ffH,	0ffH,	0ffH,	0c0H,	02H,	0b4H
	DB	0ddH,	02H,	060H,	0ddH,	01H,	05eH
	DB	0c0H,	01H,	0a8H,	00H,	020H,	053H
	DB	077H,	069H,	073H,	073H,	046H,	072H
	DB	065H,	06eH,	063H,	068H,	020H,	032H
	DB	00H

	DW	0		; Zero count terminates patch list.


; Patches for < SwissFrench 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	7,	026H	; Count, starting offset
	DB	0e4H,	02H,	0e7H,	02H,	06H,	03H
	DB	0f7H

	DW	5,	0228H	; Count, starting offset
	DB	0a7H,	0b0H,	0ffH,	0c0H,	022H

	DW	2,	0240H	; Count, starting offset
	DB	024H,	0a3H

	DW	3,	0254H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	64,	02bdH	; Count, starting offset
	DB	031H,	0a6H,	0ffH,	032H,	040H,	0ffH
	DB	033H,	023H,	0ffH,	034H,	0b0H,	0ffH
	DB	035H,	0a7H,	0ffH,	036H,	0acH,	0ffH
	DB	037H,	07cH,	0ffH,	038H,	0a2H,	0ffH
	DB	0baH,	05bH,	0ffH,	0c0H,	05dH,	0ffH
	DB	0dcH,	07bH,	0ffH,	0dfH,	07dH,	0ffH
	DB	0e2H,	05cH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0dbH,	040H,	0b4H,	0ddH,	02H,	060H
	DB	0ddH,	01H,	05eH,	0c0H,	01H,	022H
	DB	0ddH,	040H,	07eH,	00H,	020H,	053H
	DB	077H,	069H,	073H,	073H

	DW	9,	02fdH	; Count, starting offset
	DB	046H,	072H,	065H,	06eH,	063H,	068H
	DB	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < French 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	12,	022H	; Count, starting offset
	DB	0d3H,	02H,	0d5H,	02H,	0bH,	03H
	DB	0eH,	03H,	01fH,	03H,	015H,	03H

	DW	9,	03bH	; Count, starting offset
	DB	0bdH,	08H,	09H,	041H,	05aH,	045H
	DB	052H,	054H,	059H

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

	DW	38,	0224H	; Count, starting offset
	DB	024H,	02aH,	01dH,	0bfH,	03aH,	02fH
	DB	0ffH,	0c0H,	0f9H,	025H,	0ffH,	0dbH
	DB	029H,	0b0H,	0ffH,	0dcH,	023H,	0b5H
	DB	0ffH,	0ddH,	05eH,	0a8H,	01bH,	0deH
	DB	0b5H,	0a3H,	0ffH,	0dfH,	021H,	0a7H
	DB	0ffH,	0bbH,	03dH,	02bH,	0ffH,	0bcH
	DB	02cH,	03fH

	DW	9,	024eH	; Count, starting offset
	DB	09fH,	0beH,	03bH,	02eH,	0ffH,	0e2H
	DB	03cH,	03eH,	01cH

	DW	1,	02b3H	; Count, starting offset
	DB	0b7H

	DW	66,	02bbH	; Count, starting offset
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

	DW	34,	02fdH	; Count, starting offset
	DB	05cH,	07cH,	0bfH,	0ffH,	0ffH,	0bcH
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH,	0ffH,	0ffH,	0ffH,	0ddH
	DB	01H,	05eH,	0ddH,	02H,	0a8H,	00H
	DB	020H,	046H,	072H,	065H,	06eH,	063H
	DB	068H,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < SwissGerman 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	12,	022H	; Count, starting offset
	DB	0c3H,	02H,	0c5H,	02H,	0fbH,	02H
	DB	0feH,	02H,	01aH,	03H,	0bH,	03H

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

	DW	1,	0165H	; Count, starting offset
	DB	0a7H

	DW	2,	0224H	; Count, starting offset
	DB	0fcH,	0dcH

	DW	6,	0230H	; Count, starting offset
	DB	0dfH,	03fH,	0ffH,	0dcH,	0e4H,	0c4H

	DW	2,	023cH	; Count, starting offset
	DB	0f6H,	0d6H

	DW	2,	0254H	; Count, starting offset
	DB	023H,	040H

	DW	66,	02bbH	; Count, starting offset
	DB	0baH,	00H,	0c0H,	00H,	0deH,	00H
	DB	0dcH,	00H,	0ffH,	0ffH,	032H,	0ffH
	DB	040H,	033H,	0ffH,	023H,	034H,	0ffH
	DB	024H,	036H,	0ffH,	05eH,	038H,	0ffH
	DB	07bH,	039H,	0ffH,	07dH,	030H,	0ffH
	DB	0ffH,	0bbH,	0ffH,	0ffH,	0dbH,	0ffH
	DB	0ffH,	0ddH,	0ffH,	027H,	0baH,	0ffH
	DB	05bH,	0c0H,	0ffH,	05dH,	0deH,	0ffH
	DB	0ffH,	0dcH,	0ffH,	0acH,	0bfH,	0ffH
	DB	07eH,	0dfH,	05cH,	07cH,	0beH,	0ffH
	DB	0ffH,	0bdH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	29,	02fdH	; Count, starting offset
	DB	0ffH,	0ddH,	02H,	060H,	0ddH,	01H
	DB	05eH,	0c0H,	01H,	0a8H,	0ddH,	080H
	DB	0b4H,	00H,	020H,	053H,	077H,	069H
	DB	073H,	073H,	047H,	065H,	072H,	06dH
	DB	061H,	06eH,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 106
	DW	(offset PatchBase) + 232
	DW	(offset PatchBase) + 535
	DW	-1

