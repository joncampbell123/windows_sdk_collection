/**[f******************************************************************
* sfdir.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/********************************   sfdir.c   ******************************/
/*
*  SFdir:  Memory manager for soft font install directory.
*
*
*  The SF directory structure looks like this:           SF buffer:
*
*     ------------------------------------               ---------
*     |          (header)                |               | headr.|
*     ------------------------------------               ---------
*     | SF_PATH | usage | size | offset ---------------> | ind 0 |
*     ------------------------------------               |       |
*     | SF_FILE | usage | size | offset -----------      |       |
*     ------------------------------------        |      ---------
*     | SF_TKST | usage | size | offset --------  -----> | ind 1 |
*     ------------------------------------     |         |       |
*     | SF_FILE | usage | size | offset -----  |         |       |
*     ------------------------------------  |  |         |       |
*     | SF_xxx  | usage | size | offset  |  |  |         |       |
*     ------------------------------------  |  |         ---------
*     | SF_xxx  | usage | size | offset  |  |  --------> | ind 2 |
*     ------------------------------------  |            |       |
*                                           |            |       |
*                                           |            |       |
*                                           |            |       |
*                                           |            ---------
*                                           -----------> | ind 3 |
*                                                        |       |
*                                                        |       |
*                                                        ---------
*
*
*  The structure consists of an array of pointers, type SFDIRENTRY,
*  which point into a large buffer (type char).  The entries in the
*  buffer vary and are of variable length.  All structures begin with
*  two BYTES which contain the index of the array entry that points
*  to the structure.  Thus, the SF dir may be traversed two ways:
*
*         1. Step through the array of pointers.
*         2. Step through the buffer looking back at the
*            corresponding entry in the array of pointers.
*
*  The SF dir utilities step through the dir using whatever method is
*  most convenient.  For example, addSFdirEntry() uses method one and
*  compressSFdir() uses method two.  Most use method one.
*
*  This structure holds most of the information used by the soft
*  font installer.  Its primary purpose is to manage the life of
*  a file regardless of the owner.  For example, a file may switch
*  owners but the file information itself does not change.  Or a file
*  may be deleted or copied from one place to another -- all owners
*  of the file are immediately updated.
*
*  The other soft font install functions allocate another structure,
*  of type SFLB (see sfutils.h).  This structure contains an array
*  of pointers to the SF dir's array of pointers.  This double-indirection
*  allows the SF dir utilities to shuffle and compress the contents
*  of the SF dir without breaking the SFLB (SF listbox) list.
*
*  The SF dir is also useful because each structure uses exactly the
*  memory it needs (so it is not wasteful on memory -- although it
*  pays the price of more overhead).  It also is useful because, while
*  several small structures are created, changed, and deleted, it places
*  few demands on the Windows memory manager (theoretically it should be
*  faster to allocate one big global block and shuffle information around
*  inside of it instead of allocating lots of small global blocks).
*
*  The SF dir utilities use the concept of FREE and NULL pointers.
*  A FREE pointer is a SFDIRENTRY that points to a block of data but
*  is marked for delete (SF_DEL) and has a usage count of zero.  A
*  NULL pointer is a SFDIRENTRY whose offset value is -1 (i.e., invalid
*  pointer).
*
*  All the space in the buffer is accounted for at all times.  At startup,
*  the SF dir consists of one FREE pointer to all of the buffer.  The rest
*  of the pointers are NULL.  As structures are added, the space pointed
*  to by the FREE pointer is split and one of the NULL pointers is used
*  to point to the split-off space.
*/
  
// **************************************************************************
// History (latest first)
//
// 23 sep 89    peterbe     Slight modification of fix.
// 22 sep 89    peterbe     Moved file name up 2 bytes in lpBuf, in
//              addSFdirFile, to eliminate seg. wraparound.
//              This fixes major bug.
//              Moved #define LOCAL...
// 21 sep 89    peterbe     cleaned up some #ifdefs, removed extra
//              DBG output.
// 20 sep 89    peterbe     in compressSFDir(), moved calc of next
//              value of sf to inside {}, with if(), so
//              we won't try to access past end of segment.
//              Redrew diagram to reflect reality.
//
// **************************************************************************
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOMEMMGR
#include "windows.h"
#include "sfdir.h"
#include "neededh.h"
#include "dosutils.h"
  
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGaddsf(msg)           /*DBMSG(msg)*/
    #define DBGcompress(msg)       /*DBMSG(msg)*/
    #define DBGdumpSFstate(s)      /*dumpSFstate(s)*/
    #define DBGdumpTstate(s)       /*dumpTstate(s)*/
    #define DBGsplitfree(msg)      /*DBMSG(msg)*/
    #define DBGlockentry(msg)      /*DBMSG(msg)*/
    #define DBGdelfile(msg)     /*DBMSG(msg)*/
    #define DBGlocksf(msg)          /*DBMSG(msg)*/
    #define DBGexpand(msg)          /*DBMSG(msg)*/
    #define DBGaddnull(msg)     /*DBMSG(msg)*/
    #define DBGfindnull(msg)       /*DBMSG(msg)*/
#else
    #define DBGaddsf(msg)           /*null*/
    #define DBGcompress(msg)       /*null*/
    #define DBGdumpSFstate(s)      /*null*/
    #define DBGdumpTstate(s)       /*null*/
    #define DBGsplitfree(msg)      /*null*/
    #define DBGlockentry(msg)      /*null*/
    #define DBGdelfile(msg)     /*null*/
    #define DBGlocksf(msg)          /*null*/
    #define DBGexpand(msg)          /*null*/
    #define DBGaddnull(msg)     /*null*/
    #define DBGfindnull(msg)       /*null*/
#endif
  
#define LOCAL static
  
#define MAX_UNUSED  20
#define SF_INITDIRLEN   80
#define SF_INITBUFSZ    1024
#define SF_INITDIRSZ (sizeof(SFDIR) + ((SF_INITDIRLEN-1) * sizeof(SFDIRENTRY)))
#define SF_STEPDIRLEN   80
#define SF_STEPBUFSZ    1024
#define SF_STEPDIRSZ    (SF_STEPDIRLEN * sizeof(SFDIRENTRY))
  
  
  
