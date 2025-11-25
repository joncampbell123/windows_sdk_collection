/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

//
// Public include file for MDSPlay.DLL
//

// Returned errors
//
#define MDS_SUCCESS                 (0)
#define MDS_ERR_NOMEM               (1)
#define MDS_ERR_NOFILE              (2)
#define MDS_ERR_BADFILE             (3)
#define MDS_ERR_BADFLAGS            (4)
#define MDS_ERR_MIDIERROR           (5)
#define MDS_ERR_INVALHANDLE         (6)
#define MDS_ERR_BADSTATE            (7)


//
// LoadMDSImage
// 
// Allocate space for and read the given image in preparation for playback.
//
// lpbImage may be freed after this call
//

// One of these flags MUST be set
//
// lpbImage -> string containing filename 
//
#define MDS_F_FILENAME              0x00000001L

// lpbImage -> memory image of file
//
#define MDS_F_MEMORY                0x00000002L

// NOTE: cbImage only used if MDS_F_MEMORY is set.
//
DWORD LoadMDSImage(HANDLE *hImage, PBYTE pbImage, DWORD cbImage, DWORD fdw);

//
// FreeMDSImage
//
// Release all memory associated with the given MDS image.
//
// If the given MDS is still playing, it will be stopped and the device
// closed.
//
DWORD FreeMDSImage(HANDLE hImage);
// 
// Basic operations on the MDS image
//

// Loop until told to stop
//
#define MDS_F_LOOP                  0x00000001L

// Play from beginning or last pause state.
//
DWORD PlayMDS(HANDLE hImage, DWORD fdw);

// Pause until next play
//
DWORD PauseMDS(HANDLE hImage);

// Stop. Next play will start at beginning of file again
//
DWORD StopMDS(HANDLE hImage);
