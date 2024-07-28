;******************************************************************************
;  DNLOCAL.INC - EQUATES AND STRUCTURES FOR DOSNET DEVICE
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp. 1986-1990
;
;   Module:   DNLOCAL.INC - Include file for DOSNET device
;
;   Version:  1.00
;
;   Date:     Jun 9, 1989
;
;   Author:   ARR
;
;******************************************************************************
;
;   CHANGE LOG:
;     DATE     VERSION	   Description
;   --------  --------	  --------------------------------------------------
;   06/09/89  1.00	  Original
;
;******************************************************************************

;
; DOSNET Control block structure
;
;   This is the structure of the per VM information that the DOSNET device
;	stores in each VM's control block.
;
;   There are two byte arrays indexed by drive number (A = 0)
;
;	DN_VM_LocalNetUseFlgs	indicates the status of the drive in this VM
;
;	DN_VM_LocalNetInhRefCnt indicates inheritance information (all entries
;				    should be zero except in the SYSTEM VM)
;
DN_Control_Blk_Struc STRUC

DN_VM_LocalNetUseFlgs	db	26 DUP (0)
DN_VM_LocalNetInhRefCnt db	26 DUP (0)

DN_Control_Blk_Struc ENDS

;
; Bits of DM_VM_LocalNetUseFlgs
;
DN_LocUse_F_InUse	equ	00000001B
DN_LocUse_F_InUseBit	equ	0	    ; Drive is a network drive
DN_LocUse_F_Inherit	equ	00000010B
DN_LocUse_F_InheritBit	equ	1	    ; Drive was inherited from SYS VM
DN_LocUse_F_Global	equ	00000100B
DN_LocUse_F_GlobalBit	equ	2	    ; Drive was in use when WIN386
					    ;	 started