/*  Forward references
*/
LOCAL LPSFDIR  lockSFdir(void);
LOCAL BOOL     unlockSFdir(LPSFDIR);
LOCAL BOOL     sfmemsame(LPSTR, LPSTR, int);
LOCAL LPSFDIR  splitFreeSpace(LPSFDIR, int, int FAR *, WORD);
LOCAL WORD     compressSFdir(LPSFDIR, long FAR *);
LOCAL LPSFDIR  expandDirBuf(LPSFDIR, WORD FAR *, WORD);
LOCAL LPSFDIR  addNullEntries(LPSFDIR, int FAR *);
LOCAL int      findNullInd(LPSFDIR);
LOCAL int      replaceSFdirEntry(LPSFDIR, int, LPSTR, WORD, int);
LOCAL void     dumpTstate(TOKENSTATE);
LOCAL void     dumpSFstate(TOKENSTATE);
  
  
LOCAL HANDLE gHSFdir = 0;
LOCAL LPSFDIR gLPSFdir = 0L;
LOCAL WORD gCountLocks = 0;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  addSFdirEntry
*/
int FAR PASCAL addSFdirEntry(lpSFdir, lpEntry, state, size)
LPSFDIR lpSFdir;
LPSTR lpEntry;
WORD state;
WORD size;
{
    LPSFDIRENTRY sf;
    LPSTR lpDir;
    WORD freeSize;
    BOOL loadedOnEntry = FALSE;
    int nullInd;
    int freeInd;
    int sfind = -1;
    int ind;
  
    DBGaddsf(("addSFdirEntry(%lp,%lp,%d,%d)",
    lpSFdir, lpEntry, state, size));
    DBGdumpSFstate(state);
    DBMSG(("\n"));
  
    if (lpSFdir)
        loadedOnEntry = TRUE;
    else if (!(lpSFdir = lockSFdir()))
    {
        DBGaddsf(("addSFdirEntry(): failed to lock SF directory\n"));
        goto done;
    }
  
    /*  Roll through the list of directory entries looking for a
    *  duplicate entry.  If one is found, we'll break from the loop
    *  and exit the procedure.  While we're looking, pick up the first
    *  null entry and the best "free" or deleted entry.  These will be
    *  useful if we don't find a match.
    */
    for (nullInd=freeInd=-1, ind=0, freeSize=0xFFFF, sf=&lpSFdir->sf[0];
        ind < lpSFdir->len; ++ind, ++sf)
    {
        /*  If the entry does not point to anything in the directory
        *  then it is a "null" entry.
        */
        if (sf->offset > -1)
        {
            /*  Not worth looking at entries whose size is smaller
            *  than that of the target entry.
            */
            if (sf->size >= size)
            {
                /*  If an entry is marked for delete and has a zero
                *  usage count, it is a "free" entry.  Pick up the
                *  one whose size is closest to the target entry.
                */
                if ((sf->state & SF_DEL) && !sf->usage &&
                    (sf->size < freeSize))
                {
                    freeInd = ind;
                    freeSize = sf->size;
                    DBGaddsf(("addSFdirEntry(): freeInd=%d, freeSize=%d\n",
                    freeInd, freeSize));
                }
  
                /*  If the target entry and the entry we're looking at
                *  are not marked to be unique or marked for file delete,
                *  they have the same type, and their contents are the
                *  same, then we've found a duplicate entry.
                */
                if (!(state & SF_UNIQ) && !(sf->state & SF_UNIQ & SF_FDEL) &&
                    (sf->state & state & SF_TYPE) &&
                    sfmemsame((LPSTR)lpEntry+2,
                    (LPSTR)&lpSFdir->lpDir[sf->offset]+2, size-2))
                {
                    ++sf->usage;
                    sf->state &= ~(SF_DEL);
                    sfind = ind;
                    DBGaddsf(("addSFdirEntry(): matching entry at ind=%d, usage=%d\n",
                    ind, sf->usage));
                    break;
                }
            }
        }
        else if (nullInd < 0)
        {
            nullInd = ind;
            DBGaddsf(("addSFdirEntry(): nullInd=%d\n", nullInd));
        }
    }
  
  
    /*  If we failed to find a match, then we will add a new entry.
    *  If we found a free pointer during the traversal of the directory,
    *  then we'll use that entry.  If we did not find a free pointer,
    *  we'll compress the directory and allocate whatever memory is
    *  needed to accomodate the new entry.
    */
    if (sfind < 0)
    {
        DBGaddsf(("addSFdirEntry(): match not found, freeInd=%d, nullInd=%d\n",
        freeInd, nullInd));
  
        if ((freeInd > -1) &&
            (nullInd > -1 || freeSize - size <= MAX_UNUSED))
            copyentry:
        {
            /*  Make sure that the entry we are going to use is not
            *  too large -- if it is, then create a new free entry
            *  from the unused space.
            */
            DBGaddsf(
            ("addSFdirEntry(): Checking freeSize against splitFreeSpace()\n"));
            if ((freeSize - size > MAX_UNUSED) &&
                !(lpSFdir = splitFreeSpace(lpSFdir,freeInd,&nullInd,size)))
            {
                DBGaddsf(("addSFdirEntry(): failed to split free space\n"));
                goto done;
            }
  
            /*  Copy in the new entry.
            */
            DBGaddsf(("addSFdirEntry(): Copy in new entry\n"));
  
            sf = &lpSFdir->sf[freeInd];
  
            sf->state = state;
            sf->usage = 1;
            lpDir = (LPSTR)&lpSFdir->lpDir[sf->offset];
  
            DBGaddsf(("addSFdirEntry(): lmemset(lpDir=%lp,0,%d)\n",
            lpDir, sf->size));
            lmemset(lpDir, 0, sf->size);
  
            DBGaddsf((
            "addSFdirEntry(): lmemcpy(lpDir=%lp, lpEntry=%lp, size=%d)\n",
            lpDir, lpEntry, size));
            lmemcpy(lpDir, (LPSTR)lpEntry, size);
  
            DBGaddsf(("addSFdirEntry(): save freeInd\n"));
            *((int FAR *)lpDir) = freeInd;
  
            sfind = freeInd;
  
            DBGaddsf(("addSFdirEntry(): new entry at %d, size=%d\n",
            freeInd, sf->size));
        }
        else
        {
            long freeOffset;
  
            DBGaddsf(("addSFdirEntry(): Collect Free entries, call compressSFDir() \n"));
  
            /*  Collect together all free entries in the directory.
            */
            freeSize = compressSFdir(lpSFdir, &freeOffset);
  
            DBGaddsf(("addSFdirEntry(): freeSize = %d\n", freeSize));   // xxx
  
            /*  If that does not buy enough space, then allocate
            *  more memory for the directory.
            */
            if (freeSize < size)
            {
                DBGaddsf(("addSFdirEntry(): freeSize < size\n"));
                if (!(lpSFdir = expandDirBuf(lpSFdir,&freeSize,size)))
                {
                    DBGaddsf(("addSFdirEntry(): failed to expand dir buf\n"));
                    goto done;
                }
            }
  
            /*  If there are no more null entries in the directory,
            *  then allocate some more.
            */
            if ((nullInd < 0) && ((nullInd = findNullInd(lpSFdir)) < 0) &&
                !(lpSFdir = addNullEntries(lpSFdir,&nullInd)))
            {
                DBGaddsf(("addSFdirEntry(): failed to add null entries\n"));
                goto done;
            }
  
            /*  Create a "free" or deleted entry and jump up to the
            *  code for handling such an entry.
            */
            sf = &lpSFdir->sf[nullInd];
            sf->offset = freeOffset;
            sf->size = freeSize;
            freeInd = nullInd;
            *((int FAR *)(&lpSFdir->lpDir[freeOffset])) = freeInd;
            nullInd = findNullInd(lpSFdir);
            DBGaddsf(("addSFdirEntry(): transferred null ind to free ind\n"));
  
            goto copyentry;
        }
    }
  
    done:
    if (!loadedOnEntry && lpSFdir)
    {
        DBGaddsf(("addSFdirEntry(): unlockSFdir(%lp)\n", lpSFdir));
        if (unlockSFdir(lpSFdir))
            lpSFdir = 0;
  
    }
  
    DBGaddsf(("addSFdirEntry(): return(%d)\n", sfind));
  
    return (sfind);
  
} // addSFDirEntry()
  
