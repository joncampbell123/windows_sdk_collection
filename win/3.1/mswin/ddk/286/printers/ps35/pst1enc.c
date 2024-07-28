/**[f******************************************************************
 * pst1enc.c - Type 1 CharString/encryption functions
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1990, 1991 Microsoft Corporation.
 * Company confidential.
 *
 * This module provides all of the support necessary to generate the
 * eexec-encrypted portion of a Type 1 font.  This includes encoding
 * an outline definition, encrypting it using CharString encryption,
 * and output of the data in hexadecimal format.  Initialization code
 * handles the "four random bytes in the beginning" bogosity and provides
 * a general, restartable, encryption cipher.
 *
 **f]*****************************************************************/

#define NOCOMM
#define NOKANJI
#include "pscript.h"
#include "pst1enc.h"
#include "channel.h"
#include "printers.h"
#include "resource.h"
#include "getdata.h"
#include "psdata.h"

#ifdef T1SNOOP
#define DPRINT(x) PrintChannel x
//  doesn't work worth a damn
static  LPDV  Glpdv;
static char *szCmdNames[34] = { NULL, NULL, NULL,
                                NULL, "VMOVETO", "RLINETO", "HLINETO",
                                "VLINETO", "RRCURVETO", "CLOSEPATH",
                                NULL, NULL, "SBW", "HSBW", "ENDCHAR", NULL,
                                NULL, NULL, NULL, NULL, NULL, "RMOVETO",
                                "HMOVETO", NULL, NULL, NULL, NULL, NULL,
                                NULL, NULL, "VHCURVETO", "HVCURVETO"
                              };
#endif

#define CSALLOC 1000    // Initial size of CharString buffer 
#define CSGROW  500     // Amount to grow CharString buffer on a ReAlloc

#define RemCSBuffer()   (pCSEnd - pCSPos)   // # bytes free left in buffer

#define abs(x)  (((x) < 0) ? (-(x)) : (x))

static HANDLE hCSBuffer;
static PBYTE pCSBuffer, pCSPos, pCSEnd;
static WORD wCSSize;

static WORD rEExec;     // current seed for eexec functions 

#ifdef  DEBUG
void FAR PASCAL OutputDebugString(LPSTR);
#endif

/***************************************************************************
*
* Random number generation routines.  We don't want to pull in the C 
* runtimes just for this function.  The algorithm is taken from Knuth
* Vol. 2, pp. 170-171.  Not particularly exciting but it is a pretty good
* random number generator and it is efficient.  The Type 1 spec says the
* first four bytes of eexec-encrypted data must be random.  This seems 
* strange unless the PostScript font manager uses this as an unique ID of
* some type.
*
***************************************************************************/

static unsigned seed = 2946; // arbitrary number

/* sets the random number generator seed. */
void srand(unsigned newseed)
{
    seed = newseed;
}

/* returns a 16 bit random number. */
int rand(void)
{

    seed = LOWORD(3141L * seed) + 1;

    return seed;
}


/****************************************************************
* Name: AllocCSBuffer()
*
* Action: Allocates a moveable chunk of local memory to store
*	  a Type 1 character definition.  CharString fills in
*	  the buffer based on the commands passed by its caller,
*	  then returns the buffer handle to the caller.  The initial
*         buffer size is determined by the CSALLOC constant.
*
*	  There is no FreeCSBuffer call since CharString dictates
*	  that the caller is responsible for freeing the memory.
*
* Returns: local (moveable) handle to CharString buffer.
*
**************************************************************/

HANDLE NEAR PASCAL AllocCSBuffer(void)
{
    wCSSize = CSALLOC;
    hCSBuffer = LocalAlloc(LHND, wCSSize);

    /* If the allocation succeeded put the buffer info in our globals. */
    if (hCSBuffer) {
        pCSBuffer = pCSPos = LocalLock(hCSBuffer);
        pCSEnd = pCSBuffer + wCSSize;
    }

    return hCSBuffer;
}


