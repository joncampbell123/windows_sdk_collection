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

ifndef _WINREG_
endif
MAXKEYNAME	EQU	256
MAXVALUENAME_LENGTH	EQU	MAXKEYNAME
MAXDATA_LENGTH	EQU	16*1024
ifndef REG_SZ
REG_SZ	EQU	0001H
endif
ifndef REG_BINARY
REG_BINARY	EQU	0003H
endif
ifndef REG_DWORD
REG_DWORD	EQU	0004H
endif
ifndef FALSE
FALSE	EQU	0
endif
ifndef TRUE
TRUE	EQU	NOT FALSE
endif
ifndef	ERROR_SUCCESS
ERROR_SUCCESS	EQU	0
endif
ifndef ERROR_FILE_NOT_FOUND
ERROR_FILE_NOT_FOUND	EQU	2
endif
ifndef ERROR_ACCESS_DENIED
ERROR_ACCESS_DENIED	EQU	5
endif
ifndef ERROR_BADDB
ERROR_BADDB	EQU	1009
endif
ifndef ERROR_MORE_DATA
ERROR_MORE_DATA	EQU	234
endif
ifndef ERROR_BADKEY
ERROR_BADKEY	EQU	1010
endif
ifndef ERROR_CANTOPEN
ERROR_CANTOPEN	EQU	1011
endif
ifndef ERROR_CANTREAD
ERROR_CANTREAD	EQU	1012
ERROR_CANTWRITE	EQU	1013
endif
ifndef ERROR_REGISTRY_CORRUPT
ERROR_REGISTRY_CORRUPT	EQU	1015
ERROR_REGISTRY_IO_FAILED	EQU	1016
endif
ifndef ERROR_KEY_DELETED
ERROR_KEY_DELETED	EQU	1018
endif
ifndef ERROR_OUTOFMEMORY
ERROR_OUTOFMEMORY	EQU	14
endif
ifndef ERROR_INVALID_PARAMETER
ERROR_INVALID_PARAMETER	EQU	87
endif
ifndef ERROR_LOCK_FAILED
ERROR_LOCK_FAILED	EQU	167
endif
ifndef ERROR_NO_MORE_ITEMS
ERROR_NO_MORE_ITEMS	EQU	259
endif
ifndef ERROR_CANTOPEN16_FILENOTFOUND32
ERROR_CANTOPEN16_FILENOTFOUND32	EQU	0ffff0000H
ERROR_CANTREAD16_FILENOTFOUND32	EQU	0ffff0001H
endif
ifndef HKEY_LOCAL_MACHINE
HKEY_CLASSES_ROOT	EQU	80000000H
HKEY_CURRENT_USER	EQU	80000001H
HKEY_LOCAL_MACHINE	EQU	80000002H
HKEY_USERS	EQU	80000003H
HKEY_PERFORMANCE_DATA	EQU	80000004H
HKEY_CURRENT_CONFIG	EQU	80000005H
HKEY_DYN_DATA	EQU	80000006H
endif
ifndef HKEY_PREDEF_KEYS
HKEY_PREDEF_KEYS	EQU	7
endif
MAXREGFILES	EQU	HKEY_PREDEF_KEYS
RegOpenKey_Idx	EQU	100H
RegCreateKey_Idx	EQU	101H
RegCloseKey_Idx	EQU	102H
RegDeleteKey_Idx	EQU	103H
RegSetValue_Idx	EQU	104H
RegQueryValue_Idx	EQU	105H
RegEnumKey_Idx	EQU	106H
RegDeleteValue_Idx	EQU	107H
RegEnumValue_Idx	EQU	108H
RegQueryValueEx_Idx	EQU	109H
RegSetValueEx_Idx	EQU	10AH
RegFlushKey_Idx	EQU	10BH
RegLoadKey_Idx	EQU	10CH
RegUnLoadKey_Idx	EQU	10DH
RegSaveKey_Idx	EQU	10EH
RegRestore_Idx	EQU	10FH
RegRemapPreDefKey_Idx	EQU	110H

Reg_Query_Restore_s	STRUC
dwRQR_Err	DD	?
hRQR_RootKey	DD	?
dwRQR_Reference	DD	?
szRQR_SubKey	DB	MAXKEYNAME DUP (?)
szRQR_FileName	DB	256 DUP (?)
Reg_Query_Restore_s	ENDS
