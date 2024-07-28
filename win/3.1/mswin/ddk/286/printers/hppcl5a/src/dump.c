//*********************************************************************
// dump.c -
//
// Copyright (C) 1988,1989 Aldus Corporation
// Copyright (C) 1988-1990 Microsoft Corporation.
// Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
//             All rights reserved.
// Company confidential.
//
//*********************************************************************
//
// The purpose of this module is to convert a banding buffer bitmap
// into a series of PCL graphics and cursor-positioning escapes.
//
// This is based on the original Aldus DUMP.C, but much of the 'inner-loop'
// code has been moved to DUMPUTIL.ASM (which is in the SAME segment).
//
// TETRA |  All PCL V support in this driver is included in this file
// TETRA |  (dump.c).  There is an additional compression function that
// TETRA |  handles Mode 3, as well as conditional code that handles
// TETRA |  differences caused by using a raster seed row.  The lpDevice
// TETRA |  raster bitmap is used as the seedrow; illegal references to
// TETRA |  memory before the bitmap are avoided by printing the first
// TETRA |  raster row of each strip in compression Mode 2.
//
//********************************************************************
  
//**********************************************************************
//
//
// 16 jul 90   SD  Added back in code to skip empty space in a raster
//                 row in order to be able to use mode 2 compression when
//                 there isn't enough memory to bitmap and entire page --
//                 Bug #19.
// 11 Jul 90 - Terry - Copied code from the MicroSoft driver to support  TS
//                     huge bitmaps for the Boise Aug 1 mini-release.    TS
//                                                                       TS
// 28 mar 90 t-bobp Changed references to bitmap to go through           TS
//   bitmap header, in order to work with protected                      TS
//   mode changes that put the bitmap outside of                         TS
//   lpDevice.                                                           TS
//
//
// 22 jan 90    Ken O   Set left margin to 0 for first row in band
//
// 20 jan 90    Ken O   Removed support for non-Galaxy printers
//
// 08 jan 90    Ken O   Fixed landscape & Spud bugs.  Optimized LJ III support
//
// 03 jan 90    Ken O   Avoided sending End Graphics sequence that would
//          reset LJ III's internal seed row.  Inhibit bit-
//          stripping for Mode 3 (we need trailing white bytes
//          to compare with previous row).
//
// 29 dec 89    Ken O   Use Comp Mode 1 for first raster row to avoid
//          reference to memory in front of mem bitmap as a
//          seed row when driving a LJ III
//
// 18 dec 89    Ken O   Added support for raster mode 3
//
// 01 dec 89    peterbe #ifdef'd out DumpLaserPort() call for 3.0.
//
// 07 dec 89    Ken O   minor optimizations
//
// 25 sep 89    peterbe Temporary fix for basic LJ cursor bug: inhibit
//          bitstripping in landscape for HPJET.
//
// 19 sep 89    peterbe Big move of LJ IIP ('Entris') code into this
//          file from includes.
//
// 15 sep 89    peterbe Code cleanup.  Made ordering of positioning
//          escapes same in port. as in landscape.
//          Use SendGraphics() in NEWGRXF.H for SPUD.
//
// 14 sep 89    peterbe Mainly, added SPUD code for portrait mode.
//
// 13 sep 89    peterbe Removed start-graphics escape and y-pos init at
//          start of landscape. Corrected count for graphics
//          bitmap escape, landsc.  Landscape works.  Still
//          need to fix basic LJ cursor correction code.
//
// 12 sep 89    peterbe Adding bitstripping support in landscape mode.
//          Y-positioning (top margin) needs work.
//          CurX(), CurY() need much change in handling
//          of epXerr, epYerr for basic HPPCL printer.
//
// 10 sep 89    peterbe Fixed bug in processing bit strips. Train picture
//          prints OK now (so far bitstripping only in portrait
//          mode - WILL CHANGE SOON!).
//
// 22 aug 89    peterbe Removed commented-out TransposeBitMap().
//
// 16 may 89    peterbe Added () pairs in #defines of escapes
//
// 10 may 89    peterbe Added DumpLaserPort() call.
//
// 27 apr 89    peterbe Replaced code calling dmTranspose() with
//          code calling TransposeBitmap() in DUMPUTIL.A.
//
// 26 apr 89    peterbe Made more variables local to inner blocks.
//          Debugged Landscape output.
//          Optimizing Portrait output.
//
// 25 apr 89    peterbe Added some comments in landscape mode section.
//
// 21 apr 89    peterbe Optimization of portrait-mode output for single-
//          -strip scanlines.
//
// 20 apr 89    peterbe Make scanbits global for debug.
//
// 19 apr 89    peterbe Now using DUMPUTIL.A: FindBitStrips().
//
// 14 apr 89    peterbe Making variables for bitstripping more local.
//
// 13 apr 89    peterbe Now use CompByteArray() for complementing buffer
//          (GDI-to-laser conversion of bitmap) and StripTRail()
//          for finding last nonwhite byte in a scanline.
//
// 12 apr 89    peterbe Modifying indentation for portrait mode.
//
// 11 apr 89    peterbe Modifying code to use SendGrEsc() in DUMPUTIL.A.
//
// 21 mar 89    peterbe Checked in comment & cosmetic changes.
//
//   1-18-89    jimmat  Removed some static data items, eliminated a far
//          ptr to our data seg, and cleaned up some _really_
//          ugly code (lots more still here).
//   2-24-89    jimmat  LaserPort code now in lasport.a, no stubs here.
//
//**********************************************************************
  
  
//#define DEBUG
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#include "fontman.h"
#include "strings.h"
#include "memoman.h"
#include "dump.h"
  
  
#ifdef DEBUG
#define DBGdump(msg) DBMSG(msg)
#define DBGgrx(msg) DBMSG(msg)
#else
#define DBGdump(msg) /* DBMSG(msg) */
#define DBGgrx(msg) /*null*/
#endif
  
