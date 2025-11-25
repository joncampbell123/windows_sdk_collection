/*===========================================================================*\
|
|  File:        cgexcpt.cpp
|
|  Description: 
|       Exception handling code for sample games
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#include "cgres.h"
#include "cgexcpt.h"

/*---------------------------------------------------------------------------*\
|
|       Class CGameException
|
|  DESCRIPTION:
|       Container class for most exceptions.
|
|
\*---------------------------------------------------------------------------*/

CGameException::CGameException(
        int resID   // resource ID of information string; 0 == no message
        ) : mResID( resID )
{
    // !!! does nothing for now
}   

CGameException::~CGameException()
{
    
}   

/*---------------------------------------------------------------------------*\
|
|       Class CGameStartupError
|
|  DESCRIPTION:
|       Handles exceptions which prevent app from running
|
|
\*---------------------------------------------------------------------------*/

CGameStartupError::CGameStartupError(
        int resID   // resource ID of information string; 0 == no message
        ) : CGameException(resID)
{
    // !!! does nothing for now
}   

CGameStartupError::~CGameStartupError()
{
    
}   

/*---------------------------------------------------------------------------*\
|
|       Class CGameResourceError
|
|  DESCRIPTION:
|       Handles general out-of-resource exceptions
|
|
\*---------------------------------------------------------------------------*/

CGameResourceError::CGameResourceError(
        // may want to add a resource type parameter
        ) : CGameException(0)
{
    // !!! may want to dump a memory ballast or some such
}   

CGameResourceError::~CGameResourceError()
{
    
}   

