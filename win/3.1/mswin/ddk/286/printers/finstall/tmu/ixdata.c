/**[f******************************************************************
 * ixdata.c - 
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
 *    Module:   ixdata.c
 *
 *    This module gathers information for an entry into Intellifont
 *    Bullet's typeface directory, if.fnt.
 *
 *    functions:
 *       GLOBAL UWORD
 *       IXdata(libname, tfnum, ix)
 *           BYTE        *libname;
 *           LONG        tfnum;
 *           INDEX_ENTRY *ix;
 *
 *      Calls:
 *          VOID  UTgetString();
 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//#define DEBUG

#include    <windows.h>
#ifdef NODEF
#include    <stdlib.h>
#include    <stdio.h>
#include    <fcntl.h>
#include    <io.h>
#include    <string.h>
#include    <malloc.h>
#endif
#include    <ctype.h>
#include    "_port.h"
#include    "debug.h"
#include    "_tmu.h"
#include    "utils.h"
#include    "_ut.h"


/****************************************************************************\
 *				Debug Definitions
\****************************************************************************/


#ifdef DEBUG
   #define	DBGfmseek(msg)      /*DBMSG(msg)*/
   #define	DBGfindbt(msg)      /*DBMSG(msg)*/
   #define	DBGprocface(msg)    /*DBMSG(msg)*/
   #define	DBGproclib(msg)     /*DBMSG(msg)*/
   #define	DBGixdata(msg)      /*DBMSG(msg)*/
#else
   #define	DBGfmseek(msg)      /*null*/
   #define	DBGfindbt(msg)      /*null*/
   #define	DBGprocface(msg)    /*null*/
   #define	DBGproclib(msg)     /*null*/
   #define	DBGixdata(msg)      /*null*/
#endif


/****************************************************************************\
 *			   
\****************************************************************************/


EXTERN  VOID  UTgetString();

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


               /* ------------------------------------ */
               /* Typeface Header Structure Definition */
               /* ------------------------------------ */

struct typefaceHeaderType {
  unsigned short      NFACES;
  char                typeFaceName[50];
  char                familyName[20];
  char                weight[20];
  long                typeFaceSet[12];
};

struct descriptorSetType {
       unsigned char  stemStyle;
       unsigned char  stemMod;
       unsigned char  stemWeight;
       unsigned char  slantStyle;
       unsigned char  horizStyle;
       unsigned char  vertXHeight;
       unsigned char  videoStyle;
       unsigned char  copyUsage;
} ;

               /* ------------------------------------ */
               /* Font Alias Data Structure Definition */
               /* ------------------------------------ */

struct fontAliasTableType {
  char                        aliasTableName[20];
  unsigned short              NXREF;
  char                        xRefArray[100];
  unsigned long               CGtfnum;
};

struct fontAliasDataType {
  unsigned short              NFATS;
  struct fontAliasTableType   fontAliasTable[1];
};


/*
  This file contains the definition of the HP/PCL font alias segment
  as an offset into the 100 byte structure
 */

#define FATYPEABBREV	(0)	/* two character typeface abbrev (char)	*/
#define FATREATMENT	(3)	/* one character treatment code	(char)	*/
				/* fixed / prop			(char)	*/
#define FAPCLWIDTH	(7)	/* width type + 5		(int)	*/
				/* reserved			(int)	*/
#define FASTRUCTURE	(12)	/* style structure		(int)	*/
#define FAWIDTH		(15)	/* style width			(int)	*/
#define FAPOSTURE	(17)	/* style posture		(int)	*/
#define FAWEIGHT	(19)	/* weight + 8			(int)	*/
#define FATYPEFAMILY	(22)	/* typeface family		(int)	*/
#define FASERIF		(28)	/* serif style			(int)	*/
#define FASIMPLESERIF	(31)	/* simple serif flag		(int)	*/
#define FACOMPRESSFLG	(33)	/* compressed flag		(int)	*/
#define FASHORTNAME	(35)	/* short typeface name		(char)	*/
#define FASHORTNAMESIZE	(16)
#define FACGNUMBER	(52)	/* CG typeface number		(long)	*/
#define FACLASSINDEX	(59)	/* family class index		(int)	*/

