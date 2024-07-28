/*
 * control2.h
 *
 */

PPAPER	FAR PASCAL GetPaperMetrics(PPRINTER,LPPSDEVMODE);
short	FAR PASCAL EnumPaperMetrics(LPDV, LPRECT, short, short);
short	FAR PASCAL SetPaperMetrics(LPDV, LPPSDEVMODE, LPRECT);
int FAR PASCAL StartNewPage(LPDV lpdv);

