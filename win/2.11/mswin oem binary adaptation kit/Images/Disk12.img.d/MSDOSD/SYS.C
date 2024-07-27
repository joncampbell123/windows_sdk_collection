/* sys.c - perform system copying operations
 *
 * The Sys routine is intended to mimic the functions of the SYS command
 * under MSDOS:  to transfer a version of the operating system from a source
 * disk to a destination such that the destination will be bootable.
 *
 *  The requirements of the source disk is that it contain:
 *
 *      1) a command processor (COMMAND.COM)
 *      2) a default set of device drivers (IBMBIO.COM)
 *      3) an operating system (IBMDOS.COM)
 *      4) a boot sector appropriate to the device drivers
 *
 *  The requirements for the destination disk are either:
 *
 *      1) first two directory entries are empty
 *      2) the first N clusters free where N = ceil (size IBMBIO/secPerClus)
 *      3) there is enough room on the disk for IBMBIO/IBMDOS/COMMAND
 *
 *  - or -
 *
 *      1) the first two directory entries are IBMBIO.COM and IBMDOS.COM
 *                                         or  IO.SYS and MSDOS.SYS
 *      2) the first N clusters are alloced to these files where N is defined
 *          above
 *      3) there is enough room on the disk for IBMBIO/IBMDOS/COMMAND after
 *          deleting the IBMBIO/IBMDOS/COMMAND on the disk.
 */

#include "msdosd.h"

/* static data */
#define CFILES      3
#define FILENAMELEN (1+1+1+8+1+3+1)
static char *fileName[CFILES] = { NULL, NULL, "COMMAND.COM" };
static BYTE fileAttr[CFILES] =  { A_H|A_S|A_RO, A_H|A_S|A_RO, NULL          };
static char FNDRV[] = "A:\\";
static char FNSRC[FILENAMELEN];
static char FNDST[FILENAMELEN];


/* Sys (dSrc, dDst) will copy a system (boot sector, bios, dos, and command)
 * from the source disk(ette) to the destination diskette.  The only subtle
 * piece of business is that of updating the boot sector obtained from the
 * source disk before transferring it to the destination disk.
 *
 *  Inputs:     dSrc    1-based (0=default) drive number of source drive
 *                      containing valid boot sector, bios, dos and command
 *              dDst    1-based (0=default) drive number of formatted drive
 *                      for destination.
 *  Returns:    0       Successful transferral of boot sector and files
 *              <> 0    error code (see error.h)
 *  Side effects:       none
 */
