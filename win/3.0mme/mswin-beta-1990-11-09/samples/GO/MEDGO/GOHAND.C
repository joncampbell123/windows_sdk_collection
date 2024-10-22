/* gohand.c
 *
 * GOHandler implements the logical MediaMan handler for the GO media
 * element type, and the physical MediaMan handler for the GO file format.
 *
 * The GO file format is a RIFF form defined as follows:
 *
 *	RIFF('GO' data(<row-0>...<row-18>))
 *
 * <row-i> is the 19 BYTE values representing the i-th row of the Go board.
 */

#include <windows.h>
#include <mediaman.h>
#include "medgoi.h"


/* constants for secondary handler part of GOHandler */
#define formtypeGO	medFOURCC('G', 'O', ' ', ' ')
#define ckidDATA	medFOURCC('d', 'a', 't', 'a')


/* private prototypes */
WORD NEAR PASCAL LoadGO(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo);
WORD NEAR PASCAL SaveGO(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo);


/* LoadGO(medid, fpDisk, fpMedGo)
 *
 * Load the GO file identified by <medid> into memory.  <fpDisk> contains
 * information about the status of the loading process.  <fpMedGo> is
 * the element's instance information block.
 */
WORD NEAR PASCAL
LoadGO(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo)
{
	PGoBoard	pGoBoard;	// ptr. to Go board aliased data
	HMED		hMed;		// handle to open element file
	WORD		medf = MEDF_ERROR; // return status (for error return)
	char		ach[80];	// buffer for string loading
	WORD		w;

	/* open the file for reading; make the buffer size twice the
	 * size of GoBoard (which should be more than enough space to
	 * hold an entire GO file), so we load the GO file with
	 * a single disk read
	 */
	hMed = medOpen(medid, MOP_READ, 2 * sizeof(GoBoard));
	if (hMed == NULL)
		return MEDF_ERROR;
	
	/* allocate the GoBoard */
	fpMedGo->hGoBoard =
		(HGoBoard) GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
					(DWORD) sizeof(GoBoard));
	if (fpMedGo->hGoBoard == NULL)
	{
		medClose(hMed);
		return MEDF_ERROR;		// error creating element
	}

	/* lock the GoBoard */
	pGoBoard = (PGoBoard) GlobalLock(fpMedGo->hGoBoard);

	/* from this point onword, goto LoadGO_ERROR_ABORT on error */

	/* display progress bar (not strictly necessary considering
	 * how quickly a GO file can be read, but this is sample
	 * code after all)
	 */
	LockData(0);
	LoadString(ghInst, IDS_LOADINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 0, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto LoadGO_ERROR_ABORT;
	}

	/* descend into the RIFF() chunk and check form type */
	if (medDescend(hMed) != ckidRIFF)
		goto LoadGO_ERROR_ABORT;	// this isn't a RIFF() chunk
	if (medReadFormHeader(hMed) != formtypeGO)
		goto LoadGO_ERROR_ABORT;	// this isn't a 'GO' form
	
	/* ignore all chunks except the 'data' chunk */
	if (!medFindChunk(hMed, ckidDATA))
		goto LoadGO_ERROR_ABORT;
	
	/* read the chunk data, which happens to be the same format
	 * as a GoBoard structure
	 */
	if (medRead(hMed, (LPSTR) pGoBoard, (DWORD) sizeof(GoBoard)) == MED_EOF)
		goto LoadGO_ERROR_ABORT;

	/* final progress bar update */
	LockData(0);
	LoadString(ghInst, IDS_DONELOADINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 100, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto LoadGO_ERROR_ABORT;
	}

	/* normal exit: close the file, unlock the GoBoard, and return
	 * success
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);
	return MEDF_OK;

LoadGO_ERROR_ABORT:

	/* error/abort exit: close the file, deallocate the GoBoard, and
	 * return MEDF_ERROR or MEDF_ABORT (whatever is in <medf>)
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);
	GlobalFree(fpMedGo->hGoBoard);
	fpMedGo->hGoBoard = NULL;

	/* generic load error message is "invalid GO file" */
	if (medf == MEDF_ERROR)
		medSetExtError(GOERR_INVALIDFILE, ghInst);

	return medf;
}


/* SaveGO(medid, fpDisk, fpMedGo)
 *
 * Save the GO file identified by <medid> into memory, using the GO
 * file format.  <fpDisk> contains information about the status of
 * the saving process.  <fpMedGo> is the element's instance information
 * block.
 */