// definitions of escapes
  
// Begin graphics -- numeric value must be 1
#define W_START_GRX ('*'+256*'r')
#define B_START_GRX 'A'
  
// end graphics -- no numeric value, use NONUM = 0x8000 as flag
#define W_END_GRX   ('*'+256*'r')
#define B_END_GRX   'B'
  
// SendGrEsc() recognizes this special value -- no numeric field.
#define NONUM       0x8000
  
// Set X position in dots
#define W_SETX      ('*'+256*'p')
#define B_SETX      'X'
  
// Set Horizontal position in decipoints.
#define W_SETH      ('&'+256*'a')
#define B_SETH      'H'
  
// relative vertical settings are always positive ('+') here.
  
// Set Y position in dots (relative)
#define W_SETY      ('*'+256*'p')
#define B_SETY      'Y'
  
// Set Vertical position in decipoints (relative).
#define W_SETV      ('&'+256*'a')
#define B_SETV      'V'
  
// Output graphics.  Numeric value is following byte count.
#define W_GRX       ('*'+256*'b')
#define B_GRX       'W'
  
// graphics compression mode escape:
//
// Set Raster compression mode "* b <mode> M"
// mode 0 is normal LJ raster transfer
// mode 1 is compressed: count + data repeat blocks
// mode 2 is mixed repeated runs and literal runs.  We use THIS mode
//      for the first raster row in each band.
// mode 3 is offset and count bytes followed by differences from previous row.
//      We use THIS mode for most LJ III output.
  
#define W_COMPRESS_MODE ('*'+256*'b')
#define B_COMPRESS_MODE 'M'
  
extern WORD __AHINCR;                                                   //TS
#define ahincr  ((WORD)(&__AHINCR))                                      //TS
//TS
#define LOWORD(l)       ((WORD)(l))                                       //TS
#define HIWORD(l)       ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))             //TS
//TS
// Type definition                                                      //TS
//TS
#define POSSIZE 12                                                      //TS
//TS
typedef struct                                                          //TS
{                                                                       //TS
    short startpos;                                                     //TS
    short endpos;                                                       //TS
} PosArray[POSSIZE];                                                    //TS
  
  
// functions defined in DUMPUTIL.A
  
// send an escape sequence, and possibly some graphics.
int NEAR PASCAL SendGrEsc(LPDEVICE, // lpDevice
WORD,       // 2 characters starting escape
BOOL,       // num. value relative?
int,        // numeric value
char,       // terminating character (endEsc)
LPSTR);     // --> graphics bytes if not NULL.
  
// complement bytes in a scanline, to turn GDI blackness into laser
// printer gloom.
void NEAR PASCAL CompByteArray(LPSTR,   // points to array.                //TS
long);       // byte count                                                //TS
  
// Find last nonzero byte in a scanline (black = 0)
int NEAR PASCAL StripTrail(LPSTR,   // points to beginning of scanline.
int);       // width in bytes
  
/* Begin Bug#19 */
// Find sequences of nonzero bytes in a scanline
// returns number of bit strips found.
int NEAR PASCAL FindBitStrips(LPSTR,       // points to beginning of scanline,
PosArray FAR *,       // points to PosArray structure
int,                  // length of scanline,
int);                 // no. of entries in PosArray.
/* End Bug#19 */
  
  
// Copy a byte column of a landscape-mode bitmap into a buffer,
// transposing the bit array:  in the buffer, the scanlines run
// 'vertically' relative to the landscape-mode page.
// The dimensions of the buffer are 8 scanlines of heightbytes bytes.
  
// 01 may 90 t-bobp Returns TRUE if data transposed, FALSE if no data    //TS
//TS
BOOL NEAR PASCAL TransposeBitmap(LPSTR, // top of column in bitmap.      //TS
LPSTR, // origin of buffer.                                          //TS
int, // width of band (bitmap)                                       //TS
int, // height in bytes of band.                                     //TS
int, // last valid offset in bitmap segment                          //TS
int); // height in scanlines                                         //TS
  
// Dump graphics using LaserPort (or similar, compatible) hardware.
// CONTROL previously set up parameters via calls to functions in
// LASPORT.A.  This just calls an INT 2F function.
  
#ifdef VISUALEDGE
void NEAR PASCAL DumpLaserPort();
#endif
  
// compress scanline for IIP
int NEAR PASCAL LJ_IIP_Comp(LPSTR, LPSTR, int);
// compress scanline for III
int NEAR PASCAL LJ_III_Comp(LPSTR, LPSTR, LPSTR, int);
  
// Forward declarations -- these are declared here
  
void NEAR PASCAL SendGraphics(LPDEVICE, LPSTR, int);
void SetY(LPDEVICE, int, BOOL /* bRel */);
void SetX(LPDEVICE, int, BOOL /* bRel */);
  