/****************************************************************
* Name: GrowCSBuffer()
*
* Action: Grows the currently allocated CharString buffer by CSGROW
*	  bytes.                                                
*
* Returns: TRUE if buffer was grown successfully, FALSE otherwise.
*
**************************************************************/

BOOL NEAR PASCAL GrowCSBuffer(void)
{
    WORD wPos = pCSPos - pCSBuffer;
    HANDLE hNewCSBuffer;
    BOOL bSuccess = TRUE;

    /* 
    ** ReAlloc the pointer, being careful not to lose the original buffer 
    ** handle.  Notice in the case of failure the new buffer handle is 
    ** conveniently set to the old handle and the remainder of the code 
    ** handles success or failure.
    */
    LocalUnlock(hCSBuffer);
    hNewCSBuffer = LocalReAlloc(hCSBuffer, wCSSize + CSGROW, LMEM_MOVEABLE);
    if (!hNewCSBuffer) {
        hNewCSBuffer = hCSBuffer;
        bSuccess = FALSE;
    }

    pCSBuffer = LocalLock(hNewCSBuffer);
    pCSPos = pCSBuffer + wPos;
    wCSSize += bSuccess ? CSGROW : 0;
    pCSEnd = pCSBuffer + wCSSize;
    hCSBuffer = hNewCSBuffer;

    return bSuccess;
}

 
/****************************************************************
* Name: CSAddNumber()
*
* Action: Converts a long int into the Type 1 representation of 
*	  numbers (described in Chapter 6 of the Black Book.)
*	  The basic idea is they have a few special ranges
*	  where they can represent the long in < 4 bytes and
*         store a long + prefix for everything else.
*
*	  The if statements show the range of numbers and the
*	  body of the if statements compute the representation
*	  for that range.  The formulas were derived by reversing
*         the formulas given in the book (which tells how to convert
*         an encoded number back to a long.)  As an example take the
*         108 <= dw <= 1131 range.  The derivation is as follows:
*
*         dw = ((v - 247) * 256) + w + 108.  Find v,w given dw.
*         dw - 108 = ((v - 247) * 256) + w
*         v - 247 = (dw - 108) / 256   
*         *** v = 247 + (dw - 108) / 256 ***
*         *** w = (dw - 108) % 256       ***
*
*         The rest of the derivations are no harder than this one.
*
* Returns: Nothing (not used currently.)
*
**************************************************************/

#define  SHIFTBITS  0
       
LONG NEAR PASCAL CSAddNumber(LONG dw)
{
    LPBYTE lpNum = ((LPBYTE) ((LPDWORD) &dw)) + 3;
    int i;
    BYTE byV1, byV2;

    //  this line is a test!
    dw >>= 16 - SHIFTBITS;  // truncate fraction - see ttfont.c
                            // TTDownLoadT1Font()  which also
                            // defines SHIFTBITS

    /* make sure buffer has room */
    if (RemCSBuffer() < 5 && !GrowCSBuffer())
        return -1L;

    DPRINT((Glpdv, "%ld(%x) ", dw, abs((int)HIWORD(dw))));

    /* encode number based on its value */
    if (dw >= -1131 && dw <= -108) {

        dw = -(dw + 108);
        byV1 = (BYTE) (251 + (dw >> 8));
        byV2 = (BYTE) (dw - ((byV1 - 251) << 8));
        *pCSPos++ = byV1;
        *pCSPos++ = byV2;

    } else if (dw >= -107 && dw <= 107) {

        *pCSPos++ = (BYTE) (dw + 139);

    } else if (dw >= 108 && dw <= 1131) {

        dw -= 108;
        byV1 = (BYTE) (247 + (dw >> 8));
        byV2 = (BYTE) (dw - (byV1 - 247) * 256);
        *pCSPos++ = byV1;
        *pCSPos++ = byV2;

    } else {

        /* fill in prefix */
        *pCSPos++ = 255;

        /* and the DWORD in BIGENDIAN format */
        for (i = 4; i; --i)
            *pCSPos++ = *lpNum--;

    }

    return 0L;
}