WORD NEAR PASCAL
SaveGO(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo)
{
	PGoBoard	pGoBoard;	// ptr. to Go board aliased data
	HMED		hMed;		// handle to open element file
	WORD		medf = MEDF_ERROR; // return status (for error return)
	char		ach[80];	// buffer for string loading
	WORD		w;

	/* open the file for writing; make the buffer size twice the
	 * size of GoBoard (which should be more than enough space to
	 * hold an entire GO file), so we save the GO file with
	 * a single disk write
	 */
	hMed = medOpen(medid, MOP_WRITE, 2 * sizeof(GoBoard));
	if (hMed == NULL)
		return MEDF_ERROR;
	
	/* lock the GoBoard */
	pGoBoard = (PGoBoard) GlobalLock(fpMedGo->hGoBoard);

	/* from this point onword, goto SaveGO_ERROR_ABORT on error */

	/* display progress bar (not strictly necessary considering
	 * how quickly a GO file can be written, but this is sample
	 * code after all)
	 */
	LockData(0);
	LoadString(ghInst, IDS_SAVINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 0, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto SaveGO_ERROR_ABORT;
	}

	/* create the RIFF() chunk -- let MediaMan figure out its size */
	if (medCreateChunk(hMed, ckidRIFF, 0L) == MED_EOF)
		goto SaveGO_ERROR_ABORT;
	if (medWriteFormHeader(hMed, formtypeGO) == MED_EOF)
		goto SaveGO_ERROR_ABORT;

	/* create the data() chunk -- we know the size of this one */
	if (medCreateChunk(hMed, ckidDATA, (DWORD) sizeof(GoBoard))
			== MED_EOF)
		goto SaveGO_ERROR_ABORT;

	/* write the chunk data, which happens to be the same format
	 * as a GoBoard structure
	 */
	if (medWrite(hMed, (LPSTR) pGoBoard, (DWORD) sizeof(GoBoard))
			== MED_EOF)
		goto SaveGO_ERROR_ABORT;

	/* final progress bar update */
	LockData(0);
	LoadString(ghInst, IDS_DONESAVINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 100, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto SaveGO_ERROR_ABORT;
	}

	/* normal exit: close the file, unlock the GoBoard, and return
	 * success
	 */
	medf = MEDF_OK;
	/* fall through */

SaveGO_ERROR_ABORT:

	/* error/abort exit: close the file, deallocate the GoBoard, and
	 * return MEDF_ERROR or MEDF_ABORT (whatever is in <medf>)
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);

	/* generic save error message is "error saving GO file" */
	if (medf == MEDF_ERROR)
		medSetExtError(GOERR_ERRORSAVING, ghInst);

	return medf;
}


/* GOHandler()
 *
 * This is the MediaMan handler for Go elements.  This handler is both
 *   (a) a primary handler for the memory representation of Go
 *       elements;
 *   (b) a secondary handler for the "GO" file format
 *
 * The primary (memory) representation of a Go element is described
 * in the header file "medgo.h".
 */