/*  delSFdirEntry
*/
int FAR PASCAL delSFdirEntry(lpSFdir, sfind)
LPSFDIR lpSFdir;
int sfind;
{
    LPSFDIRENTRY sf;
    LPSTR lpEntry;
  
    DBGaddsf(("delSFdirEntry(%lp,%d)\n", lpSFdir, sfind));
  
    if (lpEntry=lockSFdirEntry(lpSFdir, sfind))
    {
        sf = &gLPSFdir->sf[sfind];
  
        /*  Remove any other structures pointed to by
        *  this structure.
        */
        switch (sf->state & SF_TYPE)
        {
            case SF_FILE:
            {
                LPSFDIRFILE lpSFfile = (LPSFDIRFILE)lpEntry;
  
                if (lpSFfile->indLOGdrv > -1)
                    delSFdirEntry(lpSFdir, lpSFfile->indLOGdrv);
                if (lpSFfile->indScrnFnt > -1)
                    delSFdirEntry(lpSFdir, lpSFfile->indScrnFnt);
                if (lpSFfile->indDLpath > -1)
                    delSFdirEntry(lpSFdir, lpSFfile->indDLpath);
                if (lpSFfile->indPFMpath > -1)
                    delSFdirEntry(lpSFdir, lpSFfile->indPFMpath);
                break;
            }
  
            case SF_TKST:
            {
                LPSFDIRTKSTATE lpTstate = (LPSFDIRTKSTATE)lpEntry;
                int FAR *lpIND;
                int ind;
  
                for (ind=0, lpIND=&lpTstate->indLOGdrv[0];
                    ind < lpTstate->numLOGdrv; ++ind, ++lpIND)
                {
                    delSFdirEntry(lpSFdir, *lpIND);
                }
                break;
            }
  
            case SF_SCRN:
            {
                LPSFDIRSCRFNT lpSFscrn = (LPSFDIRSCRFNT)lpEntry;
  
                if (lpSFscrn->indLOGdrv > -1)
                    delSFdirEntry(lpSFdir, lpSFscrn->indLOGdrv);
                if (lpSFscrn->indFNpath > -1)
                    delSFdirEntry(lpSFdir, lpSFscrn->indFNpath);
                break;
            }
  
            case SF_PATH:
            case SF_LDRV:
            default:
                break;
        }
  
        /*  Drop the usage count and mark for delete if usage
        *  is zero (0).
        */
        if ((sf->usage > 0) && (--sf->usage == 0))
        {
            sf->state |= SF_DEL;
        }
  
        unlockSFdirEntry(sfind);
    }
    else
        sfind = -1;
  
    return (sfind);
}
  
/*  delSFdirFile
*
*  Delete a file only if it has one owner.  As a rule, the caller should
*  delete this entry with delSFdirEntry after deleting the file.
*/
int FAR PASCAL delSFdirFile(lpSFdir, sfind, lpBuf, bufsz)
LPSFDIR lpSFdir;
int sfind;
LPSTR lpBuf;
int bufsz;
{
    LPSFDIRENTRY sf = 0L;
    LPSFDIRFILE lpSFfile = 0L;
  
    DBGdelfile(("delSFdirFile(%lp,%d,%lp,%d)\n",
    lpSFdir, sfind, lpBuf, bufsz));
  
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir, sfind))
    {
        sf = &gLPSFdir->sf[sfind];
  
        if ((sf->state & SF_FILE) &&
            !(sf->state & SF_DEL) &&
            !(sf->state & SF_FDEL) &&
            (sf->usage <= 1))
        {
            /*  Delete PFM file.
            */
            if (makeSFdirFileNm(lpSFdir, sfind, TRUE, lpBuf, bufsz))
            {
                dos_delete(lpBuf);
                DBGdelfile(("delSFdirFile(): %ls deleted\n", lpBuf));
            }
  
            /*  Delete downloadable font file.
            */
            if (makeSFdirFileNm(lpSFdir, sfind, FALSE, lpBuf, bufsz))
            {
                dos_delete(lpBuf);
                DBGdelfile(("delSFdirFile(): %ls deleted\n", lpBuf));
            }
  
            /*  Mark file struct for file delete.
            */
            sf->state |= SF_FDEL;
        }
  
        unlockSFdirEntry(sfind);
    }
    else
        sfind = -1;
  
    return(sfind);
}
  
/*  makeSFdirFileNm
*/
BOOL FAR PASCAL makeSFdirFileNm(lpSFdir, sfind, mkPFMnm, lpBuf, bufsz)
LPSFDIR lpSFdir;
int sfind;
BOOL mkPFMnm;
LPSTR lpBuf;
int bufsz;
{
    LPSFDIRFILE lpSFfile;
    LPSFDIRSTRNG lpStr;
    LPSTR lpFile;
    WORD offs;
    BOOL success = FALSE;
    int ind;
  
    *lpBuf = '\0';
  
    if ((sfind > -1) &&
        (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir, sfind)))
    {
        offs = mkPFMnm ? lpSFfile->offsPFMname : lpSFfile->offsDLname;
        ind = mkPFMnm ? lpSFfile->indPFMpath : lpSFfile->indDLpath;
  
        if (offs)
        {
            lpFile = &lpSFfile->s[offs];
  
            if (ind > -1)
            {
                /*  Merge path with file name.
                */
                if (lpStr=(LPSFDIRSTRNG)lockSFdirEntry(lpSFdir, ind))
                {
                    if (lstrlen(lpStr->s) < bufsz)
                    {
                        lstrcpy(lpBuf, lpStr->s);
  
                        if (lstrlen(lpBuf) < bufsz - lstrlen(lpFile))
                        {
                            lstrcat(lpBuf, lpFile);
                            success = TRUE;
                        }
                    }
                    unlockSFdirEntry(ind);
                }
            }
            else
            {
                /*  No path, just file name.
                */
                if (lstrlen(lpFile) < bufsz)
                {
                    lstrcpy(lpBuf, lpFile);
                    success = TRUE;
                }
            }
        }
  
        unlockSFdirEntry(sfind);
    }
  
    return (success);
}
  
