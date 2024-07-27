; 'patch_fs.inc' keyboard table patch file based on:
;  'fs1.wk2', 'fs2.wk2', 'fs1.wk2', 'fs4.wk2', 'fs5.wk2', 'fs6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < FinnishSwedish 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	12,	022H	; Count, starting offset
	DB	0c1H,	02H,	0c3H,	02H,	0d8H,	02H
	DB	0dbH,	02H,	0faH,	02H,	0e8H,	02H

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	1,	0165H	; Count, starting offset
	DB	023H

	DW	1,	0230H	; Count, starting offset
	DB	040H

	DW	61,	02bdH	; Count, starting offset
	DB	0c0H,	00H,	0deH,	00H,	0ffH,	0ffH
	DB	033H,	0a3H,	0ffH,	037H,	027H,	0ffH
	DB	0dbH,	05eH,	07eH,	0ddH,	027H,	060H
	DB	0baH,	05bH,	07bH,	0dcH,	05dH,	07dH
	DB	0bfH,	05cH,	07cH,	0ffH,	0ffH,	0ffH
	DB	0ddH,	04H,	0b4H,	0ddH,	08H,	060H
	DB	0baH,	02H,	05eH,	0baH,	01H,	0a8H
	DB	00H,	020H,	046H,	069H,	06eH,	06eH
	DB	069H,	073H,	068H,	053H,	077H,	065H
	DB	064H,	069H,	073H,	068H,	020H,	032H
	DB	00H

	DW	0		; Zero count terminates patch list.


; Patches for < FinnishSwedish 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	08H

	DW	12,	022H	; Count, starting offset
	DB	0c1H,	02H,	0c3H,	02H,	0e4H,	02H
	DB	0e7H,	02H,	09H,	03H,	0f7H,	02H

	DW	5,	0165H	; Count, starting offset
	DB	023H,	0ffH,	034H,	034H,	0a4H

	DW	2,	021cH	; Count, starting offset
	DB	02cH,	02cH

	DW	3,	0228H	; Count, starting offset
	DB	027H,	02aH,	0ffH

	DW	3,	0234H	; Count, starting offset
	DB	0a7H,	0bdH,	01cH

	DW	3,	0254H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	76,	02bdH	; Count, starting offset
	DB	0c0H,	00H,	0deH,	00H,	0ffH,	0ffH
	DB	032H,	040H,	0ffH,	033H,	0a3H,	0ffH
	DB	034H,	024H,	0ffH,	037H,	07bH,	0ffH
	DB	038H,	05bH,	0ffH,	039H,	05dH,	0ffH
	DB	030H,	07dH,	0ffH,	0bbH,	05cH,	0ffH
	DB	0baH,	07eH,	0ffH,	04dH,	0b5H,	0ffH
	DB	0e2H,	07cH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0dbH,	01H,	0b4H,	0dbH,	02H,	060H
	DB	0baH,	02H,	05eH,	0baH,	01H,	0a8H
	DB	0baH,	040H,	07eH,	00H,	020H,	046H
	DB	069H,	06eH,	06eH,	069H,	073H,	068H
	DB	053H,	077H,	065H,	064H,	069H,	073H
	DB	068H,	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < FinnishSwedish 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	0165H	; Count, starting offset
	DB	023H

	DW	40,	02c6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0ffH,	0deH
	DB	0ffH,	0acH,	0dcH,	0ffH

	DW	8,	02f3H	; Count, starting offset
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH

	DW	1,	031bH	; Count, starting offset
	DB	035H

	DW	0		; Zero count terminates patch list.


; Patches for < FinnishSwedish 6>

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

	DW	1,	0162H	; Count, starting offset
	DB	080H

	DW	40,	02c6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0ffH,	0deH
	DB	0ffH,	0acH,	0dcH,	0ffH

	DW	8,	02f3H	; Count, starting offset
	DB	0ffH,	0ffH,	0beH,	0ffH,	0ffH,	0bdH
	DB	0ffH,	0ffH

	DW	1,	031bH	; Count, starting offset
	DB	036H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 122
	DW	(offset PatchBase) + 266
	DW	(offset PatchBase) + 369
	DW	-1

