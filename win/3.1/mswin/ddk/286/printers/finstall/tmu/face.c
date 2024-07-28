/* face.c - 
 *
 * Copyright (C) 1987,1988,1989,1990 Compugraphic Corporation.
 * Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
 *     All rights reserved.
 *     Company confidential.
 *  3 Feb 92 Changed $ include files to _ include files
 *
 **f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *
 *    Module:   face.c
 *
 *    This module updates if.fnt, the Intellifont Bullet typeface
 *    directory and if.dsc, a list of human readable typeface names.
 *
 *    functions:
 *         FACElist()
 *         FACEadd()
 *         FACEdelete()
 *         FACEinstall()
 *      Calls:
 *          IXdata()     in ix.c
 *          UTfileSize() in ut.c
 *  
 *    10-04-91  RK(HP) In FACEadd chagned IX and ftName to be static because
 *              of stack overflow. These took over 2K.
 *  
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//#define DEBUG 

#include    <windows.h>
#ifdef NODEF
#include    <stdio.h>
#include    <stdlib.h>
#include    <fcntl.h>
#include    <io.h>
#include    <malloc.h>
#include    <string.h>
#endif
#include    <sys/types.h>
#include    <sys/stat.h>
#include    "debug.h"
#include    "_port.h"
#include    "_tmu.h"
#include    "_sfpfm2.h"
#include    "_ut.h"

#define MIN_BUFFER_SIZE 2

EXTERN UWORD IXdata();
EXTERN ULONG UTfileSize();


/****************************************************************************\
 *				Debug Definitions
\****************************************************************************/


#ifdef DEBUG
   #define	DBGfacelist(msg)    /*DBMSG(msg)*/
   #define	DBGfaceadd(msg)     /*DBMSG(msg)*/
   #define	DBGfacedel(msg)     /*DBMSG(msg)*/
   #define	DBGfaceinst(msg)    /*DBMSG(msg)*/
   #define	DBGmisc(msg)        /*DBMSG(msg)*/
#else
   #define	DBGfacelist(msg)    /*null*/
   #define	DBGfaceadd(msg)     /*null*/
   #define	DBGfacedel(msg)     /*null*/
   #define	DBGfaceinst(msg)    /*null*/
   #define	DBGmisc(msg)        /*null*/
#endif




/*--------------------------------*/
/*       font index entry         */
/*--------------------------------*/

typedef struct
{
    LONG    tfnum;        /* typeface number                          */
    UWORD   name_off;     /* offset to full path name of library file */
    ULONG   fhoff;        /* offset in library file for face header   */
    UWORD   fhcount;      /* BYTE count of face header                */
    WORD    bucket_num;   /* Associated limited sensitive face:
                           *     0 = 5720    Serif         Normal
                           *     1 = 5721    Serif         Bold
                           *     2 = 5723    Sans-serif    Normal
                           *     3 = 5724    Sans-serif    Bold
                           *
                           *   Bit definitions:
                           *      bit         0           1
                           *       0         serif       sans-serif
                           *       1         normal      bold
                           *       2         non-italic  italic
                           *       3-15      reserved
                           */
} INDEX_ENTRY;
typedef INDEX_ENTRY FAR * LPINDEX_ENTRY;

#define IE_SIZE  sizeof(INDEX_ENTRY)


char IF_DSC_name[] = "if.dsc";
char IF_FNT_name[] = "if.fnt";
char HQ3_DSC_name[] = "hq3.dsc";
char HQ3_FNT_name[] = "hq3.fnt";
LPSTR index_name;
LPSTR list_name;

#define FPERLIB  32   /* set higher to install multi face libraies */
MLOCAL WORD update;            /* 0 = normal font index file
                                * 1 = all typeface numbers will be negated
                                * in the font index file.
                                */


/*---------------------*/
/*      load_file      */
/*---------------------*/
/*  Load the entire file "fname" into memory. Malloc() the buffer memory
 *  for this file and make the buffer "extra_space" bigger. This is used
 *  to load the font index file "if.fnt" and the installed typeface
 *  list file "if.dsc". The callers of this function intend to add or
 *  delete an entry and then write the file back out. If no file exists,
 *  this function does not fail.
 *
 *
 *  Return:
 *          *ppbuf        pointer to buffer containing file
 *          *pbuf_size    size of buffer 
 *
 *  Errors:
 *
 */