int FAR PASCAL dump(lpDevice)
LPDEVICE lpDevice;
{
    // NOTE: many variables used in this function are local to inner
    // blocks.  There is NO run-time disadvantage to this, and it saves
    // stack space and increases code readability.
    /* Begin Bug#19 */
    PosArray scanbits;             /* start/end indices for bitmapping*/
    /* End Bug#19 */
    LPSTR buf, bmp;
    int widthbytes, height, compwidth;
    int buflen, oldlen;     // buflen = scanline buffer length
    // oldlen = previous buflen
    int err = SUCCESS;
    BOOL bIngraphics;           // flags graphic esc. has been sent
    BOOL bDidCompEsc;           // sent escape for Mode 2 or 3
    /* Begin Bug#19 */
    BOOL bUseModeIII;                 // use compression mode 3
    /* End Bug#19 */
#ifdef DEBUG_FUNCT
    DB(("Entering dump\n"));
#endif
  
    if (RealMode)                                                        //TS
        lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;                    //TS
  
#ifdef VISUALEDGE
    // Are we going to let the INT 2F function on a laser printer
    // card do this?
    if (lpDevice->epOptions & OPTIONS_DPTEKCARD)
    {
        DumpLaserPort();
        SendGrEsc(lpDevice, W_END_GRX, FALSE, NONUM, B_END_GRX, NULL);
        return(SUCCESS);
    }
#endif
  
    bDidCompEsc = FALSE;
  
    // force graphics escape on first output
    bIngraphics = FALSE;
  
    // get dimensions of band
    widthbytes = lpDevice->epBmpHdr.bmWidthBytes;
    height = lpDevice->epBmpHdr.bmHeight;
  
    /* Begin Bug#19 */
    /* if insuffienct available memory for a full page bitmap, use mode 2 */
    if (lpDevice->epAvailMem < 850000)
        bUseModeIII = FALSE;
    else
        bUseModeIII = TRUE;
    /* End Bug#19 */
  
#ifdef DEBUG
    DBGdump(("lpDevice->epBmpHdr.bmWidthBytes: %d\n", widthbytes));
    DBGdump(("lpDevice->epBmpHdr.bmHeight: %d\n", height));
#endif
  
    /*                                                                   //TS
    * 05 apr 90    t-bobp  Complement the ENTIRE bitmap, it's easier    //TS
    *       that way. (bmWidthBytes has been modified to                //TS
    *       reflect smaller, right justified bitmap.)                   //TS
    *       (In landscape mode.)                                        //TS
    */                                                                  //TS
    if ( lpDevice->epType == (short)DEV_LAND )                           //TS
        compwidth = ((lpDevice->epBandDepth+15)/16)*2;                   //TS
    else                                                                 //TS
        compwidth = widthbytes;                                          //TS
    //TS
    height = lpDevice->epBmpHdr.bmHeight;                                //TS
    //TS
    bmp = lpDevice->epBmpHdr.bmBits;                                     //TS
    //TS
#if 0                                                                    //TS
    // Complement bitmap: black is 0 in GDI, 1 for the PCL laser printer.//TS
    for ( i = 0; i < height; ++i )                                       //TS
    {                                                                //TS
        CompByteArray(bmp, compwidth);     // Do it a line at a time     //TS
        if ( ! lpDevice->epBmpHdr.bmSegmentIndex                         //TS
            || ( (0x10000 - (long)compwidth) -                               //TS
            (long)LOWORD(bmp) ) > compwidth )                           //TS
            bmp += compwidth;                                            //TS
        else     // Otherwise we're at segment boundary                  //TS
            bmp = (LPSTR)MAKELONG(HIWORD(bmp) + ahincr, 0);              //TS
    }                                                                //TS
#else                                                                    //TS
    //TS
    /* this guy now supports BIG things!                                 //TS
    * CraigC                                                            //TS
    */                                                                  //TS
    if ( ProtectMode )                                                   //TS
        CompByteArray(bmp, GlobalSize(HIWORD(bmp)));                     //TS
    else                                                                 //TS
        CompByteArray(bmp, (long)(compwidth*height));                    //TS
    //TS
#endif                                                                   //TS
    //TS
    // is it Landscape mode??                                                //TS
    //TS
    /****************** Landscape does not work with huge bitmaps!!  **********/
  
    if (lpDevice->epType == (short)DEV_LAND)
    {   // landscape
  
        // local variables for landscape.
        int heightbytes;
        int bytecolumn;
        int relxpos;
        int x;
  
        DBGdump(("dump(): (landscape)\n"));
  
        // if top margin has changed, set Y
        if (lpDevice->epCury)
            SetY(lpDevice, 0, FALSE);
  
        // reset blank-scanline counter
        relxpos = 0;
  
        // width of bitmap in bytes = height of band in bytes
        heightbytes = height / 8;
  
        // move right-to-left across band bitmap, a byte column at a time.
        // The raster lines of GDI's band bitmap are at right angles to
        // what the printer needs, so we do a bit transposition.
  
        for (bytecolumn = widthbytes - 1; bytecolumn >= 0; bytecolumn--)
        {
            // fill an (8 x heightbytes) buffer with transposed
            // pixels from the original bitmap.
  
  
  
            if ( ! TransposeBitmap(
                ((LPSTR) (lpDevice->epBmpHdr.bmBits)) + bytecolumn,
                ((LPSTR) lpDevice) + lpDevice->epBuf,
                widthbytes, heightbytes,
                lpDevice->epBmpHdr.bmWidthBytes *
                ( lpDevice->epBmpHdr.bmScanSegment - 1 ) + bytecolumn,
                height)
                )
            {
                relxpos += BYTESIZE;    // skip these 8 lines
                continue;
            }
  
            // now output the 8 transposed scanlines from the buffer.
  
            buf = ((LPSTR) lpDevice) + lpDevice->epBuf;
            /* Begin Bug#19 */
            if (bUseModeIII)
            {
                /* End Bug #19 */
  
                for (x = 0; x < BYTESIZE; x++, buf += heightbytes)
                {
                    //  strip trailing blanks:  find the last nonzero byte.
                    //  For Galaxy, strip trailing blanks, but leave enough
                    //  to compare with the last non-blank in the seed row
  
                    buflen = StripTrail(buf, heightbytes);
                    if (x && buflen && (oldlen > buflen))
                    {
                        int swap;
  
                        swap = oldlen;
                        oldlen = buflen;
                        buflen = swap;
                    }
                    else
                        oldlen = buflen;
  
#ifdef DEBUG
                    if (buflen)
                        DBGdump(("buflen = %d\n", buflen));
#endif
  
                    // (more landscape)
  
                    if (buflen)
                    {   // (vertical) scanline isn't blank
  
                        // do we need to send an end-graphics escape?
                        if (relxpos && bIngraphics)
                        {
                            bIngraphics = FALSE;
                            SendGrEsc(lpDevice, W_END_GRX, FALSE,
                            NONUM, B_END_GRX, NULL);
                        }
  
                        // if there have been blank lines, adjust cursor.
                        if (relxpos)
                        {   // this is a relative motion.
                            SetX(lpDevice, -relxpos << lpDevice->epScaleFac,
                            TRUE);
                            relxpos = 0;
                        }
  
                        // if necessary, send start graphics:  Esc * r 1 A
                        if (!bIngraphics)
                        {
                            bIngraphics = TRUE;
                            SendGrEsc(lpDevice, W_START_GRX, FALSE, 1,
                            B_START_GRX, NULL);
                        }
  
                        // This sets the compression mode
                        // We do this the first time we do graphics for
                        // each band now.
  
                        /* are we on the first raster row? */
                        if (x == 0) {
                            /* Inform SendGraphics() that we're on */
                            /* the band's FIRST ROW of raster      */
                            /* graphics by using the sign bit      */
                            buflen = -(buflen);
  
                            SendGrEsc(lpDevice, W_COMPRESS_MODE,
                            FALSE, 2, B_COMPRESS_MODE, NULL);
  
                            /* Now we're out of Mode 3 */
                            bDidCompEsc = FALSE;
#ifdef DEBUG
                            DBGdump(("Mode 2\n"));
#endif
                        }
                        else
                        {
                            if (!bDidCompEsc)
                            {
                                SendGrEsc(lpDevice, W_COMPRESS_MODE,
                                FALSE, 3, B_COMPRESS_MODE, NULL);
                                bDidCompEsc = TRUE;
  
#ifdef DEBUG
                                DBGdump(("Mode 3\n"));
#endif
                            }   /* if (!bDidCompEsc) */
                        }       /* else ...  [x != 0] */
  
                        // do data conversion before outputting (landscape mode)
                        SendGraphics(lpDevice, buf, buflen);
                    }
                    else
                        //empty raster line -- just increment blank line counter
                        relxpos++;
  
                }   // for (x...)
                /* Begin Bug#19 */
            }                 /* end of UseModeIII */
            else         /* use mode II  */
  
  
            {
                for (x = 0; x < BYTESIZE; x++, buf += heightbytes)
                {
                    // we're processing a scanline of the transposed buffer.
                    int nstrips;     // number of bit strips in the scanline
  
                    // Find the bit strips in a scanline (see comment under
                    // portrait mode).
  
                    nstrips = FindBitStrips((LPSTR)buf, (PosArray FAR *)scanbits,
                    heightbytes, POSSIZE);
  
                    //  strip trailing blanks:  find the last nonzero byte.
  
                    buflen = StripTrail(buf, heightbytes);
                    if (buflen)
                        scanbits[nstrips-1].endpos = buflen;
  
#ifdef DEBUG
                    if (buflen)
                    {
                        int i;
  
                        DBMSG(("nstrips = %d ", nstrips));
                        for (i = 0; i < nstrips; i++)
                            DBMSG(("<%d, %d>", scanbits[i].startpos,
                            scanbits[i].endpos));
                        DBMSG((", buflen = %d\n", buflen));
                    }
#endif
  
  
  
                    // (more landscape)
  
                    if (buflen)
                    {  // (vertical) scanline isn't blank
  
                        int stripno;
                        int striplen;
                        int y;
  
                        // output bit strips
                        for (stripno = 0; stripno < nstrips; stripno++)
                            if ((scanbits[stripno].startpos != -1) &&
                                (scanbits[stripno].endpos != -1))
  
                            {   // output next strip
                                striplen = scanbits[stripno].endpos -
                                scanbits[stripno].startpos;
  
                                // get Y position of this strip.
                                y = scanbits[stripno].startpos <<
                                (lpDevice->epScaleFac + 3);
  
                                // do we need to send an end-graphics escape?
                                if (((stripno != 0) ||
                                    (lpDevice->epCury != y) ||
                                    (relxpos != 0)) && bIngraphics)
                                {
                                    bIngraphics = FALSE;
                                    SendGrEsc(lpDevice, W_END_GRX, FALSE,
                                    NONUM, B_END_GRX, NULL);
                                }
  
                                // Adjust X for strips after the 'topmost' one
                                // (the printer automatically decrements X after each
                                // strip, we have to back up the cursor).
                                if (stripno != 0)
                                    SetX(lpDevice, 1 << lpDevice->epScaleFac, TRUE);
  
                                // if top margin has changed, set Y
                                if (lpDevice->epCury != y)
                                    SetY(lpDevice, y, FALSE);
  
                                // if there have been blank lines, adjust cursor.
                                if (relxpos != 0)
                                { // this is a relative motion.
                                    SetX(lpDevice, -relxpos << lpDevice->epScaleFac,
                                    TRUE);
                                    relxpos = 0;
                                }
  
                                // This sets compression mode 2.
                                // We do this the first time we do graphics for
                                // each band now.
  
                                if (!bDidCompEsc)
                                {
                                    SendGrEsc(lpDevice, W_COMPRESS_MODE,
                                    FALSE, 2, B_COMPRESS_MODE, NULL);
                                    bDidCompEsc = TRUE;
                                }
  
                                // if necessary, send start graphics:  Esc * r 1 A
                                if (!bIngraphics)
                                {
                                    bIngraphics = TRUE;
                                    SendGrEsc(lpDevice, W_START_GRX, FALSE, 1,
                                    B_START_GRX, NULL);
                                }
  
                                // do data conversion before outputting (landscape mode)
                                /* NOTE:  There is a kludge here -- a negative value in the
                                incount parameter clues SendGraphics in that mode 2
                                compression should be used.
                                */
                                SendGraphics(lpDevice,
                                buf + scanbits[stripno].startpos,
                                - striplen);
                            } /*end output next strip */
  
                    } /* buflen non zero   */
                    else
                        //empty raster line -- just increment blank line counter
                        relxpos++;
  
                }   // for (x...)
  
            } /* use mode II */
            /* End Bug#19 */
  
        }   // for (bytecolumn ...)
  
        // Send the end graphics escape: esc * r B
        SendGrEsc(lpDevice, W_END_GRX, FALSE, NONUM, B_END_GRX, NULL);
  
        if (relxpos)
        {
            SetX(lpDevice, -(relxpos << lpDevice->epScaleFac), TRUE);
            relxpos = 0;
        }
  
    }   // end of landscape
  
  
    // Handle PORTRAIT mode.
  
    else    // The mode is PORTRAIT
    {
        // local variables
        int y;
        int relypos;
  
        DBGdump(("dump() (portrait)\n"));
  
        // if left margin has changed, set X
        if (lpDevice->epCurx)
            SetX(lpDevice, 0, FALSE);
  
        // reset blank-scanline counter
        relypos = 0;
  
        bmp = lpDevice->epBmpHdr.bmBits;
  
        /* Begin Bug#19 */
        if (bUseModeIII)
        {
            /* End Bug #19 */
            for (y = 0; y < height; y++)
            {
                //  Strip trailing blanks, but leave enough
                //  to compare with the last non-blank in the seed row
                //          DBGdump(("Y= %d, height = %d\n", y, height));
                //          DBGdump(("About to call StripTrail\n"));
                //          DBGdump(("StripTrail parameters are bits = (%d,)(%d), wb= %d\n",
                //          HIWORD(bmp), LOWORD(bmp), widthbytes));
  
                buflen = StripTrail(bmp, widthbytes);
                //         buflen = 300;
                //         DBGdump(("Out of the StripTrail function.\n"));
  
                if (y && buflen && (oldlen > buflen))
                {
                    int swap;
  
                    swap = oldlen;
                    oldlen = buflen;
                    buflen = swap;
                }
                else
                    oldlen = buflen;
  
#ifdef DEBUG
                // portrait, remember
                //          if (buflen)
                //              DBGdump(("buflen = %d\n", buflen));
#endif
  
                // (more portrait)
  
                if (buflen)
                { // scanline isn't blank
  
                    // do we need to send an end-graphics escape?
                    if (relypos && bIngraphics)
                    {
#ifdef DEBUG
                        DBGdump(("Esc*rB\n"));
#endif
                        bIngraphics = FALSE;
                        SendGrEsc(lpDevice, W_END_GRX, FALSE,
                        NONUM, B_END_GRX, NULL);
                    }
  
                    if (relypos)
                    {
                        SetY(lpDevice,
                        relypos << lpDevice->epScaleFac,TRUE);
                        relypos = 0;
                    }
  
                    // If necessary, send start Graphics: Esc * r 1 A
                    if (!bIngraphics)
                    {
#ifdef DEBUG
                        DBGdump(("Esc*r1A\n"));
#endif
                        bIngraphics = TRUE;
                        SendGrEsc(lpDevice, W_START_GRX, FALSE, 1,
                        B_START_GRX, NULL);
                    }
  
                    // This sets the compression mode
                    // We do this the first time we do graphics for
                    // each band now.
  
                    /* are we on the first raster row? */
                    if ((y == 0) || (LOWORD(bmp) < widthbytes)){                    //TS
                        /* Inform SendGraphics() that we're on */
                        /* the band's FIRST ROW of raster      */
                        /* graphics by using the sign bit      */
                        DBGdump(("Dumping segment address (%d)(%d)\n",
                        HIWORD(bmp), LOWORD(bmp)));
                        buflen = -(buflen);
  
                        SendGrEsc(lpDevice, W_COMPRESS_MODE,
                        FALSE, 2, B_COMPRESS_MODE, NULL);
  
                        /* Now we're out of Mode 3 */
                        bDidCompEsc = FALSE;
#ifdef DEBUG
                        DBGdump(("Mode 2\n"));
#endif
                    }
                    else
                    {
                        if (!bDidCompEsc)
                        {
                            SendGrEsc(lpDevice, W_COMPRESS_MODE,
                            FALSE, 3, B_COMPRESS_MODE, NULL);
                            bDidCompEsc = TRUE;
  
#ifdef DEBUG
                            DBGdump(("Mode 3\n"));
#endif
                        }   /* if (!bDidCompEsc) */
                    }   /* else ...  [y != 0] */
  
                    // do data conversion before outputting (landscape mode)
  
                    SendGraphics(lpDevice, bmp, buflen);
  
                } // end .. scanline isn't blank
  
                else
                {
                    //empty raster line ..  increment blank line counter
                    relypos++;
                }
  
                if ( ! lpDevice->epBmpHdr.bmSegmentIndex                         //TS
                    || ( (0x10000 - (long)widthbytes) -                              //TS
                    (long)LOWORD(bmp) ) > widthbytes )                          //TS
                {
                    bmp += widthbytes;                                           //TS
                    //          DBGdump(("Dumping row address (%d)(%d)\n",
                    //          HIWORD(bmp), LOWORD(bmp)));
                }
                else     // Otherwise we're at segment boundary                  //TS
                {
                    bmp = (LPSTR)MAKELONG(HIWORD(bmp) + ahincr, 0);              //TS
                    DBGdump(("Dumping new segment address %lp\n",bmp));
                    DBGdump(("ahincr = %d\n", ahincr));
                }
  
            }   // for (y ..)
  
            /* Begin Bug#19 */
        } /* end of use comp mode III */
        else /* use compression mode II */
        {
            DBMSG(("In Compression Mode 2 code\n"));
            for (y = 0; y < height; y++)
            {
                int nstrips;       // number of 'bit strips' in scanline
  
                // find the 'bit strips' in the scanline.  This routine scans
                // through a scanline, finds sequences of nonwhite (nonzero)
                // bytes separated by > 32 0 bytes, and puts the starting and
                // ending points in scanbits[].  The scanbits[] array must be
                // defined large enough to handle all possible bit strips
                // in a scanline.
  
                nstrips = FindBitStrips((LPSTR)bmp, (PosArray FAR *)scanbits,
                widthbytes, POSSIZE);
  
  
                //  strip trailing blanks
                buflen = StripTrail(bmp, widthbytes);
  
#ifdef DEBUG
                // portrait, remember
                if (buflen)
                {
                    int i;
  
                    DBMSG(("nstrips = %d ",  nstrips));
                    for (i = 0; i < nstrips; i++)
                    {
                        DBMSG(("<%d, %d>",
                        scanbits[i].startpos,
                        scanbits[i].endpos));
                    }
                    DBMSG((", buflen = %d\n", buflen));
                }
#endif
  
                if (buflen)
                    scanbits[nstrips-1].endpos = buflen;
  
                // (more portrait)
  
                if (buflen)
                {   // scanline isn't blank
  
                    int stripno;
                    int striplen;
                    int x;
  
                    // Output bit strips.
                    for (stripno = 0; stripno < nstrips; stripno++)
                        // start and end must both not be -1
                        if ((scanbits[stripno].startpos != -1) &&
                            (scanbits[stripno].endpos != -1))
                        {    // output next strip
  
                            // get length of strip.
                            striplen = scanbits[stripno].endpos -
                            scanbits[stripno].startpos;
  
                            // get X position.
                            x = scanbits[stripno].startpos <<
                            (lpDevice->epScaleFac + 3);
  
                            // do we need to send an end-graphics escape?
                            if (((stripno != 0) ||
                                (lpDevice->epCurx != x)||
                                (relypos != 0)) && bIngraphics)
                            {
                                bIngraphics = FALSE;
                                SendGrEsc(lpDevice, W_END_GRX, FALSE,
                                NONUM, B_END_GRX, NULL);
                                DBMSG(("Send end graphics escape\n"));
                            }
  
                            // adjust Y for strips after the leftmost one..
                            // (the printer automatically increments Y after each
                            // strip, we have to back up the cursor).
                            if (stripno != 0)
                                SetY(lpDevice, -1 << lpDevice->epScaleFac,
                                TRUE);
  
                            // if left margin has changed, set X (absolute)
                            if (lpDevice->epCurx != x)
                                SetX(lpDevice, x, FALSE);
  
                            if (relypos != 0)
                            {
                                SetY(lpDevice,
                                relypos << lpDevice->epScaleFac,TRUE);
                                relypos = 0;
                            }
  
                            // This sets compression mode 2.
                            // We do this the first time we do graphics for
                            // each band now.
  
                            if (!bDidCompEsc)
                            {
                                SendGrEsc(lpDevice, W_COMPRESS_MODE,
                                FALSE, 2, B_COMPRESS_MODE, NULL);
                                bDidCompEsc = TRUE;
                                DBMSG(("Sent Compression Mode 2\n"));
                            }
  
                            // If necessary, send start Graphics: Esc * r 1 A
                            if (!bIngraphics)
                            {
                                bIngraphics = TRUE;
                                SendGrEsc(lpDevice, W_START_GRX, FALSE, 1,
                                B_START_GRX, NULL);
                                DBMSG(("Send start graphics escape\n"));
                            }
  
                            // do data conversion before outputting (landscape mode)
                            DBMSG(("SendGraphics: stripno=%d, striplen=%d, x=%d\n",
                            stripno, striplen, x));
                            /*  NOTE:  There is a kludge here -- a negative
                            value in the incount parameter clues SendGraphics
                            that mode 2 compression should be used.
                            */
                            SendGraphics(lpDevice,
                            bmp + scanbits[stripno].startpos,
                            - striplen);
  
                        }  // end .. for (stripno...)
                } // end .. scanline isn't blank
  
                else
                {
                    //empty raster line ..  increment blank line counter
                    relypos++;
                    DBMSG(("Empty raster line\n"));
                }
  
                if ( ! lpDevice->epBmpHdr.bmSegmentIndex                         //TS
                    || ( (0x10000 - (long)widthbytes) -                              //TS
                    (long)LOWORD(bmp) ) > widthbytes )                          //TS
                {
                    bmp += widthbytes;                                           //TS
                    //          DBGdump(("Dumping row address (%d)(%d)\n",
                    //          HIWORD(bmp), LOWORD(bmp)));
                }
                else     // Otherwise we're at segment boundary                  //TS
                {
                    bmp = (LPSTR)MAKELONG(HIWORD(bmp) + ahincr, 0);              //TS
                    DBGdump(("Dumping new segment address %lp\n",bmp));
                    DBGdump(("ahincr = %d\n", ahincr));
                }
  
            } // for (y ..)
  
        } /* end of use compression mode II */
        /* End Bug#19 */
  
        // Send the end graphics escape: esc * r B
        if (bIngraphics)
#ifdef DEBUG
        {
#endif
            SendGrEsc(lpDevice, W_END_GRX, FALSE, NONUM, B_END_GRX, NULL);
#ifdef DEBUG
            DBGdump(("Esc*rB\n"));
        }
#endif
  
        // adjust Y position if there were any trailing blank scanlines.
        if (relypos)
        {
            SetY(lpDevice, relypos << lpDevice->epScaleFac, TRUE);
            relypos = 0;
        }
  
    }   // portrait
  
#ifdef DEBUG_FUNCT
    DB(("Exiting dump\n"));
#endif
    return (err);
  
}   // end dump()
  