/****************************************************************
* Name: Encrypt()
*
* Action: This routine replaces lpBuf with its encrypted version  
*	  as specified by r (the current cipher value.)  The valid
*	  values for r are:
*
*	     - REEXEC.  Initial cipher value for eexec encryption.
*            - RCS.     Initial cipher value for CharString encryption.
*            - A previous return value from Encrypt.
*
*	  See Chapter 7 of the Black Book for an in-depth discussion of
*	  Type 1 encryption.
*
* Returns: The resulting cipher value after encrypting the buffer.  This
*          value is useful for encrypting a big chunk of data a little bit
*          at a time: The first call uses RCS or REEXEC and subsequent calls
*          use the previous return value as the cipher value.
*
**************************************************************/

WORD NEAR PASCAL Encrypt(LPBYTE lpBuf, WORD len, WORD r)
{
    BYTE cipher;

    while (len--) {
        cipher = (BYTE) (*lpBuf ^ (r >> 8));
        r = (cipher + r) * 52845u + 22719u;
        *lpBuf++ = cipher;
    }

    return r;
}


/****************************************************************
* Name: eexec()
*
* Action: Replaces lpBuf with its eexec-encrypted form and dumps 
*	  the buffer in hexadecimal to the output stream. 
*
* Returns: nothing.
*
**************************************************************/

void FAR PASCAL eexec(LPDV lpdv, LPBYTE lpBuf, WORD len, BOOL reset)
{
    static int cnt = 0;
    
    if(reset)
        cnt = 0;

    rEExec = Encrypt(lpBuf, len, rEExec);

    while (len--) {
        PrintChannel(lpdv, "%2x", *lpBuf++);
        if (!(++cnt & 63))
            PrintChannel(lpdv, "\n");
    }
}


/****************************************************************
* Name: StartEExec()
*
* Action: Signals that we wish to start generating the eexec 
*	  portion of the Type 1 font.  The first four bytes of
*	  a Type 1 font must be randomly generated and satisfy 
*	  two constraints which basically ensure that they do
*         not generate whitespace or hexadecimal characters.
*         For more details read Chapter 7 of the Black Book.
*
* Returns: nothing.
*
**************************************************************/

void FAR PASCAL StartEExec(LPDV lpdv)
{
    BYTE buf[4], save[4];
    int i;

#ifdef T1SNOOP
	Glpdv = lpdv;   //  initialize the global lpdv so everyone can use
						//  PrintChannel.
#endif

#if 0
    // initialize the random number seed
    srand(LOWORD(GetTickCount()));

    // output four random bytes subject to following rules (after encryption):
    //   1. first byte cannot be blank, tab, cr, or lf.
    //   2. at least one of the bytes must not be in [0-9a-fA-F]

    do {
        rEExec = REEXEC;

        for (i = 0; i < 4; ++i)
            buf[i] = save[i] = (BYTE)rand();

        Encrypt(buf, 4, REEXEC);

        // Satisfy rule 1:
        switch (buf[0]) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                continue;
        }

        // Satisfy rule 2:
        for (i = 0; i < 4; ++i)
            if (!((buf[i] >= '0' && buf[i] <= '9')
                  || (buf[i] >= 'a' && buf[i] <= 'f')
                  || (buf[i] >= 'A' && buf[i] <= 'F')))
                break;
        if (i < 4)
            break;

    } while (1);

#endif

    rEExec = REEXEC;

    save[0] = 71;
    save[1] = 36;
    save[2] = 181;
    save[3] = 202;
    eexec(lpdv, save, 4, TRUE);
}


/****************************************************************
* Name: EndEExec()
*
* Action: Signals the driver is done generating the eexec portion
*	  of a Type 1 font.  Currently this function does nothing
*	  but it is provided as a hook for some future use and to
*	  provide an orthogonal API set.
*
* Returns: nothing.
*
**************************************************************/

void FAR PASCAL EndEExec(LPDV lpdv)
{
}


