/**[f******************************************************************
 * psoption.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

BOOL FAR PASCAL fnOptionsDialog(HWND, unsigned, WORD, LONG);
LPDV FAR PASCAL StartTmpJob(LPSTR lpFileName, LPSTR lpTitle, PPRINTER pPrinter);
void FAR PASCAL EndTempJob(LPDV lpdv, PPRINTER pPrinter);
BOOL FAR PASCAL ConfigureCommPort(void);
LONG FAR PASCAL FindLBIndex(HWND hwndList, LPSTR lpStr);