/*  addSFdirFile
*
*  Add another file (either PFM or DL) to an existing file struct.
*  The file name comes in lpBuf, and we use lpBuf as a work buffer to
*  construct the new SFDIRFILE struct (the buffer should be at least
*  100 bytes for this function to work).
*/
BOOL FAR PASCAL addSFdirFile(lpSFdir, sfind, addPFMnm, lpBuf, bufsz)
LPSFDIR lpSFdir;
int sfind;
BOOL addPFMnm;
LPSTR lpBuf;
int bufsz;
{
    LPSFDIRFILE lpSFfile;
    LPSFDIRENTRY sf;
    LPSTR s, t, u;
    WORD state;
    WORD size;
    WORD usage;
    char savec;
    int ind = -1;
    int pind;
    int k;
  
    DBGaddsf(("addSFdirFile(%lp,%d,%d,%lp,%d): %ls\n",
    lpSFdir, sfind, (WORD)addPFMnm, lpBuf, bufsz, lpBuf));
  
    if (!(*lpBuf))
        return FALSE;
    if (bufsz < 100)
        return FALSE;
  
    // Since we need 2 bytes BEFORE the file name (see addSFdirEntry()
    // call below), and we don't want segment wraparound, move the
    // file name UP 2 bytes.
    k = lstrlen(lpBuf);
    lstrcpy(lpBuf + k + 5, lpBuf);  // make a copy above the filename,
    lstrcpy(lpBuf + 2, lpBuf + k + 5);  // then copy it back down.
    lpBuf[0] = lpBuf[1] = 0x20;     // insert spaces, as a precaution.
  
#ifdef DEBUG
    // so we display OK in the DBG statement..
    DBGaddsf(("addSFdirFile(): shifted filename = <%ls>\n", lpBuf));
#endif
  
    // Then move the buffer pointer UP 2 bytes
    // and decrement the buffer size so the code after this still works.
    lpBuf += 2;
    bufsz -= 2;
  
  
    /*  Step backward through the file name stopping at the end
    *  of the path.
    */
    for (s = lpBuf + lstrlen(lpBuf);
        (s > lpBuf) && (s[-1] != ':') && (s[-1] != '\\'); --s)
        ;
  
    /*  If there is a path, insert it.
    */
    if (s > lpBuf)
    {
        /*  Turn the end character into a null.
        */
        savec = *s;
        *s = '\0';
  
        /*  Insert string path name, allow two bytes before the
        *  string for use by the SF directory utilities and
        *  one byte at the end for the null-terminator.
        */
        pind = addSFdirEntry(lpSFdir, lpBuf-2, SF_PATH, lstrlen(lpBuf)+3);
  
        *s = savec;
    }
    else
        pind = -1;
  
    /*  Shift the file name to the end of the buffer.
    */
    for (k=lstrlen(s)+1, t=lpBuf+bufsz-1, u=s+k;
        u >= s; *t = *u, --t, --u)
        ;
    ++t;
  
    /*  Lock down existing SFDIRFILE struct.
    */
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir,sfind))
    {
        sf = &gLPSFdir->sf[sfind];
  
        state = sf->state;
        size = sf->size;
        usage = sf->usage;
  
        if (size < bufsz - k)
        {
            /*  Copy existing struct.
            */
            lmemcpy(lpBuf, (LPSTR)lpSFfile, size);
  
            /*  Compress the size of the struct -- it is possible
            *  for a SF dir entry to assume the place of a deleted
            *  entry which was slightly larger, in which case the
            *  entry was padded out with NULLs.  Backup until the
            *  second-to-last character is non-NULL (the last character
            *  should be NULL).
            */
            while (lpBuf[size-2] == '\0' && size > 0)
                --size;
  
            /*  Unlock struct.
            */
            unlockSFdirEntry(sfind);
  
            /*  Copy file name from end of buffer to the
            *  end of the SFDIRFILE struct.
            */
            lstrcpy(&lpBuf[size], t);
  
            lpSFfile = (LPSFDIRFILE)lpBuf;
  
            /*  Update the pointer to the path and the offset
            *  to the file name.
            */
            if (addPFMnm)
            {
                lpSFfile->indPFMpath = pind;
                lpSFfile->offsPFMname = size - sizeof(SFDIRFILE) + 1;
            }
            else
            {
                lpSFfile->indDLpath = pind;
                lpSFfile->offsDLname = size - sizeof(SFDIRFILE) + 1;
            }
  
            /*  New size.
            */
            size += k;
  
            /*  For each existing owner of the struct, add
            *  a new owner to the path (note that we already
            *  have one owner).
            */
            if (pind > -1)
            {
                for (k=1; k < usage; ++k)
                {
                    addSFdirOwner(lpSFdir, pind);
                }
            }
  
            /*  Replace old entry with new.
            */
            ind = replaceSFdirEntry(lpSFdir, sfind, lpBuf, state, size);
        }
        else
            unlockSFdirEntry(sfind);
    }
  
    return (ind > -1);
}
  
/*  chngSFdirDesc
*
*  Change the description string on SFDIRFILE struct.
*/
BOOL FAR PASCAL chngSFdirDesc(lpSFdir, sfind, lpName, lpBuf, bufsz)
LPSFDIR lpSFdir;
int sfind;
LPSTR lpName;
LPSTR lpBuf;
int bufsz;
{
    LPSFDIRFILE lpSFfile;
    LPSFDIRENTRY sf;
    WORD state;
    WORD size;
    int ind = -1;
    int k;
  
    DBGaddsf(("chngSFdirDesc(%lp,%d,%lp,%lp,%d): %ls\n",
    lpSFdir, sfind, lpName, lpBuf, bufsz, lpName));
  
    if (!(*lpName))
        return FALSE;
  
    /*  Lock down existing SFDIRFILE struct.
    */
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir,sfind))
    {
        sf = &gLPSFdir->sf[sfind];
  
        state = sf->state;
        size = sf->size;
  
        if (lstrlen(lpName) + sf->size < bufsz)
        {
            /*  Copy SFDIRFILE struct, replacing the old description
            *  with the new description.
            */
            LPSFDIRFILE lpSFtmp = (LPSFDIRFILE)lpBuf;
  
            lmemcpy((LPSTR)lpSFtmp, (LPSTR)lpSFfile, sizeof(SFDIRFILE)-1);
            lstrcpy(lpSFtmp->s, lpName);
  
            k = lstrlen(lpSFtmp->s) + 1;
  
            if (lpSFfile->offsDLname)
            {
                lstrcpy(&lpSFtmp->s[k], &lpSFfile->s[lpSFfile->offsDLname]);
                lpSFtmp->offsDLname = k;
                k += lstrlen(&lpSFtmp->s[k]) + 1;
            }
  
            if (lpSFfile->offsPFMname)
            {
                lstrcpy(&lpSFtmp->s[k], &lpSFfile->s[lpSFfile->offsPFMname]);
                lpSFtmp->offsPFMname = k;
                k += lstrlen(&lpSFtmp->s[k]) + 1;
            }
  
            /*  New size.
            */
            size = sizeof(SFDIRFILE) + k - 1;
  
            /*  Unlock struct.
            */
            unlockSFdirEntry(sfind);
  
            /*  Replace old entry with new.
            */
            ind = replaceSFdirEntry(lpSFdir, sfind, lpBuf, state, size);
        }
        else
            unlockSFdirEntry(sfind);
    }
  
    return (ind > -1);
}
  
/*  chngSFdirPath
*/
int FAR PASCAL chngSFdirPath(lpSFdir, sfind, chngPFMpth, lpPath)
LPSFDIR lpSFdir;
int sfind;
BOOL chngPFMpth;
LPSTR lpPath;
{
    LPSFDIRFILE lpSFfile;
    LPSFDIRENTRY sf;
    int owners;
    int oldind;
    int ind = -1;
    int i;
  
    DBGaddsf(("chngSFdirPath(%lp,%d,%d,%lp): %ls\n",
    lpSFdir, sfind, (WORD)chngPFMpth, lpPath, lpPath));
  
    /*  Add new path.
    */
    if ((ind=addSFdirEntry(lpSFdir,lpPath-2,SF_PATH,lstrlen(lpPath)+3)) > -1)
    {
        /*  Lock down SFDIRFILE struct containing the path.
        */
        if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir,sfind))
        {
            owners = gLPSFdir->sf[sfind].usage;
  
            /*  Swap new path for old.
            */
            if (chngPFMpth)
            {
                oldind = lpSFfile->indPFMpath;
                lpSFfile->indPFMpath = ind;
            }
            else
            {
                oldind = lpSFfile->indDLpath;
                lpSFfile->indDLpath = ind;
            }
            unlockSFdirEntry(sfind);
  
            /*  Remove the old path for every owner of this
            *  SFDIRFILE struct.
            */
            if (oldind > -1)
            {
                for (i=0; i < owners; ++i)
                {
                    delSFdirEntry(lpSFdir, oldind);
                }
            }
  
            /*  Add an owner to the path for every owner of this
            *  SFDIRFILE struct (note that there already exists
            *  one owner).
            */
            for (i=1; i < owners; ++i)
            {
                addSFdirOwner(lpSFdir, ind);
            }
        }
    }
  
    return (ind);
}
  