#define FONTALIASNAME "HP                  "



BYTE bucket_descriptors[32] =
{

    /* Type Face 5720  */

        20,    /* stem style:   */
        20,    /* stem mode:    */
        30,    /* stem weight:  */
        10,    /* slant style:  */
        10,    /* horiz. style: */
        20,    /* vert. style:  */
        10,    /* video style:  */
        40,    /* copy usage:   */

    /* Type Face 5721  */

        20,    /* stem style:   */
        20,    /* stem mode:    */
        40,    /* stem weight:  */
        10,    /* slant style:  */
        10,    /* horiz. style: */
        20,    /* vert. style:  */
        10,    /* video style:  */
        40,    /* copy usage:   */

    /* Type Face 5723  */

        10,    /* stem style:   */
        40,    /* stem mode:    */
        30,    /* stem weight:  */
        10,    /* slant style:  */
        10,    /* horiz. style: */
        20,    /* vert. style:  */
        10,    /* video style:  */
        40,    /* copy usage:   */

    /* Type Face 5724  */

        10,    /* stem style:   */
        40,    /* stem mode:    */
        40,    /* stem weight:  */
        10,    /* slant style:  */
        10,    /* horiz. style: */
        20,    /* vert. style:  */
        10,    /* video style:  */
        40    /* copy usage:   */
};



MLOCAL WORD update=0;          /* 0 = normal font index file
                                * 1 = all typeface numbers will be negated
                                * in the font index file.
                                */


#define MAXTF    32

/****************************************************************************\
 *			   
\****************************************************************************/


/*------------------*/
/*  FMseek_read     */
/*------------------*/
GLOBAL UWORD
FMseek_read(f, offset, count, buffer)
    WORD   f;
    LONG   offset;     /* offset within file       */
    UWORD  count;      /* number of BYTEs to read  */
    LPSTR  buffer;     /* buffer to raed data into */
{
    DBGfmseek(("                FMseek_read()   offset: %ld   count: %u\n",
                                                          offset, count));

    if(_llseek(f, offset, LSEEK_SET) != offset)
    {
        DBGfmseek(("                    lseek() failed\n"));
        return ERRix_seek;
    }

    if(_lread(f, buffer, count) != count)
    {
        DBGfmseek(("                    _lread() failed\n"));
        return ERRix_read;
    }
    return SUCCESS;
}




