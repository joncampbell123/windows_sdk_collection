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

arena	STRUC
arena_signature	DB	?
arena_owner	DW	?
arena_size	DW	?
arena_reserved	DB	3 DUP (?)
arena_name	DB	8 DUP (?)
arena	ENDS

arena_owner_system	EQU	0        
arena_signature_normal	EQU	4DH     
arena_signature_end	EQU	5AH     
FIRST_FIT	EQU	00H 
BEST_FIT	EQU	01H 
LAST_FIT	EQU	02H 
LOW_FIRST	EQU	00H 
HIGH_FIRST	EQU	80H 
HIGH_ONLY	EQU	40H 
LINKSTATE	EQU	01H 
HF_MASK	EQU	( NOT HIGH_FIRST)
HO_MASK	EQU	( NOT HIGH_ONLY)
STRAT_MASK	EQU	(HF_MASK  AND  HO_MASK) 