/*  addSFdirOwner
*/
int FAR PASCAL addSFdirOwner(lpSFdir, sfind)
LPSFDIR lpSFdir;
int sfind;
{
    LPSFDIRENTRY sf;
    LPSTR lpEntry;
  
    DBGaddsf(("addSFdirOwner(%lp,%d)\n", lpSFdir, sfind));
  
    if (lpEntry=lockSFdirEntry(lpSFdir, sfind))
    {
        lpSFdir = gLPSFdir;
  
        sf = &lpSFdir->sf[sfind];
  
        /*  Add another owner to any structures pointed to
        *  by this structure.
        */
        switch (sf->state & SF_TYPE)
        {
            case SF_FILE:
            {
                LPSFDIRFILE lpSFfile = (LPSFDIRFILE)lpEntry;
  
                if (lpSFfile->indLOGdrv > -1)
                    addSFdirOwner(lpSFdir, lpSFfile->indLOGdrv);
                if (lpSFfile->indScrnFnt > -1)
                    addSFdirOwner(lpSFdir, lpSFfile->indScrnFnt);
                if (lpSFfile->indDLpath > -1)
                    addSFdirOwner(lpSFdir, lpSFfile->indDLpath);
                if (lpSFfile->indPFMpath > -1)
                    addSFdirOwner(lpSFdir, lpSFfile->indPFMpath);
                break;
            }
  
            case SF_TKST:
            {
                LPSFDIRTKSTATE lpTstate = (LPSFDIRTKSTATE)lpEntry;
                int FAR *lpIND;
                int ind;
  
                for (ind=0, lpIND=&lpTstate->indLOGdrv[0];
                    ind < lpTstate->numLOGdrv; ++ind, ++lpIND)
                {
                    addSFdirOwner(lpSFdir, *lpIND);
                }
                break;
            }
  
            case SF_SCRN:
            {
                LPSFDIRSCRFNT lpSFscrn = (LPSFDIRSCRFNT)lpEntry;
  
                if (lpSFscrn->indLOGdrv > -1)
                    addSFdirOwner(lpSFdir, lpSFscrn->indLOGdrv);
                if (lpSFscrn->indFNpath > -1)
                    addSFdirOwner(lpSFdir, lpSFscrn->indFNpath);
                break;
            }
  
            case SF_PATH:
            case SF_LDRV:
            default:
                break;
        }
  
        /*  Add another owner.
        */
        ++sf->usage;
  
        unlockSFdirEntry(sfind);
    }
    else
        sfind = -1;
  
    return (sfind);
}
  
/*  endSFdir
*/
void FAR PASCAL endSFdir(lpSFdir)
LPSFDIR lpSFdir;
{
    HANDLE hDir;
  
    DBGaddsf(("endSFdir(%lp)\n", lpSFdir));
  
    if (!gHSFdir)
        return;
  
    if (lpSFdir || (lpSFdir = lockSFdir()))
    {
        hDir = lpSFdir->hDir;
  
#ifdef SANITY_ALERT
        examineSFdir(lpSFdir);
#endif
  
        if (unlockSFdir(lpSFdir))
            lpSFdir = 0L;
  
        //     while ((GlobalFlags(hDir) & GMEM_LOCKCOUNT) > 0)
        //         GlobalUnlock(hDir);
  
        if (hDir)
            GlobalFree(hDir);
    }
  
  
    //    while ((GlobalFlags(gHSFdir) & GMEM_LOCKCOUNT) > 0)
    //        GlobalUnlock(gHSFdir);
  
    if (gHSFdir)
    {
        GlobalFree(gHSFdir);
        gHSFdir = 0;
    }
  
}
  
/*  lockSFdirEntry
*/
LPSTR FAR PASCAL lockSFdirEntry(lpSFdir, sfind)
LPSFDIR lpSFdir;
int sfind;
{
    LPSFDIRENTRY sf;
    LPSTR lpSFentry = 0L;
  
    DBGlockentry(("lockSFdirEntry(%lp,%d)\n", lpSFdir, sfind));
  
    if (lpSFdir || (lpSFdir = gLPSFdir) || (lpSFdir = lockSFdir()))
    {
        gLPSFdir = lpSFdir;
  
        ++gCountLocks;
  
        if ((sfind > -1) && (sfind < lpSFdir->len))
        {
            sf = &lpSFdir->sf[sfind];
  
            if (!(sf->state & SF_DEL) && (sf->offset > -1))
            {
                lpSFentry = &lpSFdir->lpDir[sf->offset];
  
                DBGlockentry(("lockSFdirEntry(): locked at %lp, count=%d\n",
                lpSFentry, gCountLocks));
            }
            else
            {
                DBGlockentry((
                "lockSFdirEntry(): item marked for delete or invalid offset\n"));
            }
        }
        else
        {
            DBGlockentry(("lockSFdirEntry(): invalid sfind=%d\n", sfind));
        }
    }
    else
    {
        DBGlockentry(("lockSFdirEntry(): failed to lock SF dir\n"));
    }
  
    return (lpSFentry);
}
  
  
  
/*  unlockSFdirEntry
*/
void FAR PASCAL unlockSFdirEntry(sfind)
int sfind;
{
    DBGlockentry(("unlockSFdirEntry(%d)\n", sfind));
  
    if (gCountLocks && (--gCountLocks == 0) && gLPSFdir)
    {
        if (unlockSFdir(gLPSFdir))
            gLPSFdir = 0L;
        DBGlockentry(("unlockSFdirEntry(%d): ...unlocked\n", sfind));
    }
    else
    {
        DBGlockentry(("unlockSFdirEntry(%d): not unlocked, count=%d\n",
        sfind, gCountLocks));
    }
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
/*  lockSFdir
*/
LOCAL LPSFDIR lockSFdir()
{
    LPSFDIR lpSFdir;
    LPSFDIRENTRY sf;
    BOOL initDir = FALSE;
    int ind;
  
    DBGlocksf(("lockSFdir()\n"));
  
    if (!gHSFdir &&
        !(gHSFdir = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (DWORD)SF_INITDIRSZ)))
    {
        DBGlocksf(("lockSFdir(): failed to alloc gHSFdir\n"));
        goto backout0;
    }
  
    if (!(lpSFdir = (LPSFDIR)GlobalLock(gHSFdir)))
    {
        DBGlocksf(("lockSFdir(): failed to lock lpSFdir\n"));
        goto backout1;
    }
  
    if (!lpSFdir->hDir)
    {
        initDir = TRUE;
        if (!(lpSFdir->hDir = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
            (DWORD)SF_INITBUFSZ)))
        {
            DBGlocksf(("lockSFdir(): failed to alloc lpSFdir->hDir\n"));
            goto backout2;
        }
    }
  
    if (!(lpSFdir->lpDir = GlobalLock(lpSFdir->hDir)))
    {
        DBGlocksf(("lockSFdir(): failed to lock lpSFdir->lpDir\n"));
        goto backout3;
    }
  
    if (initDir)
    {
        /*  New soft font directory, set up the first entry as a "free"
        *  pointer to the whole directory.
        */
        DBGlocksf(("lockSFdir(): **new** soft font directory\n"));
        lpSFdir->len = SF_INITDIRLEN;
        lpSFdir->size = SF_INITBUFSZ;
  
        /*  Mark the first entry as a "free" pointer to the whole
        *  buffer.
        */
        sf = &lpSFdir->sf[0];
        sf->state = SF_DEL;
        sf->size = SF_INITBUFSZ;
        sf->offset = 0;
  
        /*  Mark the rest of the entries as "null" pointers.
        */
        for (ind=1, ++sf; ind < lpSFdir->len; ++ind, ++sf)
            sf->offset = -1;
    }
  
    return (lpSFdir);
  
    backout3:
    GlobalFree(lpSFdir->hDir);
    lpSFdir->hDir = 0;
  
    backout2:
    GlobalUnlock(gHSFdir);
    lpSFdir = 0L;
  
    backout1:
    GlobalFree(gHSFdir);
    gHSFdir = 0;
  
    backout0:
    return (0L);
}
  
