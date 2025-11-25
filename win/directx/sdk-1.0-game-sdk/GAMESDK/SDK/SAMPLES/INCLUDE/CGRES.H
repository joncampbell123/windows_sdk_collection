/*===========================================================================*\
|
|  File:        cgres.h
|
|  Description: 
|       resource loading classes
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#ifndef CGRES_H
#define CGRES_H

#include <windows.h>

#define MAX_STRING_LENGTH   255

class CGameStringResource
{
public:
    CGameStringResource( HINSTANCE, UINT );
    virtual ~CGameStringResource();

    char mString[MAX_STRING_LENGTH];
};

#endif // CGRES_H
