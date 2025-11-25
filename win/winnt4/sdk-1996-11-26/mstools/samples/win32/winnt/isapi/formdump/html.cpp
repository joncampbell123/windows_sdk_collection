// html.cpp - Some HTML authoring functions

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <httpext.h>

#include <stdio.h>
#include <stdarg.h>

#include "html.h"

//
// WriteString writes an ASCII string to the web browser
//

void WriteString (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz)
    {
    DWORD dwBytesWritten;

    dwBytesWritten = lstrlen (lpsz);
    pECB->WriteClient (pECB->ConnID, (PVOID) lpsz, &dwBytesWritten, 0);
    }

//
// HtmlCreatePage adds <HTML> and a title
//

void HtmlCreatePage (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszTitle)
    {
    WriteString (pECB, TEXT("<HTML>\r\n\r\n"));

    if (lpszTitle)
        {
        WriteString (pECB, TEXT("<HEAD><TITLE>"));
        HtmlWriteText (pECB, lpszTitle);
        WriteString (pECB, TEXT("</TITLE></HEAD>\r\n\r\n"));
        }

    WriteString (pECB, TEXT("<BODY>\r\n\r\n"));
    }

void HtmlEndPage (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</BODY>\r\n\r\n"));
    WriteString (pECB, TEXT("</HTML>\r\n"));
    }


//
// Heading tools
//

void HtmlHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading, 
                  LPCTSTR lpszText)
    {
    HtmlBeginHeading (pECB, nHeading);
    HtmlWriteText (pECB, lpszText);
    HtmlEndHeading (pECB, nHeading);
    }

void HtmlBeginHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading)
    {
    TCHAR szCode[16];

    wsprintf (szCode, TEXT("<H%i>"), nHeading);
    WriteString (pECB, szCode);
    }

void HtmlEndHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading)
    {
    TCHAR szCode[16];

    wsprintf (szCode, TEXT("</H%i>"), nHeading);
    WriteString (pECB, szCode);
    }

//
// Text tools
//

void HtmlWriteTextLine (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz)
    {
    HtmlWriteText (pECB, lpsz);
    WriteString (pECB, TEXT("\r\n"));
    }

void HtmlWriteText (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz)
    {
    TCHAR szBuf[1028];
    int nLen;
    int i;

    //
    // Build up enough data to make call to WriteString
    // worthwhile; convert special chars too.
    //
    nLen = 0;
    for (i = 0 ; lpsz[i] ; i++)
        {
        if (lpsz[i] == TEXT('<'))
            lstrcpy (&szBuf[nLen], TEXT("&lt;"));
        else if (lpsz[i] == TEXT('>'))
            lstrcpy (&szBuf[nLen], TEXT("&gt;"));
        else if (lpsz[i] == TEXT('&'))
            lstrcpy (&szBuf[nLen], TEXT("&amp;"));
        else if (lpsz[i] == TEXT('\"'))
            lstrcpy (&szBuf[nLen], TEXT("&quot;"));
        else
            {
            szBuf[nLen] = lpsz[i];
            szBuf[nLen + 1] = 0;
            }

        nLen += lstrlen (&szBuf[nLen]);

        if (nLen >= 1024)
            {
            WriteString (pECB, szBuf);
            nLen = 0;
            }
        }

    if (nLen)
        WriteString (pECB, szBuf);
    }

void HtmlEndParagraph (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<P>\r\n"));
    }

//
// HtmlHyperLink adds a hyptertext link.  lpszDoc is the destination
// document, and lpszText is the display text.
//
// HtmlHyperLinkAndBookmark adds a hyperlink with a bookmark link.
// HtmlBookmarkLink adds only a bookmark link.
//

void HtmlHyperLink (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszDoc, LPCTSTR lpszText)
    {
    WriteString (pECB, TEXT("<A HREF=\""));
    HtmlWriteText (pECB, lpszDoc);
    WriteString (pECB, TEXT("\">"));
    HtmlWriteText (pECB, lpszText);
    WriteString (pECB, TEXT("</A>\r\n"));
    }
        
void HtmlHyperLinkAndBookmark (EXTENSION_CONTROL_BLOCK *pECB, 
                               LPCTSTR lpszDoc, LPCTSTR lpszBookmark,
                               LPCTSTR lpszText)
    {
    WriteString (pECB, TEXT("<A HREF=\""));
    if (lpszDoc)
        HtmlWriteText (pECB, lpszDoc);
    WriteString (pECB, TEXT("#"));
    HtmlWriteText (pECB, lpszBookmark);
    WriteString (pECB, TEXT("\">"));
    HtmlWriteText (pECB, lpszText);
    WriteString (pECB, TEXT("</A>\r\n"));
    }

void HtmlBookmarkLink (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszBookmark,
                       LPCTSTR lpszText)
    {
    HtmlHyperLinkAndBookmark (pECB, NULL, lpszBookmark, lpszText);
    }

//
// The following support list formatting.
//

void HtmlBeginUnnumberedList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<UL>\r\n"));
    }

void HtmlBeginListItem (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<LI>"));
    }

void HtmlEndUnnumberedList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</UL>"));
    }

void HtmlBeginNumberedList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<OL>"));
    }

