/*===========================================================================*\
|
|  File:        strrec.cpp
|
|  Description: 
|       string record parser class implementation
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#include <windows.h>
#include <string.h>
#include "strrec.h"

/*---------------------------------------------------------------------------*\
|
|       Class CStringRecord
|
|  DESCRIPTION:
|       parses a delimited string into fields
|
|
\*---------------------------------------------------------------------------*/
CStringRecord::CStringRecord(
    char* pString,
    char* pDelims
    ) : mpFields( NULL ),
        mNumFields( 0 )
{
    char* tempFields[MAX_FIELDS];

    char* pWork = new char[lstrlen(pString)+1];
    lstrcpy(pWork, pString);

    char* pLook = strtok( pWork, pDelims );

    while (pLook && mNumFields < MAX_FIELDS)
    {
        tempFields[mNumFields] = new char[lstrlen(pLook)+1];
        lstrcpy(tempFields[mNumFields], pLook);

        pLook = strtok( NULL, pDelims);
        ++mNumFields;
    }

    // now save the fields array
    mpFields = new char*[mNumFields];

    for (int i=0; i<mNumFields; i++)
    {
        mpFields[i] = new char[lstrlen(tempFields[i])+1];
        lstrcpy(mpFields[i], tempFields[i]);
    }
}

CStringRecord::~CStringRecord()
{
    if (mpFields)
    {
        for (; mNumFields > 0; mNumFields--)
        {
            delete[] mpFields[mNumFields-1];
        }

        delete[] mpFields;
    }
}

char*
CStringRecord::operator[](int field)
{
    char* pResult = NULL;

    if (field < mNumFields)
    {
        pResult = mpFields[field];
    }

    return pResult;
}

