; 'patch_gr.inc' keyboard table patch file based on:
;  'gr1.wk2', 'gr2.wk2', 'gr1.wk2', 'gr4.wk2', 'gr5.wk2', 'gr6.wk2'

; Table 3 (for 86-key keyboards) is the same as table 1


PatchBase label byte

; Patches for < German 2>

	DW	1,	09H	; Count, starting offset
	DB	02H

	DW	7,	026H	; Count, starting offset
	DB	0d2H,	02H,	0d5H,	02H,	0e6H,	02H
	DB	0dcH

	DW	3,	057H	; Count, starting offset
	DB	0dcH,	010H,	0bfH

	DW	20,	082H	; Count, starting offset
	DB	0e4H,	06aH,	0e3H,	0dH,	025H,	028H
	DB	027H,	026H,	0e6H,	091H,	0ffH,	06fH
	DB	07aH,	07bH,	07cH,	07dH,	07eH,	07fH
	DB	0e0H,	0e1H

	DW	1,	0162H	; Count, starting offset
	DB	0ffH

	DW	1,	0172H	; Count, starting offset
	DB	09eH

	DW	3,	0228H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	3,	0234H	; Count, starting offset
	DB	023H,	05eH,	0ffH

	DW	3,	0240H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	3,	0254H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	32,	02c3H	; Count, starting offset
	DB	0ddH,	07eH,	0ffH,	0baH,	040H,	060H
	DB	0bbH,	05bH,	07bH,	0dcH,	05dH,	07dH
	DB	0bfH,	05cH,	07cH,	0ffH,	0ffH,	0ffH
	DB	0ddH,	01H,	0b4H,	0ddH,	02H,	060H
	DB	00H,	020H,	047H,	065H,	072H,	06dH
	DB	061H,	06eH

	DW	3,	02e3H	; Count, starting offset
	DB	020H,	032H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < German 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	048H

	DW	12,	022H	; Count, starting offset
	DB	0e3H,	02H,	0e5H,	02H,	06H,	03H
	DB	09H,	03H,	01dH,	03H,	013H,	03H

	DW	3,	057H	; Count, starting offset
	DB	0dcH,	010H,	0bfH

	DW	1,	0172H	; Count, starting offset
	DB	09eH

	DW	2,	021cH	; Count, starting offset
	DB	02cH,	02cH

	DW	1,	022aH	; Count, starting offset
	DB	01cH

	DW	1,	0236H	; Count, starting offset
	DB	0ffH

	DW	3,	0240H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	1,	0256H	; Count, starting offset
	DB	0ffH

	DW	39,	02bbH	; Count, starting offset
	DB	031H,	00H,	032H,	00H,	033H,	00H
	DB	034H,	00H,	035H,	00H,	036H,	00H
	DB	037H,	00H,	038H,	00H,	039H,	00H
	DB	030H,	00H,	0dbH,	00H,	0ddH,	00H
	DB	0baH,	00H,	0bbH,	00H,	0c0H,	00H
	DB	0deH,	00H,	0dcH,	00H,	0bfH,	00H
	DB	0bcH,	00H,	0beH

	DW	58,	02e3H	; Count, starting offset
	DB	0ffH,	0ffH,	032H,	0b2H,	0ffH,	033H
	DB	0b3H,	0ffH,	037H,	07bH,	0ffH,	038H
	DB	05bH,	0ffH,	039H,	05dH,	0ffH,	030H
	DB	07dH,	0ffH,	0dbH,	05cH,	0ffH,	051H
	DB	040H,	0ffH,	0bbH,	07eH,	0ffH,	04dH
	DB	0b5H,	0ffH,	0e2H,	07cH,	0ffH,	0ffH
	DB	0ffH,	0ffH,	0ddH,	01H,	0b4H,	0ddH
	DB	02H,	060H,	0dcH,	01H,	05eH,	00H
	DB	020H,	047H,	065H,	072H,	06dH,	061H
	DB	06eH,	020H,	034H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < German 5>

	DW	1,	09H	; Count, starting offset
	DB	05H

	DW	8,	026H	; Count, starting offset
	DB	0f9H,	02H,	0fcH,	02H,	0dH,	03H
	DB	03H,	03H

	DW	1,	057H	; Count, starting offset
	DB	0bfH

	DW	5,	082H	; Count, starting offset
	DB	0dH,	0ffH,	0ffH,	0ffH,	0ffH

	DW	1,	0a6H	; Count, starting offset
	DB	0dH

	DW	12,	0f6H	; Count, starting offset
	DB	026H,	0ffH,	06dH,	025H,	0ffH,	027H
	DB	06bH,	0ffH,	028H,	0ffH,	02dH,	02eH

	DW	1,	0162H	; Count, starting offset
	DB	0ffH

	DW	1,	0172H	; Count, starting offset
	DB	09eH

	DW	1,	0229H	; Count, starting offset
	DB	05eH

	DW	1,	0236H	; Count, starting offset
	DB	01cH

	DW	3,	0240H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	31,	02c4H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	05dH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	0ffH,	0baH
	DB	0ffH

	DW	42,	02e3H	; Count, starting offset
	DB	05bH,	0c0H,	0ffH,	0ffH,	0deH,	0ffH
	DB	0acH,	0bfH,	0ffH,	07eH,	0e2H,	05cH
	DB	07cH,	0bcH,	0ffH,	0ffH,	0beH,	0ffH
	DB	0ffH,	0bdH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0ffH,	0ddH,	01H,	0b4H,	0ddH,	02H
	DB	060H,	00H,	020H,	047H,	065H,	072H
	DB	06dH,	061H,	06eH,	020H,	035H,	00H

	DW	0		; Zero count terminates patch list.


; Patches for < German 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	8,	026H	; Count, starting offset
	DB	0f9H,	02H,	0fcH,	02H,	0dH,	03H
	DB	03H,	03H

	DW	3,	057H	; Count, starting offset
	DB	0dcH,	010H,	0bfH

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
	DB	0ffH

	DW	1,	0172H	; Count, starting offset
	DB	09eH

	DW	3,	0228H	; Count, starting offset
	DB	03cH,	03eH,	01cH

	DW	3,	0234H	; Count, starting offset
	DB	023H,	05eH,	0ffH

	DW	3,	0240H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	3,	0254H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	31,	02c4H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	05dH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	0ffH,	0baH
	DB	0ffH

	DW	42,	02e3H	; Count, starting offset
	DB	05bH,	0c0H,	0ffH,	0ffH,	0deH,	0ffH
	DB	0acH,	0dcH,	0ffH,	07eH,	0bfH,	05cH
	DB	07cH,	0bcH,	0ffH,	0ffH,	0beH,	0ffH
	DB	0ffH,	0bdH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0ffH,	0ddH,	01H,	0b4H,	0ddH,	02H
	DB	060H,	00H,	020H,	047H,	065H,	072H
	DB	06dH,	061H,	06eH,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 135
	DW	(offset PatchBase) + 308
	DW	(offset PatchBase) + 470
	DW	-1

