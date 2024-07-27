
NESTINGDEPTH	equ	7	;the number of macro that may be kept
				;on the stack

;
; WOA OPcode equates
;
OP_ERROR	equ	0
OPKEYSTROKE	equ	1
OPABORT 	equ	2
OPCLOSE 	equ	3
OPCOPY		equ	4
OPMACRO 	equ	5
OPMARK		equ	6
OPMAXIMIZE	equ	7
OPMENU		equ	8
OPMINIMIZE	equ	9
OPNULL		equ	0AH
OPPASTE 	equ	0BH
OPRESTORE	equ	0CH
OPSCANCODE	equ	0DH
OPSCROLL	equ	0EH
OPSHOWTITLE	equ	0FH
OPSWITCH	equ	10H
;
;parameter equates
;
PR_NONE 	equ	1
PR_COORDINATES	equ	2
PR_CHARACTER	equ	3
PR_KEYEQUIV	equ	4
PRSCREEN	equ	5
PR_ON		equ	6
PR_OFF		equ	7
PRMF_GRAYED	equ	00000001b
PRMF_DISABLED	equ	00000010b
PRMF_ENABLED	equ	00000000b
PRMF_CHECKED	equ	00001000b
PRMF_UNCHECKED	equ	00000000b

;
; shift state mask equates
;
; note: 10 is the flag for left shift, 01 is the flag for right shift
;
SHIFTMASK	equ	00000010b
CTRLMASK	equ	00000100b
ALTMASK 	equ	00001000b
;
; keystroke equates
;
KYBREAK 	equ	70
KYDOWN		equ	80
KYEND		equ	79
KYENTER 	equ	28
KYESC		equ	1
KYF10		equ	68
KYF11		equ	0
KYF12		equ	0
KYF1		equ	59
KYF2		equ	60
KYF3		equ	61
KYF4		equ	62
KYF5		equ	63
KYF6		equ	64
KYF7		equ	65
KYF8		equ	66
KYF9		equ	67
KYHOME		equ	71
KYLEFT		equ	75
KYNULL		equ	 0
KYNUM		equ	69
KYPGDN		equ	81
KYPGUP		equ	73
KYPRTSC 	equ	55
KYRIGHT 	equ	77
KYSCROLL	equ	70
KYSYS		equ	0
KYTAB		equ	15
KYUP		equ	72

