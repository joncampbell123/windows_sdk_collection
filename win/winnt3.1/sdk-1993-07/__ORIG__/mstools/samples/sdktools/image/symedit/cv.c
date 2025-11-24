/*++


Copyright (c) 1992  Microsoft Corporation

Module Name:

    cv.c

Abstract:

    This module handles the conversion activities requires for converting
    COFF debug data to CODEVIEW debug data.

Author:

    Wesley A. Witt (wesw) 19-April-1993

Environment:

    Win32, User Mode

--*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cv.h"
#include "symcvt.h"
#include "cvcommon.h"

typedef struct tagOFFSETSORT {
    DWORD       dwOffset;          // offset for the symbol
    DWORD       dwSection;         // section number of the symbol
    DATASYM32   *dataSym;          // pointer to the symbol info
} OFFSETSORT;


#define n_name          N.ShortName
#define n_zeroes        N.Name.Short
#define n_nptr          N.LongName[1]
#define n_offset        N.Name.Long

static VOID   GetSymName( PIMAGE_SYMBOL Symbol, PUCHAR StringTable, char *s );
DWORD  CreateModulesFromCoff( PPOINTERS p );
DWORD  CreatePublicsFromCoff( PPOINTERS p );
DWORD  CreateSegMapFromCoff( PPOINTERS p );

BOOL
ConvertCoffToCv( PPOINTERS p )

/*++

Routine Description:

    This is the control function for the conversion of COFF to CODEVIEW
    debug data.  It calls individual functions for the conversion of
    specific types of debug data.


Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    TRUE     - conversion succeded
    FALSE    - conversion failed

--*/

{
    p->pCvCurr = p->pCvStart.ptr = malloc( p->iptrs.fsize );
    if (p->pCvStart.ptr == NULL) {
        return FALSE;
    }
    memset( p->pCvStart.ptr, 0, p->iptrs.fsize );

    try {
        CreateSignature( p );
        CreateModulesFromCoff( p );
        CreatePublicsFromCoff( p );
        CreateSymbolHashTable( p );
        CreateAddressSortTable( p );
        CreateSegMapFromCoff( p );
        CreateDirectories( p );
    } except (EXCEPTION_EXECUTE_HANDLER) {
        free( p->pCvStart.ptr );
        p->pCvStart.ptr = NULL;
        return FALSE;
    }

    p->pCvStart.ptr = realloc( p->pCvStart.ptr, p->pCvStart.size );

    return TRUE;
}


DWORD
CreateModulesFromCoff( PPOINTERS p )

/*++

Routine Description:

    Creates the individual CV module records.  There is one CV module
    record for each .FILE record in the COFF debug data.  This is true
    even if the COFF size is zero.


Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    The number of modules that were created.

--*/

