/**[f******************************************************************
 * fais.c - 
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
 *
 *    This module reads an FAIS diskette and builds a list
 *    of all the typefaces on the diskette or loads a particular
 *    typeface creating a typeface library file.
 *
 *    functions:
 *         FAISlist()
 *         FAISload()
 *      Calls:
 *         lib_load()
 *  
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//#define DEBUG

#include    <windows.h>
#ifdef NODEF
#include    <windows.h>
#include    <fcntl.h>
#include    <io.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <malloc.h>
#include    <string.h>
#include    <process.h>
#endif


#include    <ctype.h>
#include    <dos.h>
#include    "dosutils.h"            /* BUG #135 */
#include    "debug.h"
#include    "_port.h"
#include    "_tmu.h"
#include    "_ut.h"

#define MAX_FONT_FILES 32
#include    "_fais.h"     /* Intellifont header (note: includes library.h) */


/****************************************************************************\
 *				Debug Definitions
\****************************************************************************/


#ifdef DEBUG
   #define	DBGfaislist(msg)    /*DBMSG(msg)*/
   #define	DBGfaisload(msg)    /*DBMSG(msg)*/
#else
   #define	DBGfaislist(msg)    /*null*/
   #define	DBGfaisload(msg)    /*null*/
#endif






EXTERN WORD lib_load();


/**************************************************************************

    calculated space required to load (install) specified face

        Find every file on the diskette which is associated with
        this typeface id.  Sum up all their sizes.
 **************************************************************************/

MLOCAL LONG
calc_space_req(faisDir, id)
    LPSTR faisDir;
    LONG  id;
{
    LONG space_required;
    WORD i, rc;
    BYTE buf[80], filename[13];
    DIRDATA fileinfo;                          /* BUG #135 */
    space_required = 0;
    for (i=0; (i < index_file.nFiles); i++)    /* loop through index */
    {
        lstrncpy(buf, index_file.font[i].face, 6);
        buf[6] = '\0';
        if (id == atol(buf))            /* typeface id matches */
        {
          /* get file size */
            lmemset(filename, '\0', sizeof(filename));
            lmemcpy(filename, index_file.font[i].fileName, 
                sizeof(index_file.font[i].fileName));
            wsprintf ((LPSTR)buf, "%s\\%s", faisDir, (LPSTR)filename);
            rc = (WORD) dos_opend(&fileinfo, (LPSTR) buf, _A_NORMAL);  /* BUG #135 */
//          rc = _dos_findfirst((LPSTR)buf, _A_NORMAL, (struct find_t FAR *)&fileinfo);
//          rc = _dos_findfirst(buf, _A_NORMAL, &fileinfo);

            if (!rc)
                space_required += fileinfo.size;
        }    
    }

    return(space_required);
}

/************************************************************************

    Procedure: FAISlist()
    Summary:   Reads an FAIS diskette and builds a list of all the
               FAIS typefaces on the diskette.
    Inputs:    disk drive containing the FAIS diskette
               ptr to an allocated listptr

    Outputs:   List of typefaces, in array of FAIS_INFO_STRUCT

    Calls:
    Cautions/Assumptions:

 ************************************************************************/

