; 'patch_fr.inc' keyboard table patch file based on:
;  'fr1.wk2', 'fr2.wk2', 'fr1.wk2', 'fr4.wk2', 'fr5.wk2', 'fr6.wk2'

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


; Patches for < French 4>

	DW	1,	09H	; Count, starting offset
	DB	04H

	DW	1,	011H	; Count, starting offset
	DB	048H

	DW	12,	022H	; Count, starting offset
	DB	0e3H,	02H,	0e5H,	02H,	09H,	03H
	DB	0cH,	03H,	01dH,	03H,	013H,	03H

	DW	1,	03bH	; Count, starting offset
	DB	0bbH

	DW	1,	059H	; Count, starting offset
	DB	0dcH

	DW	1,	063H	; Count, starting offset
	DB	0dfH

	DW	1,	015aH	; Count, starting offset
	DB	00H

	DW	5,	016eH	; Count, starting offset
	DB	09bH,	036H,	02dH,	036H,	09fH

	DW	7,	0178H	; Count, starting offset
	DB	05fH,	038H,	09cH,	039H,	0e7H,	039H
	DB	09eH

	DW	1,	0225H	; Count, starting offset
	DB	0a3H

	DW	15,	0234H	; Count, starting offset
	DB	02aH,	0b5H,	01cH,	0ddH,	05eH,	0b7H
	DB	01bH,	0deH,	0b2H,	0ffH,	0ffH,	0e2H
	DB	03cH,	03eH,	01cH

	DW	4,	024bH	; Count, starting offset
	DB	0dfH,	021H,	0a7H,	0ffH

	DW	4,	0253H	; Count, starting offset
	DB	0bdH,	0ffH,	0ffH,	0ffH

	DW	42,	02d1H	; Count, starting offset
	DB	0bbH,	00H,	0ddH,	00H,	0baH,	00H
	DB	0c0H,	00H,	0dcH,	00H,	0bcH,	00H
	DB	0beH,	00H,	0bfH,	00H,	0dfH,	00H
	DB	0ffH,	0ffH,	032H,	07eH,	0ffH,	033H
	DB	023H,	0ffH,	034H,	07bH,	0ffH,	035H
	DB	05bH,	0ffH,	036H,	07cH,	0ffH,	037H
	DB	060H,	0ffH,	038H,	05cH,	0ffH,	039H

	DW	34,	02fbH	; Count, starting offset
	DB	05eH,	0ffH,	030H,	040H,	0ffH,	0dbH
	DB	05dH,	0ffH,	0bbH,	07dH,	0ffH,	0baH
	DB	0a4H,	0ffH,	0ffH,	0ffH,	0ffH,	0ddH
	DB	01H,	05eH,	0ddH,	02H,	0b7H,	00H
	DB	020H,	046H,	072H,	065H,	06eH,	063H
	DB	068H,	020H,	034H,	00H

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


; Patches for < French 6>

	DW	1,	09H	; Count, starting offset
	DB	06H

	DW	8,	026H	; Count, starting offset
	DB	0bH,	03H,	0eH,	03H,	01fH,	03H
	DB	015H,	03H

	DW	1,	059H	; Count, starting offset
	DB	0dcH

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

	DW	14,	0234H	; Count, starting offset
	DB	03cH,	03eH,	01cH,	0ddH,	05eH,	0a8H
	DB	01bH,	0deH,	060H,	0a3H,	0ffH,	0dfH
	DB	0ffH,	0ffH

	DW	1,	024eH	; Count, starting offset
	DB	01fH

	DW	3,	0254H	; Count, starting offset
	DB	0ffH,	0ffH,	0ffH

	DW	37,	02d6H	; Count, starting offset
	DB	0ffH,	040H,	033H,	0ffH,	023H,	034H
	DB	0ffH,	024H,	036H,	0ffH,	05eH,	038H
	DB	0ffH,	07bH,	039H,	0ffH,	07dH,	030H
	DB	0ffH,	0ffH,	0bbH,	0ffH,	0ffH,	0dbH
	DB	0ffH,	0ffH,	0ddH,	0ffH,	05bH,	0baH
	DB	0ffH,	05dH,	0c0H,	0ffH,	0acH,	0deH
	DB	0b5H

	DW	36,	02fbH	; Count, starting offset
	DB	0b5H,	0dcH,	05cH,	07cH,	0bfH,	0ffH
	DB	0ffH,	0bcH,	0ffH,	0ffH,	0beH,	0ffH
	DB	0ffH,	0bdH,	0ffH,	0ffH,	0ffH,	0ffH
	DB	0ffH,	0ddH,	01H,	05eH,	0ddH,	02H
	DB	0a8H,	00H,	020H,	046H,	072H,	065H
	DB	06eH,	063H,	068H,	020H,	036H,	00H

	DW	0		; Zero count terminates patch list.

; The following table contains pointers to patches
; for keyboard types 2, 3 and 4

PatchTable label word

	DW	(offset PatchBase) + 0
	DW	-1
	DW	(offset PatchBase) + 163
	DW	(offset PatchBase) + 355
	DW	(offset PatchBase) + 490
	DW	-1

