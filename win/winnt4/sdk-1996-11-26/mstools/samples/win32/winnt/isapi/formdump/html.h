// html.h - Some HTML authoring functions 


// Direct write of text, no translation
void WriteString (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz);

// Required page definition functions
void HtmlCreatePage (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszTitle);
void HtmlEndPage (EXTENSION_CONTROL_BLOCK *pECB);

// Rest of the calls are optional
void HtmlHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading, LPCTSTR lpszText);
void HtmlBeginHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading);
void HtmlEndHeading (EXTENSION_CONTROL_BLOCK *pECB, int nHeading);

void HtmlWriteTextLine (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz);
void HtmlWriteText (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpsz);
void HtmlEndParagraph (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlHyperLink (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszDoc, LPCTSTR lpszText);

void HtmlHyperLinkAndBookmark (EXTENSION_CONTROL_BLOCK *pECB, 
                               LPCTSTR lpszDoc, LPCTSTR lpszBookmark,
                               LPCTSTR lpszText);

void HtmlBookmarkLink (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszBookmark,
                       LPCTSTR lpszText);

void HtmlBeginListItem (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginUnnumberedList (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndUnnumberedList (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginNumberedList (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndNumberedList (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginDefinitionList (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndDefinitionList (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlDefinition (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszTerm,
                     LPTSTR lpszDef);

void HtmlBeginDefinitionTerm (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlBeginDefinition (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginPreformattedText (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndPreformattedText (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginBlockQuote (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndBlockQuote (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginAddress (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndAddress (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginDefine (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndDefine (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginEmphasis (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndEmphasis (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginCitation (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndCitation (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginCode (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndCode (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginKeyboard (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndKeyboard (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginStatus (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndStatus (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginStrong (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndString (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBeginVariable (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndVariable (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlBold (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText);
void HtmlBeginBold (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndBold (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlItalic (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText);
void HtmlBeginItalic (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndItalic (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlFixed (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszText);
void HtmlBeginFixed (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlEndFixed (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlLineBreak (EXTENSION_CONTROL_BLOCK *pECB);
void HtmlHorizontalRule (EXTENSION_CONTROL_BLOCK *pECB);

void HtmlImage (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszPicFile,
                LPCTSTR lpszAltText);

void HtmlPrintf (EXTENSION_CONTROL_BLOCK *pECB, LPCTSTR lpszFormat, ...);