/*--------------------*/
/*  find_bucket_type  */
/*--------------------*/
MLOCAL UWORD
find_bucket_type(f, tfnum, globoff, globcount, p_bucket_type, tfName)
    WORD     f;
    LONG     tfnum;
    ULONG    globoff;
    UWORD    globcount;
    LPUWORD  p_bucket_type;
    LPTFNAME tfName;
{
    UWORD status;

    HANDLE hseg;
    LPSTR fgseg;
    LPSTR typeface_seg;
    LPSTR fontalias_seg;

    LPSTR p, pp;

    UWORD type;
    ULONG size;

    UWORD key;
    ULONG off;
    UWORD count;

    LPSTR segptr;


  /*  Fontalias segment */

static BYTE
       faisweights[15] = {0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03,
                          0x03, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05};

    UWORD    NFATS;
    UWORD    NXREF;
    ULONG    FACENUM;
    UWORD    i,j;
    BOOLEAN  found_table_name;
    LPSTR    fa_table;

  /* Typeface header segment */

    UWORD  NFACES;
    LPSTR  face_classification;

  /* bucket matching */

    WORD  word1, word2, word3;
    UWORD current, closest;


    DBGfindbt(("\n        find_bucket_type()  offset: %ld   count: %d\n",
                           globoff, globcount));

  /* get face global buffer */

    if(!(hseg = GlobalAlloc(GMEM_MOVEABLE, (LONG)globcount)))
    {
        DBGfindbt(("            malloc failed\n"));
        return ERRix_malloc;
    }

    if(!(fgseg = GlobalLock(hseg)))
    {
        DBGfindbt(("            malloc failed\n"));
	GlobalFree(hseg);
        return ERRix_malloc;
    }


  /* read face global data into buffer */

    if(status = FMseek_read(f, globoff, globcount, fgseg))
    {
        DBGfindbt(("            face global file error\n"));
        MyFreeMem((HANDLE FAR *)&hseg);
        return status;
    }

  /*  Read segment directory and set segment pointers. First
   *  set segment ptrs to nul- not all segments may be present
   */

    typeface_seg   = (LPSTR)0;
    fontalias_seg  = (LPSTR)0;

    p = fgseg;

    type = *((LPUWORD)p);     p += 2;
    size = *((LPULONG)p);     p += 4;
    DBGfindbt(("            type = %d    size = %ld\n",type, size));

    DBGfindbt(("\n            directory\n"));
    do
    {
        key =   *((LPUWORD)p);     p += 2;
        off =   *((LPULONG)p);     p += 4;
        count = *((LPUWORD)p);     p += 2;
        DBGfindbt(("            key / offset / count  %d   %ld   %d\n",
                                              key,off,count));

        segptr = (LPSTR)(fgseg + off);
        switch (key)
        {
            case 107:  typeface_seg = segptr + 6;
                       break;
            case 110:  fontalias_seg = segptr + 6;
                       break;
            default:   break;
        }
    }
    while (key != -1);

    if(typeface_seg)
    {
        DBGfindbt(("\n\n    T y p e f a c e    H e a d e r    S e g m e n t\n\n"));
        p = typeface_seg;

        NFACES = *(LPUWORD)p;     p += 2;
        DBGfindbt(("NFACES = %d\n", NFACES));

        UTgetString(*tfName, p, 50);
        DBGfindbt(("    Primary name: --%50.50s--\n", p));
        p += 50;

        DBGfindbt(("    Family name: --%20.20s--\n", p));
        p += 20;

        DBGfindbt(("  Weight description: --%20.20s--\n", p));
        p += 20;

        pp = p + (4*NFACES);   /*  point to face description array */

        DBGfindbt(("    Typefaces:\n"));
        for(i=0; i<NFACES; i++)
        {
            DBGfindbt(("        %lu\n", *(LPULONG)p));
            if(*(LPULONG)p == tfnum) break;
            p += 4;
        }
        /*  i is now set to index of typeface  */

        if(i==NFACES)
        {
            DBGfindbt(("tfnum %ld not found in typeface header segment\n", tfnum));
            MyFreeMem((HANDLE FAR *)&hseg);
            return ERRix_tf_not_in_lib;
        }

        p = pp + (8*i);     /*  index into descriptor array */
        face_classification = p;
        
        DBGfindbt(("    Face Description:\n"));        
        DBGfindbt(("        stem style:   %u\n", *p));
        DBGfindbt(("        stem mode:    %u\n", *(p+1)));
        DBGfindbt(("        stem weight:  %u\n", *(p+2)));
        DBGfindbt(("        slant style:  %u\n", *(p+3)));
        DBGfindbt(("        horiz. style: %u\n", *(p+4)));
        DBGfindbt(("        vert. style:  %u\n", *(p+5)));
        DBGfindbt(("        video style:  %u\n", *(p+6)));
        DBGfindbt(("        copy usage:   %u\n", *(p+7)));
    }

    if(fontalias_seg)
    {
        DBGfindbt(("\n\n    F o n t a l i a s    S e g m e n t\n\n"));

        p = fontalias_seg;
        fa_table = (LPSTR)0;

        NFATS = *(LPUWORD)p;    p += 2;
        DBGfindbt(("    NFATS = %d\n", NFATS));
        for(i=0; i<NFATS; i++)
        {
            DBGfindbt(("    Alias table name: --%20.20s--\n", p));

            found_table_name = (lstrncmp((LPSTR)FONTALIASNAME, p, 20) == 0);
            p += 20;

            NXREF = *(LPUWORD)p;  p += 2;
            DBGfindbt(("        NXREF = %d\n", NXREF));
            for(j=0; j<NXREF; j++)
            {
                FACENUM = *(LPULONG)(p+100);
                DBGfindbt(("            %lu\n", FACENUM));
                DBGfindbt(("            %25.25s\n", p));
                DBGfindbt(("            %25.25s\n", p+25));
                DBGfindbt(("            %25.25s\n", p+50));
                DBGfindbt(("            %25.25s\n", p+75));

                if(found_table_name && (FACENUM == tfnum))
                {
                    fa_table = p;
                    break;
                }
                p += 104;
            }
            if(fa_table) break;
        }
        if(i == NFATS)
        {
            DBGfindbt(("font alias table not found\n"));
            MyFreeMem((HANDLE FAR *)&hseg);
            return ERRix_no_font_alias;
        }

        DBGfindbt(("Replacing classification data from font alias table:\n\n"));

      /* Stem style */
        DBGfindbt(("    stem style:\n"));
        DBGfindbt(("        fa_table[FASIMPLESERIF] = %d\n", 
                                        myatoi ((LPSTR)&fa_table[FASIMPLESERIF])));
        DBGfindbt(("        old stem style = %d\n", face_classification[0]));

        face_classification[0] = (BYTE)
                        (10 * (myatoi((LPSTR)&fa_table[FASIMPLESERIF]) + 1));

        DBGfindbt(("        new stem style = %d\n", face_classification[0]));

      /* stem weight */
        


        DBGfindbt(("    stem weight:\n"));
        DBGfindbt(("    fa_table[FAWEIGHT]) - 1 = %d\n",
                                             myatoi((LPSTR)&fa_table[FAWEIGHT]) - 1));
        DBGfindbt(("        old stem weight = %d\n", face_classification[2]));

        face_classification[2] = (BYTE)
                      (10 * faisweights[myatoi((LPSTR)&fa_table[FAWEIGHT]) - 1]);
        DBGfindbt(("        new stem weight = %d\n", face_classification[2]));




      /* slant style */

        DBGfindbt(("    slant style:\n"));
        DBGfindbt(("        fa_table[FAPOSTURE] = %d\n",
                                         myatoi((LPSTR)&fa_table[FAPOSTURE])));
        DBGfindbt(("        old slant style = %d\n",
                                        face_classification[3]));
        face_classification[3] = (BYTE)
                          (10 * ((myatoi((LPSTR)&fa_table[FAPOSTURE])&1) + 1));
        DBGfindbt(("        new slant style = %d\n", face_classification[3]));
    }

  /*  Compute limited sensitive bucket */

    closest = 0xffff;
    p = bucket_descriptors;
    for(i=0; i<4; i++)
    {
        word1 = (face_classification[2] - p[2])/10;
        word2 = (face_classification[3] - p[3])/10;
        word3 = (face_classification[0] - p[0])/10;

        current = (((UWORD)word1&0x8000)>>7) +
                  ((UWORD)((ABS(word1))<<9)) +
                  (((UWORD)word2&0x8000)>>11) +
                  ((UWORD)((ABS(word2))<<5)) +
                  (((UWORD)word3&0x8000)>>15) +
                  (UWORD)((ABS(word3))<<1);
               
        if (closest > current)
        {
            closest = current;
            *p_bucket_type = i;
        }
        p += 8;
    }

    MyFreeMem((HANDLE FAR *)&hseg);
    return SUCCESS;
}

