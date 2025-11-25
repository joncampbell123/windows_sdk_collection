//--------------------------------------------------------------------------
//
// Module Name:  FONTTREE
//
// Brief Description:  Device font tree querying routines
//
// Author:  Kent Settle (kentse)
// Created: 18-Apr-1991
//
// Copyright (C) 1991 - 1992 Microsoft Corporation.
//
// This module contains DrvQueryFontTree.
//
// History:
//   18-Apr-1991    -by-    Kent Settle       (kentse)
// Created.
//--------------------------------------------------------------------------

#include "pscript.h"
#include "enable.h"
#include "mapping.h"
#include <string.h>

#define MAX_MAPSUBTABLES    256
#define MAX_GLYPHTABLES     16
#define MAX_GLYPHS          16

PVOID BuildGLYPHSET(PDEVDATA, ULONG);
PVOID BuildKernPairs(PDEVDATA, ULONG);

//--------------------------------------------------------------------------
// PVOID DrvQueryFontTree (dhpdev,iFile,iFace,iMode)
// DHPDEV    dhpdev;
// ULONG     iFile;
// ULONG     iFace;
// ULONG     iMode;
// ULONG     *pid;
// 
// This function is used by GDI to obtain pointers to tree structures that
// define one of the following.
// 
//     1    The mapping from Unicode to glyph handles, including glyph 
//          variants; 
//     2    The mapping of ligatures and their variants to glyph handles; 
//     3    The mapping of kerning pairs to kerning handles.
// 
// Parameters:
//   dhpdev    
//     This is a PDEV handle returned from an earlier call to DrvEnablePDEV.
//   iFile
//     This identifies the font file (if device supports loading font files
//     using the DDI).
//   iFace
//     This is the index of the driver font.  The index of the first font
//     is one.  
//   iMode
//     This defines the type of information to be returned.  This may take
//     one of the following values:
// 
//       QFT_UNICODE GDI requests a pointer to a TREE_UNICODE structure 
//                   that defines the mappings from single Unicode characters
//                   to glyph handles.  This includes all glyph variants.
// 
//       QFT_LIGATURES GDI requests a pointer to a TREE_LIGATURES 
//                     structure that defines the mapping from strings 
//                     of Unicode characters to glyph handles.  This 
//                     includes all ligature variants.
// 
//       QFT_KERNPAIRS GDI requests a pointer to a TREE_KERNPAIRS 
//                     structure that define the mappings from pairs 
//                     of Unicode characters to kerning pair handles.  
//                     These kerning handles are later passed to
//                     DrvQueryFontData to obtain kerning corrections.
// 
// Returns:
//   The return value is a pointer to the requested structure. This 
//   structure is to remain unchanged during the lifetime of the 
//   associated PDEV.
//
// Note:  The following explanation was "borrowed" from lindsayh and the
//        Generic driver.
//
//   COMMENTS ON THE UNICODE TREE STRUCTURES:
//	The following is intended to help understand the strange operations
//  being done in the following function.  UNICODE is a sparse encoding,
//  so a tree structure is used to store the data.  The following function
//  produces a tree,  the leaves of which are the GLYPH HANDLES that the
//  engine passes to us to identify perticular glyphs.  These handles are
//  32 bits long,  contents up to the driver.  We need to allocate storage
//  for these and fill it in.  The first step is to determine how much
//  storage is needed.  This requires understanding how the tree is built.
//	The tree has 3 levels.  At the top level there is a MAPTABLE
//  structure.  This has as many as 256 sub trees.  These correspond to
//  the 8 MSBs of the unicode value.  This structure is folllowed by up to
//  256 PTRDIFFs (each a long).  The structure itself contains a 256 byte
//  array:  this is an index into the array of PTRDIFFs following.  Hence,
//  there only needs to be as many PTRDIFFs as there are subtrees (plus 1,
//  for the "no entry" value).  For example,  if there is only subtree,
//  there need be only 2 PTRDIFFs - the active one,  and a 0 entry.  The
//  byte array will mostly contain the index to the 0 entry,  but the
//  active entry will point at the other PTRDIFF.
//
//	Each of the above active PTRDIFFs points to a MAPSUBTABLE.  Each of
//  these can handle 16 sub trees.  It uses the same method as the MAPTABLE
//  to handle sparseness - i.e. there is a 16 byte array containing
//  indices into the array of PTRDIFFs following.  The 16 byte array
//  is indexed by the next 4 bits in the UNICODE value (i.e. the high
//  nibble of the low byte).  These PTRDIFFs point to GLYPHTABLEs,
//  the next lower level in the tree.
//
//	The final layer is the GLYPHTABLE data.  Each of these contains
//  up to 16 GLYPH HANDLEs,  and is indexed by the 4 LSBs of the UNICODE
//  value.  Like the MAPSUBTABLE,  there is a byte array containing
//  index values of the glyph handles following.
//
//   HOW MUCH MEMORY IS NEEDED??
//  The tricky part is now determining how much memory to allocate for
//  these structures.  The answer is:  a DWORD for each GLYPH HANDLE,
//  as many GLYPHTABLEs as there are groups defined by the high order
//  12 bits of the UNICODE value (i.e., up to 4096),  as many MAPSUBTABLES
//  as there are groups of 16 GLYPHTABLES (up to 256), plus a MAPTABLE
//  to amalgamate the top level.  As well,  the MAPTABLE and MAPSUBTABLE
//  require PTRDIFFs to the lower level - this is as many as there are
//  lower level entities.
//
//	Determining this is done as follows:  allocate a 64k BIT array,
//  considered as an array of 4096 WORDS (16 bits each).  For every
//  available glyph,  set the corresponding bit in the array,  and
//  increment a counter.  At the end of this stage,  we have a count of
//  the number of (non-zero) GLYPH HANDLES needed.  Now,  by scanning
//  the bit array by WORD,  count each non-zero word.  This tells us
//  how many GLYPHTABLEs are required.   A second bit array is also
//  updated at this time.  This array consists of 256 bits,  one for
//  each of the possible entries in the MAPTABLE.  Passing through the
//  WORD array,  we can also set bits in the 256 bit array corresponding
//  to the number of MAPTABLES required.  Finally,  counting the number
//  of set bits in the 256 bit array gives us the number of MAPTABLES
//  required.  Given this information,  it is possible to determine
//  the amount of storage required.  Note that it is necessary to allow
//  for a 0 entry in the PTRDIFF and HGLYPH arrays.
// 
//   If an error occurs, NULL should be returned and an error code should 
//   be logged.
//
// History:
//   18-Apr-1991    -by-    Kent Settle       (kentse)
// Wrote it.
//--------------------------------------------------------------------------