// Function for setting relative or absolute Y position.
  
void SetY(lpDevice, y, bRel)
LPDEVICE lpDevice;
int y;
BOOL bRel;
{
#ifdef DEBUG_FUNCT
    DB(("Entering SetY\n"));
#endif
    //  save where we last set Y
    if (bRel)
        lpDevice->epCury += y;
    else
        lpDevice->epCury = y;
  
    // set Y position in points
    // esc * p <num> Y
    SendGrEsc(lpDevice, W_SETY, bRel, y, B_SETY, NULL);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting SetY\n"));
#endif
} // SetY
  
// Function for setting X position.
  
void SetX(lpDevice, x, bRel)
LPDEVICE lpDevice;
int x;
BOOL bRel;
{
    //  save where we last set X
#ifdef DEBUG_FUNCT
    DB(("Entering SetX\n"));
#endif
    if (bRel)
        lpDevice->epCurx += x;
    else
        lpDevice->epCurx = x;
  
    // Set X position in dots (pixels)
    SendGrEsc(lpDevice, W_SETX, bRel, x,
    B_SETX, NULL);
#ifdef DEBUG_FUNCT
    DB(("Exiting SetX\n"));
#endif
} // SetX
  
//===========================================================================
// Output a (compressed) scanline.
// This is used in both landscape and portrait mode.
//===========================================================================
  