/*  unlockSFdir
*/
LOCAL BOOL unlockSFdir(lpSFdir)
LPSFDIR lpSFdir;
{
    DBGlocksf(("unlockSFdir(%lp)\n", lpSFdir));
  
    if (lpSFdir != NULL)
    {
        GlobalUnlock(lpSFdir->hDir);
        lpSFdir->lpDir = 0L;
  
        GlobalUnlock(gHSFdir);
        return TRUE;
    }
    return FALSE;
}
  
  
/*  sfmemsame
*/
LOCAL BOOL sfmemsame(p, q, len)
LPSTR p;
LPSTR q;
int len;
{
    for (; len > 0  && *p == *q; --len, ++p, ++q)
        ;
  
    return (len == 0);
}
  
/*  splitFreeSpace
*/
LOCAL LPSFDIR splitFreeSpace(lpSFdir, freeInd, lpNullInd, size)
LPSFDIR lpSFdir;
int freeInd;
int FAR *lpNullInd;
WORD size;
{
    LPSFDIRENTRY lpFree;
    LPSFDIRENTRY lpNull;
    int FAR *lpInt;
    int nullInd = *lpNullInd;
  
    DBGsplitfree(("splitFreeSpace"));
    DBGsplitfree(("(%lp,%d,%lp,%d)\n",
    lpSFdir, freeInd, lpNullInd, size));
  
    *lpNullInd = -1;
  
    if ((nullInd < 0) && !(lpSFdir=addNullEntries(lpSFdir,&nullInd)))
    {
        DBGsplitfree(("splitFreeSpace(): failed to add null entries\n"));
        return (0L);
    }
  
    lpFree = &lpSFdir->sf[freeInd];
    lpNull = &lpSFdir->sf[nullInd];
  
    lpNull->state = SF_DEL;
    lpNull->usage = 0;
    lpNull->size = lpFree->size - size;
    lpNull->offset = lpFree->offset + size;
    lpFree->size = size;
  
    lpInt = (int FAR *)&lpSFdir->lpDir[lpNull->offset];
    *lpInt = nullInd;
  
    DBGsplitfree(("splitFreeSpace(): new free entry at ind=%d, size=%d\n",
    nullInd, lpNull->size));
  
    return (lpSFdir);
}
  
/*  compressSFdir
*/
LOCAL WORD compressSFdir(lpSFdir, lpFreeOffset)
LPSFDIR lpSFdir;
long FAR *lpFreeOffset;
{
    LPSFDIRENTRY sf;
    LPSTR p, q, r;
    LPSTR lpDir;
    LPSTR end;
    long freeOffset;
    WORD freeSize;
    WORD sizeleft;
    WORD size;
  
    DBGcompress(("compressSFdir(%lp,%lp)\n", lpSFdir, lpFreeOffset));
  
    for (p = lpDir = lpSFdir->lpDir,
        freeOffset = lpSFdir->size,
        end = p + freeOffset,
        freeSize=0;
        p < end; p += size)
    {
  
        for (q=p, size=0, sf=&lpSFdir->sf[*((int FAR *)p)];
            (q < end) && (sf->state & SF_DEL) && !sf->usage; /* */ )
        {
            size += sf->size;
            sf->size = 0;
            sf->state = 0;
            sf->offset = -1;
  
            q=p+size;           // point to next entry in buffer
            if (q < end)        // this MAY point past seg end!
                sf=&lpSFdir->sf[*((int FAR *)q)];   // be careful!
        }
  
        if (size)
        {
            for (r=p, sizeleft=0; q < end; ++r, ++q, --sizeleft)
            {
                if (!sizeleft)
                {
                    sf = &lpSFdir->sf[*((int FAR *)q)];
                    sizeleft = sf->size;
                    sf->offset = r - lpDir;
                }
                *r = *q;
            }
            freeSize += size;
            freeOffset -= size;
            end -= size;
            size = 0;
        }
        else
        {
            size = sf->size;
        }
    }
  
    *lpFreeOffset = freeOffset;
  
    if (freeSize > 0)
        lmemset((LPSTR)&lpSFdir->lpDir[freeOffset], 0, freeSize);
  
    DBGcompress(("...end compressSFdir(): freeSize=%d, freeOffset=%ld\n",
    freeSize, freeOffset));
  
    return (freeSize);
}
  
/*  expandDirBuf
*/
LOCAL LPSFDIR expandDirBuf(lpSFdir, lpFreeSize, size)
LPSFDIR lpSFdir;
WORD FAR *lpFreeSize;
WORD size;
{
    DWORD dsize;
    HANDLE hDir;
    HANDLE tmp;
  
    DBGexpand(("expandDirBuf(%lp,%lp,%d)\n", lpSFdir, lpFreeSize, size));
  
    if (size < *lpFreeSize)
        return (lpSFdir);
  
    if (size < SF_STEPBUFSZ)
        size = SF_STEPBUFSZ;
  
    dsize = lpSFdir->size + size - *lpFreeSize;
  
    GlobalUnlock(hDir=lpSFdir->hDir);
    lpSFdir->lpDir = 0L;
  
    GlobalUnlock(gHSFdir);
    lpSFdir = 0L;
  
    if (!(tmp = GlobalReAlloc(hDir, dsize, GMEM_MOVEABLE)))
    {
        DBGexpand(("expandDirBuf(): failed to realloc hDir\n"));
        goto backout;
    }
  
    hDir = tmp;
  
    if (!(lpSFdir = (LPSFDIR)GlobalLock(gHSFdir)))
    {
        DBGexpand(("expandDirBuf(): failed to lock lpSFdir\n"));
        goto backout;
    }
  
    if (!(lpSFdir->lpDir = GlobalLock(lpSFdir->hDir=hDir)))
    {
        DBGexpand(("expandDirBuf(): failed to lock lpDir\n"));
        goto hairy;
    }
  
    lpSFdir->size = dsize;
    *lpFreeSize = size;
  
    DBGexpand(("...expandDirBuf() freeSize=%d, totalSize=%ld\n",
    (WORD)*lpFreeSize, dsize));
  
    return (lpSFdir);
  
    hairy:
    GlobalUnlock(gHSFdir);
    lpSFdir = 0L;
  
    backout:
    GlobalFree(hDir);
    hDir = 0;
    GlobalFree(gHSFdir);
    gHSFdir = 0;
    *lpFreeSize = 0;
    return (0L);
}
  
