/* gotxhand.c
 *
 * GOTXHandler implements the physical MediaMan handler for the GOTX
 * file format.  GOTX stands for "Go Text Format", and is a text file
 * with the following format:
 *
 *	<row-0-text>
 *	<row-1-text>
 *	     ...
 *	<row-18-text>
 *
 * <row-i-text> is 19 characters representing the i-th row.  The character
 * '0' represents an unoccupied Go board position; '1' represents a position
 * occupied by Player 1; '2' represents a position occupied by Player 2.
 * The GOTX handler writes each row on a separate line, but all white space
 * (including newline characters) are ignored on input.
 */

#include <windows.h>
#include <mediaman.h>
#include "medgoi.h"


/* define some constants */
#define formtypeGOTX	medFOURCC('G', 'O', 'T', 'X')


/* private prototypes */
WORD NEAR PASCAL LoadGOTX(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo);
WORD NEAR PASCAL SaveGOTX(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo);


/* LoadGOTX(medid, fpDisk, fpMedGo)
 *
 * Load the GOTX file identified by <medid> into memory.  <fpDisk> contains
 * information about the status of the loading process.  <fpMedGo> is
 * the element's instance information block.
 */
WORD NEAR PASCAL
LoadGOTX(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo)
{
	PGoBoard	pGoBoard;	// ptr. to Go board aliased data
	HMED		hMed;		// handle to open element file
	WORD		medf = MEDF_ERROR; // return status (for error return)
	char		ach[80];	// buffer for string loading
	int		r, c;		// row and column index (0 to 18)
	WORD		w;

	/* open the file for reading; make the buffer size twice the
	 * size of GoBoard (which is typically more than enough space to
	 * hold an entire GOTX file), so we load the GOTX file with
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

	/* from this point onword, goto LoadFile_ERROR_ABORT on error */

	/* display progress bar (not strictly necessary considering
	 * how quickly a GOTX file can be read, but this is sample
	 * code after all)
	 */
	LockData(0);
	LoadString(ghInst, IDS_LOADINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 0, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto LoadFile_ERROR_ABORT;
	}

	/* start reading characters -- ignore white space, but the only 
	 * other characters allowed are '0', '1', and '2'
	 */
	for (r = 0; r < GO_DIMEN; r++)
	{
		for (c = 0; c < GO_DIMEN; c++)
		{
			int	iByte;

			while (TRUE)
			{
				iByte = medGetBYTE(hMed);
				switch (iByte)
				{
				case '0':
				case '1':
				case '2':
					/* this is a valid Go cell character */
					(*pGoBoard)[r][c] = (BYTE) iByte - '0';
					break;

				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '\f':
					/* ignore white space -- keep reading */
					continue;
				
				default:
					/* anything else is an error */
					goto LoadFile_ERROR_ABORT;
				}

				break;		// okay, go to next cell
			}
		}
	}

	/* final progress bar update */
	LockData(0);
	LoadString(ghInst, IDS_DONELOADINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 100, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto LoadFile_ERROR_ABORT;
	}

	/* normal exit: close the file, unlock the GoBoard, and return
	 * success
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);
	return MEDF_OK;

LoadFile_ERROR_ABORT:

	/* error/abort exit: close the file, deallocate the GoBoard, and
	 * return MEDF_ERROR or MEDF_ABORT (whatever is in <medf>)
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);
	GlobalFree(fpMedGo->hGoBoard);
	fpMedGo->hGoBoard = NULL;

	/* generic load error message is "invalid GOTX file" */
	if (medf == MEDF_ERROR)
		medSetExtError(GOERR_INVALIDFILE, ghInst);

	return medf;
}


/* SaveGOTX(medid, fpDisk, fpMedGo)
 *
 * Save the GOTX file identified by <medid> into memory.  <fpDisk> contains
 * information about the status of the saving process.  <fpMedGo> is
 * the element's instance information block.
 */
