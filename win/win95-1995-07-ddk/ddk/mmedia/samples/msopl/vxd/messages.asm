        page    60, 132

;******************************************************************************
        title   MESSAGES.ASM - localizeable messages
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    MSOPL.386 - MICROSOFT OPL2/OPL3 386 Driver
;
;   Module:   MESSAGES.ASM - localizeable messages
;
;   Version:  4.00
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include msgmacro.inc

CREATE_MESSAGES equ     VMM_TRUE
        include messages.inc
        .list

        end
