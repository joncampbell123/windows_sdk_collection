//---------------------------------------------------------------------------
//
//  Module:   config.h
//
//  Description:
//     resource IDs
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1995 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

// bilingual...

#ifdef RC_INVOKED
    #define RCID(id)    id
#else
    #define RCID(id)    MAKEINTRESOURCE(id)
#endif


    #define IDD_PROPDLG_SB            RCID( 11 )
    #define IDD_PROPDLG_NOSB          RCID( 12 )
    
    #define IDC_DRVPROP_EMULATION_GRP     0x100
    #define IDC_DRVPROP_ENABLESB          0x101
    #define IDC_DRVPROP_ADVANCED_GRP      0x102
    #define IDC_DRVPROP_ACCEPTCLOSERATES  0x103
    #define IDC_DRVPROP_SINGLEMODEDMA     0x104
    #define IDC_DRVPROP_SETDEFAULTS       0x105

//---------------------------------------------------------------------------
//  End of File: config.h
//---------------------------------------------------------------------------