void HtmlEndNumberedList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</OL>"));
    }
    
void HtmlBeginDefinitionList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<DL>"));
    }

void HtmlEndDefinitionList (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</DL>"));
    }

void HtmlDefinition (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszTerm,
                     LPTSTR lpszDef)
    {
    int nStart, nEnd, nLen;
    TCHAR tcHolder;

    WriteString (pECB, TEXT("<DT> "));
    HtmlWriteText (pECB, lpszTerm);
    WriteString (pECB, TEXT("\r\n"));
    WriteString (pECB, TEXT("<DD> "));

    nStart = 0 ;
    nLen = lstrlen (lpszDef);
    do  {
        nEnd = nStart + 70;
        if (nEnd >= nLen)
            {
            HtmlWriteText (pECB, &lpszDef[nStart]);
            WriteString (pECB, TEXT("\r\n"));
            break;
            }

        while (nEnd > nStart)
            if (lpszDef[nEnd] == TEXT(' '))
                break;

        if (nEnd == nStart)
            // too long!
            nEnd = nStart + 70;

        // write defintion segment
        tcHolder = lpszDef[nEnd];
        lpszDef[nEnd] = 0;
        HtmlWriteText (pECB, &lpszDef[nStart]);
        WriteString (pECB, TEXT("\r\n"));
        lpszDef[nEnd] = tcHolder;
        nStart = nEnd;

        // skip excess whitespace
        while (lpszDef[nStart] == TEXT(' '))
            nStart++;

        // pretty formatting
        if (nStart < nLen)
            WriteString (pECB, TEXT("     "));
        } while (nStart < nLen);
    }
        
// For complex defintions
void HtmlBeginDefinitionTerm (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<DT>"));
    }

void HtmlBeginDefinition (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<DD>"));
    }

//
// Text formatting
//

void HtmlBeginPreformattedText (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<PRE>"));
    }

void HtmlEndPreformattedText (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</PRE>"));
    }

void HtmlBeginBlockQuote (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<BLOCKQUOTE>"));
    }

void HtmlEndBlockQuote (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</BLOCKQUOTE>"));
    }

void HtmlBeginAddress (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<ADDRESS>"));
    }

void HtmlEndAddress (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</ADDRESS>"));
    }

void HtmlBeginDefine (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<DFN>"));
    }

void HtmlEndDefine (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</DFN>"));
    }

void HtmlBeginEmphasis (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<EM>"));
    }

void HtmlEndEmphasis (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</EM>"));
    }

void HtmlBeginCitation (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<CITE>"));
    }

void HtmlEndCitation (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</CITE>"));
    }

void HtmlBeginCode (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<CODE>"));
    }

void HtmlEndCode (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</CODE>"));
    }

void HtmlBeginKeyboard (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<KBD>"));
    }

void HtmlEndKeyboard (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</KBD>"));
    }

void HtmlBeginStatus (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<SAMP>"));
    }

void HtmlEndStatus (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</SAMP>"));
    }

void HtmlBeginStrong (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<STRONG>"));
    }

void HtmlEndString (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</STRONG>"));
    }

void HtmlBeginVariable (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<VAR>"));
    }

void HtmlEndVariable (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</VAR>"));
    }

void HtmlBold (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText)
    {
    HtmlBeginBold (pECB);
    HtmlWriteText (pECB, lpszText);
    HtmlEndBold (pECB);
    }

void HtmlBeginBold (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<B>"));
    }

void HtmlEndBold (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</B>"));
    }

void HtmlItalic (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText)
    {
    HtmlBeginItalic (pECB);
    HtmlWriteText (pECB, lpszText);
    HtmlEndItalic (pECB);
    }

void HtmlBeginItalic (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<I>"));
    }

void HtmlEndItalic (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</I>"));
    }

void HtmlFixed (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText)
    {
    HtmlBeginFixed (pECB);
    HtmlWriteText (pECB, lpszText);
    HtmlEndFixed (pECB);
    }

void HtmlBeginFixed (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<TT>"));
    }

void HtmlEndFixed (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("</TT>"));
    }


//
// Line breaks and other formatting
//

void HtmlLineBreak (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("<BR>\r\n"));
    } 

void HtmlHorizontalRule (EXTENSION_CONTROL_BLOCK *pECB)
    {
    WriteString (pECB, TEXT("\r\n<HR>\r\n"));
    }


//
// Images
//

void HtmlImage (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszPicFile,
                LPCTSTR lpszAltText)
    {
    WriteString (pECB, TEXT("<IMG SRC = \""));
    HtmlWriteText (pECB, lpszPicFile);
    WriteString (pECB, TEXT("\""));
    if (lpszAltText)
        {
        WriteString (pECB, TEXT(" ALT = \""));
        HtmlWriteText (pECB, lpszAltText);
        WriteString (pECB, TEXT("\""));
        }
    WriteString (pECB, TEXT(">\r\n"));
    }


//
// printf
//

void HtmlPrintf (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszFormat, ...)
	{
	TCHAR szBuf[8192];

	va_list list;
	va_start (list, lpszFormat);

	vsprintf (szBuf, lpszFormat, list);
	WriteString (pECB, szBuf);

	va_end (list);
	}
