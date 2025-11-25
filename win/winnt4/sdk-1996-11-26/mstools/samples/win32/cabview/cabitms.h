//*******************************************************************************************
//
// Filename : CabItms.h
//	
//				Definitions of CCabItems and CCabExtract
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#ifndef _CABITMS_H_
#define _CABITMS_H_

#include "fdi.h"

class CCabItems
{
public:
	typedef void (CALLBACK *PFNCABITEM)(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam);

	CCabItems(LPSTR szCabFile) {lstrcpyn(m_szCabFile, szCabFile, sizeof(m_szCabFile));}
	~CCabItems() {}

	BOOL EnumItems(PFNCABITEM pfnCallBack, LPARAM lParam);

private:
	char m_szCabFile[MAX_PATH];
} ;

class CCabExtract
{
public:
	#define DIR_MEM ((LPCSTR)1)

	#define EXTRACT_FALSE ((HGLOBAL *)0)
	#define EXTRACT_TRUE ((HGLOBAL *)1)

	typedef HGLOBAL * (CALLBACK *PFNCABEXTRACT)(LPCSTR pszFile, DWORD dwSize, UINT date,
		UINT time, UINT attribs, LPARAM lParam);

	CCabExtract(LPSTR szCabFile) {lstrcpyn(m_szCabFile, szCabFile, sizeof(m_szCabFile));}
	~CCabExtract() {}

	BOOL ExtractItems(HWND hwndOwner, LPCSTR szDir, PFNCABEXTRACT pfnCallBack, LPARAM lParam);

private:
	char m_szCabFile[MAX_PATH];
} ;

#endif // _CABITMS_H_
