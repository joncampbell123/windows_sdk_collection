//*******************************************************************************************
//
// Filename : Os.h
//	
//				CFileTime
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#ifndef _OS_H_
#define _OS_H_

class CFileTime
{
public:
	CFileTime() {}
	~CFileTime() {}

	static void DateTimeToString(WORD wDate, WORD wTime, LPSTR pszText);

private:
	static void FileTimeToDateTimeString(LPFILETIME lpft, LPSTR pszText);
} ;

#endif // _OS_H_
