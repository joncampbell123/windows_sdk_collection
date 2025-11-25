/*===========================================================================*\
|
|  File:         cgres.cpp
|
|  Description: 
|       Class implemetation for handling string resources
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#include "cgexcpt.h"
#include "cgres.h"

/*---------------------------------------------------------------------------*\
|
|       Class CGameStringResource
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

CGameStringResource::CGameStringResource(
    HINSTANCE hInst,    // as passed to LoadString
    UINT resId          // ID of the string
    )
{
    mString[0] = '\0';
    if (!LoadString(
                hInst,
                resId,
                mString,
                MAX_STRING_LENGTH
                )
        )
    {
        // nasty business! we may be in an exception already, so be careful
        throw CGameResourceError();
    }

}   

CGameStringResource::~CGameStringResource()
{
    
}   