void NEAR PASCAL SendGraphics(lpDevice, bits, incount)
LPDEVICE lpDevice;
LPSTR bits;
int incount;
{
    int complen;
    LPSTR compbuf;
    LPSTR seedbuf;
    //BYTE compbuf[500];
  
    // get address of compression buffer.
#ifdef DEBUG_FUNCT
    DB(("Entering SendGraphics\n"));
#endif
  
    // DBGgrx(("About to get address of compression buffer. \n"));
    compbuf = ((LPSTR)lpDevice) + lpDevice->epLineBuf;
    // DBGgrx(("Got the address of the compression buffer. \n"));
  
    /* DBGgrx(("Compress %db", incount)); */
  
    /* Is it Row 0 ? */
    if (incount < 0)
    {
        // DBGgrx(("About to do LJ_IIP_Comp. \n"));
        // DBGdump(("LJ_IIP_Comp parameters are bits pointer = %lp\n",
        //     bits));
        // DBGdump(("LJ_IIP_Comp compbuf pointer = %lp, incount = %d\n",
        //     compbuf, -(incount)));
        complen = LJ_IIP_Comp( (LPSTR) bits,
        (LPSTR) compbuf,
        -(incount));
        // DBGgrx(("Did the LJ_IIP_Comp!!!!!!!!!!!!!!! \n"));
    }
    else
    {
        /* Get seed row from appropriate memory bitmap */
        //   DBGgrx(("About to get reference the seed row. \n"));
  
        if (lpDevice->epType == (short)DEV_LAND)
            seedbuf = ((LPSTR) bits) - (lpDevice->epBmpHdr.bmHeight / 8);
        else
            seedbuf = ((LPSTR) bits) - lpDevice->epBmpHdr.bmWidthBytes;
        //   DBGgrx(("Did the seed row, now call the compression routine. \n"));
  
        complen = LJ_III_Comp( (LPSTR) bits,
        (LPSTR) seedbuf,
        (LPSTR) compbuf,
        incount);
        //   DBGgrx(("Finished calling the compression routine. \n"));
    }
  
    //   DBGgrx((", complen = %d", complen));
  
    SendGrEsc(lpDevice, W_GRX, FALSE, complen, B_GRX,
    (LPSTR) compbuf);
  
    //   DBGgrx((", sent row.\n"));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting SendGraphics\n"));
#endif
} // SendGraphics()
  