/*----------------*/
/*  process_face  */
/*----------------*/
MLOCAL UWORD
process_face(f, tfnum, fhoff, fhcount, ix, tfName)
    WORD          f;
    LONG          tfnum;
    LONG          fhoff;
    UWORD         fhcount;
    LPINDEX_ENTRY ix;
    LPTFNAME      tfName;
{
    UWORD status;

    BYTE  char_dir[6];   /* face header segment */
    LONG globoff;
    UWORD globcount;
    UWORD  name_len;
    UWORD  bucket_type;

    DBGprocface(("process_face(%ld, %ld, %u)", tfnum, fhoff, fhcount));

    DBGprocface(("Face header segment\n"));

  /* read face header link to face global segment */

    if(status = FMseek_read(f, fhoff, (UWORD)6, (LPSTR)char_dir))
        return status;

  /* Type face global segment */

    globoff =   *(LPLONG)char_dir;
    globcount = *(LPUWORD)(char_dir + 4);
    DBGprocface(("    global off / count   %ld  %d\n",globoff, globcount));

    DBGprocface(("                                              total = %d\n",
                                 fhcount+globcount));
  /* Find type of limited sensitive bucket */

    if(status = find_bucket_type(f, tfnum, globoff, globcount,
                                          (LPUWORD)&bucket_type, (LPTFNAME)tfName))
        return status;

  /* Store entry in font index table */

    if(update) tfnum = - tfnum;    /* negate typeface number for update */
    DBGprocface(("%ld %ld %u %u\n", tfnum, fhoff, fhcount, bucket_type));

    ix->tfnum = tfnum;
    ix->fhoff = fhoff;
    ix->fhcount = fhcount;
    ix->bucket_num = bucket_type;

    return SUCCESS;
}