/****************************************************************
* Name: efprintf()
*
* Action: Outputs a formatted control string in eexec format and
*	  sent to the output stream in hexadecimal.  The control
*         string can be a pointer to a NULL-terminated string OR
*         an int resource (assumes segment 0 is an invalid selector
*         (it is) and stores the resource number in the low word.
*         This is exactly what USER does with MAKEINTRESOURCE.)
*
*         *** This routine must be FAR (not FAR PASCAL) since it 
*         *** uses a variable argument list.
*
*         Note: The resources that load this are slightly peculiar
*               in that they have a special marker at the end.  The
*               NullTerminateData() function replaces the marker with
*               a NULL.  Make sure that all resources passed to 
*               efprintf() are in this format.
*
*         Note: Performance may show that duplicating the code in
*               eexec instead of doing the call may be a significant
*               win.  If this is the case it will be done.  Until then
*               the current implementation is cleaner.
*
* Returns: nothing.
*
**************************************************************/

void FAR efprintf(LPDV lpdv, LPSTR lpFmt, ...)
{
    static char buf[512];
    int len;
    HANDLE hFmt;
    BOOL bResource;

    bResource = !HIWORD((DWORD)lpFmt);  // assume resource if segment is 0

    /* if resource load it and convert to null-terminated string */
    if (bResource) {
        lpFmt = GetResourceData(&hFmt, MAKEINTRESOURCE(LOWORD((DWORD)lpFmt)),
                                MAKEINTRESOURCE(PS_DATA));
        if (lpFmt)
            NullTerminateData(lpFmt, TERMCHAR);
    }

    /* kick out if invalid pointer */
    if (!lpFmt)
        return;

    /* apply the control string to the input and place in a buffer */
    len = wvsprintf(buf, lpFmt, (LPSTR) (&lpFmt + 1));

    /* free the control string if it was a resource */
    if (bResource)
        UnGetResourceData(hFmt);

#ifdef  T1SNOOP
    PrintChannel(lpdv, buf);
#else
    /* encrypt the data and output */
    eexec(lpdv, buf, len, FALSE);
#endif
}


/****************************************************************
* Name: CharString()
*
* Action: Translates symbolic Type 1 character commands into
*	  their encoded, encrypted equivalent.  The list of
*	  available commands is in pst1enc.h.  They are used
*	  by passing the command constant followed by the long
*         arguments required by the function.
*
*         Example: CharString(RMOVETO, lx, ly);
*
*         To make a character definition use STARTCHAR, followed
*	  by all of the Type 1 character commands, and ending with
*	  ENDCHAR.  The return value from CharString(ENDCHAR) is a
*	  DWORD containing the local handle in the high word and the
*         length in the low word.  The buffer contains the CharString
*         encrypted/encoded representation.  Given the length and the
*         properly encrypted data, the caller has enough information 
*         to generate PS code that will add the character to a font
*         definition.  For more detail see Chapters 2 and 6 in the 
*         Black Book.
*
*         *** This routine must be FAR (not FAR PASCAL) since it 
*         *** uses a variable argument list.
*
* Returns: 0L if the command succeeded, -1L if the command failed.  If 
*          dwCmd == ENDCHAR, the return value is -1L for failure and
*          a DWORD where the low word is the buffer length and the high
*          word is the buffer handle if the command succeeded.
*
**************************************************************/

