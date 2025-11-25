/*===========================================================================*\
|
|  File:        cgexcpt.h
|
|  Description: 
|       Exception classes
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#ifndef CGEXCPT_H
#define CGEXCPT_H

#include <windows.h>

class CGameException
{
public:
    CGameException(int resID = 0);  // 0 == no message
    virtual ~CGameException();

    virtual int GetResID(){return mResID;}

protected:
    int mResID;
};

class CGameStartupError : public CGameException
{
public:
    CGameStartupError(int resID = 0);   // 0 == no message
    virtual ~CGameStartupError();
};

class CGameResourceError : public CGameException
{
public:
    CGameResourceError();
    virtual ~CGameResourceError();
};

#endif // CGEXCPT_H