PVOID DrvQueryFontTree (dhpdev,iFile,iFace,iMode,pid)
DHPDEV    dhpdev;
ULONG     iFile;
ULONG     iFace;
ULONG     iMode;
ULONG    *pid;
{
    PDEVDATA            pdev;

    UNREFERENCED_PARAMETER(iFile);

    // This can be used by the driver to flag or id the data returned.
    // May be useful for deletion of the data later by DrvFree().

    *pid = 0;           // don't need to use for this driver

    // get a pointer to our PDEV.

    pdev = (PDEVDATA)dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
    {
	RIP("PSCRIPT!DrvQueryFontTree: invalid PDEV.");
	SetLastError(ERROR_INVALID_PARAMETER);
        return((PVOID)NULL);
    }

    // validate the iFace.  we should only be called for device fonts,
    // ie iFace >= 1.

    if ((iFace < 1) || (iFace > (pdev->cDeviceFonts + pdev->cSoftFonts)))
    {
	RIP("PSCRPT!BuildGLYPHSET: invalid iFace.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return((PVOID)NULL);
    }

    switch (iMode)
    {
	case QFT_GLYPHSET:
	    return(BuildGLYPHSET(pdev, iFace));
	    break;

	case QFT_KERNPAIRS:
            return(BuildKernPairs(pdev, iFace));
	    break;

	default:
	    RIP("PSCRIPT!DrvQueryFontTree: invalid iMode.\n");
	    SetLastError(ERROR_INVALID_PARAMETER);
	    return((PVOID)NULL);
    }
}

//--------------------------------------------------------------------------
// PVOID BuildGLYPHSET(pdev, iFace)
// PDEVDATA    pdev;
// ULONG       iFace;
//
//   If an error occurs, NULL should be returned and an error code should 
//   be logged.
//
// History:
//   10-Feb-1992	  -by-	  Kent Settle	     (kentse)
// Borrowed some of if from RASDD, wrote the rest.
//--------------------------------------------------------------------------

PVOID BuildGLYPHSET(pdev, iFace)
PDEVDATA    pdev;
ULONG	    iFace;
{
    PNTFM	pntfm;		// pointer to font metrics.
    PUCMap	pmap;		// pointer to UNICODE<->PSCRIPT mapping table.
    PUCMap	pmapReset;	// place to save pointer.
    DWORD       i;              // loop counter.
    PBYTE	pCharCode;	// pointer to character code.
    PBYTE       pCharReset;     // place to save pointer.
    WCHAR	wchCurrent;	// current UNICODE value.
    WCHAR	wchPrevious;	// previous UNICODE value.
    DWORD       cGlyphs;        // count of glyph handles.
    DWORD       cRuns;          // count of runs within FD_GLYPHSET.
    DWORD	cbTotalMem;	// count of bytes needed for FD_GLYPHSET.
    HGLYPH     *phg;		// pointer to HGLYPH's.
    FD_GLYPHSET *pGLYPHSET;	// pointer to FD_GLYPHSET.
    WCRUN      *pWCRUN; 	// pointer to WCRUN.
    PULONG      pulUCTree;      // pointer to UNICODE tree.

    // get the font information for the given font.

    pntfm = pdev->pfmtable[iFace - 1].pntfm;

    // point to the appropriate mapping table in mapping.h.

    if (!strcmp((char *)pntfm + pntfm->loszFontName, "Symbol"))
        pmap = SymbolMap;

    else if (!strcmp((char *)pntfm + pntfm->loszFontName, "ZapfDingbats"))
        pmap = DingbatsMap;
    else
        pmap = LatinMap;

    pmapReset = pmap;

    // for every character in the mapping table, do a lookup in the NTFM
    // structure to find a matching PostScript character code.  For each
    // character found, we can extract the corresponding UNICODE value.
    // remember that the character codes are sitting in a BYTE array
    // which is sitting directly after a (USHORT) array of character
    // widths.

    pCharCode = ((PBYTE)pntfm + pntfm->loCharMetrics +
                       DWORDALIGN(pntfm->cCharacters * sizeof(USHORT)));

    // save the original pointer.

    pCharReset = pCharCode;

    cRuns = 1;  // there must be at least one run!
    cGlyphs = 0;

    // set wchPrevious equal to first UNICODE character code.

    wchPrevious = (WCHAR)pmap->usUCValue;

    while (pmap->szChar)
    {
        for (i = 0; i < (DWORD)pntfm->cCharacters; i++)
        {
            // search through all the characters in the NTFM structure,
            // looking for a matching PostScript character code. remember
            // that the high bit is used to indicate the font needs
            // to be remapped.  so ignore the high bit while checking
            // for a character match.

            if ((CHAR)*pCharCode == (CHAR)pmap->usPSValue)
            {
                // we have found the character, now get its UNICODE value
                // and set its BIT in the BIT array.  also, increment the
                // glyph count, if this UNICODE value has not yet been used.
                // since the mapping tables in mapping.h are sorted by
                // UNICODE values, we can simply check this value against
                // the one we found last time.  if it is the same, ignore 
                // it, if it is different, use it.

		wchCurrent = (WCHAR)pmap->usUCValue;

		if (wchCurrent != wchPrevious)
                {
		    cGlyphs++;

		    // see if we are starting new run.

                    if ((wchCurrent - wchPrevious) != 1)
                        cRuns++;
                }
                
		wchPrevious = wchCurrent;
                break;
            }

            pCharCode++;
        }
        
        // point back to start of character list in NTFM structure.

        pCharCode = pCharReset;

        // point to next character in mapping table.

        pmap++;
    }

    // allocate memory to build the FD_GLYPHSET structure in.  this
    // include space for the FD_GLYPHSET structure itself, as well
    // as space for all the glyph handles.

    cbTotalMem = (cGlyphs * sizeof(HGLYPH)) + (cRuns * sizeof(WCRUN)) +
		 sizeof(FD_GLYPHSET) - sizeof(WCRUN);

    // DWORD bound it.

    cbTotalMem = (cbTotalMem + 3) & ~3;

    // this memory is to be kept around until the pdev is destroyed, so
    // we don't have to free this memory until the heap is destroyed.

    phg = pulUCTree = (PVOID)HeapAlloc(pdev->hheap, 0, cbTotalMem);

    if (phg == NULL)
    {
	RIP("PSCRIPT!BuildGLYPHSET: HeapAlloc for phg failed.\n");
	return((PVOID)NULL);
    }

    // fill in the FD_GLYPHSET structure.

    pGLYPHSET = (FD_GLYPHSET *)phg;

    pGLYPHSET->cjThis = sizeof(FD_GLYPHSET) + (cRuns - 1) * sizeof(WCRUN);
    pGLYPHSET->flAccel = 0;		// no accelerators for us.
    pGLYPHSET->cGlyphsSupported = cGlyphs;
    pGLYPHSET->cRuns = cRuns;

    // now set the phg pointer to the first WCRUN structure.

    (BYTE *)phg += sizeof(FD_GLYPHSET) - sizeof(WCRUN);

    // point to first WCRUN in array.

    pWCRUN = (WCRUN *)phg;

    // point to data area for glyph handles.

    (BYTE *)phg += sizeof(WCRUN) * cRuns;

    // for every character in the font, do a lookup in the mapping
    // table in mapping.h to find the corresponding UNICODE value.
    // first point to the character codes in the NTFM structure.
    // remember that the character codes are sitting in a BYTE array
    // which is sitting directly after a (USHORT) array of character
    // widths.

    pCharCode = pCharReset;
    pmap = pmapReset;

    // locate the first glyph, and initialize the first WCRUN.

    for (i = 0; i < (DWORD)pntfm->cCharacters; i++)
    {
	// search for the matching code in mapping.h.	remember
	// that the high bit is used to indicate the font needs
	// to be remapped.  so ignore the high bit while checking
	// for a character match.

	if ((CHAR)*pCharCode == (CHAR)pmap->usPSValue)
	{
	    // we have found the character, now get its UNICODE value
	    // and set its BIT in the BIT array.  also, increment the
	    // glyph count, if this UNICODE value has not yet been used.
	    // since the mapping tables in mapping.h are sorted by
	    // UNICODE values, we can simply check this value against
	    // the one we found last time.  if it is the same, ignore
	    // it, if it is different, use it.

	    wchPrevious = (WCHAR)pmap->usUCValue;

	    pWCRUN->wcLow = wchPrevious;
	    pWCRUN->cGlyphs = 1;
	    pWCRUN->phg = phg;

	    // store the glyph handle, which we will store as the
	    // PostScript character code.

	    *phg++ = (HGLYPH)*pCharCode;

            break;
	}

        // point to the next character in the NTFM structure.

        pCharCode++;
    }

    // move to the second character in the mapping table.

    pmap++;

    while (pmap->szChar)
    {
        // reset pointer to start of character list in NTFM structure.
        
        pCharCode = pCharReset;

        for (i = 0; i < ((DWORD)pntfm->cCharacters - 1); i++)
        {
            // search for the matching code in mapping.h.  remember
            // that the high bit is used to indicate the font needs
            // to be remapped.  so ignore the high bit while checking
            // for a character match.

            if ((CHAR)*pCharCode == (CHAR)pmap->usPSValue)
            {
                // we have found the character, now get its UNICODE value
                // and set its BIT in the BIT array.  also, increment the
                // glyph count, if this UNICODE value has not yet been used.
                // since the mapping tables in mapping.h are sorted by
                // UNICODE values, we can simply check this value against
                // the one we found last time.  if it is the same, ignore 
                // it, if it is different, use it.

		wchCurrent = (WCHAR)pmap->usUCValue;

		if (wchCurrent != wchPrevious)
		{
		    // see if we are starting a new run.

		    if ((wchCurrent - wchPrevious) != 1)
		    {
			// start the new WCRUN.

			pWCRUN++;
			pWCRUN->wcLow = wchCurrent;
                        pWCRUN->cGlyphs = 0;
                        pWCRUN->phg = phg;
                    }

		    // fill in the glyph handle.

		    pWCRUN->cGlyphs++;
                    *phg++ = (HGLYPH)*pCharCode;
                }
                
		wchPrevious = wchCurrent;
                break;
            }

            pCharCode++;
        }

        // point to next character in mapping table.

        pmap++;
    }

    return(pulUCTree);
}


//--------------------------------------------------------------------------
// PVOID BuildKernPairs(pdev, iFace)
// PDEVDATA    pdev;
// ULONG       iFace;
//
//  This routine returns a pointer to a zero terminated array of
//  FD_KERNINGPAIR structures.  They should be sorted in order such that
//  the first character is the most significant, and the second character
//  is the least.
//
//   If an error occurs, NULL should be returned and an error code should 
//   be logged.
//
// History:
//   12-Mar-1992	  -by-	  Kent Settle	     (kentse)
// Wrote it.
//--------------------------------------------------------------------------

PVOID BuildKernPairs(pdev, iFace)
PDEVDATA    pdev;
ULONG	    iFace;
{
    PNTFM	    pntfm;          // pointer to font metrics.
    FD_KERNINGPAIR *pKernPairs;     // pointer to FD_KERNINGPAIRs.
    FD_KERNINGPAIR *pKernPairsSave; // pointer to FD_KERNINGPAIRs.
    DWORD           cjMem;          // count of bytes.
    DWORD           cKernPairs;     // cound of kernpairs.
    FD_KERNINGPAIR *pkp;            // pointer to kernpair in ntfm struct.

    // get the font information for the given font.

    pntfm = pdev->pfmtable[iFace - 1].pntfm;

    // allocate enough memory to hold all the FD_KERNINGPAIR strutures.
    // remember to allocate room for the zero terminated structure.
    // NOTE!  the memory allocated here is never explicitly freed.  this
    // will happen when the pdev, and therefore the heap, is destroyed.
    // this is done, since this memory is supposed to remain here for
    // the engine until the pdev is destroyed.

    cKernPairs = (DWORD)pntfm->cKernPairs;
    cjMem = ((cKernPairs + 1) * sizeof(FD_KERNINGPAIR));

    if (!(pKernPairs = (FD_KERNINGPAIR *)HeapAlloc(pdev->hheap, 0, cjMem)))
    {
        RIP("PSCRIPT!BuildKernPairs:  HeapAlloc for pKernPairs failed.\n");
        return((PVOID)NULL);
    }

    // save a copy of the original pointer.

    pKernPairsSave = pKernPairs;

    // fill in the FD_KERNINGPAIR structure for each kerning pair.

    pkp = (FD_KERNINGPAIR *)((BYTE *)pntfm + pntfm->loKernPairs);

    while (cKernPairs--)
        *pKernPairs++ = *pkp++;

    // fill in the zero terminating FD_KERNINGPAIR structure.

    pKernPairs->wcFirst = (WCHAR)'\0';
    pKernPairs->wcSecond = (WCHAR)'\0';
    pKernPairs->fwdKern = (FWORD)0;

    return(pKernPairsSave);
}
