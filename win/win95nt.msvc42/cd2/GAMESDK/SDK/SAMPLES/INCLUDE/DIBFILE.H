/*===========================================================================*\
|
|  File:        dibfile.h
|
|  Description: 
|       Classes for saving DIB files
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#ifndef DIBFILE_H
#define DIBFILE_H

#include <windows.h>
#include "cgdib.h"

#if 0
class TempBackupFile
{
public:
    // constructor makes a temporary backup copy of the file
    TempBackupFile(char* pOrgFileName);

    // if Commit() never called, destructor restores the original file from the backup
    virtual ~TempBackupFile();

    void Commit();      // commit the org file by disgarding the temp backup

private:
    char* mpTempFileName;
    char* mpOrgFileName;
};
#endif

// writes a DIB file
class DIBWriteFile : public CGameDIB
{
public:
    // constructor creates the DIB file & the DIBWriteFile object
    DIBWriteFile(char* pFileName, LPBITMAPINFO pBmi);

    // destructor closes the file & deletes the DIBWriteFile object
    virtual ~DIBWriteFile();

#if 0
    HBITMAP GetHBitmap()
    {
        return mhBitmap;
    }

    LPVOID GetBits()
    {
        return mpBits;
    }
#endif

    void Write();

//  void SetColorTable( LPBITMAPINFO pBmi );
    void SetColorTable( CGameDIB* pDIB );
    void FillDIB( BYTE color );     // fill the DIB with color

private:
//  TempBackupFile* mpTempBackup;
//  HBITMAP mhBitmap;
//  LPBITMAPINFO mpInfo;    // ptr to our BITMAPINFO
//  LPVOID mpBits;          // ptr to DIB bits

//  char* mpFileName;
};

#endif // DIBFILE_H
