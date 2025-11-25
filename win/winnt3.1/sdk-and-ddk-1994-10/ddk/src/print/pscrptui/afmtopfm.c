//--------------------------------------------------------------------------
//
// Module Name:  AFMTOPFM.C
//
// This module of the afm compiler parses the afm file and collects
// information in the NTfM structure.  It then passes control to the
// pfm module which outputs the pfm file.
//
// Author:  Kent Settle (kentse)
// Created: 18-Mar-1991
//
// Copyright (c) 1988 - 1993 Microsoft Corporation
//--------------------------------------------------------------------------

#include <string.h>
#include "pscript.h"
#include "mapping.h"
#include "pscrptui.h"

int     _fltused;   // HEY, it shut's up the linker.  That's why it's here.

//#define   ALL_METRICS

// external declarations.

extern VOID InitPfm(PPARSEDATA);
extern VOID ParseAfm(PPARSEDATA);
extern BOOL GetFirstLastChar(PPARSEDATA);
extern VOID SetWidths(PPARSEDATA);
extern BOOL WritePFM(PWSTR, PPARSEDATA);
extern VOID BuildNTFM(PPARSEDATA, PSTR);

//--------------------------------------------------------------------------
// BOOL CreatePFMFromAFM(pwstrAFMFile, pwstrPFMFile)
// PSZ	pszAFMFile;
// PSZ	pszPFMFile;
//
// Returns:
//   This routine returns TRUE for success, FALSE otherwise.
//
// History:
//   15-Jan-1992    -by-    Kent Settle     (kentse)
//  Modified to become part of PSCRPTUI.
//   20-Mar-1991    -by-    Kent Settle    (kentse)
//  ReWrote it, got rid of PFM.C and CHARCODE.C.
//   18-Mar-1991    -by-    Kent Settle    (kentse)
//  Brought in from PM, and cleaned up.
//--------------------------------------------------------------------------

BOOL CreatePFMFromAFM(pwstrAFMFile, pwstrPFMFile)
PWSTR   pwstrAFMFile;
PWSTR   pwstrPFMFile;
{
    WCHAR       wcbuf[MAX_PATH];
    PWSTR       pwstrPFAFile;
    CHAR       *pPFA;
    PPARSEDATA  pdata;
    BOOL        bReturn;

    // allocate memory for parsing data.

    if (!(pdata = (PPARSEDATA)LocalAlloc((LMEM_FIXED | LMEM_ZEROINIT),
                                         sizeof(PARSEDATA))))
    {
        RIP("PSCRPTUI!CreatePFMFromAFM: LocalAlloc for pdata failed.\n");
	return(FALSE);
    }

    // allocate temporary storage to build metrics into.

    if (!(pdata->pntfm = (PNTFM)LocalAlloc((LMEM_FIXED | LMEM_ZEROINIT), INIT_PFM)))
    {
        RIP("PSCRPTUI!CreatePFMFromAFM: LocalAlloc for pdata->pntfm failed.\n");
        LocalFree((LOCALHANDLE)pdata);
	return(FALSE);
    }

    // build the IFIMETRICS structure in temporary storage, until we
    // know its exact size.

    if (!(pdata->pTmpIFI = (PIFIMETRICS)LocalAlloc((LMEM_FIXED | LMEM_ZEROINIT),
                                                   INIT_IFI)))
    {
        RIP("PSCRPTUI!CreatePFMFromAFM: LocalAlloc for pTmpIFI failed.\n");
        LocalFree((LOCALHANDLE)pdata->pntfm);
        LocalFree((LOCALHANDLE)pdata);
        return(FALSE);
    }

    // initialize the NTFM structure.

    InitPfm(pdata);

    // open AFM file for input.

    pdata->hFile = CreateFile(pwstrAFMFile, GENERIC_READ,
                          FILE_SHARE_READ, NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);

    if (pdata->hFile == INVALID_HANDLE_VALUE)
    {
#if DBG
	DbgPrint("PSCRPTUI!CreatePFMFromAFM:  CreateFile for %s failed.\n",
		 pwstrAFMFile);
#endif
        LocalFree((LOCALHANDLE)pdata->pntfm);
        LocalFree((LOCALHANDLE)pdata);
        return(FALSE);
    }

    // parse the AFM file, filling in the NTFM structure.

    ParseAfm(pdata);

    // close the AFM file.

    if (!CloseHandle(pdata->hFile))
	RIP("PSCRPTUI!CreatePFMFromAFM: CloseHandle failed to close afm file.\n");

    // open the corresponding .PFA file, which had been created just
    // prior to calling this routine.

    wcsncpy(wcbuf, pwstrPFMFile, (sizeof(wcbuf) / 2));
    pwstrPFAFile = wcbuf;

    while(*pwstrPFAFile)
	pwstrPFAFile++;

    pwstrPFAFile--;
    *pwstrPFAFile = (WCHAR)'A';

    // reset pointer.

    pwstrPFAFile = wcbuf;

    if (!(pPFA = MapFile(pwstrPFAFile)))
    {
	RIP("PSCRPTUI!CreatePFMFromAFM: MapFile failed.\n");
        LocalFree((LOCALHANDLE)pdata->pntfm);
        LocalFree((LOCALHANDLE)pdata->pTmpIFI);
        LocalFree((LOCALHANDLE)pdata);
        return(FALSE);
    }

    BuildNTFM(pdata, pPFA);

    // we are, in fact, done with the .PFA file, so unmap it, and
    // even delete it.

    UnmapViewOfFile((PVOID)pPFA);
    DeleteFile(pwstrPFAFile);

    // create the PFM file from the NTFM structure.

#ifdef ALL_METRICS
    DbgPrint("Size of PFM file = %d\n", pntfm->cjThis);
#endif

    bReturn = WritePFM(pwstrPFMFile, pdata);

    LocalFree((LOCALHANDLE)pdata->pntfm);
    LocalFree((LOCALHANDLE)pdata->pTmpIFI);
    LocalFree((LOCALHANDLE)pdata);

    return(bReturn);
}
