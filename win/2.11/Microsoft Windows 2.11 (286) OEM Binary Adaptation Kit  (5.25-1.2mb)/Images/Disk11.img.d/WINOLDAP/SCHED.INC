include pdb1.inc
include tdb.inc

SG_TASKSWITCH = 10H
SG_EXIT       = 20H


TASKREG STRUC
TASKDX  DW      ?
TASKBX  DW      ?
TASKES  DW      ?
TASKCX  DW      ?
TASKAX  DW      ?
TASKDI  DW      ?
TASKSI  DW      ?
TASKDS  DW      ?
TASKBP  DW      ?
TASKREG ENDS

win_exec        struc
win_environ     dw      ?               ; seg addr of environment
win_com_line    dd      ?               ; pointer to asciz command line
win_5C_FCB      dd      ?               ; default fcb at 5C
win_6C_FCB      dd      ?               ; default fcb at 6C
win_exec        ends

FALSE       equ     0000h
TRUE        equ     NOT FALSE
