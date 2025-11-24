        page    60, 132

;******************************************************************************
        title   VALIDATE.ASM - Detection routines
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    AVVXP500.386 - AURAVISION VxP500 386 Driver
;
;   Module:   VALIDATE.ASM - Hardware validation routines
;
;   Version:  1.00
;******************************************************************************
;
;   Functional Description:
;      VxP500 validation routines.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include avvxp500.inc
        include equates.inc
        .list


;==============================================================================
;                          P A G E A B L E   D A T A
;==============================================================================

VxD_PAGEABLE_DATA_SEG

;------------------------------------------------------------------------------
;          G L O B A L   I N I T   D A T A   D E C L A R A T I O N S
;------------------------------------------------------------------------------

VxD_PAGEABLE_DATA_ENDS

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

VxD_PNP_CODE_ENDS

end
