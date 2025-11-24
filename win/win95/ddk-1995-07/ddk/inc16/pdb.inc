;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

FilPerProc	EQU	20

Process_data_block	STRUC
pdb_Exit_Call	DW	?
pdb_block_len	DW	?
pdb_res0	DB	?
pdb_CPM_Call	DB	5 DUP (?)
pdb_Exit	DD	?
pdb_Ctrl_C	DD	?
pdb_Fatal_abort	DD	?
pdb_Parent_PID	DW	?
pdb_JFN_Table	DB	FilPerProc DUP (?)
pdb_environ	DW	?
pdb_User_stack	DD	?
pdb_JFN_Length	DW	?
pdb_JFN_Pointer	DD	?
pdb_Next_PDB	DD	?
pdb_InterCon	DB	?
pdb_Append	DB	?
pdb_Novell_Used	DB	2 DUP (?)
pdb_Version	DW	?
pdb_pad1	DB	14 DUP (?)
pdb_Call_system	DB	5 DUP (?)
pdb_pad2	DB	7 DUP (?)
pdb_FCB1	DB	16 DUP (?)
pdb_FCB2	DB	16 DUP (?)
pdb_pad3	DB	4 DUP (?)
pdb_tail	DB	128 DUP (?)
Process_data_block	ENDS
