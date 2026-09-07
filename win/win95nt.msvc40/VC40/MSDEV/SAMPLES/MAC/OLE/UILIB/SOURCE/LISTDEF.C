/*
 * LISTDEF.C
 *
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif


#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif

#include <fonts.h>
#include <standardFile.h>


#ifndef THINK_C
#include <Lists.h>
#include <Quickdraw.h>
#endif

#include <TextEdit.h>
#include <ole2.h>


#include "ole2ui.h"
#include "links.h"
#include "common.h"
OLEDBGDATA


#ifndef _MSC_VER
pascal void
#else
void __pascal
#endif
#if defined(__powerc) || defined(__MWERKS__)
EditLinksLDEF(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell,
	 short lDataOffset, short lDataLen, ListHandle lHandle)
#else
main(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell,
	 short lDataOffset, short lDataLen, ListHandle lHandle)
#endif
{
#ifndef _MSC_VER
#pragma unused(lDataOffset);
#endif

	GrafPtr			gpSaved;
	short			txFont;
	LPLINKINFO		pLinkInfo;
	short			nRight;
	char			cch;
#ifdef UIDLL
   void*       oldQD = SetLpqdFromA5();
#endif

	switch (lMessage) {

		case lInitMsg:
			break;

		case lDrawMsg:
		case lHiliteMsg:
			if (lDataLen > 0) {
				GetPort(&gpSaved);
				SetPort((**lHandle).port);

				txFont = (**lHandle).port->txFont;
				TextFont(geneva);

				lDataLen = sizeof(pLinkInfo);
				LGetCell((Ptr)&pLinkInfo, &lDataLen, lCell, lHandle);

				ForeColor(lSelect ? blackColor : whiteColor);
				PaintRect(lRect);
				ForeColor(blackColor);

				if (lSelect) {
					ForeColor(whiteColor);
					BackColor(blackColor);
				}

				//Remember where the right edge is
				nRight = lRect->right;

				lRect->left  += pLinkInfo->nColPos[0];
				lRect->right  = lRect->left + pLinkInfo->nColPos[1] - pLinkInfo->nColPos[0];

				if (pLinkInfo->lpszChoppedName != NULL)
				{
					for (cch=0; pLinkInfo->lpszChoppedName[cch]; ++cch)
						;

					TextBox(pLinkInfo->lpszChoppedName, cch, lRect, teFlushLeft);
				}
				else
					EraseRect(lRect);

				lRect->left   += pLinkInfo->nColPos[1] - pLinkInfo->nColPos[0];
				lRect->right  += pLinkInfo->nColPos[2] - pLinkInfo->nColPos[1];

				if (pLinkInfo->lpszChoppedLink != NULL)
				{
					for (cch=0; pLinkInfo->lpszChoppedLink[cch]; ++cch)
						;

					TextBox(pLinkInfo->lpszChoppedLink, cch, lRect, teFlushLeft);
				}
				else
					EraseRect(lRect);

				lRect->left   += pLinkInfo->nColPos[2] - pLinkInfo->nColPos[1];
				lRect->right   = nRight;

				if (pLinkInfo->lpszAMX != NULL)
				{
					for (cch=0; pLinkInfo->lpszAMX[cch]; ++cch)
						;

					TextBox(pLinkInfo->lpszAMX, cch, lRect, teFlushLeft);
				}
				else
					EraseRect(lRect);

				if (lSelect) {
					ForeColor(blackColor);
					BackColor(whiteColor);
				}

				TextFont(txFont);
				SetPort(gpSaved);
			}
			break;

		case lCloseMsg:
			break;
	}
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
}
