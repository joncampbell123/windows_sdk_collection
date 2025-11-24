//---------------------------------------------------------------------------
//
//  Module:   cnfgmgr.h
//
//  Description:
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str

// external function prototypes

CONFIGRET AVVXP500_Set_Config
(
    DWORD           dn,
    WORD            wBaseVXP500,
    WORD            wIRQ,
    DWORD           dwMemBase
	 
) ;

VOID AVVXP500_Remove_Config
(
    DEVNODE         dn
) ;

BOOL AVVXP500_IsOwned
(
    DEVNODE         dn
) ;

//---------------------------------------------------------------------------
//  End of File: cnfgmgr.h
//---------------------------------------------------------------------------
