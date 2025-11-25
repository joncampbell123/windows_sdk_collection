//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// texttype.h
//

// A CMediaType that can return itself as text.

class CTextMediaType : public CMediaType {

public:

    CTextMediaType(AM_MEDIA_TYPE mt):CMediaType(mt) {}

    void AsText(LPTSTR szType,
                unsigned int iLen,
                LPTSTR szAfterMajor,
                LPTSTR szAfterOthers,
                LPTSTR szAtEnd);

private:

    // Provide a string description for this format block

    void Format2String(LPTSTR szBuffer,
                       UINT iLength,
                       const GUID* pGuid,
                       BYTE* pFormat,
                       ULONG lFormatLength);

    // Convert this CLSID into a meaningful string

    void CLSID2String(LPTSTR szBuffer,
                       UINT iLength,
                       const GUID* pGuid);
};