DWORD FAR CharString(DWORD dwCmd, ...)
{
    LPLONG lpArgs = (LPLONG) (((LPDWORD) &dwCmd)+1);
    WORD  wArgCount;

    switch (dwCmd) {
        case STARTCHAR:
            /* Allocate the buffer */
            if (!AllocCSBuffer())
                return -1L;

            /* insert 4 random bytes as required by Type 1 */
//            for (wArgCount = 0; wArgCount < 4; ++wArgCount)
//                *pCSPos++ = (BYTE)rand();
            *pCSPos++ = (BYTE)71;
            *pCSPos++ = (BYTE)36;
            *pCSPos++ = (BYTE)181;
            *pCSPos++ = (BYTE)202;

            DPRINT((Glpdv, "STARTCHAR\r\n"));

            return 0L;

        case PUSHNUMBER:
            DPRINT((Glpdv, "%ld ", *lpArgs));
            return CSAddNumber(*lpArgs);

        default:

            /* attempt to optimize the command */

            switch (dwCmd) {
                case SBW:
                    /* This can be reduced to an HSBW if the Y components
                    ** are zero.
                    */

                    if (lpArgs[1] || lpArgs[3])
                        break;

                    lpArgs[1] = lpArgs[2];
                    dwCmd = HSBW;
                    break;

                case RMOVETO:
                    /* This can be reduced to a horizontal or vertical 
                    ** movement if one of the components is zero.
                    */

                    if (!lpArgs[1]) {
                        dwCmd = HMOVETO;
                    } else if (!lpArgs[0]) {
                        lpArgs[0] = lpArgs[1];
                        dwCmd = VMOVETO;
                    }
                    break;

                case RLINETO:
                    /* This can be reduced to a horizontal or vertical
                    ** line if one of the components is zero.
                    */

                    if (!lpArgs[1]) {
                        dwCmd = HLINETO;
                    } else if (!lpArgs[0]) {
                        lpArgs[0] = lpArgs[1];
                        dwCmd = VLINETO;
                    }
                    break;

                case RRCURVETO:
                    /* This can be reduced to a simpler curve operator if
                    ** the tangents at the endpoints of the Bezier are 
                    ** horizontal or vertical.
                    */

                    if (!lpArgs[1] && !lpArgs[4]) {
                        lpArgs[1] = lpArgs[2];
                        lpArgs[2] = lpArgs[3];
                        lpArgs[3] = lpArgs[5];
                        dwCmd = HVCURVETO;
                    } else if (!lpArgs[0] && !lpArgs[5]) {
                        lpArgs[0] = lpArgs[1];
                        lpArgs[1] = lpArgs[2];
                        lpArgs[2] = lpArgs[3];
                        lpArgs[3] = lpArgs[4];
                        dwCmd = VHCURVETO;
                    }
                    break;

            }

            wArgCount = HIWORD(dwCmd);  // arg count stored in HIWORD

            /* if buffer isn't big enough to hold this command expand 
            ** buffer first.  Exit if we can't grow buffer.
            **
            ** Note: The formula (wArgCount * 5 + 2) assumes the worst
            **       case size requirement for the current command (all
            **       arguments stored as full longs and a two byte
            **       command.)
            */
            if (RemCSBuffer() < (int) (wArgCount * 5 + 2) 
                && !GrowCSBuffer())
                return -1L;


            /* push the numbers onto the stack */
            while (wArgCount--) {
                if (CSAddNumber(*lpArgs++) < 0)
                    return -1L;
            }

            DPRINT((Glpdv, "%s(%x)\r\n", (LPSTR) szCmdNames[LOBYTE(dwCmd)], LOBYTE(dwCmd)));

            /* push the command onto the stack */
            *pCSPos++ = (BYTE) (dwCmd & 255);

            if (pCSPos[-1] == 12)   // two byte command
                *pCSPos++ = (BYTE) ((dwCmd >> 8) & 255);

            /* If this isn't the end of a character definition return success */
            if (dwCmd != ENDCHAR)
                return 0L;

            /* We have finished the character: encrypt it and return
            ** the final length.
            */
            Encrypt(pCSBuffer, pCSPos - pCSBuffer, RCS);
            LocalUnlock(hCSBuffer);
            return MAKELONG(pCSPos - pCSBuffer, hCSBuffer);
    }
}


#ifdef DEBUG
void FAR DbPrintf(LPSTR lpFmt, ...)
{
    char buf[256];

    wvsprintf((LPSTR) buf, lpFmt, (LPSTR) (&lpFmt + 1));
    OutputDebugString((LPSTR) buf);
}
#endif