{
    int                 i;
    DWORD               numaux;
    DWORD               nummods = 0;
    char                szSymName[256];
    PIMAGE_SYMBOL       Symbol;
    PIMAGE_AUX_SYMBOL   AuxSymbol;
    OMFModule           *m;
    int                 cSeg;
    char *              pb;
    BOOLEAN             rgfCode[100];

    memset(rgfCode, 2, sizeof(rgfCode));

    m = NULL;

    for (i= 0, Symbol = p->iptrs.AllSymbols;
         i < (int) p->iptrs.numberOfSymbols;
         i += numaux + 1, Symbol += numaux + 1) {

        /*
         *  Get the number of aux symbol records for this symbol
         */
        
        numaux = Symbol->NumberOfAuxSymbols;
        AuxSymbol = (PIMAGE_AUX_SYMBOL) (Symbol+1);

        /*
         *  If this is a FILE record -- then we need to create a
         *      module item to correspond to this file record.
         */
        
        if (Symbol->StorageClass == IMAGE_SYM_CLASS_FILE) {
            if (m == NULL) {
                m = (OMFModule *) p->pCvCurr;
            } else {
                /*
                 *      Clean up the last item,  if we saw any
                 *      section records then drop them in here
                 */
                 
                if (cSeg > 0) {
                    m->cSeg  = cSeg;
                    pb = (char *) &m->SegInfo[cSeg];
                    *pb = strlen(szSymName);
                    memcpy(pb+1, szSymName, *pb);
                
                    m = NextMod(m);
                    nummods++;
                }
            }

            cSeg = 0;
            m->ovlNumber        = 0;
            m->iLib             = 0;
            m->Style[0]         = 'C';
            m->Style[1]         = 'V';

            /*
             *  Save off the file name to use when we have finished
             *  processing this module
             */

            memcpy(szSymName, (char *)AuxSymbol, numaux*sizeof(IMAGE_AUX_SYMBOL));
            szSymName[numaux*sizeof(IMAGE_AUX_SYMBOL)] = 0;
            
        }
        /*
         *  We have found a "SECTION" record.  Add the info to the
         *      module record
         */
        else if ((Symbol->SectionNumber & 0xffff) > 0xfff0) {
            continue;
        } else if (Symbol->SectionNumber > sizeof(rgfCode)/sizeof(rgfCode[0])) {
            return 0;
        } else if ((m != NULL) &&
                 (rgfCode[Symbol->SectionNumber] != 0) &&
                 (Symbol->StorageClass == IMAGE_SYM_CLASS_STATIC) &&
                 ((*Symbol->n_name == '.') ||
                  (Symbol->Type == IMAGE_SYM_TYPE_NULL)) &&
                 (Symbol->NumberOfAuxSymbols == 1) &&
                 (AuxSymbol->Section.Length != 0)) {

            if (rgfCode[Symbol->SectionNumber] == 2) {
                if ((p->iptrs.sectionHdrs[Symbol->SectionNumber - 1].
                    Characteristics & IMAGE_SCN_CNT_CODE) == 0) {
                    rgfCode[Symbol->SectionNumber] = 0;
                    continue;
                }
                rgfCode[Symbol->SectionNumber] = 1;
            }

            m->SegInfo[cSeg].Seg = Symbol->SectionNumber;
            m->SegInfo[cSeg].cbSeg = AuxSymbol->Section.Length;
            m->SegInfo[cSeg].Off = Symbol->Value -
                     p->iptrs.sectionHdrs[Symbol->SectionNumber-1].
                       VirtualAddress;
            cSeg += 1;
        }
    }

    /*
     *  Wrap up the last possible open module record
     */
    
    if (m != NULL) {
        if (cSeg > 0) {
            m->cSeg             = cSeg;
            pb = (char *) &m->SegInfo[cSeg];
            *pb = strlen(szSymName);
            memcpy(pb+1, szSymName, *pb);
                
            m = NextMod(m);
            nummods++;
        }
    }

    UpdatePtrs( p, &p->pCvModules, (LPVOID)m, nummods );

    return nummods;
}


DWORD
CreatePublicsFromCoff( PPOINTERS p )

/*++

Routine Description:

    Creates the individual CV public symbol records.  There is one CV
    public record created for each COFF symbol that is marked as EXTERNAL
    and has a section number greater than zero.  The resulting CV publics
    are sorted by section and offset.


Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    The number of publics created.

--*/