/*----------------*/
/*  process_lib   */
/*----------------*/
MLOCAL UWORD
process_lib(f, ix, tfName, maxNum, pnumFound)
    WORD          f;          /* open library file */
    LPINDEX_ENTRY ix;
    LPTFNAME      tfName;
    LPUWORD       pnumFound;
{
    UWORD status;

    UWORD type;

    WORD  i;
    WORD  num_faces;

    UWORD key,count;      /* File segment directroy */
    LONG off;

    LONG fdoff;           /* File directory */
    UWORD fdcount;

    LONG tfnum[MAXTF], tfoff[MAXTF];   /* Face header segments */
    UWORD tfcount[MAXTF];
    BOOLEAN found;

    DBGproclib(("process_lib()\n"));

    if(_lread(f,(LPSTR)&type,2) != 2)
        return ERRix_read;

    if(type != 68)
    {
        DBGproclib(("Library file type not 'disk'\n"));
        return ERRix_not_disk_lib;
    }

  /* File segment directory */

    DBGproclib(("\nFile segment directory\n"));
    do
    {
        if(_lread(f,(LPSTR)&key,2) != 2)            return ERRix_read;
        if(_lread(f,(LPSTR)&off,4) != 4)            return ERRix_read;
        if(_lread(f,(LPSTR)&count,2) != 2)          return ERRix_read;

        DBGproclib(("    key / offset / count  %d   %ld   %d\n",key,off,count));
        if(key == 2)
        {
            fdoff = off;       /* save file directory segment */
            fdcount = count;
        }
    }
    while (key != -1);

  /* File Directory Segment */

    DBGproclib(("\nFile directory segment\n"));
    if( _llseek(f, fdoff, LSEEK_SET) != fdoff)
    {
        DBGproclib(("seek error\n"));
        return ERRix_seek;
    }

    for(i=0; i<MAXTF; i++)
    {
        if(_lread(f,(LPSTR)&tfnum[i],4) != 4)            return ERRix_read;
        if(_lread(f,(LPSTR)&tfoff[i],4) != 4)            return ERRix_read;
        if(_lread(f,(LPSTR)&tfcount[i],2) != 2)          return ERRix_read;

        DBGproclib(("    face / offset / count %ld   %ld  %d\n",
                                      tfnum[i], tfoff[i], tfcount[i]));
        if(tfnum[i] == -1) break;
    }
    num_faces = i;
    DBGproclib(("%d faces in library\n", num_faces));

    *pnumFound = 0;
    if(num_faces > maxNum)
        return ERRix_too_many_faces;

    for(i=0; i<num_faces; i++)
    {
        if(status = process_face(f, tfnum[i], tfoff[i], tfcount[i],
                                       ix, tfName))
            return status;

        (*pnumFound)++;
        ix++;
        tfName++;
    }
    return SUCCESS;
}




/*----------------*/
/*     IXdata     */
/*----------------*/
GLOBAL UWORD
IXdata(libname, ix, tfName, maxNum, pnumFound, updateType)
    LPSTR         libname;
    LPINDEX_ENTRY ix;
    LPTFNAME      tfName;
    UWORD         maxNum;
    LPUWORD       pnumFound;
    WORD updateType;
{
    UWORD status;
    WORD  f;               /* library file */

    DBGixdata(("IXdata(%s, %d)\n",libname, maxNum));

    update = updateType;

    if ((f = _lopen((LPSTR)libname, 0 /*READ*/)) == -1)
    {
        DBGixdata(("Library file %s not found\n", libname));
        return ERRix_no_lib_file;
    }
    status = process_lib(f, ix, tfName, maxNum, pnumFound);
    _lclose(f);
    return status;
}