WORD FAR PASCAL
FAISlist(faisDir, num_entries, typeInfo)
    LPSTR      faisDir;
    LPUWORD    num_entries;
    LPTYPEINFO typeInfo;
{
    BOOLEAN count_typefaces;
    int dir;  
    WORD  f;                   /* file handle for fontindx.fi */
    WORD  i, k, found;
    UWORD len;
    BYTE  strng[20], buff[256];
    LPSTR ptr = NULL;
    long id, comp, space_required;
    BYTE typeface_name[80];

    VOID  UTgetString();
    LONG  calc_space_req();
    HANDLE hmem;


  /* read the font index file */

    wsprintf((LPSTR)strng, "%s\\fontindx.fi", faisDir);
    if((f = _lopen((LPSTR)strng, 0 /*READ*/)) == -1)
        return ERRnot_fais;

    if ((_lread(f, (LPSTR)&index_file, sizeof(index_file))) == -1)
    {
        _lclose(f);
        return ERRfais_read;
    }
    _lclose(f);

#ifdef DEBUG
    DBGfaislist(("volId     %u\n", index_file.volId));
    DBGfaislist(("nVols     %u\n", index_file.nVols));
    DBGfaislist(("volNo     %u\n", index_file.volNo));
    DBGfaislist(("volLength %ld\n", index_file.volLength));
    DBGfaislist(("nFiles    %u\n", index_file.nFiles));
    for(i=0; i<index_file.nFiles; i++)
    {
        DBGfaislist(("______________________________\n"));
        DBGfaislist(("face       %.6s\n", index_file.font[i].face));
        DBGfaislist(("comp       %.6s\n", index_file.font[i].comp));
        DBGfaislist(("style      %.2s\n", index_file.font[i].style));
        DBGfaislist(("pointSize  %.4s\n", index_file.font[i].pointSize));
        DBGfaislist(("setSize    %.4s\n", index_file.font[i].setSize));
        DBGfaislist(("res        %.4s\n", index_file.font[i].res));
        DBGfaislist(("orient     %.4s\n", index_file.font[i].orient));
        DBGfaislist(("fileId     %.5s\n", index_file.font[i].fileId));
        DBGfaislist(("fileType   %.2s\n", index_file.font[i].fileType));
        DBGfaislist(("fill       %.5s\n", index_file.font[i].fill));
        DBGfaislist(("dupCode    %u\n", index_file.font[i].dupCode));
        DBGfaislist(("nVol       %u\n", index_file.font[i].nVol));
        DBGfaislist(("iVol       %u\n", index_file.font[i].iVol));
        DBGfaislist(("fileName   %.8s\n", index_file.font[i].fileName));
    }
#endif

  /* go through table, read each dir file */  

  /*  If(*num_entries == 0) then caller wants us to count the number of
   *  FAIS_INFOs we'll return;
   *  otherwise, the caller has passed us an array of FAIS_INFOs and
   *  wants us to fill them in.
   */

    count_typefaces = !(*num_entries);

    *num_entries = 0;
    for(i=0; i<index_file.nFiles; i++)    /* loop thru indx file table*/
    {
      /* Skip attribute files no matter what we're doing */

        if(lmemcmp(index_file.font[i].fileType, "FA", 2))
            continue;                /* if not, skip */

      /*  If we're only counting typefaces for the caller; bump and
       *  continue
       */

        if(count_typefaces)
        {
            (*num_entries)++;
            continue;
        }

      /* get typeface id */
        lstrncpy(strng,index_file.font[i].face,6);
        strng[6] = '\0';
        id = atol(strng);

      /* get complement */
        lstrncpy(strng,index_file.font[i].comp,6);
        strng[6] = '\0';
        comp = atol(strng);

      /* open attribute file */
        wsprintf((LPSTR)strng, "%s\\%.8s", faisDir, (LPSTR)index_file.font[i].fileName);
        if((dir=_lopen((LPSTR)strng, 0 /*READ*/)) == -1)
            return ERRfais_read;

      /* setup attrib file header */
        _llseek(dir, 0L, LSEEK_SET);
        if (_lread(dir, (LPSTR)buff, 256) == -1)
        {
            _lclose(dir);
            return ERRfais_read;
        }
        attrFileHeader = (struct seg_type FAR *)buff;

        if (attrFileHeader->nkeys > 50)
            return ERRfais_malloc;
        else
            len = attrFileHeader->nkeys*6+8;

	if(!(hmem = GlobalAlloc(GMEM_MOVEABLE, (LONG)len)))
	{
	    DBGfaislist(("            malloc failed\n"));
	    return ERRfais_malloc;
	}
        if((ptr=GlobalLock(hmem)) == NULL)
        {    GlobalFree(hmem);
            _lclose(dir);
            return ERRfais_malloc;
        }
        _llseek(dir, 0L, LSEEK_SET);
        if (_lread(dir, (LPSTR)ptr, len) == -1)
        {
            _lclose(dir);
            MyFreeMem((HANDLE FAR *)&hmem);
            return ERRfais_read;
        }
        attrFileHeader = (struct seg_type FAR *)ptr;

      /* get typeface hdr information */
        typeface_name[0] = '\0';
        for(k=found=0; k<attrFileHeader->nkeys; k++)
        {
            if(attrFileHeader->seg[k].key == TYPEFACEHEADER)
            {
                found = 1;
                break;
            }
        }
        if(found)
        {
	    _llseek(dir, attrFileHeader->seg[k].offset, LSEEK_SET);
            if (_lread(dir, (LPSTR)buff, 256) == -1)
            {
                _lclose(dir);
                MyFreeMem((HANDLE FAR *)&hmem);
                return ERRfais_read;
            }
            else
            {
                typefaceHeader = (struct typefaceHeaderType FAR *)buff;
                UTgetString((LPSTR)typeface_name,(LPSTR)&typefaceHeader->typeFaceName[0],50);
                DBGfaislist(("typeface_name  %ls\n", (LPSTR)typeface_name));
            }
        }

        _lclose(dir);
        MyFreeMem((HANDLE FAR *)&hmem);

      /* get space requirements */
        space_required = calc_space_req(faisDir, id);
        DBGfaislist(("    space_required %ld\n", space_required));


      /* return info in next entry in caller's TYPEINFO array */

        typeInfo->id = id;
        typeInfo->complement = comp;
        typeInfo->space_req = space_required;
        wsprintf ((LPSTR)typeInfo->typefaceName, "%s", (LPSTR)typeface_name);
        lstrcpy((LPSTR)typeInfo->nameOrDir, faisDir);
        typeInfo++;
        (*num_entries)++;

    } /* end loop through indx file table */

  return SUCCESS;
}




