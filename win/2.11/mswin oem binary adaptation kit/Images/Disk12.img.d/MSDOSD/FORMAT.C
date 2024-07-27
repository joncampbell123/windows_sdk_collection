/* format.c - format an empty disk and place a valid boot sector on it
 *
 * The Format routine is intended to mimic the actions of the FORMAT command
 * on MSDOS.  We restrict the possible set of operations that Format must use
 * in order to simplify life; Format will NOT:
 *
 *  Place a bootable system on the diskette
 *  Format anything other than the maximum capacity that the disk can handle
 *
 * The requirements for Format are:
 *
 *  1) there be a disk in a "source" drive that contains a valid boot sector
 *  2) there be a disk in a "destination" drive that is formattable.
 *
 * Format will attempt to format the disk to the maximum capacity.  The
 * algorithm for determining the capacity is as follows:
 *
 *	Ask INT 11 for the number of diskettes attached.
 *	If this number is < 2 then
 *	    error
 *	If this number < destination then
 *	    error
 *	Ask INT 13 for the drive type.
 *	If error then {
 *	    assume 48tpi double side
 *	    Attempt to format track 0, head 1
 *	    If error then
 *	    assume 48tpi single side
 *	else
 *	if sectors per track = 15 then
 *	    assume 96tpi
 *	else
 *	    error
 *
 * Note that this does NOT take into account non-contiguous drives (see 3.2
 * spec) nor future drives nor user-installed device drivers.
 */

#include "msdosd.h"

/* single sided 48-tpi */
struct bpbType bpbList [] = {
/* cbSec secPerClus cSecRes cFAT cDir	 cSec bMedia secPerFAT secPerTrack cHead cSecHidden */
{   512,      1,       1,     2,  64,  1*8*40, 0xFE,	 1,	     8,      1,     0},   /*  8 sector 1 side 48tpi */
{   512,      2,       1,     2, 112,  2*8*40, 0xFF,	 1,	     8,      2,     0},   /*  8 sector 2 side 48tpi */
{   512,      1,       1,     2,  64,  1*9*40, 0xFC,	 2,	     9,      1,     0},   /*  9 sector 1 side 48tpi */
{   512,      2,       1,     2, 112,  2*9*40, 0xFD,	 2,	     9,      2,     0},   /*  9 sector 2 side 48tpi */
{   512,      1,       1,     2, 224, 2*15*80, 0xF9,	 7,	    15,      2,     0},   /* 15 sector 2 side 96tpi */
{   512,      2,       1,     2, 112,  2*9*80, 0xF9,	 3,	     9,      2,     0},   /*  9 sector 2 side 96tpi */
{   512,      1,       1,     2, 224,	 2880, 0xF0,	 9,	    18,      2,     0}	  /*  3.5 High capacity     */
};

/* cCluster = (cSec/secPerClus) - cSecRes
				- (cFAT * secPerFat)
				- (cDir*32+cbSec-1)/cbSec
*/

unsigned cCluster[] = { 0x139, 0x13B, 0x15F, 0x162, 0x943, 0x2c9, 0xb1f, 0 };

DevPB  device_parameters;

struct dbtType dbtSave;

/* Format (dSrc, dDst, pVID) will format drive dDst using an updated boot
 * sector from drive dSrc.  If pVID != NULL then it is a pointer to a volume ID
 * which is created on the destination drive after formatting.	We will
 * allocate two blocks of memory, but only one at a time.  The first one we
 * allocate is the bit-map of bad clusters that we find during the format.  The
 * second is for the boot sector.
 *
 *  Inputs:	dSrc	    1-based drive (0=default) of drive with valid boot
 *			    sector
 *		dDst	    1-based drive to be formatted
 *		dDstInt13   0-based floppy index
 *		lpVID	    pointer to volume ID or NULL for no VID.
 *  Returns:	0	    Success
 *		<> 0	    error code (see error.h)
 *  Side Effects:	    none
 */

unsigned int dosver;	    /* the dos version number, bytes reversed */
			    /* i.e. dos 3.2 => dosver = 314h */
