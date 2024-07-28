/**[f******************************************************************
 * lib.c - 
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
 *    Module:   lib.c
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
#include    <dos.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    "debug.h"
#include    "_port.h"
#include    "_tmu.h"
#include    "_dosutil.h"
#include    "_ut.h"


/****************************************************************************\
 *				Debug Definitions
\****************************************************************************/


#ifdef DEBUG
   #define	DBGchklib(msg)      /*DBMSG(msg)*/
   #define	DBGliblist(msg)     /*DBMSG(msg)*/
#else
   #define	DBGchklib(msg)      /*null*/
   #define	DBGliblist(msg)     /*null*/
#endif


/****************************************************************************\
 *			   
\****************************************************************************/

#define ERRlib_dos_find     400
#define ERRlib_no_lib_list  401
#define ERRlib_fstat        402
#define ERRlib_buffer_size  403
#define ERRlib_read         404
#define ERRlib_new_file     405
#define ERRlib_write        406


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

#define FPERLIB  1   /* set higher to install multi face libraies */

MLOCAL BYTE   infoFile[MAXNAME];
MLOCAL BYTE   pathName[MAXNAME];
MLOCAL TFNAME tfName[FPERLIB];

MLOCAL WORD update;            /* 0 = normal font index file
                                * 1 = all typeface numbers will be negated
                                * in the font index file.
                                */



#define MAX_FONT_FILES 32


EXTERN ULONG UTfileSize();
EXTERN UWORD IXdata();




/****************************************************************************\
 *			   
\****************************************************************************/


/*------------------*/
/*    check_lib     */
/*------------------*/
MLOCAL UWORD
check_lib(f, libDir, fileName, pnumFound)
    WORD FAR *f;
    LPSTR libDir;
    LPSTR fileName;
    WORD FAR *pnumFound;
{
#define TISIZE sizeof(TYPEINFO)

    INDEX_ENTRY  ix[FPERLIB];
    LPINDEX_ENTRY pi;
    LPTFNAME      pn;
    UWORD numFound, i;
    TYPEINFO typeInfo;

    wsprintf ((LPSTR) pathName, "%s\\%s", libDir, fileName);

    DBGchklib(("typ filename in checklib=    %s\n", pathName));

    *pnumFound = 0;
    if(!IXdata((LPSTR) pathName, (LPINDEX_ENTRY)ix, (LPTFNAME)tfName, (UWORD)FPERLIB, (LPUWORD)&numFound, (WORD) update))
    {
      /*  No errors from IXdata() so we must have a valid library file.
       *  If we get an error return from IXdata() we ignore the file
       *  (even though it may be valid with a hardware error) and report
       *  none found. IXdata() returns an error from this call if
       *  there is more than FPERLIB faces in the library. We assume such
       *  libraries for system use such as plugins etc.
       */
        for(i=0, pi=ix, pn=tfName; i<numFound; i++, pi++, pn++)
        {
            lmemset((LPSTR)&typeInfo, 0, sizeof(typeInfo));

          /*  Build TYPEINFO structure  */
            typeInfo.id         = pi->tfnum;
            typeInfo.complement = 0;
            typeInfo.space_req  = UTfileSize((LPSTR) pathName);
            lstrcpy(typeInfo.typefaceName, *pn);
            lstrcpy(typeInfo.nameOrDir, (LPSTR) pathName);

	     if (*f == -1)
		{
		/*  Open a new TYPEINFO file */

		unlink((LPSTR)infoFile);
		DBGchklib(("After call to unlink in LIBlist\n"));

		if ((*f = _lcreat((LPSTR)infoFile, 0 /*normal*/)) == -1)
		    {
		    DBGliblist(("Cannot open %s\n", infoFile));
		    return ERRlib_new_file;
		    }
		}

          /*  Write TYPEINFO to file  */
            if(TISIZE != _lwrite(*f, (LPSTR)(&typeInfo), TISIZE))
            {
                DBGchklib(("write() failed\n"));
                return ERRlib_write;
            }
        }
        *pnumFound = numFound;
    }
    return SUCCESS;
}




/*------------------*/
/*   LIBlist        */
/*------------------*/
/*  Exaimine each file in the directory libDir.
 *  If typeInfo = NULL, return the number of valid files found.
 *                      else fill in typeInfo with data for each found file.
 */


WORD FAR PASCAL
LIBlist(libDir, pcount, typeInfo, UpdateType)
    LPSTR      libDir;
    LPUWORD    pcount;
    LPTYPEINFO typeInfo;
    WORD       UpdateType;
{
    UWORD  status;
    WORD   numFound;
    WORD   f;
    UWORD  file_size;
    struct stat st;
    DIRDATA dirdata;

    update = UpdateType;
    DBGliblist(("Insids Liblist\n"));

  /* Build path name of file containing array of TYPEINFO */

    wsprintf ((LPSTR) infoFile, "%s\\%s", libDir, (LPSTR) "libinfo.dsc" );


  /*  If(*pcount == 0) then caller wants us to count the number of
   *  TYPEINFOs we'll return;
   *  otherwise, the caller has passed us an array of TYPEINFOs and
   *  wants us to fill them in.
   */

    if(!(*pcount))
    {
	 f = -1;

        wsprintf((LPSTR) pathName, "%s\\*.*", libDir);

      /*  For each file in the directory, check how many
       *  typefaces are in it and keep a running count in
       *  *pcount. Write a TYPEINFO structure for each
       *  typeface found.
       */

	if (dos_opend(&dirdata, (LPSTR)pathName, 0x01)) 
            return ERRlib_dos_find;

#ifdef NODEF
	if(_dos_findfirst((LPSTR) pathName, _A_NORMAL | _A_RDONLY, (struct find_t FAR *)&file))
            return ERRlib_dos_find;
#endif
        if(status = check_lib((WORD FAR *)&f, (LPSTR)libDir, (LPSTR)dirdata.name, (LPUWORD)&numFound))
            return status;
        *pcount = numFound;

#ifdef NODEF
        while(_dos_findnext((struct find_t FAR *) &file) == 0)
#endif
	while (dos_readd(&dirdata) == 0 )
        {
            if(status = check_lib((WORD FAR *)&f, (LPSTR)libDir, (LPSTR)dirdata.name, (LPUWORD)&numFound))
                return status;
            *pcount += numFound;
        }

	 if (f != -1)
	     _lclose(f);
    }
    else
    {
      /*  Copy list of TYPEINFOs from "libDir\libinfo.dsc" to caller
       *  supplied array.
       */


      /* Open file and get file size */

	if ((f = _lopen((LPSTR)infoFile, 0 /*READ*/)) == -1)
            return ERRlib_no_lib_list;
        if(myfstat(f, (LPSTAT)&st))
        {
            _lclose(f);
            return ERRlib_fstat;
        }
        file_size = (UWORD)st.st_size;
        DBGliblist(("file_size = %u\n", file_size));

      /* Check caller's buffer size */

        if(*pcount * sizeof(TYPEINFO) != file_size)
            return ERRlib_buffer_size;

      /* Read file into buffer 
       */
        if(file_size != _lread(f, (LPSTR)typeInfo, file_size))
            return ERRlib_read;

      /* dtk - close the file and delete it from the disk 
       */
        _lclose(f);
        unlink((LPSTR)infoFile);

    }

    return SUCCESS;
}