/*  addNullEntries
*/
LOCAL LPSFDIR addNullEntries(lpSFdir, lpNullInd)
LPSFDIR lpSFdir;
int FAR *lpNullInd;
{
    LPSFDIRENTRY sf;
    LPSTR lpDir;
    HANDLE hDir;
    HANDLE tmp;
    long dsize;
    int nullInd;
    int ind;
  
    DBGaddnull(("addNullEntries(%lp,%lp)\n", lpSFdir, lpNullInd));
  
    *lpNullInd = nullInd = lpSFdir->len;
  
    GlobalUnlock(hDir=lpSFdir->hDir);
    lpSFdir->lpDir = 0L;
  
    GlobalUnlock(gHSFdir);
    lpSFdir = 0L;
  
    dsize = GlobalSize(gHSFdir) + (DWORD)SF_STEPDIRSZ;
  
    if (!(tmp = GlobalReAlloc(gHSFdir, dsize, GMEM_MOVEABLE)))
    {
        DBGaddnull(("addNullEntries(): failed to realloc gHSFdir\n"));
        goto backout;
    }
  
    if (!(lpSFdir = (LPSFDIR)GlobalLock(gHSFdir=tmp)))
    {
        DBGaddnull(("addNullEntries(): failed to lock lpSFdir\n"));
        goto backout;
    }
  
    if (!(lpSFdir->lpDir = GlobalLock(lpSFdir->hDir)))
    {
        DBGaddnull(("addNullEntries(): failed to lock lpDir\n"));
        goto hairy;
    }
  
    lpSFdir->len += SF_STEPDIRLEN;
  
    for (sf=&lpSFdir->sf[ind=nullInd]; ind < lpSFdir->len; ++ind, ++sf)
    {
        sf->state = 0;
        sf->usage = 0;
        sf->size = 0;
        sf->offset = -1;
    }
  
    return(lpSFdir);
  
    hairy:
    GlobalUnlock(gHSFdir);
    lpSFdir = 0L;
  
    backout:
    GlobalFree(hDir);
    hDir = 0;
    GlobalFree(gHSFdir);
    gHSFdir = 0;
    *lpNullInd = 0;
    return(lpSFdir);
}
  
/*  findNullInd
*/
LOCAL int findNullInd(lpSFdir)
LPSFDIR lpSFdir;
{
    LPSFDIRENTRY sf;
    int ind;
  
    DBGfindnull(("findNullInd(%lp) ", lpSFdir));
  
    for (ind=0, sf=&lpSFdir->sf[0];
        ind < lpSFdir->len; ++ind, ++sf)
    {
        if (sf->offset < 0)
        {
            DBGfindnull(("found at %d\n", ind));
            return (ind);
        }
    }
  
    DBGfindnull(("not found\n"));
  
    return (-1);
}
  
/*  replaceSFdirEntry
*
*  Replace the SF dir entry at sfind with the contents of the
*  entry pointed to by lpEntry.
*/
LOCAL int replaceSFdirEntry(lpSFdir, sfind, lpEntry, state, size)
LPSFDIR lpSFdir;
int sfind;
LPSTR lpEntry;
WORD state;
int size;
{
    LPSFDIRFILE lpSFfile;
    LPSFDIRENTRY sfold;
    LPSFDIRENTRY sfnew;
    int ind = -1;
  
    DBGaddsf(("replaceSFdirEntry(%lp,%d,%lp,%d,%d)\n",
    lpSFdir, sfind, lpEntry, state, size));
  
    /*  Does not make sense to use this function to replace
    *  a unique entry -- since there is only one owner, the
    *  caller should delete and add the entry.
    */
    if (state & SF_UNIQ)
        return (-1);
  
    /*  Add the new entry as a unique entry.
    */
    ind = addSFdirEntry(lpSFdir,lpEntry,(state | SF_UNIQ),size);
  
    /*  Lock down the new entry.
    */
    if ((ind > -1) &&
        (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(lpSFdir, ind)))
    {
        SFDIRENTRY swap;
  
        sfnew = &gLPSFdir->sf[ind];
        sfold = &gLPSFdir->sf[sfind];
  
        /*  Kill the uniq bit -- we did that just to guarantee
        *  we got a unique new entry.
        */
        sfnew->state &= ~(SF_UNIQ);
  
        /*  Give the new struct the same usage count
        *  as the old struct.
        */
        sfnew->usage = sfold->usage;
  
        /*  Switch the contents of the dir entries -- now the
        *  old SF dir entry points to the new struct.
        */
        swap = *sfold;
        *sfold = *sfnew;
        *sfnew = swap;
  
        /*  Update the index values stored in the
        *  directory buffer.
        */
        *((int FAR *)(&gLPSFdir->lpDir[sfold->offset])) = sfind;
        *((int FAR *)(&gLPSFdir->lpDir[sfnew->offset])) = ind;
  
        /*  Kill the new entry (which is actually the old
        *  entry after the swap).
        */
        sfnew->usage = 0;
        sfnew->state |= SF_DEL;
  
        DBGdumpSFbuf(lpSFdir);
  
        unlockSFdirEntry(ind);
    }
  
    return (ind);
}
  
