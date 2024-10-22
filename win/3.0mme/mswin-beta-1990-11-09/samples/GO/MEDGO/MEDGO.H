/* medgo.h
 *
 * This is the external definition of the MEDGO handler DLL.
 * This file requires "windows.h", "mediaman.h"
 */

#ifndef _MEDGO_H_
#define _MEDGO_H_

/* GO library element types */
#define medtypeGO	medMEDTYPE('g', 'o', ' ', ' ')
#define medtypeGOTX	medMEDTYPE('g', 'o', 'T', 'X')


/* Define error message IDs here.  These IDs will be returned to the
 * user from medGetError.  They must be the ID of a resource string
 * in this DLL that contains a textual description of the error.
 */

#define GOERR_RANDOMBLOWUP	101
#define	GOERR_INVALIDFILE	102
#define GOERR_ERRORSAVING	103


/************************************************************************
 * 
 *  Definitions ONLY for GO secondary handler writers...
 */


/* GoBoard is the data representation for one instance of a GO element.
 *
 * A GoBoard is a two-dimensional array of bytes, where each
 * byte represents a position on the Go board.  The first index
 * is the row number (0 to GO_DIMEN, with 0 being the top row) and
 * the second index is the column number (0 to GO_DIMEN, with 0 being
 * the leftmost column).
 *
 * A position on the Go board is represented by a byte as follows:
 *	0	position is not occupied
 *	1	position is occupied by player 1
 *	2	position is occupied by player 2
 *
 * MedGo is the instance information block maintained by MediaMan for
 * each instance of a Go element that's currently "accessed" (loaded).
 * MedGo simply contains a handle to a global memory block containing
 * a GoBoard.
 */

#define	GO_DIMEN	19		// width and height of a Go board

typedef BYTE	GoBoard[GO_DIMEN][GO_DIMEN];

typedef GLOBALHANDLE	HGoBoard;	// handle to a Go board
typedef GoBoard FAR *	PGoBoard;	// pointer to a Go board

typedef struct _MedGo			// MediaMan instance info for Go board
{
	HGoBoard	hGoBoard;
} MedGo;

typedef MedGo FAR *FPMedGo;


#endif  /*  _MEDGO_H_  */