WORD FAR PASCAL Format (dSrc, dDst, dDstInt13, lpVID, lpProc)
BYTE dSrc, dDst;
BYTE dDstInt13;
LPSTR lpVID;
FARPROC lpProc;
{
    struct bpbType *pBPB;
    int cClusters;
    WORD cTracks, track, head, iClusBad, iSecBad, secData, i;
    WORD erc;
    LPSTR lpBoot;
    HANDLE hBoot;
    LPSTR lpClusBad;
    HANDLE hClusBad;
    LPSTR lpTmp;
    LPDBT lpDBT;

    /* initialize for cleanup */

    erc = NOERROR;
    hBoot = NULL;
    lpBoot = NULL;
    hClusBad = NULL;
    lpClusBad = NULL;
    DiskReset ();
    if (!(lpDBT = GetDBT()))
	return INTERNALERROR;
    dbtSave = *lpDBT;

    dosver = GetVersionNumber();

    /* convert drives into 0-based drives */

    dSrc = NormalizeDrive (dSrc);
    dDst = NormalizeDrive (dDst);
    if (dDst == dSrc)
	return DSTDISKERROR;

    /* allocate boot sector, sector buffer, track buffer */

    if (!(hBoot = GlobalAlloc(GHND, (long)(2*CBSECTOR))))
	{
	erc = NOMEMORY;
	goto cleanup;
	}
    if (!(lpBoot = GlobalLock(hBoot)))
	{
	erc = NOMEMORY;
	goto cleanup;
	}

    /* if dos >= 3.2, use generic get_device_parameters to get BPB */
    if (dosver >= 0x314)
	{
	GetDeviceParameters(dDst, &device_parameters);
	pBPB = &(device_parameters.BPB);

	/* (c-ralphp 6/14/88) Begin Modification */
	/* Fix for HP DOS 3.2 and up in order to support 1.44M floppies */
	/* DrivParms in DOS are calculated wrong and give incorrect */
	/* number of total sectors */
	if(IsHPMachine())
	      pBPB->cSec = device_parameters.NumCyls *
					      pBPB->secPerTrack* pBPB->cHead;
	/* (c-ralphp) End Modification */

	cClusters = (pBPB->cSec / pBPB->secPerClus) - pBPB->cSecRes -
		    (pBPB->cFAT * pBPB->secPerFAT);


	/* hack for making it work above 3.2	    */
	device_parameters.dontcare1[0] = 0x05;
	SetDeviceParameters(dDst, &device_parameters);

	}
    else
	{
	/* see if INT 13 knows the drive type */

	switch (GetDriveType (dDstInt13)) {
	case -1:			    /* error => must be old ROMS */
	case NOCHANGE:
	    /* We must presume that the machine is using old ROMS:  this is a
	     * 48-tpi system.  Presume a 9-sector double-sided 48-tpi disk
	     */
	    pBPB = &bpbList[DS48];
	    cClusters = cCluster[DS48];
	    lpDBT->lastsector = pBPB->secPerTrack;
	    lpDBT->gaplengthrw = 0x50;
	    /* attempt to format, now, side 1.	If this fails, we need to reset
	     * to single side 48tpi
	     */
	    if (FormatTrackHead (dDstInt13, 0, 1, pBPB->secPerTrack, lpBoot) != NOERROR)
		{
		pBPB = &bpbList[SS48];
		cClusters = cCluster[SS48];
		}
	    break;
	case CHANGE:
	    pBPB = &bpbList[DS96];
	    cClusters = cCluster[DS96];
SetupHighDensityMode:
	    break;
	default:
	    erc = INTERNALERROR;
	    goto cleanup;
	    }
	}

    lpDBT->lastsector = pBPB->secPerTrack;
    lpDBT->gaplengthrw = (pBPB->secPerTrack == 15 ? 0x54 : 0x50);

    /* if 96tpi, fix up the Disk Base Table */
    if (pBPB->bMedia == 0xF9)	    /* high density */
    /* if high density disk, put in high density mode */
	if (pBPB->secPerTrack == 15)	    /* then 1.2 Meg */
	    SetDASD(dDstInt13, 3);	 /* 1.2M in 1.2M drive */

    /* we believe that we know EXACTLY what is out there.  Allocate the boot
     * sector and the bad-cluster bit-map.  The boot sector buffer is reused as
     * two consecutove sectors of the FAT.
     */

    if (!(hClusBad = GlobalAlloc(GHND, (long)((2 + cClusters + 7) / 8))))
	{
	erc = NOMEMORY;
	goto cleanup;
	}
    if (!(lpClusBad = GlobalLock(hClusBad)))
	{
	erc = NOMEMORY;
	goto cleanup;
	}

    /* Let's format 1 track at a time and record the bad sectors in the
     * bitmap.	Note that we round DOWN the number of tracks so that we
     * don't format what might not be ours.  Fail if there are any bad
     * sectors in the system area.
     */

    /* Compute number of tracks to format */
    cTracks = pBPB->cSec / pBPB->secPerTrack;

    /* Compute the starting track/head */
    track = pBPB->cSecHidden / pBPB->secPerTrack;
    head = pBPB->cSecHidden % pBPB->secPerTrack;

    /* Compute the number of the first sector after the system area */
    secData = pBPB->cSecRes + pBPB->cFAT * pBPB->secPerFAT +
	       (pBPB->cDir * 32 + pBPB->cbSec-1) / pBPB->cbSec;

    erc = FORMATCANCELLED;
    if (lpProc && !(*lpProc)( track ))
	goto cleanup;
    while (cTracks) {
	if (FormatTrackHead (dDstInt13, track, head, pBPB->secPerTrack, lpBoot) != NOERROR) {

	    if (lpProc && !(*lpProc)( -track ))
		goto cleanup;

	    /* Bad Track.  Compute the number of the first bad sector */
	    iSecBad = track * pBPB->secPerTrack + head;

	    /* Fail if bad sector is in the system area */
	    if (iSecBad < secData) {
		erc = BADSYSTEMAREA;
		goto cleanup;
		}

	    /* Enumerate all bad sectors and mark the corresponding
	     * clusters as bad.
	     */
	    for (i = iSecBad; i < iSecBad + pBPB->secPerTrack; i++) {
		iClusBad = (i - secData) / pBPB->secPerClus + 2;
		lpClusBad [iClusBad / 8] |= 1 << (iClusBad % 8);
		}
	    }
	cTracks--;
	if (++head >= pBPB->cHead) {
	    head = 0;
	    track++;
	    if (cTracks)
		if (lpProc && !(*lpProc)( track ))
		    goto cleanup;
	    }
	}
    erc = NOERROR;

    /* dump out boot sector */
    if (WriteBootSec (dSrc, dDst, pBPB, lpBoot) == -1)
	erc = INVALIDBOOTSEC;

    /* format is complete.  Create correct DPB in system */

    SetDPB (dDst, pBPB, &DPB);

    /* Output correct FAT */

    OpenFAT( FAT_WRITE );
    if (PackFAT (&DPB, lpBoot, 0, pBPB->bMedia+0xFF00) == -1 ||
	PackFAT (&DPB, lpBoot, 1, 0xFFFF) == -1) {
	erc = DSTDISKERROR;
	goto cleanup;
	}
    for (i = 2; i < 2 + cClusters; i++)
	if (PackFAT (&DPB, lpBoot, i,
		     (lpClusBad[i / 8] & (1 << (i % 8)) ) ? 0xFFF7 : 0) == -1) {
	    erc = DSTDISKERROR;
	    goto cleanup;
	    }

    if (FlushFAT (&DPB, lpBoot) == -1) {
	erc = DSTDISKERROR;
	goto cleanup;
	}

    /* clean out the root directory */
    for (i = 0, lpTmp = lpBoot; i < CBSECTOR; ++i)
	*lpTmp++ = 0;

    for (i = 0; i < (pBPB->cDir * 32 + pBPB->cbSec-1)/pBPB->cbSec; i++)
	if (int26 (dDst, lpBoot, 1, DPB.dir_sector + i) == -1) {
	    erc = DSTDISKERROR;
	    goto cleanup;
	    }

    /* drop in volume ID if necessary */
    if (lpVID != NULL)
	if (WriteVolumeID( &DPB, dDst, lpVID, lpBoot ) == -1) {
	    erc = DSTDISKERROR;
	    goto cleanup;
	    }

    /* all done, release resources */
cleanup:
    if (lpDBT)
	*lpDBT = dbtSave;
    if (lpBoot)
	GlobalUnlock(hBoot);
    if (hBoot)
	GlobalFree(hBoot);
    if (lpClusBad)
	GlobalUnlock(hClusBad);
    if (hClusBad)
	GlobalFree (hClusBad);

    if (dosver >= 0x314) {
	/* reset back the normal mode	    */
	device_parameters.dontcare1[0] = 0x04;
	SetDeviceParameters(dDst, &device_parameters);
    }
    return erc ;
}