/**************************************************************************/
/*****************************   Debug Utils   ****************************/
  
  
#ifdef DEBUG
void FAR PASCAL dumpSFbuf(lpSFdir)
LPSFDIR lpSFdir;
{
    LPSFDIRENTRY sf;
    LPSFDIRFILE lpSFfile;
    LPSFDIRSCRFNT lpSFscrn;
    LPSFDIRLOGDRV lpSFdrv;
    LPSFDIRSTRNG lpDirStrng;
    LPSFDIRTKSTATE lpTstate;
    LPSTR p, s, q, end;
    BOOL lockedOnEntry = FALSE;
    BOOL newline;
    int i, ind, offset;
  
    DBMSG(("dumpSFbuf(): ******** dump sf buffer ********\n"));
  
    if (lpSFdir)
        lockedOnEntry = TRUE;
    else if (!(lpSFdir = lockSFdir()))
    {
        DBMSG(("dumpSFbuf(): failed to lock SF directory\n"));
        return;
    }
  
    for (p=&lpSFdir->lpDir[0], end=&lpSFdir->lpDir[lpSFdir->size], offset=0;
        p < end; p += sf->size, offset += sf->size)
    {
        ind = *((int FAR *)p);
        sf = &lpSFdir->sf[ind];
        DBMSG(("%c%c%c%c%c%c%c%d->%d: %c%c%c%d bytes, %d owner(s)",
        ((ind / 1000 > 0) ? '\0' : ' '),
        ((ind / 100 > 0) ? '\0' : ' '),
        ((ind / 10 > 0) ? '\0' : ' '),
        ((offset / 10000 > 0) ? '\0' : ' '),
        ((offset / 1000 > 0) ? '\0' : ' '),
        ((offset / 100 > 0) ? '\0' : ' '),
        ((offset / 10 > 0) ? '\0' : ' '),
        ind, offset,
        ((sf->size / 1000 > 0) ? '\0' : ' '),
        ((sf->size / 100 > 0) ? '\0' : ' '),
        ((sf->size / 10 > 0) ? '\0' : ' '),
        sf->size, sf->usage));
        dumpSFstate(sf->state);
        DBMSG(("\n"));
  
        if (!(sf->state & SF_DEL))
        switch (sf->state & SF_TYPE)
        {
            case SF_FILE:
                lpSFfile = (LPSFDIRFILE)&lpSFdir->lpDir[sf->offset];
                DBMSG(("             orient=%d, ind LOGdrv=%d, Scrn=%d\n",
                lpSFfile->orient, lpSFfile->indLOGdrv,
                lpSFfile->indScrnFnt));
                DBMSG((
                "             ind DLpth=%d, PFMpth=%d, offs DLnm=%d, PFMnm=%d\n",
                lpSFfile->indDLpath, lpSFfile->indPFMpath,
                lpSFfile->offsDLname, lpSFfile->offsPFMname));
                s = lpSFfile->s;
                q = (LPSTR)lpSFfile + sf->size;
                goto dumpstrng;
  
            case SF_SCRN:
                lpSFscrn = (LPSFDIRSCRFNT)&lpSFdir->lpDir[sf->offset];
                DBMSG((
                "             unused1=%d, ind LOGdrv=%d, width=%d, height=%d\n",
                lpSFscrn->unused1, lpSFscrn->indLOGdrv,
                lpSFscrn->width, lpSFscrn->height));
                DBMSG((
                "             ind FNpth=%d, scrnType=%d, offsNM=%d\n",
                lpSFscrn->indFNpath, lpSFscrn->scrnType, lpSFscrn->offsFN));
                s = lpSFscrn->s;
                q = (LPSTR)lpSFscrn + sf->size;
                goto dumpstrng;
  
            case SF_LDRV:
                lpSFdrv = (LPSFDIRLOGDRV)&lpSFdir->lpDir[sf->offset];
                DBMSG(("             priority=%d, offs Label=%d, Desc=%d\n",
                lpSFdrv->priority, lpSFdrv->offsLabel, lpSFdrv->offsDesc));
                s = lpSFdrv->s;
                q = (LPSTR)lpSFdrv + sf->size;
                goto dumpstrng;
  
            case SF_TKST:
                lpTstate = (LPSFDIRTKSTATE)&lpSFdir->lpDir[sf->offset];
                DBMSG(("             prevind=%d, hFile=%d, ch=%d%c, state=",
                lpTstate->prevind, lpTstate->hFile, (int)lpTstate->ch,
                ((lpTstate->ch < ' ') ? ' ' : lpTstate->ch)));
                DBGdumpTstate(lpTstate->state);
                DBMSG(("\n"));
                DBMSG((
                "             hMd=%d, hSFlb=%d, hDB=%d, idLB=%d, reportErr=%ls\n",
                lpTstate->hMd, lpTstate->hSFlb, lpTstate->hDB, lpTstate->idLB,
                (lpTstate->reportErr ? (LPSTR)"TRUE" : (LPSTR)"FALSE")));
                DBMSG(("             sline=%ls\n", (LPSTR)lpTstate->sline));
            {
                int j;
                for (j=0; j < lpTstate->numLOGdrv; ++j)
                {
                    DBMSG(("             indLOGdrv[%d]=%d\n",
                    j, lpTstate->indLOGdrv[j]));
                }
            }
                break;
  
            case SF_PATH:
                lpDirStrng = (LPSFDIRSTRNG)&lpSFdir->lpDir[sf->offset];
                s = lpDirStrng->s;
                q = (LPSTR)lpDirStrng + sf->size;
                dumpstrng:
                for (i=0, newline=TRUE; s < q; ++s, ++i)
                {
                    if (newline)
                    {
                        DBMSG(("             %c%c%d: ",
                        ((i / 100 > 0) ? '\0' : ' '),
                        ((i / 10 > 0) ? '\0' : ' '), i));
                        newline = FALSE;
                    }
  
                    if (*s)
                    {
                        DBMSG(("%c", (char)*s));
                    }
                    else if (s[-1] == '\0' && (s+1) < q)
                    {
                        /* String of NULLs
                        */
                        if (i % 2 == 0)
                            DBMSG(("*"));
                        else
                            DBMSG(("-"));
  
                        if (s[1] != '\0')
                        {
                            DBMSG(("\n"));
                            newline = TRUE;
                        }
                    }
                    else
                    {
                        DBMSG(("\n"));
                        newline = TRUE;
                    }
                }
                break;
        }
  
        if (sf->size <= 0)
            break;
    }
  
    for (++ind, sf=&lpSFdir->sf[ind]; ind < lpSFdir->len; ++ind, ++sf)
    {
        if ((offset = (int)sf->offset) > -1)
        {
            DBMSG(("%c%c%c%c%c%c%c%d->%d: %c%c%c%d bytes, %d owner(s)",
            ((ind / 1000 > 0) ? '\0' : ' '),
            ((ind / 100 > 0) ? '\0' : ' '),
            ((ind / 10 > 0) ? '\0' : ' '),
            ((offset / 1000 > 0) ? '\0' : ' '),
            ((offset / 100 > 0) ? '\0' : ' '),
            ((offset / 10 > 0) ? '\0' : ' '),
            ((offset < 0) ? '\0' : ' '),
            ind, offset,
            ((sf->size / 1000 > 0) ? '\0' : ' '),
            ((sf->size / 100 > 0) ? '\0' : ' '),
            ((sf->size / 10 > 0) ? '\0' : ' '),
            sf->size, sf->usage));
            dumpSFstate(sf->state);
            DBMSG(("\n"));
        }
    }
  
    DBMSG(("*** end of dump (%d entries) ***\n", lpSFdir->len));
  
    if (!lockedOnEntry)
        if (unlockSFdir(lpSFdir))
            lpSFdir = 0;
}
  
  
  
LOCAL void dumpSFstate(state)
WORD state;
{
    if (state & SF_DEL)
        DBMSG((" SF_DEL"));
    if (state & SF_UNIQ)
        DBMSG((" SF_UNIQ"));
    if (state & SF_FILE)
        DBMSG((" SF_FILE"));
    if (state & SF_PATH)
        DBMSG((" SF_PATH"));
    if (state & SF_LDRV)
        DBMSG((" SF_LDRV"));
    if (state & SF_SCRN)
        DBMSG((" SF_SCRN"));
    if (state & SF_TKST)
        DBMSG((" SF_TKST"));
    if (state & SF_FDEL)
        DBMSG((" SF_FDEL"));
    if (state & SF_MOVEABLE)
        DBMSG((" SF_MOVEABLE"));
    if (state & SF_LOADED)
        DBMSG((" SF_LOADED"));
}
  
LOCAL void dumpTstate(state)
TOKENSTATE state;
{
    switch(state)
    {
        case tk_null:
            DBMSG(("tk_null"));
            break;
        case tk_fatalerr:
            DBMSG(("tk_fatalerr"));
            break;
        case tk_fontdef:
            DBMSG(("tk_fontdef"));
            break;
        case tk_package:
            DBMSG(("tk_package"));
            break;
        case tk_family:
            DBMSG(("tk_family"));
            break;
        case tk_logdrive:
            DBMSG(("tk_logdrive"));
            break;
        case tk_port:
            DBMSG(("tk_port"));
            break;
        case tk_land:
            DBMSG(("tk_land"));
            break;
    }
}
#endif