/************************************************************************
    Procedure: FAISload()
    Summary:   Installs a single typeface.
               Actions:
                   1. Calls FACEdelete() to remove the typeface library
                      file and update if.fnt and if.dsc in case the
                      typeface was already installed.
                   2. Spawns to the loader to make a new library file.
                   3. Calls FACEadd() to update if.fnt and if.lst.

    Inputs:    Pointer to FAIS_INFO structure and source drive
    Outputs:   Status.

    Calls:     FACEdelete()
               spawns to loader1.exm
               FACEadd()

    Cautions/Assumptions:

 ************************************************************************/


WORD FAR PASCAL
FAISload (typeInfo, typePath, libName)
    LPTYPEINFO typeInfo;
    LPSTR      typePath;
    LPSTR      libName;
{
    UWORD status;
    WORD i;
    LPSTR args[6];
    char id_buf[12], comp_buf[12];
    BYTE sourceDir[MAXNAME];

    DBGfaisload(("Inside Libload\n"));

  /*  Build destination library file path name in caller's
   *  string variable so as to return it.
   */

    wsprintf ((LPSTR)libName, "%s\\%.2ld.typ", (LPSTR)typePath, typeInfo->id);
    unlink(libName); 


    wsprintf ((LPSTR)id_buf, "%.2ld", typeInfo->id);
    wsprintf ((LPSTR)comp_buf, "%ld", typeInfo->complement);
    wsprintf ((LPSTR)sourceDir, "/d=%s\\", typeInfo->nameOrDir);
    args[0] = (LPSTR)"loader1.exm";
    args[1] = (LPSTR)libName;
    args[2] = (LPSTR)id_buf;
    args[3] = (LPSTR)comp_buf;
    args[4] = (LPSTR)sourceDir;
    args[5] = NULL;

#ifdef DEBUG
    DBGfaisload(("args[] for loader1.exm\n"));
    for(i=0; i<5; i++)
        DBGfaisload(("    %ls\n", (LPSTR)args[i]));
#endif

/*    status = spawnv (P_WAIT, "loader1.exm", args);  */

    status = lib_load((WORD)5, (LPSTR)args);

    return status;
}