MLOCAL UWORD
load_file(bulletPath, fname, extra_space, ppbuf, pbuf_size, lphBuf)
    LPSTR bulletPath;
    LPSTR fname;
    UWORD extra_space;
    LPSTR FAR *ppbuf;
    UWORD FAR *pbuf_size; HANDLE FAR *lphBuf;
{
    UWORD  status = SUCCESS;
    int   f;
    UWORD  file_size;
    struct stat st;
    char pathname[MAXNAME];

    wsprintf ((LPSTR)pathname, "%s\\%s", bulletPath, fname);

  /* Open file and get file size */

    if ((f = _lopen((LPSTR)pathname, 0 /*READ*/)) == -1)
        file_size = 0;
    else
    {
        if(myfstat(f, (LPSTAT)&st))
        {
            DBGmisc(("myfstat() failed\n"));
            _lclose(f);
            return ERRface_fstat;
        }
        file_size = (UWORD)st.st_size;
    }
    DBGmisc(("file_size = %d\n", file_size));

  /* Get buffer memory */

    *pbuf_size = file_size + extra_space;
    if((*lphBuf = GlobalAlloc(GMEM_MOVEABLE, (LONG) (*pbuf_size + MIN_BUFFER_SIZE))) == NULL)
    {
        DBGmisc(("    GlobalAlloc(%u) failed\n", *pbuf_size));
        if(f != -1) _lclose(f);
        return ERRface_malloc;
    }

    if(!(*ppbuf = GlobalLock(*lphBuf)))
    {	GlobalFree(*lphBuf);
        DBGmisc(("    malloc(%u) failed\n", *pbuf_size));
        if(f != -1) _lclose(f);
        return ERRface_malloc;
    }
    lmemset(*(LPSTR FAR *)ppbuf, 0, *pbuf_size);

  /* Read file into buffer */

    if(f == -1)
        return SUCCESS;
    else
    {
        if(file_size != _lread(f, *ppbuf, file_size))
            status = ERRface_read;

        _lclose(f);
        return status;
    }
}




/*---------------------*/
/*     new_file        */
/*---------------------*/
MLOCAL UWORD
new_file(bulletPath, fname, buffer, file_size, hBuf)
    LPSTR bulletPath;
    LPSTR fname;
    LPSTR buffer;
    WORD file_size;
    HANDLE hBuf;
{
    int f;
    char pathname[MAXNAME];

    wsprintf ((LPSTR)pathname, "%s\\%s", bulletPath, fname);

    unlink((LPSTR)pathname);
    if ((f = _lcreat((LPSTR)pathname, 0 /*Normal read write */)) == -1)
    {
        DBGmisc(("Cannot open %s\n", fname));
        MyFreeMem((HANDLE FAR *)&hBuf);
        return ERRnew_file;
    }

    if(file_size != _lwrite(f, buffer, file_size))
    {
       DBGmisc(("write() failed\n"));
        MyFreeMem((HANDLE FAR *)&hBuf);
        return ERRface_write;
    }

    _lclose(f);
    MyFreeMem((HANDLE FAR *)&hBuf);
    return SUCCESS;
}





