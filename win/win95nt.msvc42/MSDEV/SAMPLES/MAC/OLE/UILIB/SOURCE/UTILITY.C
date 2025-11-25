/*
 * UTILITY.C
 *
 * Utility routines for functions inside OLE2UI.DLL
 *
 *  Misc Tools:
 *  ----------------------
 *  ChopText                Chop a file path to fit within a specified width
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#ifndef _MSC_VER
#include <QuickDraw.h>
#else
#include <QuickDra.h>
#endif

#include <stdio.h>
#include <string.h>

#include <ole2.h>


#include "olestd.h"
#include "uidebug.h"
#include "utility.h"


OLEDBGDATA


#ifdef _DEBUG
void __UIASSERTCONDSZ(char *expr, char *sz, char *file, int line);
#endif


/*
 * ChopText
 *
 * Purpose:
 *  Parse a string (pathname) and convert it to be within a specified
 *  length by chopping the least significant part
 *
 * Parameters:
 *  nWidth          max width of string in pixels
 *  pch             pointer to beginning of the string
 *
 * Return Value:
 *  pointer to the modified string
 */

#ifndef _MSC_VER
#pragma segment UtilitySeg
#else
#pragma code_seg("UtilitySeg", "SWAPPABLE")
#endif
char *ChopText(int nWidth, char *pch)
{
#ifdef OLD_METHOD
	char		szPrefix[38];
	char		*lpsz;
	Boolean		fDone = false;
	int			i;
	Rect		rc;
#endif

	if (!nWidth || !pch)
		return NULL;

#ifdef OLD_METHOD
	/* check horizontal extent of string */
	if (TextWidth(pch, 0, strlen(pch)) > nWidth)
	{
		/* string is too long to fit in static control; chop it */
		/* set up new prefix & determine remaining space in control */
		BlockMove(pch, szPrefix, sizeof(szPrefix));

		lpsz = szPrefix;
		while (*lpsz && (*lpsz++ != ':'));
		strcpy(lpsz, ".:");

		nWidth -= TextWidth(szPrefix, 0, strlen(szPrefix));
	
		/*
		** advance a directory at a time until the remainder of the
		** string fits into the static control after the "hd:.:" prefix
		*/
		while (!fDone) {

			while (*pch && (*pch++ != ':'));

			if (!*pch || TextWidth(pch, 0, strlen(pch)) <= nWidth) {
				if (!*pch)
					/*
					** Nothing could fit after the prefix; remove the
					** final ":" from the prefix
					*/
					szPrefix[strlen(szPrefix) - 1] = 0;
				else
					/* rest of string fits -- stick prefix on front */
					for (i = strlen(szPrefix) - 1; i >= 0; --i)
						*--pch = szPrefix[i];
					fDone = true;
			}
		}
	}
#else
	if (TextWidth(pch, 0, (short)strlen(pch)) > nWidth)
	{
		pch[strlen(pch) - 1] = '.';
		while (strlen(pch) > 2 && (TextWidth(pch, 0, (short)strlen(pch)) > nWidth || pch[strlen(pch) - 2] == ' '))
		{
			pch[(short)(strlen(pch) - 2)] = '.';
			pch[(short)(strlen(pch) - 1)] = 0;
		}
	}
#endif

	return pch;
}

#ifdef _DEBUG
void __UIASSERTCONDSZ(char *expr, char *sz, char *file, int line)
{
	char	s[255];

	if (sz != NULL)
		sprintf(s, "%s(%ld): %s (%s)", file, line, expr, sz);
	else
		sprintf(s, "%s(%ld): %s", file, line, expr);

	DebugStr((StringPtr)c2pstr(s));
}
#endif
