
/*---------------------------- getdata.c ---------------------------------*/

// ascii code indicating end of a text resource
// this will be replaced with a NULL by the NullTerminateData function
// there must be TWO TERMCHARs in a row in order for NullTerminateData
// to work (since USER caches the resource in memory)

#define TERMCHAR    3

BOOL	FAR PASCAL GetImageRect(PPRINTER, LPPSDEVMODE, int, LPRECT);
BOOL	FAR PASCAL PaperSupported(PPRINTER pPrinter, LPPSDEVMODE lpdm, int iPaper);
PPRINTER FAR PASCAL GetPrinter(short);
int    FAR PASCAL FindPaperType(PPRINTER pPrinter, LPPSDEVMODE lpdm, int iPaper, PAPER_REC FAR* lpPaper);
int    FAR PASCAL GetPaperEntry(PPRINTER pPrinter, LPPSDEVMODE lpdm, int iPaperIndex);
int     FAR PASCAL GetNumPapers(PPRINTER pPrinter);

#define FreePrinter(pPrinter)	LocalUnlock(LocalHandle((WORD)pPrinter))

LPSTR	FAR PASCAL GetResourceData(LPHANDLE,LPSTR,LPSTR);
BOOL	FAR PASCAL UnGetResourceData(HANDLE);
BOOL	FAR PASCAL DumpPSS(LPDV,short,short,short);
BOOL	FAR PASCAL DumpResourceString(LPDV,short,short);
void    FAR PASCAL NullTerminateData(BYTE FAR *, int);