/*---------------------*/
/*    FACEadd          */
/*---------------------*/
WORD FAR PASCAL
FACEadd(typeInfo, bulletPath)
    LPTYPEINFO typeInfo;
    LPSTR      bulletPath;
{
    UWORD       status;

    UWORD extra_space;
    static INDEX_ENTRY ix[FPERLIB];            /* new entry */                           // RK 10/04/91
    static TFNAME tfName[FPERLIB];                                                       // RK 10/04/91
    UWORD  numFound;
    UWORD entry_ct;            /* number of entries in font index */
    LPINDEX_ENTRY index_entry; /* start of entries */
    LPSTR libnames;            /* start of library names */
    UWORD sizeentry;           /* size of INDEX_ENTRYs in bytes */
    UWORD sizename;            /* size of library names in bytes */
    UWORD lib_name_len;        /* length of library name */
    LPINDEX_ENTRY x;
    LPSTR n;                   /* library name to delete */
    UWORD size;
    LPSTR buf;
    UWORD count, i, j;
    LPTYPEINFO p;
    int  compare;
    HANDLE hBuf;
    int temp;


    DBGfaceadd(("Inside of Face add\n"));

/*----------------------- update if.fnt------------------------- */

  /* make a new font INDEX_ENTRY */

    temp = sizeof(TFNAME);

    DBGfaceadd(("Inside of Face add, typname = %ls\n", (LPSTR)typeInfo->nameOrDir));

    if(status = IXdata((LPSTR)typeInfo->nameOrDir, (LPINDEX_ENTRY)ix,
                                      (LPTFNAME)tfName, (UWORD)FPERLIB, (LPUWORD)&numFound, (WORD) update))
    {
        return status;
    }

    DBGfaceadd(("Inside of Face add: AFTER CALL TO IXdata\n"));

  /* Read current font index in buffer big enough for new entry. The
   * 2 in the extra_space parameter is in case if.fnt does not exist.
   * it's space for entry_ct.
   */

    lib_name_len = lstrlen(typeInfo->nameOrDir)+1;
    extra_space = 2 + IE_SIZE + lib_name_len;
    if(status = load_file(bulletPath, (LPSTR) index_name, extra_space,
	(LPSTR FAR *)&buf, (UWORD FAR *)&size, (HANDLE FAR *)&hBuf))
        return status;

    if(size > extra_space)
        size -= 2;

    DBGfaceadd(("Inside of Face add 2\n"));

  /* Set up access pointers. Note that if if.fnt did not exist, then buf
   * points to a block big enough to add the new entry, it has been cleared
   * to 0 so we'll set entry count to 0 and all else should work out.
   */

    entry_ct = *(LPUWORD)buf;
    index_entry = (LPINDEX_ENTRY)(buf+2);
    libnames  = buf + 2 + (entry_ct * IE_SIZE);

    sizeentry = entry_ct * IE_SIZE;
    sizename = size - 2 - sizeentry - IE_SIZE - lib_name_len;

    DBGfaceadd(("Inside of Face add 3\n"));

  /* Don't add entry if it's already there */

    for(j = 0; j < numFound; ++j)
        if (typeInfo->id == ix[j].tfnum)
		break;
    DBGfaceadd(("Inside of Face add 4\n"));


    if(j == numFound)
	{
        MyFreeMem((HANDLE FAR *)&hBuf);
	return ERRface_bad_info;
	}

    for(i=0, x=index_entry; i<entry_ct; i++, x++)
        if ((x->tfnum == typeInfo->id)) break;

    DBGfaceadd(("Inside of Face add 5\n"));

    if(i<entry_ct)
    {
        DBGfaceadd(("Inside of Face add 6\n"));
        MyFreeMem((HANDLE FAR *)&hBuf);   /*Shouldnt we return at this point and not change if.dsc? */
        return ERRface_entry_exists;
    }
    else
    {
    DBGfaceadd(("Inside of Face add 7\n"));

      /* add new entry */
        lmemcpy(libnames+IE_SIZE, libnames, sizename);  /* move lib names */
        libnames += IE_SIZE;                            /* to make room   */
        lmemcpy((LPSTR)x, (LPSTR)&ix[j], IE_SIZE); /* insert new index entry */
        x->name_off = sizename;                 /* fill in offset to name */
        lmemcpy((LPSTR)libnames+sizename,
		 typeInfo->nameOrDir, lib_name_len);       /* insert name */
        (*((WORD FAR *)buf))++;                          /* bump entry_ct */
                                                        /* Write new file */
        if(status = new_file(bulletPath, (LPSTR) index_name, buf, size, hBuf))
            return status;
    }

/*----------------------- update if.dsc ------------------------- */

    if(status = load_file(bulletPath, (LPSTR) list_name,
                sizeof(TYPEINFO),
		(LPSTR FAR *)&buf, (UWORD FAR *)&size, (HANDLE FAR *)&hBuf))
    {
        return status;
    }

  /* number of current entries */

    count = size / sizeof(TYPEINFO) - 1;

  /* find location to insert new entry */

    p = (LPTYPEINFO)buf;
    for(i=0; i<count; i++)
    {
        compare = lstrncmpi(typeInfo->typefaceName, p->typefaceName, 50);
        if(compare == 0)
	    {
            MyFreeMem((HANDLE FAR *)&hBuf);
            return SUCCESS;  /* already there */
	    }
        if(compare < 0)
            break;
        p++;
    }

  /* slide tail down */

    lmemcpy((LPSTR)((LPTYPEINFO)p+1), (LPSTR)p, (count-i) * sizeof(TYPEINFO));

  /* copy in new entry */

    lmemcpy((LPSTR)p, (LPSTR)typeInfo, sizeof(TYPEINFO));

    return new_file(bulletPath, (LPSTR)list_name, buf, size, hBuf);
}