{
    int                 i;
    DWORD               numaux;
    DWORD               numsyms = 0;
    char                szSymName[256];
    PIMAGE_SYMBOL       Symbol;
    OMFSymHash          *omfSymHash;
    DATASYM32           *dataSym;
    DATASYM32           *dataSym2;

    omfSymHash = (OMFSymHash *) p->pCvCurr;
    dataSym = (DATASYM32 *) (PUCHAR)((DWORD)omfSymHash + sizeof(OMFSymHash));

    for (i= 0, Symbol = p->iptrs.AllSymbols;
         i < p->iptrs.numberOfSymbols;
         i += numaux + 1, Symbol += numaux + 1) {

        if ((Symbol->StorageClass == IMAGE_SYM_CLASS_EXTERNAL) &&
            (Symbol->SectionNumber > 0)) {
            
            GetSymName( Symbol, p->iptrs.stringTable, szSymName );
            dataSym->rectyp = S_PUB32;
            dataSym->seg = Symbol->SectionNumber;
            dataSym->off = Symbol->Value -
              p->iptrs.sectionHdrs[Symbol->SectionNumber-1].VirtualAddress;
            dataSym->typind = 0;
            dataSym->name[0] = strlen( szSymName );
            strcpy( &dataSym->name[1], szSymName );
            dataSym2 = NextSym32( dataSym );
            dataSym->reclen = (USHORT) ((DWORD)dataSym2 - (DWORD)dataSym) - 2;
            dataSym = dataSym2;
            numsyms += 1;
        }
        numaux = Symbol->NumberOfAuxSymbols;
    }

    UpdatePtrs( p, &p->pCvPublics, (LPVOID)dataSym, numsyms );

    omfSymHash->cbSymbol = p->pCvPublics.size - sizeof(OMFSymHash);
    omfSymHash->symhash  = 0;
    omfSymHash->addrhash = 0;
    omfSymHash->cbHSym   = 0;
    omfSymHash->cbHAddr  = 0;

    return numsyms;
}                               /* CreatePublisFromCoff() */


DWORD
CreateSegMapFromCoff( PPOINTERS p )

/*++

Routine Description:

    Creates the CV segment map.  The segment map is used by debuggers
    to aid in address lookups.  One segment is created for each COFF
    section in the image.

Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    The number of segments in the map.

--*/

{
    int                         i;
    SGM                         *sgm;
    SGI                         *sgi;
    PIMAGE_SECTION_HEADER       sh;


    sgm = (SGM *) p->pCvCurr;
    sgi = (SGI *) ((DWORD)p->pCvCurr + sizeof(SGM));

    sgm->cSeg = p->iptrs.numberOfSections;
    sgm->cSegLog = p->iptrs.numberOfSections;

    sh = p->iptrs.sectionHdrs;

    for (i=0; i<p->iptrs.numberOfSections; i++, sh++) {
        sgi->sgf.fRead        = (USHORT) (sh->Characteristics & IMAGE_SCN_MEM_READ) ==    IMAGE_SCN_MEM_READ;
        sgi->sgf.fWrite       = (USHORT) (sh->Characteristics & IMAGE_SCN_MEM_WRITE) ==   IMAGE_SCN_MEM_WRITE;
        sgi->sgf.fExecute     = (USHORT) (sh->Characteristics & IMAGE_SCN_MEM_EXECUTE) == IMAGE_SCN_MEM_EXECUTE;
        sgi->sgf.f32Bit       = 1;
        sgi->sgf.fSel         = 0;
        sgi->sgf.fAbs         = 0;
        sgi->sgf.fGroup       = 1;
        sgi->iovl             = 0;
        sgi->igr              = 0;
        sgi->isgPhy           = (USHORT) i + 1;
        sgi->isegName         = 0;
        sgi->iclassName       = 0;
        sgi->doffseg          = 0;
        sgi->cbSeg            = sh->SizeOfRawData;
        sgi++;
    }

    UpdatePtrs( p, &p->pCvSegMap, (LPVOID)sgi, i );

    return i;
}


void
GetSymName( PIMAGE_SYMBOL Symbol, PUCHAR StringTable, char *s )

/*++

Routine Description:

    Extracts the COFF symbol from the image symbol pointer and puts
    the ascii text in the character pointer passed in.


Arguments:

    Symbol        - COFF Symbol Record
    StringTable   - COFF string table
    s             - buffer for the symbol string


Return Value:

    void

--*/

{
    DWORD i;

    if (Symbol->n_zeroes) {
        for (i=0; i<8; i++) {
            if ((Symbol->n_name[i]>0x1f) && (Symbol->n_name[i]<0x7f)) {
                *s++ = Symbol->n_name[i];
            }
        }
        *s = 0;
    }
    else {
        strcpy( s, &StringTable[Symbol->n_offset] );
    }
}
