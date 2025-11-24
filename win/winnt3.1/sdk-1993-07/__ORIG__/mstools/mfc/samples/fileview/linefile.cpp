// linefile.cpp : Defines the class behaviors for the line file class.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "fileview.h"

IMPLEMENT_DYNAMIC(CLineFile, CStdioFile)

CLineFile::~CLineFile()
{
}

CLineFile::CLineFile() : CStdioFile()
{
	m_lBeginLine = 0L;
}

CLineFile::CLineFile(const char* pszFileName, UINT nOpenFlags)
		: CStdioFile(pszFileName, nOpenFlags)
{
	m_lBeginLine = 0L;
}

//
// assuming the current file offset points to the beginning of a line
// read the next line, return offset of next line
// also expand tabs to spaces
//

#define TABNUM 8

LONG
CLineFile::NextLine(char FAR* lpszArg, UINT nMax)
{
	register char FAR * lpsz = lpszArg;
	register UINT n;

	m_lBeginLine = GetPosition();

	for (n=0;;)
	{
		if ( Read( lpsz, 1) != 1 )
		{
			break;
		}

		n++;

		if ( *lpsz == '\n' )
		{
			break;
		}

		if ( *lpsz == '\r' )
		{
			continue;
		}

		if ( *lpsz == '\t' )
		{
			for ( *lpsz++ = ' '; n%TABNUM && n < nMax; n++, lpsz++ )
			{
				 *lpsz = ' ';
			}
		}
		else
		{
			lpsz++;
		}

		if ( n >= nMax )
		{
			break;
		}
	}

	*lpsz = 0;

	return GetPosition();
}

LONG
CLineFile::SetBegin(LONG newBegin)
{
	LONG save = m_lBeginLine;

	m_lBeginLine = newBegin;

	return save;
}

//
// read backwards a few ( nLines ) lines
//

#define GUESS       100L
#define MAXLINES    200

LONG
CLineFile::BackLines(char FAR* lpsz, UINT nMax, UINT nLines)
{

	LONG    lines[ MAXLINES ];
	LONG    guess;
	register UINT i;

	if (m_lBeginLine == 0L)
	{
		Seek(0L, CFile::begin);
		return NextLine(lpsz, nMax);
	}

	if (nLines > MAXLINES)
	{
		nLines = MAXLINES;
	}

	guess = m_lBeginLine - ( GUESS * nLines );

	if (guess < 0L)
	{
		guess = 0L;
	}

	TRY
	{
		Seek( guess, CFile::begin);
	}
	CATCH( CFileException, e )
	{
		TRACE( "Bad Seek in BackLines %ld\n", guess );
		return -1;
	}
	END_CATCH

	if (guess != 0L)
	{
		ReadString( lpsz, nMax );       // skip forward to a line
	}

	for ( i = 0; i < nLines; i++ )
	{
		lines[ i ] = -1L;
	}

	for ( ; ; )
	{
		for ( i=MAXLINES-2; i > 0; i-- )
		{
			lines[ i ] = lines[ i - 1 ];
		}

		lines[ 0 ] = GetPosition();

		if ( lines[0] >= m_lBeginLine )
		{
			break;
		}

		ReadString( lpsz, nMax );       // skip forward a line
	}

	for ( ; lines[ nLines ] == -1L; nLines-- );

	Seek( lines[ nLines ], CFile::begin );

	NextLine( lpsz, nMax );
	m_lBeginLine = lines[ nLines ];

	return m_lBeginLine;
}

//
// read a line near some file offset
//

LONG
CLineFile::LineNear(char FAR* lpsz, UINT nMax, LONG  lOffset)
{
	TRY
	{
		Seek(lOffset, CFile::begin);
	}
	CATCH( CFileException, e )
	{
		TRACE("Bad Seek in LineNear %ld\n", lOffset);
		return -1;
	}
	END_CATCH

	m_lBeginLine = NextLine(lpsz, nMax);    // find a real line

	return BackLines( lpsz, nMax, 1 );      // one just before it
}