/*---------------------*/
/*   FACEDelete    */
/*---------------------*/
WORD FAR PASCAL
FACEDelete(typeInfo, bulletPath, removeFile)
    LPTYPEINFO typeInfo;
    LPSTR      bulletPath;
    BOOL       removeFile;
{
    UWORD status;

    UWORD entry_ct;            /* number of entries in font index */
    LPINDEX_ENTRY index_entry; /* start of entries */
    LPSTR libnames;            /* start of library names */
    UWORD sizeentry;           /* size of INDEX_ENTRYs in bytes */
    UWORD sizename;            /* size of library names in bytes */
    UWORD lib_name_len;        /* length of library name */
    LPINDEX_ENTRY x;           /* entry to delete */
    LPSTR n;                   /* library name to delete */
    UWORD noff;                /* offset of deleted name */

    LPTYPEINFO p;
    UWORD size;
    LPSTR buf;
    UWORD count, i;
    HANDLE hBuf;

    DBGfacedel(("ENTER: FACEdelete in face.c\n"));

/*------------------------- update if.fnt -----------------------*/
    if(status = load_file(bulletPath, (LPSTR)index_name, (UWORD)0,
	(LPSTR FAR *)&buf, (UWORD FAR *)&size, (HANDLE FAR *)&hBuf))
        return status;
    if(size)        /* if there is a file if.fnt ... */
    {
      /* Set up access pointers */

        entry_ct = *(LPUWORD)buf;
        index_entry = (LPINDEX_ENTRY)(buf+2);
        libnames  = buf + 2 + (entry_ct * IE_SIZE);

        sizeentry = entry_ct * IE_SIZE;
        sizename = size - 2 - sizeentry;

      /* Find entry to delete */

        x = index_entry;
        for(i=0; i<entry_ct; i++)
        {
            if(x->tfnum == typeInfo->id) break;
            x++;
        }
        if(i == entry_ct)    /* if we can't find the entry ... */
            MyFreeMem((HANDLE FAR *)&hBuf);
        else
        {
            noff = x->name_off;
            n = libnames + noff;

          /* Delete the library file if wanted */

            if (removeFile == TRUE)
                unlink(n);

          /* Remove the library name */

            lib_name_len = lstrlen(n) + 1;
            lmemcpy(n, n+lib_name_len, sizename - noff - lib_name_len);

          /* Remove the INDEX_ENTRY */

            lmemcpy((LPSTR)x, ((LPSTR)x)+IE_SIZE,
                        (entry_ct-i-1)*IE_SIZE + sizename-lib_name_len);
            entry_ct--;
            *(LPWORD)buf = entry_ct;

          /* Update the name offsets in the INDEX_ENTRYs */

            for(i=0, x=index_entry; i<entry_ct; i++, x++)
                if(x->name_off > noff)
                    x->name_off -= lib_name_len;

          /* Write new version of file */

            size = size - IE_SIZE - lib_name_len;
            if(size == 2)
                size = 0;
            if(status = new_file(bulletPath, (LPSTR) index_name, buf, size, hBuf))
                return status;
        }
    }
    else
	MyFreeMem((HANDLE FAR *)&hBuf);

/*------------------ update if.dsc------------------------- */

    if(status = load_file(bulletPath, (LPSTR) list_name, (UWORD)0,
	(LPSTR FAR *)&buf, (LPUWORD)&size, (HANDLE FAR *)&hBuf))
        return status;

  /* find entry to delete */
    p = (LPTYPEINFO)buf;
    count = size / sizeof(TYPEINFO);
    for(i=0; i<count; i++)
    {
        if( (p->id == typeInfo->id)
             && (p->complement == typeInfo->complement))
            break;

        p++;
    }
    if(i == count)
        return SUCCESS;    /* entry not there */

  /* delete by sliding the tail up */

    lmemcpy((LPSTR)p,(LPSTR)((LPTYPEINFO)p+1),(count-i-1)*sizeof(TYPEINFO));

  /* Write out new version of file */

    return new_file(bulletPath, (LPSTR) list_name, buf, size - sizeof(TYPEINFO), hBuf);
}