WORD FAR PASCAL Sys (dSrc, dDst)
BYTE dSrc, dDst;
{
    register int i;
    WORD cbBIOS, cbCluster, cClusBIOS, clusBIOS, clusDOS;
    WORD clusTmp1, clusTmp2;
    WORD erc, fh, maxSize;
    WORD hBuf;
    char far *lpBuf;
    char *s;

    /* initialize variables for cleanup */

    erc = NOERROR;
    hBuf = NULL;
    lpBuf = NULL;

    /* Convert 1-based drives into 0-based drives */

    dSrc = NormalizeDrive (dSrc);
    dDst = NormalizeDrive (dDst);

    GetDriveType(dDst);     /* reset disk if necessary */

    /* Allocate temp buffer for copy and for boot sector */

    maxSize = 0xFFF0;
    while (maxSize != 0 && !(hBuf = GlobalAlloc(GHND, (long)maxSize)))
        maxSize >>= 1;
    if (maxSize < CBSECTOR * 2 || !(lpBuf = GlobalLock(hBuf)))
        {
        erc = NOMEMORY;
        goto cleanup;
        }

    /* Acertain presence of BIOS/DOS/COMMAND and grab their sizes */

    fileName[ 0 ] = "IBMBIO.COM";
    fileName[ 1 ] = "IBMDOS.COM";
    for (i = 0; i < CFILES; i++) {
        strcpy (FNSRC, FNDRV);
        strcat (FNSRC, fileName[i]);
        FNSRC[0] += dSrc;
        if ((fh = OpenFile( FNSRC, 0 )) == -1) {
            if (!i) {
                fileName[ 0 ] = "IO.SYS";
                fileName[ 1 ] = "MSDOS.SYS";
                strcpy (FNSRC+3, fileName[0]);
                if ((fh = OpenFile( FNSRC, 0 )) != -1)
                    goto gotbios;
                }

            erc = NOSRCFILEBIAS + i;
            goto cleanup;
            }
gotbios:
        if (i == 0)
            cbBIOS = (WORD)GetFileSize (fh);

        CloseFile( fh );
        }

    /* Copy and adjust boot sector from source to destination */

    if (WriteBootSec (dSrc, dDst, NULL, lpBuf) != NOERROR) {
        erc = INVALIDBOOTSEC;
        goto cleanup;
        }

    /* Grab DPB for destination */
    if ((erc = GetDPB (dDst + 1, &DPB)) != NOERROR)
        goto cleanup;

    /* Get bytes per cluster for destination */
    if (!(cbCluster = GetClusterSize( dDst + 1 ))) {
        erc = INVALIDDSTDRIVE;
        goto cleanup;
        }

    /* Convert size of BIOS into full clusters */
    cClusBIOS = (cbBIOS + cbCluster - 1) / cbCluster;

    /* Grab first sector of destination root directory */
    if (int25 (dDst, lpBuf, 1, DPB.dir_sector) == -1) {
        erc = DSTDISKERROR;
        goto cleanup;
        }

    /* are first two directory entries empty? */
    OpenFAT( FAT_READ );
    if ((lpBuf[0] == 0 || (unsigned char) lpBuf[0] == 0xE5) &&
        (lpBuf[sizeof (struct dirType)] == 0 ||
         (unsigned char) lpBuf[sizeof (struct dirType)] == 0xE5)) {

        /* any of first N (= BIOS size) clusters not empty? */

        for (i = 0; i < cClusBIOS; i++)
            if (UnpackFAT (&DPB, lpBuf, i + 2) != 0) {
                erc = NOTSYSABLE;
                goto cleanup;
                }
        }
    else
        {
        /* first two directory entries NOT BIOS/DOS? */
        if (CheckFileName( lpBuf, fileName[0] ) ||
            CheckFileName( lpBuf+sizeof(struct dirType), fileName[1] )) {
            erc = NOTSYSABLE1;
            goto cleanup;
            }

        /* any of first N clusters NOT allocated to BIOS/DOS */
        clusBIOS = ((struct dirType far *) lpBuf) -> first;
        clusDOS = ((struct dirType far*) (lpBuf + sizeof (struct dirType))) -> first;

        /* do it the hard way, for each cluster 2 .. N+2 see if it is in the
         * chain.
         */
        for (i = 0; i < cClusBIOS; i++) {
            clusTmp1 = clusBIOS;
            clusTmp2 = clusDOS;
            while (TRUE) {
                if (i+2 == clusTmp1 || i+2 == clusTmp2)
                    break;
                if (clusTmp1 != -1)
                    clusTmp1 = UnpackFAT (&DPB, lpBuf, clusTmp1);
                if (clusTmp2 != -1)
                    clusTmp2 = UnpackFAT (&DPB, lpBuf, clusTmp2);
                if (clusTmp1 == -1 && clusTmp2 == -1) {
                    erc = NOTSYSABLE2;
                    goto cleanup;
                    }
                }
            }
        }

    /* Delete destination BIOS/DOS/COMMAND */

    for (i = 0; i < CFILES; i++) {
        strcpy (FNSRC, FNDRV);
        strcat (FNSRC, fileName[i]);
        FNSRC[0] += dDst;
        DeleteFile (FNSRC);
        }

    /* Copy files */

    for (i = 0; i < CFILES; i++) {
        strcpy (FNSRC, FNDRV);
        strcat (FNSRC, fileName[i]);
        FNSRC[0] += dSrc;
        strcpy (FNDST, FNDRV);
        strcat (FNDST, fileName[i]);
        FNDST[0] += dDst;
        if (CopyFile( FNSRC, FNDST, fileAttr[i], lpBuf, maxSize )) {
            erc = i + COPYFILEBIAS;
            goto cleanup;
            }
        }

cleanup:
    if (lpBuf)
        GlobalUnlock (hBuf);
    if (hBuf)
        GlobalFree(hBuf);
    return erc;
}


CheckFileName( lpTmp, pTmp )
char far *lpTmp;
char *pTmp;
{
    char c1, c2;
    int i;

    for (i = 0; i < 11; ++i) {
        c1 = *lpTmp++;
        if ((c2 = *pTmp++) == '.') {
            while (c1 == ' ' && i < 11) {
                c1 = *lpTmp++;
                i++;
                }

            c2 = *pTmp++;
            }

        if (c1 != c2)
            break;
        }

    return (i != 11);
}