/************************************************************************/
/* Function:      LJ_III_Comp                                           */
/*                                  */
/* Description:   Compresses a string of bytes using the PCL Mode 3     */
/*                algorithm.  Returns the compressed string at pbReturn */
/*                and the new string length as its return value.        */
/*                                                                      */
/* Return:        int outbytes                      */
/*                                                                      */
/* History:   9/07/89 -- created by rbt             */
/*            11/22/89 tw fixed bug at end of line by changing how  */
/*                            pbTemp is initialized the last time   */
/*                            Also, to save making a second pass through*/
/*                            as we are compressing, this line is       */
/*                            saved to the last line                    */
/*            12/18/89 KO converted from PM to Windows and added    */
/*                mnemonic variable names           */
/*                                                                      */
/*                Copyright (C) 1989, 1990 Hewlett-Packard Company      */
/************************************************************************/
int NEAR LJ_III_Comp( pbData,
pbSeedRow,
pbReturn,
usTotalBytes)
  
LPSTR pbData;       /* pointer to original string */
LPSTR pbSeedRow;    /* pointer to previous scanline's string */
LPSTR pbReturn;     /* pointer to return string */
int usTotalBytes;   /* original number of bytes */
  
{
    short int      l;          /* index variable */
    register short int j;      /* index variable */
    short int      cap=0;      /* Current Active Position (x only) */
    int        outbytes=0; /* number of return bytes */
    register short int      diffbytes=0;/* number of bytes since last change */
    short int      offset1,offset2,offset3;  /* offsets from last change. */
    /* We'll never need > 3 bytes */
    /* for LJ III or earlier. */
    LPSTR          pbTemp;     /* temporary index pointer */
    register LPSTR pbLoc;      /* local copy of pointer to string */
    register LPSTR pbLocSeed;  /* local copy of pointer to seed row */
  
#ifdef DEBUG_FUNCT
    DB(("Entering LJ_III_Comp\n"));
#endif
#ifdef DEBUG
    //    DBGdump(("LJ_III_Comp"));
#endif
  
    pbLoc = pbData;
    pbLocSeed = pbSeedRow;
  
    for(j=0;j < usTotalBytes;j++)
    {
        if (*pbLocSeed++ == *pbLoc++) /* new byte and seed byte are same */
        {
            if (diffbytes) /* if 1st non-change after at least 1 change */
            {
                /* compute offsets from last changed byte */
                offset1 = j - cap - diffbytes;
                cap = j;
                if (offset1 > 30)
                {
                    offset2 = offset1 - 31;
                    offset1 = 31;
                    if (offset2 > 254)
                    {
                        offset3 = offset2 - 255;
                        offset2 = 255;
                    }
                }
  
                /* setup byte1 */
                switch (diffbytes)
                {
                    case 1 : *pbReturn = (char)offset1;
                        break;
                    case 2 : *pbReturn = (char)(0x20 | offset1);
                        break;
                    case 3 : *pbReturn = (char)(0x40 | offset1);
                        break;
                    case 4 : *pbReturn = (char)(0x60 | offset1);
                        break;
                    case 5 : *pbReturn = (char)(0x80 | offset1);
                        break;
                    case 6 : *pbReturn = (char)(0xA0 | offset1);
                        break;
                    case 7 : *pbReturn = (char)(0xC0 | offset1);
                        break;
                    case 8 : *pbReturn = (char)(0xE0 | offset1);
                        break;
                }
                pbReturn++;
                outbytes++;
                if (offset1 == 31)
                {
                    *pbReturn++ = (char)offset2;
                    outbytes++;
                    if (offset2 == 255)
                    {
                        *pbReturn++ = (char)offset3;
                        outbytes++;
                    } /* if (offset3 <= 0) ... */
                }    /* if (offset2 >= 0) ... */
  
                pbTemp = pbLoc - diffbytes -1;
                for (l=0;l < diffbytes;l++)
                    *pbReturn++ = *pbTemp++;
                outbytes += diffbytes;
                diffbytes = 0;
            }               /* if (diffbytes) ... */
  
        }              /* if (*pbLocSeed == *pbLoc) ... */
        else               /* seed and new byte are different */
        {
            if (++diffbytes == 8)
            {
                offset1 = j - cap - 7;
                cap = j + 1;
                if (offset1 > 30)
                {
                    offset2 = offset1 - 31;
                    offset1 = 31;
                    if (offset2 > 254)
                    {
                        offset3 = offset2 - 255;
                        offset2 = 255;
                    }
                }
                *pbReturn++ = (char)(0xE0 | offset1);
                outbytes++;
                if (offset1 == 31)
                {
                    *pbReturn++ = (char)offset2;
                    outbytes++;
                    if (offset2 == 255)
                    {
                        *pbReturn++ = (char)offset3;
                        outbytes++;
                    }         /* if (offset3 <= 0) ... */
                }            /* if (offset2 >= 0) ... */
                pbTemp = pbLoc - diffbytes;
                for (l=0;l < diffbytes;l++)
                    *pbReturn++ = *pbTemp++;
                outbytes += diffbytes;
                diffbytes = 0;
            }               /* if (++diffbytes == 8) ... */
        }             /* else ... */
    }                /* for ... */
    if (diffbytes)
    {
        offset1 = j - cap - diffbytes;
        if (offset1 > 30)
        {
            offset2 = offset1 - 31;
            offset1 = 31;
            if (offset2 > 254)
            {
                offset3 = offset2 - 255;
                offset2 = 255;
            }
        }
  
        switch (diffbytes)
        {
            case 1 : *pbReturn = (char)offset1;
                break;
            case 2 : *pbReturn = (char)(0x20 | offset1);
                break;
            case 3 : *pbReturn = (char)(0x40 | offset1);
                break;
            case 4 : *pbReturn = (char)(0x60 | offset1);
                break;
            case 5 : *pbReturn = (char)(0x80 | offset1);
                break;
            case 6 : *pbReturn = (char)(0xA0 | offset1);
                break;
            case 7 : *pbReturn = (char)(0xC0 | offset1);
                break;
            case 8 : *pbReturn = (char)(0xE0 | offset1);
                break;
        }
        pbReturn++;
        outbytes++;
        if (offset1 == 31)
        {
            *pbReturn++ = (char)offset2;
            outbytes++;
            if (offset2 == 255)
            {
                *pbReturn++ = (char)offset3;
                outbytes++;
            }
        }
        pbTemp = pbLoc - diffbytes;        // 11/22/89 tw
  
        for (l=0;l < diffbytes;l++)
            *pbReturn++ = *pbTemp++;
        outbytes += diffbytes;
    }                   /* if (diffbytes) ... */
  
#ifdef DEBUG_FUNCT
    DB(("Exiting LJ_III_Comp\n"));
#endif
    return(outbytes);
  
}   /* LJ_III_Comp */
  