DWORD FAR PASCAL
GOHandler(MEDID medid, MEDMSG medmsg, MEDINFO medinfo, LONG lParam,
		LONG lParam2)
{
	FPMedGo		fpMedGo;	// ptr. to Go instance data
	FPMedGo		fpMedGoNew;	// ptr. to a new Go instance block
	MEDINFO		medinfoNew;	// handle to new instance block
	PGoBoard	pGoBoard;	// ptr. to Go board aliased data
	PGoBoard	pGoBoardNew;	// ptr. to new Go board aliased data
	int		r, c;		// Go board row, column (0..GO_DIMEN-1)


	/* obtain a pointer to this element's instance data block;
	 * the storage for this block is maintained by the MEDIAMAN.
	 */
	fpMedGo = (FPMedGo) MedInfoInstance(medinfo);

	switch (medmsg)
	{


	/*********************************************************************
	 *
	 *  Primary handler part of GOHandler()...
	 *
	 */

	case MED_TYPEINIT:

		/* Return the size of the instance block that this
		 * element type needs.  This is the size of my instance
		 * data structure defined in medgo.h.
		 */
		return (DWORD) sizeof(MedGo);

	case MED_INIT:

		/* A element instance is being initialized.  Fill in
		 * instance data structure with reasonable values.
		 */

		/* no Go board is allocated yet */
		fpMedGo->hGoBoard = NULL;
		return TRUE;

	case MED_CREATE:

		/* A element instance is being dynamically created.  Fill in
		 * the element instance block so that it matches the condition
		 * that a MED_LOADINIT message leaves the element in.
		 * In the case of Go, allocate a GoBoard and store the
		 * handle to it in the MedGo structure.
		 */

		/* allocate the GoBoard */
		fpMedGo->hGoBoard =
			(HGoBoard) GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
					       (DWORD) sizeof(GoBoard));
		if (fpMedGo->hGoBoard == NULL)
			return FALSE;		// error creating element

		/* initialize the Go board -- all cells are "unoccupied" */
		pGoBoard = (PGoBoard) GlobalLock(fpMedGo->hGoBoard);
		for (r = 0; r < GO_DIMEN; r++)
			for (c = 0; c < GO_DIMEN; c++)
				(*pGoBoard)[r][c] = 0; // unoccupied
		GlobalUnlock(fpMedGo->hGoBoard);

		return TRUE;			// return "success"

	case MED_NEWNAME:

		/* This element is being renamed.  Normal, non-streaming
		 * element types can ignore this message.
		 */
		break;

	case MED_COPY:

		/* The media element manager is copying this element in
		 * preparation for a SaveAs operation.  Copy all aliased
		 * data into the <medinfo> instance block provided in
		 * <lParam>. Leave the <fpMedGoNew> condition identical
		 * to that of fpMedGo.
		 */

		medinfoNew = (MEDINFO) lParam;
		fpMedGoNew = (FPMedGo) MedInfoInstance(medinfoNew);

		/* allocate the new GoBoard */
		fpMedGoNew->hGoBoard =
			(HGoBoard) GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
					       (DWORD) sizeof(GoBoard));
		if (fpMedGoNew->hGoBoard == NULL)
			return FALSE;		// error creating element

		/* copy this Go board to the new Go board */
		pGoBoard = (PGoBoard) GlobalLock(fpMedGo->hGoBoard);
		pGoBoardNew = (PGoBoard) GlobalLock(fpMedGoNew->hGoBoard);
		for (r = 0; r < GO_DIMEN; r++)
			for (c = 0; c < GO_DIMEN; c++)
				(*pGoBoardNew)[r][c] = (*pGoBoard)[r][c];
		GlobalUnlock(fpMedGo->hGoBoard);
		GlobalUnlock(fpMedGoNew->hGoBoard);

		return TRUE;

	case MED_LOCK:

		/* element is being locked.  The Go handler returns a
		 * pointer to the GoBoard.
		 */
		return (DWORD) GlobalLock(fpMedGo->hGoBoard);

	case MED_UNLOCK:

		/* element is being unlocked.  The Go unlocks the GoBoard
		 */
		GlobalUnlock(fpMedGo->hGoBoard);
		return 0L;

	case MED_POSTSAVE:
	case MED_POSTLOAD:

		/* These messages sent to the primary handler when
		 * a element load or save completes successfully.
		 * Do any post-processing on the instance data here.
		 * The state of the element after a MED_POSTLOAD message
		 * is received should be such that a MED_LOCK can be
		 * successfully processed.  Also, the MED_CREATE message
		 * should leave created elements in an identical state to
		 * that that is setup by the MED_POSTLOAD message.
		 */
		return MEDF_OK;

	case MED_PRELOAD:
	case MED_PRESAVE:
		return MEDF_OK;
		
	case MED_UNLOAD:

		/* The element is being unloaded from memory --
		 * deallocate the GoBoard and set the handle to NULL.
		 */

		GlobalFree(fpMedGo->hGoBoard);
		fpMedGo->hGoBoard = NULL;
		return TRUE;


	/*********************************************************************
	 *
	 *  Secondary handler part of GOHandler()...
	 *
	 */

	case MED_GETLOADPARAM:
	case MED_FREELOADPARAM:
		/* Prompt the user if necessary about loading parameters */
		return MEDF_OK;

	case MED_LOAD:

		/* Perform the element load */
		return LoadGO(medid, (FPMedDisk) lParam, fpMedGo);

	case MED_GETSAVEPARAM:
	case MED_FREESAVEPARAM:

		/* Prompt the user if necessary about saving parameters */
		return MEDF_OK;

	case MED_SAVE:

		/* Perform the element save */
		return SaveGO(medid, (FPMedDisk) lParam, fpMedGo);
	} 

	return 0L;
}