/*---------------------*/
/*    FACElist         */
/*---------------------*/
WORD FAR PASCAL
FACElist(pcount, typeInfo, bulletPath)
    LPUWORD    pcount;
    LPTYPEINFO typeInfo;
    LPSTR      bulletPath;
    {
    UWORD  status = SUCCESS;
    int   f;
    struct stat st;
    char pathname[MAXNAME];

    DBGfacelist(("inside FACElist in face.c\n"));

    index_name = (LPSTR) IF_FNT_name;
    list_name =  (LPSTR) IF_DSC_name;

    wsprintf ((LPSTR)pathname, "%s\\%s", bulletPath, (LPSTR)list_name);

    if(!*pcount)                      /* if 0, return file size of if.dsc */
    {
        if ((f = _lopen((LPSTR)pathname, 0 /*READ*/)) == -1)
        {
            *pcount = 0;
            return SUCCESS;
        }
        else
        {
            if(myfstat(f, (LPSTAT)&st))
                status = ERRface_fstat;
            else
                *pcount = (UWORD)st.st_size/sizeof(TYPEINFO);
        }
    }
    else                           /* load list file into caller's buffer */
    {
        if(!*pcount) return SUCCESS;

        if ((f = _lopen((LPSTR)pathname, 0 /*READ*/)) == -1)
            return ERRno_list_file;

        if(myfstat(f, (LPSTAT)&st))
            status = ERRface_fstat;
        else
            if((UWORD)st.st_size !=
                               _lread(f, (LPSTR)typeInfo, (UWORD)st.st_size))
                status = ERRface_read;
    }
    _lclose(f);
    return status;
}




/*---------------------*/
/*    FACEinstall      */
/*---------------------*/
WORD FAR PASCAL
FACEinstall(typeInfo, typePath, bulletPath, UpdateType, MakePfm, gSymSet, pfm_name)
    LPTYPEINFO typeInfo;
    LPSTR      typePath;
    LPSTR      bulletPath;
    WORD       UpdateType;
    WORD       MakePfm;
    LPSTR      gSymSet;
    LPSTR      pfm_name;
{
    UWORD    status;
    TYPEINFO ti;
    WORD     f;
    struct stat st;
    WORD    len;
    LPSTR   lpF;


    DBGfaceinst(("inside FACEinstall in face.c: update type = %d\n", UpdateType));

    if (UpdateType == UPDATE_IF_FNT)
	{
	index_name = (LPSTR) IF_FNT_name;
	list_name =  (LPSTR) IF_DSC_name;
	}
    else if (UpdateType == UPDATE_HQ3_FNT)
	{
	index_name = (LPSTR) HQ3_FNT_name;
	list_name =  (LPSTR) HQ3_DSC_name;
	}
    else
	return ERRbad_update_type;

    update = UpdateType;

    if(typeInfo->complement)
    {

        if(status = FAISload (typeInfo, typePath, (LPSTR)ti.nameOrDir))
            return status;
      DBGfaceinst(("After call to face load: status = %d\n", status));

      /*  Build library file version of typeInfo (so we don't write on
       *  the caller's data). The library file name has already been
       *  filled in by FAISload().
       */

        ti.id = typeInfo->id;
        ti.complement = 0;
        ti.space_req  = UTfileSize((LPSTR)ti.nameOrDir);
        lstrcpy(ti.typefaceName, typeInfo->typefaceName);

        status = FACEadd((LPTYPEINFO)&ti, bulletPath);

        if (status)
               return(status);

//        if ((status) && (status != ERRface_entry_exists))
//               return status;

    }
    else
    {

        /* need a typeinfo struct with the right destination pathname */
        ti = *typeInfo;
        lstrncpy(ti.nameOrDir, typePath, sizeof(ti.nameOrDir));
        len = lstrlen(ti.nameOrDir);   /* assumes pathname has been resolved */
        if ((ti.nameOrDir[len-1] != '\\') && /* if not root, append bslash */
            (ti.nameOrDir[len-1] != '/'))
                lstrcat(ti.nameOrDir, "\\");

        /* get just the file name part of the path */
        lpF = typeInfo->nameOrDir + lstrlen(typeInfo->nameOrDir) - 1;
        while ((lpF > typeInfo->nameOrDir) && (*(lpF-1) != '\\') &&
               (*(lpF-1) != '/') && (*(lpF-1) != ':'))
	    {
    	    --lpF;
	    }
        if ((lstrlen(ti.nameOrDir) + lstrlen(lpF) + 1) > sizeof(ti.nameOrDir))
            return (-1);            /* name is too long !! */
        lstrcat(ti.nameOrDir, lpF);

        ti.space_req  = UTfileSize((LPSTR)ti.nameOrDir);
        status = FACEadd((LPTYPEINFO)&ti, bulletPath);

        if (status)
               return(status);

    }
    return (0);             /* good status */
}