WORD NEAR PASCAL
SaveGOTX(MEDID medid, FPMedDisk fpDisk, FPMedGo fpMedGo)
{
	PGoBoard	pGoBoard;	// ptr. to Go board aliased data
	HMED		hMed;		// handle to open element file
	WORD		medf = MEDF_ERROR; // return status (for error return)
	char		ach[80];	// buffer for string loading
	int		r, c;		// row and column index (0 to 18)
	WORD		w;

	/* open the file for writing; make the buffer size twice the
	 * size of GoBoard (which should be more than enough space to
	 * hold an entire GOTX file), so we save the GOTX file with
	 * a single disk write
	 */
	hMed = medOpen(medid, MOP_WRITE, 2 * sizeof(GoBoard));
	if (hMed == NULL)
		return MEDF_ERROR;
	
	/* lock the GoBoard */
	pGoBoard = (PGoBoard) GlobalLock(fpMedGo->hGoBoard);

	/* from this point onword, goto SaveFile_ERROR_ABORT on error */

	/* display progress bar (not strictly necessary considering
	 * how quickly a GOTX file can be written, but this is sample
	 * code after all)
	 */
	LockData(0);
	LoadString(ghInst, IDS_SAVINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 0, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto SaveFile_ERROR_ABORT;
	}

	/* start writing characters -- write a newline after each row */
	for (r = 0; r < GO_DIMEN; r++)
	{
		for (c = 0; c < GO_DIMEN; c++)
		{
			if (medPutBYTE(hMed, (*pGoBoard)[r][c] + '0')
			    == MED_EOF)
				goto SaveFile_ERROR_ABORT;
		}
		if (medPutBYTE(hMed, '\r') == MED_EOF)
			goto SaveFile_ERROR_ABORT;
		if (medPutBYTE(hMed, '\n') == MED_EOF)
			goto SaveFile_ERROR_ABORT;
	}

	/* final progress bar update */
	LockData(0);
	LoadString(ghInst, IDS_DONESAVINGFILE, ach, sizeof(ach));
	w = medUpdateProgress(fpDisk, 100, ach);
	UnlockData(0);
	if (w == MEDF_ABORT)
	{
		medf = MEDF_ABORT;
		goto SaveFile_ERROR_ABORT;
	}

	/* normal exit: close the file, unlock the GoBoard, and return
	 * success
	 */
	medf = MEDF_OK;
	/* fall through */

SaveFile_ERROR_ABORT:

	/* error/abort exit: close the file, deallocate the GoBoard, and
	 * return MEDF_ERROR or MEDF_ABORT (whatever is in <medf>)
	 */
	medClose(hMed);
	GlobalUnlock(fpMedGo->hGoBoard);

	/* generic save error message is "error saving GOTX file" */
	if (medf == MEDF_ERROR)
		medSetExtError(GOERR_ERRORSAVING, ghInst);

	return medf;
}


/* GOTXHandler()
 * 
 * Secondary handler for the GOTX disk representation of the GO data type.
 * 
 * Secondary handlers receive only MED_TYPEINIT, MED_LOADINIT, MED_LOAD,
 * MED_SAVEINIT, and MED_SAVE messages.  They do not get any memory
 * management messages.  Their sole purpose in life is to read or write files
 * into/from the instance data structures of a primary element.
 */
DWORD FAR PASCAL
GOTXHandler(MEDID medid, MEDMSG medmsg, MEDINFO medinfo, LONG lParam,
		LONG lParam2)
{
	FPMedGo		fpMedGo;

	fpMedGo = (FPMedGo) MedInfoInstance(medinfo);

	switch (medmsg)
	{

	case MED_GETLOADPARAM:
	case MED_FREELOADPARAM:

		/* Prompt the user if necessary about loading parameters */
		return MEDF_OK;

	case MED_LOAD:

		/* Perform the element load */
		return LoadGOTX(medid, (FPMedDisk) lParam, fpMedGo);

	case MED_GETSAVEPARAM:
	case MED_FREESAVEPARAM:

		/* Prompt the user if necessary about saving parameters */
		return MEDF_OK;

	case MED_SAVE:

		/* Perform the element save */
		return SaveGOTX(medid, (FPMedDisk) lParam, fpMedGo);
	} 

	return 0L;
}
