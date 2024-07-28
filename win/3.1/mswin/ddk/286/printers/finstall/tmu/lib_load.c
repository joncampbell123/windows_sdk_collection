/**[f******************************************************************
 * lib_load.c - 
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
 *    Module:   lib_load.c
 *
 *  Current Status:
 *   1. Can not add characters to an exisiting library face.
 *   2. Can not handle multi-diskette FAIS volumes.
 *
 *  Copyright (C) 1987,1988, All Rights Reserved, by
 *  Compugraphic Corporation, Wilmington, Ma.
 * 
 *  This software is furnished under a license and may be used and copied
 *  only	in accordance with the terms of such license and with the
 *  inclusion of the above copyright notice. This software or any other
 *  copies thereof may not be provided or otherwise made available to any
 *  other person. No title to and ownership of the software is hereby
 *  transferred.
 * 
 *  The information in this software is subject to change without notice
 *  and should not be construed as a commitment by Compugraphic
 *  Corporation.
 *  
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//#define DEBUG

#include <windows.h>
#ifdef NODEF
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#endif
#include <ctype.h>

#include "_port.h"
#include "_loader.h"
#include "_loaderr.h"
#include "_ut.h"
#include "debug.h"



/****************************************************************************\
 * 							Debug Definitions
\****************************************************************************/


#ifdef DEBUG
    #define DBGlib_load(msg)       /*DBMSG(msg)*/
    #define DBGparse(msg)          /*DBMSG(msg)*/
    #define DBGglobalseg(msg)      /*DBMSG(msg)*/
#else
    #define DBGlib_load(msg)       /*null*/
    #define DBGparse(msg)          /*null*/
    #define DBGglobalseg(msg)      /*null*/
#endif


/****************************************************************************\
 *  New routines to windowize loader
\****************************************************************************/

#ifdef LINTARGS

GLOBAL WORD lib_load(WORD,LPSTR far *);
GLOBAL ULONG align(ULONG,UWORD);
GLOBAL WORD parse(WORD,LPSTR far *);
GLOBAL WORD directory_setup();
GLOBAL WORD face_header_setup();
GLOBAL WORD library_header_setup(LPSTR);
GLOBAL WORD global_segment_setup();
GLOBAL WORD write_characters();
GLOBAL WORD check_sum(LPSTR);
MLOCAL VOID sort(charwidth_type far *,unpackedkern_type far *,unpackedkern_type far *);
MLOCAL VOID pack_kerning(packedkern_type far *,unpackedkern_type far *);
MLOCAL WORD assemblecompoundchar(WORD far *, WORD far *, 
            complementheader_type far *,
            ccharmetrics_type far *,
            ccid_type far *);
MLOCAL WORD code_to_cgnum(UWORD, isocodetable_type far *);

#else
GLOBAL WORD lib_load();
GLOBAL ULONG align();
GLOBAL WORD parse();
GLOBAL WORD directory_setup();
GLOBAL WORD face_header_setup();
GLOBAL WORD library_header_setup();
GLOBAL WORD global_segment_setup();
GLOBAL WORD write_characters();
GLOBAL WORD check_sum();
MLOCAL VOID sort();
MLOCAL VOID pack_kerning();
MLOCAL WORD assemblecompoundchar();
MLOCAL WORD code_to_cgnum();
#endif


/****************************************************************************\
 * 
\****************************************************************************/


GLOBAL WORD
lib_load(argc,argv)
WORD argc;
LPSTR far *argv;
{
  int ret_code;
/*   Process 1.2.1   Parse Command Line               */
  ret_code = parse(argc,argv);
  if(ret_code) 
  {
    DBGlib_load(("Call to parse FAILED !  status = %d\n",ret_code));
    return(ret_code);
  }

/*   Process 1.2.2   Diskette Directory Setup         */
  ret_code = directory_setup();
  if(ret_code) 
  {
    DBGlib_load(("Call to directory_setup FAILED !  status = %d\n",ret_code));
    return(ret_code);
  }

/*   Process 1.2.3   Face Header Setup                */
  ret_code = face_header_setup();
  if(ret_code) 
  {
    DBGlib_load(("Call to face_header_setup FAILED !  status = %d\n",ret_code));
    return(ret_code);
  }

/*   Process 1.2.4   Library Header Setup             */
  ret_code = library_header_setup((LPSTR)libName);
  if(ret_code) 
  {
    DBGlib_load(("Call to library_header_setup FAILED !  status = %d\n",ret_code));
    return(ret_code);
  }


/*   Process 1.2.6   Global Segment Setup             */
    /* get the text and design kerning buffers (also used for other things) */

    if (!(htkrn = GlobalAlloc(GMEM_MOVEABLE, (LONG)MAX_TEXKERN)) ||
        !(textkern = GlobalLock(htkrn)))
    {
      if (htkrn)                     /* added test and reset value BUG #175 */     
    	   htkrn = GlobalFree(htkrn);  /* in case 'lock' was what failed */
      return ld_err_mem;
    }

    if (!(hdkrn = GlobalAlloc(GMEM_MOVEABLE, (LONG)MAX_DESKERN)) ||
        !(designkern = GlobalLock(hdkrn)))
    {
      if (hdkrn)                     /* added test and reset value BUG #175 */
    	   hdkrn = GlobalFree(hdkrn);  /* in case 'lock' was what failed */
      return ld_err_mem;
    }

  ret_code = global_segment_setup();
    GlobalUnlock(htkrn);    /* these 2 buffers are no longer needed */
    GlobalFree(htkrn);
    GlobalUnlock(hdkrn);
    GlobalFree(hdkrn);
  if(ret_code) return(ret_code);

/*   Process 1.2.7   Write Characters                 */
  ret_code = write_characters();
  if(ret_code) return(ret_code);

/*   Process 1.2.8   Check Library                    */
  ret_code = check_sum((LPSTR)libName);
  return(ret_code);
}





/*   Process 1.2.1
**   Parse command line:
**   >loader library/switch face complement [orThreshold]
**   [orThreshold] is optional. If present and != 0 overwrites orThreshold
**   value coming from Global Intellifont Data.
**                                          
*/
GLOBAL WORD
parse(argc,argv)
WORD argc;
LPSTR far *argv;
{
#if ERRORFILE
  fprintf(errfile, words, VERSION, LASTMODIFIED );
  if( (argc < 4) | (argc > 6) ) {
    fprintf(errfile,"Invalid input argument(s)\n");
    fprintf(errfile,"Program usage:\n");
    fprintf(errfile,"LOADER Library[/D/N/V/M] Typeface Complement [/O=orThreshold] [/D=FAISdrive]\n");
    DBGparse(("1st 'ld_err_arg' in parse !  argc = %d\n",argc));
    return(ld_err_arg);
  }
#endif
/*
**  Set default switch to '/D' indicating a Disk Library
*/
  type = 'D';
  disk = TRUE;
  alignment = DISK_ALIGN;
/*
**  Decode switch if any
*/
  if( argv[1][lstrlen(argv[1])-2] == '/'){
    type = argv[1][lstrlen(argv[1])-1];
    argv[1][lstrlen(argv[1])-2] = '\0';
    switch (type) {
    case 'd':
    case 'D':
      alignment = DISK_ALIGN;
      type = 'D';
      disk = TRUE;
      break;
    case 'v':
    case 'V':
      alignment = RAM_ALIGN;
      type = 'V';
      disk = FALSE;
      break;
    case 'n':
    case 'N':
      alignment = RAM_ALIGN;
      type = 'N';
      disk = FALSE;
      break;
/*    case 'm':
    case 'M':
      genMap=1;
      lstrcpy(mapName,argv[3]);
      lstrcat(mapName,".mp1");
      break;
*/
    default:
#if ERRORFILE
      fprintf(errfile,"Illegal library type\n");
#endif
      return(ld_err_1);
      break;
    }
  }
  lstrcpy((LPSTR)libName,argv[1]);
  typeface = atol(argv[2]);
  complement = atol(argv[3]);
  if ( argc == 6 )
  {
     if(isdigit(argv[4][0])) orThreshold = atoi(argv[4]);
     else 
          if(lstrncmpi(argv[4],(LPSTR)fais_drive[0],3) == 0)
                             orThreshold = atoi(&argv[4][3]);
          else {
#if ERRORFILE
            fprintf(errfile,"Invalid input argument(s)\n");
            fprintf(errfile,"Program usage:\n");
            fprintf(errfile,"LOADER Library[/D/N/V/M] Typeface Complement [/O=orThreshold] [/D=FAISdrive]\n");
#endif
            DBGparse(("2nd 'ld_err_arg' in parse !  argc = %d\n",argc));
            return(ld_err_arg);
          }
     if(lstrncmpi(argv[5],(LPSTR)fais_drive[1],3) == 0)
                            lstrcpy(fais_path,&argv[5][3]);
     else {
#if ERRORFILE
       fprintf(errfile,"Invalid input argument(s)\n");
       fprintf(errfile,"Program usage:\n");
       fprintf(errfile,"LOADER Library[/D/N/V/M] Typeface Complement [/O=orThreshold] [/D=FAISdrive]\n");
#endif
       DBGparse(("3rd 'ld_err_arg' in parse !  argc = %d\n",argc));
       return(ld_err_arg);
     }
  }
  else if(argc == 5)
  {
     if(lstrncmpi(argv[4],(LPSTR)fais_drive[1],3) == 0)
     {
        lstrcpy(fais_path,&argv[4][3]);
        orThreshold = (WORD)0;
     }
     else
     {
        if(isdigit(argv[4][0])) orThreshold = atoi(argv[4]);
        else
             if(lstrncmpi(argv[4],(LPSTR)fais_drive[0],3) == 0)
                             orThreshold = atoi(&argv[4][3]);
             else {
#if ERRORFILE
               fprintf(errfile,"Invalid input argument(s)\n");
               fprintf(errfile,"Program usage:\n");
               fprintf(errfile,"LOADER Library[/D/N/V/M] Typeface Complement [/O=orThreshold] [/D=FAISdrive]\n");
#endif
               DBGparse(("4th 'ld_err_arg' in parse !  argc = %d\n",argc));
               return(ld_err_arg);
             }
        lstrcpy(fais_path,"a:\\");
     }
  }
  else
  {
        orThreshold = (WORD)0;
        lstrcpy(fais_path,"a:\\");
  }
  return(OK);
}

/*
**  Process 1.2.2
*/
GLOBAL WORD
directory_setup()
{
  BYTE string[80];
  WORD i,found,status,l;
  LONG face;
  LONG comp;
  int  FONTINDEX;
/*
 *  Verify Font Disk on user-specified or default drive:
 */
  lstrcpy(string,fais_path);
  lstrcat(string,"FONTINDX.FI");
  if((FONTINDEX=_lopen((LPSTR)string,OF_READ))==NULL){
#if ERRORFILE
    fprintf(errfile,"Cg font diskette file %sFONTINDX.FI not found\n",fais_path);
#endif
    return(ld_err_2);
  }
/*
 *  Read Font Index File
 */
  if(_lread(FONTINDEX,(LPSTR)&index_file,sizeof(index_file)) == -1){
#if ERRORFILE
    fprintf(errfile,"Error reading Cg font disk %sFONTINDX.FI\n",fais_path);
#endif
    return(ld_err_3);
  }
/*
**  Validate Font Diskette
*/
  _lclose(FONTINDEX);
  if(index_file.nFiles > MAX_FONT_FILES) {
#if ERRORFILE
    fprintf(errfile,"Maximum files limit exceeded\n");
#endif
    return(ld_err_4);
  }

/*
 *  Verify Font in Font Disk
 */
  found = FALSE;
  for(i=0;i<index_file.nFiles;i++){
    if((index_file.font[i].fileType[0]=='F')&&
                                (index_file.font[i].fileType[1]=='D')){
      lstrncpy(string,index_file.font[i].face,6);
      string[6] = '\0';
      face = atol(string);
      lstrncpy(string,index_file.font[i].comp,6);
      string[6] = '\0';
      comp = atol(string);
      if((typeface==face)&&(complement==comp)){
        found=TRUE;
        break;
      }
    }
  }
  if(!found) {
#if ERRORFILE
    fprintf(errfile,"Typeface/Complement number not found\n");
#endif
    return(ld_err_5);
  }
/*
**  Open Font Display File
*/
  lstrcpy(string,fais_path);
  l = lstrlen(string);
  lstrncpy(&string[l],index_file.font[i].fileName,8);
  string[l+8] = '\0';
  if((FONTDISPLAY=_lopen((LPSTR)string,OF_READ))==NULL){
#if ERRORFILE
      fprintf(errfile,"Can not open font file\n");
#endif
    return(ld_err_6);
  }

/*
 *  Locate Attribute File on Font Disk
 */
  found = FALSE;
  for(i=0;i<index_file.nFiles;i++){
    if((index_file.font[i].fileType[0]=='F')&&
                                (index_file.font[i].fileType[1]=='A')){
      lstrncpy(string,index_file.font[i].face,6);
      string[6] = '\0';
      typeface = atol(string);
      lstrncpy(string,index_file.font[i].comp,6);
      string[6] = '\0';
      complement = atol(string);
      if((typeface==face)&&(complement==comp)){
        found=TRUE;
        break;
      }
    }
  }
  if(!found) {
#if ERRORFILE
    fprintf(errfile,"Typeface/Complement number not found\n");
#endif
    return(ld_err_5);
  }
/*
**  Open Font Attribute File
*/
  lstrcpy(string,fais_path);
  l = lstrlen(string);
  lstrncpy(&string[l],index_file.font[i].fileName,8);
  string[l+8] = '\0';
  if((FONTATTRIBUTE=_lopen((LPSTR)string,OF_READ))==NULL){
#if ERRORFILE
    fprintf(errfile,"Can not open font file\n");
#endif
    return(ld_err_6);
  }
  return(OK);
}

GLOBAL WORD
face_header_setup()
{

    keyoffset_type far *ptr;
    UWORD i;
    ULONG cgcharnumsoffset,globalchdataoffset;
    globalchdata_type far *globalchdata;
    cgcharnums_type far *cgcharnums;
    char_type far *charptr;
    LONG far *offsets,far *nextoffset;
    faceheader_type far *faceheaderptr;

    

    //  read display file header
    status = _lread(FONTDISPLAY,dfileheader,MAX_HEADER);
#if ERRORFILE
    if(status <= 0) fprintf(errfile,"Error reading display file\n");
#endif

    //  locate hiqdata1
    hiqdataoffset = MAX_LONG;
    ptr = (keyoffset_type far *)(((fileheader_type far *)dfileheader)->keyOffsets);
    if(((fileheader_type far *)dfileheader)->NKEYS > MAX_KEYS )
      ((fileheader_type far *)dfileheader)->NKEYS = MAX_KEYS;
    for(i=0; i<((fileheader_type far *)dfileheader)->NKEYS; i++) {
      if(ptr->key == HIQDATA1)  {
         hiqdataoffset = ptr->offset;
         break;
      }
      ptr++;
    }
    
    if(hiqdataoffset == MAX_LONG) {
#if ERRORFILE
       fprintf(errfile,"Error reading HIQDATA1 segment\n");
#endif
       return(ld_err_9);
    }

    //  read hiqdata1 segment 
    status = _llseek((int)FONTDISPLAY,(LONG)hiqdataoffset,(int)LSEEK_SET);
    status = _lread(FONTDISPLAY,hiqdata,MAX_HEADER);
    if(status <= 0) {
#if ERRORFILE
       fprintf(errfile,"Error reading HIQDATA1 segment\n");
#endif
       return(ld_err_9);
    }

    //  locate cgcharnums and globalchdata
    cgcharnumsoffset = MAX_LONG;
    globalchdataoffset = MAX_LONG;
    globintdataoffset = MAX_LONG;
    ptr = (keyoffset_type far *)(((hiqdata1_type far *)hiqdata)->keyOffsets);
    if(((hiqdata1_type far *)hiqdata)->NBKEYS > MAX_KEYS )
      ((hiqdata1_type far *)hiqdata)->NBKEYS = MAX_KEYS;
    for(i=0; i<((hiqdata1_type far *)hiqdata)->NBKEYS; i++) {
      if(ptr->key == CGCHARNUMS)
        cgcharnumsoffset = hiqdataoffset + ptr->offset;
      if(ptr->key == GLOBALCHDATA)
        globalchdataoffset = hiqdataoffset + ptr->offset;
      if(ptr->key == GLOBINTDATA) { // will need it in global_segment_setup()
        globintdataoffset = hiqdataoffset + ptr->offset;
        globintdatasize = (ptr+1)->offset - ptr->offset;
      }

      ptr++;
    }
    if(cgcharnumsoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Error reading CGCHARNUMS segment.\n");
#endif
      return(ld_err_10);
    }
    if(globalchdataoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Error reading GLOBALCHDATA segment.\n");
#endif
      return(ld_err_9);
    }

    //  read globalchdata into buffer
    globalchdata = (globalchdata_type far *)buffer;
    status = _llseek(FONTDISPLAY,globalchdataoffset,LSEEK_SET);
    status = _lread(FONTDISPLAY,(LPSTR)globalchdata,4 * MAX_CHARS + 10);

    if(status <= 0) {
#if ERRORFILE
      fprintf(errfile," Error reading GLOBALCHDATA segment.\n");
#endif
      return(ld_err_9);
    }

    //  read cgcharnums into buffer
    cgcharnums = (cgcharnums_type far *)(buffer + 4 * MAX_CHARS + 10);
    status = _llseek(FONTDISPLAY,cgcharnumsoffset,LSEEK_SET);
    status = _lread(FONTDISPLAY,(LPSTR)cgcharnums,2 * MAX_CHARS + 4);

    if(status <= 0) {
#if ERRORFILE
      fprintf(errfile,"Error reading GLOBALCHDATA segment.\n");
#endif
      return(ld_err_9);
    }

    fontslant = globalchdata->fontSlant; // need for rasterparam segment

    //  setup face header structure including all entries but keeping
    //  count of the number of actual characters in library
    faceheaderptr = (faceheader_type far *)faceheader;
    charptr = faceheaderptr->chars;
    offsets = (LONG far *)globalchdata->offsets;
    nchar_in_font = globalchdata->NCHAR;
    nchar_in_lib = 0;
    
    for(i=0; i<nchar_in_font; i++) {
      //  get size of ith character
      nextoffset = offsets+1;
      while(*nextoffset == -1)nextoffset++;
      charptr->size = (UWORD)(*nextoffset - *offsets);

      //  get offset of ith character;
      charptr->offset = *offsets;

      //  if this is a valid character add cgnumber otherwise
      //  set cgnumber to MAX_UWORD
      
      //  character is invalid if it is too large, too small or 
      //  has a negative cgnumber (PI character)
      if(charptr->size >= MIN_CHAR_SIZE &&
         charptr->size <= MAX_CHAR_SIZE &&
         (*cgcharnums&0x8000) == 0 ) {
         // this is a valid character
         nchar_in_lib++;
         charptr->cg_num = *cgcharnums;
      }
      else {
#if ERRORFILE
         if( (*cgcharnums&0x8000) == 0) 
           fprintf(errfile," Missing character number %d\n",*cgcharnums);
#endif
         charptr->cg_num = MAX_UWORD;
      }

      //  increment pointers for next time around
      charptr++;
      cgcharnums++;
      offsets++;
    }
    

  return(OK);
}






/*
**  Process 1.2.5  Library Header Setup
*/
GLOBAL WORD
library_header_setup(name)
LPSTR name;
{

   UWORD i,seg_size,file_dir_size;
   WORD far *libptr;
   seg_dir_type far *seg_dir;
   file_dir_type far *file_dir;
   UWORD num_faces;
   LPSTR ptr;

   if(disk) {
     i = 1;
     seg_size = DSK_LIB_DIR_SIZE;
     type = 'D';
   }
   else {
     i = 4;
     seg_size = RES_LIB_DIR_SIZE;
   }
   libptr = (WORD far *)buffer;

   if( myaccess(name,0) == -1 ) {

     /*  new file */
     LIB = _lcreat((LPSTR)name, 0 /*normal file attribute*/);
     lmemset((LPSTR)libptr,0,FIL_SEG_DIR_SIZE + seg_size + seg_size);

     *libptr = type;          // type of library
     libptr += i;             // skip byte count and checksum for RAM lib

     *libptr++ = fileheader_key; // key for file header segment
     *((ULONG far *)libptr)++ = FIL_SEG_DIR_SIZE; // offset to file header segment
     libptr++;                // leave size 0 for face header

     *libptr++ = filedirectory_key; // key for file directory segment
     *((ULONG far *)libptr)++ = FIL_SEG_DIR_SIZE + seg_size;
                              // offset to file directory
     *libptr++ = 20;          // size of file directory
     *libptr = endofdir_key;

     libptr = (WORD far *)(buffer + FIL_SEG_DIR_SIZE + seg_size);
                              // File directory segment
     *((ULONG far *)libptr)++ = typeface;  // set typeface
     *((ULONG far *)libptr)++ = (ULONG)(FIL_SEG_DIR_SIZE + seg_size + seg_size);
                              // offset to face header
     *libptr++ = faceheadersize = 6 + 8 * (nchar_in_lib + 1);
   
     *(ULONG far *)libptr = endofdir_key;
     status = _lwrite(LIB,buffer,FIL_SEG_DIR_SIZE + seg_size + seg_size);
     if( status != FIL_SEG_DIR_SIZE + seg_size + seg_size) {
#if ERRORFILE
         fprintf(errfile,"Error Writing %s, may be disk full.\n",libName);
#endif
         return(ld_err_26);   //  write headers to file
     }
   }
   else {

     /*  existing file */
     LIB = _lopen((LPSTR)name,OF_READ);
     status = _llseek(LIB,0L,LSEEK_SET);
     status = _lread(LIB,(LPSTR)libptr,FIL_SEG_DIR_SIZE + seg_size + seg_size);

     if(status != FIL_SEG_DIR_SIZE + seg_size + seg_size) {
#if ERRORFILE
         fprintf(errfile,"Error Reading %s.\n",libName);
#endif
         return(ld_err_22);   //  read header information
     }

     if( *libptr != type) {
#if ERRORFILE
         fprintf(errfile,"Trying to add to existing file of a different type\n");
#endif
         return(ld_err_22);   //  make sure types match
     }

     seg_dir = (seg_dir_type far *)(libptr + i);

     while( seg_dir->key != filedirectory_key) seg_dir++;  // locate file directory

     ptr = (LPSTR)libptr + seg_dir->offset;
     file_dir = (file_dir_type far *)ptr;

     num_faces = 1;   // count end of dir
     while(file_dir->face != endofdir_key) {
       file_dir++;
       num_faces++;
     }

     file_dir_size = num_faces * sizeof(file_dir_type);

     if( file_dir_size + sizeof(file_dir_type) > seg_size) {
#if ERRORFILE
        fprintf(errfile,"File directory space exhausted\n");
#endif
        return(ld_err_23);    // make sure there is room for new face
     }

     file_dir = (file_dir_type far *)ptr;
     while( file_dir->face < typeface )file_dir++;

     if(file_dir->face == typeface) {
#if ERRORFILE
        fprintf(errfile,"Typeface already in library\n");
#endif
        return(ld_err_16); // This version of the Loader 
     }

     ptr += file_dir_size;

     while(ptr > (LPSTR)file_dir) {
       lmemcpy(ptr,ptr-sizeof(file_dir_type),sizeof(file_dir_type));
       ptr -= sizeof(file_dir_type);
     }

     file_dir->face = typeface;
     file_dir->offset = align((ULONG)myfilelength(LIB),alignment);
     file_dir->size = faceheadersize = 6 + 8 * nchar_in_lib;
       
     MyRewind(LIB);
     if( _lwrite(LIB,buffer,FIL_SEG_DIR_SIZE + seg_size + seg_size) !=
	FIL_SEG_DIR_SIZE + seg_size + seg_size) {
#if ERRORFILE
         fprintf(errfile,"Writing %s, may be disk full. ",libName);
#endif
         return(ld_err_26);   //  write headers back to file
     }
     status = _llseek(LIB,file_dir->offset,LSEEK_SET);

   }

   return(OK);
}     


GLOBAL WORD
global_segment_setup()
{

   ULONG displayheaderoffset = MAX_LONG,displayheadersize;
   ULONG fontheaderoffset = MAX_LONG,fontheadersize;
   ULONG charwidthoffset = MAX_LONG,charwidthsize;
   ULONG attributeheaderoffset = MAX_LONG,attributeheadersize;
   ULONG trackkernoffset = MAX_LONG,trackkernsize;
   ULONG textkernoffset = MAX_LONG,textkernsize;
   ULONG designkernoffset = MAX_LONG,designkernsize;
   ULONG typefaceheaderoffset = MAX_LONG,typefaceheadersize;
   ULONG compoundcharoffset = MAX_LONG,compoundcharsize;
   ULONG ccharmetricsoffset = MAX_LONG,ccharmetricssize;
   ULONG ccidoffset,ccidsize = MAX_LONG;
   ULONG complementheaderoffset = MAX_LONG,complementheadersize;
   ULONG fontaliasoffset = MAX_LONG,fontaliassize;

   UWORD numsegments;
   ULONG offset;
   LPSTR ptr;
   seg_dir_type far *seg_dir;
   keyoffset_type far *keyptr;
   faceglobal_type far *faceglobal;
   compoundchar_type far *compoundchar;
   complementheader_type far *complementheader;
   ccharmetrics_type far *ccharmetrics;
   ccid_type far *ccid;
   
   UWORD NYLN,NGCD,NGYD,NGXD,NSDM,NFATS,NXREF;
   UWORD i;

   DBGglobalseg(("Proc Entry: global_segment_setup\n"));

   //  get number of segments and offsets to segments
   numsegments = 2;   // always have rasterparam segment and end of dir
   if(globintdataoffset != MAX_LONG)
     numsegments++;

   // displayheader is in display file
    keyptr = (keyoffset_type far *)(((fileheader_type far *)dfileheader)->keyOffsets);
    for(i=0; i<((fileheader_type far *)dfileheader)->NKEYS; i++) {
      if(keyptr->key == DISPLAYHEADER)  {
         displayheaderoffset = keyptr->offset;
         numsegments++;
         break;
      }
      keyptr++;
    }

    //  all the other segments are in the attribute file
    //  read attribute file header
    status = _lread(FONTATTRIBUTE,afileheader,MAX_HEADER);
    if(status <= 0) {
#if ERRORFILE
      fprintf(errfile,"Error reading Attribute file.\n");
#endif
      return(ld_err_11);
    }

    //  locate segments
    keyptr = (keyoffset_type far *)(((fileheader_type far *)afileheader)->keyOffsets);
    if(((fileheader_type far *)afileheader)->NKEYS > MAX_KEYS)
      ((fileheader_type far *)afileheader)->NKEYS = MAX_KEYS;
    for(i=0; i<((fileheader_type far *)afileheader)->NKEYS; i++) {
       switch ( keyptr->key ) {

         case FONTHEADER:
           fontheaderoffset = keyptr->offset;
           fontheadersize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case CHARWIDTH:
           charwidthoffset = keyptr->offset;
           charwidthsize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case ATTRIBUTEHEADER:
           attributeheaderoffset = keyptr->offset;
           attributeheadersize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case TRACKKERN:
           trackkernoffset = keyptr->offset;
           trackkernsize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case TEXTKERN:
           textkernoffset = keyptr->offset;
           textkernsize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case DESIGNKERN:
           designkernoffset = keyptr->offset;
           designkernsize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case TYPEFACEHEADER:
           typefaceheaderoffset = keyptr->offset;
           typefaceheadersize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case COMPOUNDCHAR:
           compoundcharoffset = keyptr->offset;
           compoundcharsize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

         case CCHARMETRICS:
           ccharmetricsoffset = keyptr->offset;
           ccharmetricssize = (keyptr+1)->offset - keyptr->offset;
           break;

         case CCID:
           ccidoffset = keyptr->offset;
           ccidsize = (keyptr+1)->offset - keyptr->offset;
           break;

         case COMPLEMENTHEADER:
           complementheaderoffset = keyptr->offset;
           complementheadersize = (keyptr+1)->offset - keyptr->offset;
           break;

         case FONTALIAS:
           fontaliasoffset = keyptr->offset;
           fontaliassize = (keyptr+1)->offset - keyptr->offset;
           numsegments++;
           break;

       }
       keyptr++;
    }

    if(fontheaderoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Font Header Segment not found.\n");
#endif
      return(ld_err_13);
    }
    if(charwidthoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Character Width Segment not found.\n");
#endif
      return(ld_err_15);
    }
    if(attributeheaderoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Attribute Header Segment not found.\n");
#endif
      return(ld_wrn_4);
    }
    if(typefaceheaderoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Typeface Header Segment not found.\n");
#endif
      return(ld_wrn_8);
    }
    if(displayheaderoffset == MAX_LONG) {
#if ERRORFILE
      fprintf(errfile,"Display Header Segment not found.\n");
#endif
      return(ld_wrn_1);
    }

#if ERRORFILE
    if(trackkernoffset == MAX_LONG)
      fprintf(errfile,"Track Kerning Segment not found.\n");
    if(textkernoffset == MAX_LONG)
      fprintf(errfile,"Text Kerning Segment not found.\n");
    if(designkernoffset == MAX_LONG)
      fprintf(errfile,"Design Kerning Segment not found.\n");
    if(compoundcharoffset == MAX_LONG)
      fprintf(errfile,"Compound Character Segment not found.\n");
    if(ccharmetricsoffset == MAX_LONG)
      fprintf(errfile,"Compound Character Metrics Segment not found.\n");
    if(ccidoffset == MAX_LONG)
      fprintf(errfile,"Compound Character ID Segment not found.\n");
    if(complementheaderoffset == MAX_LONG)
      fprintf(errfile,"Complement Header Segment not found.\n");
    if(fontaliasoffset == MAX_LONG)
      fprintf(errfile,"Font Alias Segment not found.\n");
#endif




    faceglobal = (faceglobal_type far *)buffer;

    seg_dir = &faceglobal->seg_dir;

    offset = align(numsegments * (LONG)sizeof(seg_dir_type) + 6, alignment);

    ptr = (LPSTR)faceglobal + offset;
    if(offset + COPYRIGHTSIZE > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    //  add copyright
    seg_dir->key = copyright_key;
    seg_dir->size = COPYRIGHTSIZE;
    seg_dir->offset = offset;

    status = _llseek(FONTATTRIBUTE,fontheaderoffset,LSEEK_SET);
    status = _lread(FONTATTRIBUTE,ptr,COPYRIGHTSIZE);
    if(status != COPYRIGHTSIZE) {
#if ERRORFILE
       fprintf(errfile,"Error reading Attribute file. \n");
#endif
       return(ld_err_11);
    }

    
    seg_dir++;
    offset += align((ULONG)COPYRIGHTSIZE,alignment);
    ptr += align((ULONG)COPYRIGHTSIZE,alignment);


    // add global intellifont segment if it exists
    if(globintdataoffset != MAX_LONG) {

       if(offset + globintdatasize > MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       status = _llseek(FONTDISPLAY,globintdataoffset,LSEEK_SET);
       status = _lread(FONTDISPLAY,ptr,(UWORD)globintdatasize);
       if(status != (WORD)globintdatasize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

       // get actual size of global intellifont data
       NYLN = *(UWORD far *)ptr;
       ptr += NYLN*2+2;
       NGCD = *(UWORD far *)ptr;
       ptr += NGCD*2+2;
       NGYD = *(UWORD far *)ptr;
       ptr += NGYD*3+2+(NGYD&1);
       NGXD = *(UWORD far *)ptr;
       ptr += NGXD*3+2+(NGXD&1);
       NSDM = *(UWORD far *)ptr;
       //  NSDM is now used as orThreshold, assume NSDM <= 10 is an
       //   old font and NSDM > 10 is a new font 
       if (NSDM <= 10 ) {
          /*  old style font, need to convert to new style */
          *(UWORD far *)ptr = 210;
          // set orThreshold if no value was entered
          if(orThreshold == 0)
             orThreshold = 210;
          if(NSDM > 0 ) {
          /*  remove std dimensions */
          /*  only copy 4 bytes at a time since lmemcpy can
              clobber data when moving backwards in some systems*/
             lmemcpy(ptr+4,ptr+4+NSDM*4,4);
             lmemcpy(ptr+8,ptr+8+NSDM*4,4);
             lmemcpy(ptr+12,ptr+12+NSDM*4,4);
          }
       }
       else {
          // set orThreshold if no value was entered
          if(orThreshold == 0)
             orThreshold = NSDM;
       }
       
       seg_dir->key = globalintellifont_key;
       seg_dir->size = NYLN*2 + NGCD*2 + NGYD*3 + NGXD*3 
                   + 24 + (NGYD&1) + (NGXD&1);
       seg_dir->offset = offset;

       offset += align((ULONG)seg_dir->size,alignment);
       seg_dir++;
       ptr = buffer + offset;

    }

    //  read charwidth segment into table

    if(offset + nchar_in_lib * sizeof(charwidth_type) + 6 > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    *((UWORD far *)ptr)++ = charwidth_key;
    *((LONG far *)ptr)++ = (LONG)(nchar_in_lib * sizeof(charwidth_type) + 6);

    status = _llseek(FONTATTRIBUTE,charwidthoffset,LSEEK_SET);
    status = _lread(FONTATTRIBUTE,ptr,(UWORD)charwidthsize);
    if(status != (WORD)charwidthsize) {
#if ERRORFILE
       fprintf(errfile,"Error reading Attribute file. \n");
#endif
       return(ld_err_11);
    }


    //  need to read in textkern and designkern segments and
    //  sort them with the face header to insure that the
    //  characters are in ascending order of cgnumber and that
    //  the charwiths and kerning data stays in the same order.

    if(textkernoffset != MAX_LONG) {
       status = _llseek(FONTATTRIBUTE,textkernoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,textkern,(UWORD)textkernsize);
       if(status != (WORD)textkernsize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

    }

    if(designkernoffset != MAX_LONG) {
       status = _llseek(FONTATTRIBUTE,designkernoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,designkern,(UWORD)designkernsize);
       if(status != (WORD)designkernsize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }
    }

    seg_dir->key = charwidth_key;
    seg_dir->size = nchar_in_lib * sizeof(charwidth_type) + 6;
    seg_dir->offset = offset;

    //  pack kerning from words into nibbles

    sort((charwidth_type far *)ptr,(unpackedkern_type far *)textkern,
         (unpackedkern_type far *)designkern);

    offset += align((ULONG)seg_dir->size,alignment);
    seg_dir++;
    ptr = buffer + offset;


    // add raster parameter segment

    if(offset + RASTERPARAMSIZE > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    seg_dir->offset = offset;
    *((UWORD far *)ptr)++ = seg_dir->key = rasterparam_key;
    *((LONG far *)ptr)++ = seg_dir->size = (LONG)RASTERPARAMSIZE;
    *((UWORD far *)ptr)++ = orThreshold;
    *((WORD far *)ptr)++ = fontslant;

    offset += align((ULONG)seg_dir->size,alignment);
    seg_dir++;
    ptr = buffer + offset;

    // add attribute header

    if(offset + attributeheadersize + 6 > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    *((UWORD far *)ptr)++ = seg_dir->key = attributeheader_key;
    *((LONG far *)ptr)++ = seg_dir->size = sizeof(attributeheader_type) + 6;
    seg_dir->offset = offset;

    status = _llseek(FONTATTRIBUTE,attributeheaderoffset,LSEEK_SET);
    status = _lread(FONTATTRIBUTE,ptr,seg_dir->size);
    if(status != seg_dir->size) {
#if ERRORFILE
       fprintf(errfile,"Error reading Attribute file. \n");
#endif
       return(ld_err_11);
    }

    offset += align((ULONG)seg_dir->size,alignment);
    seg_dir++;
    ptr = buffer + offset;
   
    //  add track kern if present
    if(trackkernoffset != MAX_LONG) {

       if(offset + trackkernsize + 6> MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       seg_dir->offset = offset;
       *((UWORD far *)ptr)++ = seg_dir->key = trackkern_key;
       
       status = _llseek(FONTATTRIBUTE,trackkernoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,ptr+4,(UWORD)trackkernsize);
       if(status != (WORD)trackkernsize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

       *(LONG far *)ptr = seg_dir->size = *(UWORD far *)ptr * 10 + 8;

       offset += align((ULONG)seg_dir->size,alignment);
       seg_dir++;
       ptr = buffer + offset;
    }

    //  add text kern  
       if(textkernoffset != MAX_LONG) {

       if(offset + nchar_in_lib * 4 + 6 + 6 > MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       *((UWORD far *)ptr)++ = seg_dir->key = textkern_key;
       *((LONG far *)ptr)++ = seg_dir->size = nchar_in_lib * 4 + 6 + 6;
       seg_dir->offset = offset;
      
       // kerning has already been sorted, need to pack it 
       pack_kerning((packedkern_type far *)ptr,(unpackedkern_type far *)textkern);

       offset += align((ULONG)seg_dir->size,alignment);
       seg_dir++;
       ptr = buffer + offset;

    }

    //  add design kern if present
       if(designkernoffset != MAX_LONG) {

       if(offset + nchar_in_lib * 4 + 6 + 6 > MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       *((UWORD far *)ptr)++ = seg_dir->key = designkern_key;
       *((LONG far *)ptr)++ = seg_dir->size = nchar_in_lib * 4 + 6 + 6;
       seg_dir->offset = offset;

       // kerning has already been sorted, need to pack it 
       pack_kerning((packedkern_type far *)ptr,(unpackedkern_type far *)designkern);

       offset += align((ULONG)seg_dir->size,alignment);
       seg_dir++;
       ptr = buffer + offset;

    }

    //  add typeface header

    if(offset + typefaceheadersize + 6 > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    *((UWORD far *)ptr)++ = seg_dir->key = typefaceheader_key;
    seg_dir->offset = offset;

    status = _llseek(FONTATTRIBUTE,typefaceheaderoffset,LSEEK_SET);
    status = _lread(FONTATTRIBUTE,ptr+4,(UWORD)typefaceheadersize);
    if(status != (WORD)typefaceheadersize) {
#if ERRORFILE
       fprintf(errfile,"Error reading Attribute file. \n");
#endif
       return(ld_err_11);
    }

    *(LONG far *)ptr = seg_dir->size = *(UWORD far *)(ptr+4) * 12 + 92 + 6;

    offset += align((ULONG)seg_dir->size,alignment);
    seg_dir++;
    ptr = buffer + offset;

    // get compound character segment
    // need to assemble it from many parts

    if(compoundcharoffset != MAX_LONG &&
       ccharmetricsoffset != MAX_LONG &&
       ccidoffset != MAX_LONG &&
       complementheaderoffset != MAX_LONG) {
     
       // estimate size of final cchar buffer as 1.25 * cchar size in fais
       if(offset + compoundcharsize + (compoundcharsize >> 2) > MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       // reuse kerning buffers
       if(compoundcharsize > 16 * MAX_CHARS + 6) {
#if ERRORFILE
         fprintf(errfile," Error compound char Segment too big.\n");
#endif
         return(ld_err_mem);
       }

       if(complementheadersize + ccharmetricssize + ccidsize 
                    > 16 * MAX_CHARS + 6) {
#if ERRORFILE
         fprintf(errfile," Error compound char Segment too big.\n");
#endif
         return(ld_err_mem);
       }

       compoundchar = (compoundchar_type far *)textkern;
       complementheader = (complementheader_type far *)designkern;
       ccharmetrics = (ccharmetrics_type far *)((LPSTR)designkern 
                                   + complementheadersize);
       ccid = (ccid_type far *)((LPSTR)ccharmetrics + ccharmetricssize);

       status = _llseek(FONTATTRIBUTE,compoundcharoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,(LPSTR)compoundchar,(UWORD)compoundcharsize);
       if(status != (WORD)compoundcharsize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }         

       status = _llseek(FONTATTRIBUTE,complementheaderoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,(LPSTR)complementheader,(UWORD)complementheadersize);
       if(status != (WORD)complementheadersize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

       status = _llseek(FONTATTRIBUTE,ccharmetricsoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,(LPSTR)ccharmetrics,(UWORD)ccharmetricssize);
       if(status != (WORD)ccharmetricssize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

       status = _llseek(FONTATTRIBUTE,ccidoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,(LPSTR)ccid,(UWORD)ccidsize);
       if(status != (WORD)ccidsize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }
 
       seg_dir->key = *((UWORD far *)ptr)++ = compoundchar_key;
       seg_dir->offset = offset;
       seg_dir->size = (UWORD)(*(LONG far *)ptr = 6 +
           assemblecompoundchar((WORD far *)(ptr+4),(WORD far *)compoundchar,
              complementheader,ccharmetrics,ccid) );

       offset += align((ULONG)seg_dir->size,alignment);
       seg_dir++;
       ptr = buffer + offset;
    }

    // add display header
    if(offset + sizeof(displayheader_type) + 6 > MAX_BUFFER) {
#if ERRORFILE
      fprintf(errfile,"Error buffer overflow.");
#endif
      return(ld_err_mem);
    }

    *((UWORD far *)ptr)++ = seg_dir->key = displayheader_key;
    *((LONG far *)ptr)++ = seg_dir->size = sizeof(displayheader_type) + 6;
    seg_dir->offset = offset;

    status = _llseek(FONTDISPLAY,displayheaderoffset,LSEEK_SET);
    status = _lread(FONTDISPLAY,ptr,seg_dir->size);
    if(status != seg_dir->size) {
#if ERRORFILE
       fprintf(errfile,"Error reading Attribute file. \n");
#endif
       return(ld_err_11);
    }


    offset += align((ULONG)seg_dir->size,alignment);
    seg_dir++;
    ptr = buffer + offset;

     // add font alias table
    if(fontaliasoffset != MAX_LONG) {
   
       if(offset + fontaliassize + 6 > MAX_BUFFER) {
#if ERRORFILE
         fprintf(errfile,"Error buffer overflow.");
#endif
         return(ld_err_mem);
       }

       seg_dir->key = fontalias_key;
       seg_dir->offset = offset;
       ptr += 6;

       status = _llseek(FONTATTRIBUTE,fontaliasoffset,LSEEK_SET);
       status = _lread(FONTATTRIBUTE,ptr,(UWORD)fontaliassize);
       if(status != (WORD)fontaliassize) {
#if ERRORFILE
          fprintf(errfile,"Error reading Attribute file. \n");
#endif
          return(ld_err_11);
       }

       seg_dir->size = 6;

       // count length of segment
       NFATS = *(UWORD far *)(ptr);
       seg_dir->size += 2;
       ptr += 22;
       for(i=0; i<NFATS; i++)
       {
           NXREF = *(UWORD far *)ptr;
           seg_dir->size += NXREF*104 + 22;
           ptr += NXREF*104 + 22;
       }

       // reset pointer to begining of fontalias segment
       ptr = buffer + offset;
       seg_dir->key = *((UWORD far *)ptr)++ = fontalias_key;
       *(ULONG far *)ptr = seg_dir->size;
       seg_dir->offset = offset;
       
       // offset is the size of the face global segment
       offset += align((ULONG)seg_dir->size,alignment);


    }
    seg_dir++;
    seg_dir->key = endofdir_key;
    seg_dir->size = 0;
    seg_dir->offset = (LONG)0;


    // face global immediately follows faceheader
    ((faceheader_type far *)faceheader)->faceglobal_size = (UWORD)offset;
    faceheaderoffset = myftell(LIB);
    faceheadersize = (UWORD)align((ULONG)faceheadersize,alignment);
    ((faceheader_type far *)faceheader)->faceglobal_offset = 
                   faceheaderoffset + (LONG)faceheadersize;

    // write face header
    status = _lwrite(LIB,faceheader,faceheadersize);
    if(status != faceheadersize) {
#if ERRORFILE
       fprintf(errfile,"Error writing to Library file. Disk may be full\n");
#endif
       return(ld_err_26);
    }


    faceglobal->key = faceglobal_key;
    faceglobal->size = offset;
    // write face global
    status = _lwrite(LIB,buffer,(UWORD)offset);
    if(status != (WORD)offset) {
#if ERRORFILE
       fprintf(errfile,"Error writing to Library file. Disk may be full\n");
#endif
       return(ld_err_26);
    }
    return(OK);

}

GLOBAL WORD
write_characters()
{

  char_type far *charptr;
  UWORD i,charsize;

  charptr = ((faceheader_type far *)faceheader)->chars;

  for (i=0; i<nchar_in_lib; i++) {



    status = _llseek(FONTDISPLAY,charptr->offset + hiqdataoffset,LSEEK_SET);
    status = _lread(FONTDISPLAY,buffer,charptr->size);
    if(status != charptr->size) {
#if ERRORFILE
       fprintf(errfile,"Error reading characters.\n");
#endif
       return(ld_err_21);
    }

    *(UWORD far *)buffer = charptr->size;

    charptr->offset = myftell(LIB);
    charsize = (UWORD)align((LONG)charptr->size,alignment);

    status = _lwrite(LIB,buffer,charsize);
    if(status != charsize) {
#if ERRORFILE
       fprintf(errfile,"Error writing to Library file. Disk may be full\n");
#endif
       return(ld_err_26);
    }
    charptr++;
  }
    
  // rewrite face header with new offsets
  status = _llseek(LIB,faceheaderoffset,LSEEK_SET);
  status = _lwrite(LIB,faceheader,faceheadersize);
  if(status != faceheadersize) {
#if ERRORFILE
     fprintf(errfile,"Error writing to Library file.\n");
#endif
     return(ld_err_26);
  }

  return(OK);


}
GLOBAL WORD
check_sum(name)
LPSTR name;
{
  UWORD j,status;
  UWORD checksum;

  if(!disk){
    checksum = 0;
    MyRewind(LIB);
    _lread(LIB,(LPSTR)&j,2);
    _lread(LIB,(LPSTR)&j,2);
    _lread(LIB,(LPSTR)&j,2);
    _lread(LIB,(LPSTR)&j,2);

/* rcm:windowized change */
#ifdef NODEF
    while(!feof(LIB)){
      if((status=_lread(LIB,(LPSTR)&j,2))==1){
        checksum += j;
#else
    status = 2;
    while(status==2){
      if((status=_lread(LIB,(LPSTR)&j,2))==2){
        checksum += j;
#endif
      }
    }
    MyRewind(LIB);
    _lread(LIB,buffer,512);
    *(LONG far *)(buffer+2) = myfilelength(LIB);
    *(UWORD far *)(buffer+6) = checksum;
    MyRewind(LIB);
    if((_lwrite(LIB,buffer,512)) != 512) {
#if ERRORFILE
       fprintf(errfile,"Error writing to Library file. Disk may be full\n");
#endif
       return(ld_err_26);
    }

  }

  // close files
  _lclose(LIB);
  _lclose(FONTATTRIBUTE);
  _lclose(FONTDISPLAY);
  return(OK);
}
        
     
/*
**  return n aligned to next mod boundary
*/
MLOCAL ULONG 
align(n,mod)
ULONG n;
UWORD mod;
{
  ULONG x;
  if(n%mod == 0) x=n;
  else x=(n + mod - n%mod);
  return(x);
}
     
MLOCAL VOID
sort(charwidth,textkernptr,designkernptr)
  charwidth_type far *charwidth;
  unpackedkern_type far *textkernptr;
  unpackedkern_type far *designkernptr;
  
{
  char_type far *charptr,far *lowcharptr,far *swpcharptr;
  unpkerndata_type far *textptr,far *lowtextptr,far *swptextptr;
  unpkerndata_type far *designptr,far *lowdesignptr,far *swpdesignptr;
  charwidth_type far *charwidthptr,far *lowcharwidthptr,far *swpcharwidthptr;
  charwidth_type charwidthbuf;
  char_type charbuf;
  UWORD i,j,k,temp;
  UWORD far *ptr1,far *ptr2;
  

  //  sort on cg_num in faceheader in ascending order
  swpcharptr = ((faceheader_type far *)faceheader)->chars;
  swptextptr = textkernptr->character;
  swpdesignptr = designkernptr->character;
  swpcharwidthptr = charwidth;

  for(i=0; i<nchar_in_font-1; i++) {
    lowcharptr      = swpcharptr;
    lowtextptr      = swptextptr;
    lowdesignptr    = swpdesignptr;
    lowcharwidthptr = swpcharwidthptr;

    charptr = lowcharptr+1;
    textptr = lowtextptr+1;
    designptr = lowdesignptr+1;
    charwidthptr = lowcharwidthptr+1;

    for(j=i+1; j<nchar_in_font; j++) {


      if( charptr->cg_num < lowcharptr->cg_num ) {
       lowcharptr = charptr;
       lowtextptr = textptr;
       lowdesignptr = designptr;
       lowcharwidthptr = charwidthptr;
       
      }
      charptr++;
      charwidthptr++;
      textptr++;
      designptr++;
    }

    if(lowcharptr != swpcharptr) {
      //  need to swap all four blocks
      charbuf.cg_num = swpcharptr->cg_num;
      charbuf.offset = swpcharptr->offset;
      charbuf.size   = swpcharptr->size;
      swpcharptr->cg_num = lowcharptr->cg_num;
      swpcharptr->offset = lowcharptr->offset;
      swpcharptr->size   = lowcharptr->size;
      lowcharptr->cg_num = charbuf.cg_num;
      lowcharptr->offset = charbuf.offset;
      lowcharptr->size   = charbuf.size;


      charwidthbuf.charWidth = swpcharwidthptr->charWidth;
      charwidthbuf.charFlags = swpcharwidthptr->charFlags;
      swpcharwidthptr->charWidth = lowcharwidthptr->charWidth;
      swpcharwidthptr->charFlags = lowcharwidthptr->charFlags;
      lowcharwidthptr->charWidth = charwidthbuf.charWidth;
      lowcharwidthptr->charFlags = charwidthbuf.charFlags;

      ptr1 = (UWORD far *)swptextptr;
      ptr2 = (UWORD far *)lowtextptr;
      for(k=0; k<8; k++) {
        temp = *ptr1;
        *ptr1++ = *ptr2;
        *ptr2++ = temp;
      }

      ptr1 = (UWORD far *)swpdesignptr;
      ptr2 = (UWORD far *)lowdesignptr;
      for(k=0; k<8; k++) {
        temp = *ptr1;
        *ptr1++ = *ptr2;
        *ptr2++ = temp;
      }
    }

           

    swpcharptr++;
    swpcharwidthptr++;
    swptextptr++;
    swpdesignptr++;
  }
  //  set end of character 
  charptr = (char_type far *)((LPSTR)(((faceheader_type far *)faceheader)->chars)
            + nchar_in_lib * sizeof(char_type));
  charptr->cg_num = endofdir_key;
  
}

MLOCAL VOID
pack_kerning(packed,unpacked)
  packedkern_type far *packed;
  unpackedkern_type far *unpacked;
{
  UWORD i,j,k,m1,m2;

  for(i=0;i<nchar_in_lib;i++)
  {

       //  copy header information
       packed->kernSign = unpacked->kernSign;
       packed->kernUnit = unpacked->kernUnit;
       packed->NSECT    = unpacked->NSECT;

       for(j=k=0;j<8;j+=2,k++)
       {
           m1 = (unpacked->character[i].data[j]<<4)&0x00F0;
           m2 = (unpacked->character[i].data[j+1])&0x000F;
           m1 = m1 + m2;
           packed->character[i].data[k] = (UBYTE)m1;
       }
  }
}

MLOCAL WORD
assemblecompoundchar(libcompoundchar,compoundchar,complementheader,
                     ccharmetrics,ccid)
   WORD far *libcompoundchar;
   WORD far *compoundchar;
   complementheader_type far *complementheader;
   ccharmetrics_type far *ccharmetrics;
   ccid_type far *ccid;
{

  libcompoundchar_type far *libccharptr;
  compoundchar_type far *ccharptr;
  ccharmetrics_type far *ccmetricsptr;
  ccid_type far *ccidptr;
  isocodetable_type far *tableptr;
  libcompoundchar_type far *ptr,far *nextptr,far *tempptr;
  part_type far *partptr;
  libpart_type far *libpartptr,far *nextpartptr,far *tpartptr;
  BYTE tempbuffer[68];
  UWORD compoundcharsize;
  UWORD i,j,k;
  UWORD NCCHAR;
  UWORD numparts;

  // first word of compoundchar and libcompoundchar is cchar count
  NCCHAR = *libcompoundchar = *compoundchar;

  libccharptr = (libcompoundchar_type far *)(libcompoundchar + 1);
  ccharptr = (compoundchar_type far *)(compoundchar + 1);
  ccmetricsptr = (ccharmetrics_type far *)ccharmetrics;
  ccidptr = (ccid_type far *)ccid;
  tempptr = (libcompoundchar_type far *)tempbuffer;
  tableptr = complementheader->table;
  compoundcharsize = 2;  // include the character count

  // copy entries into libcompoundchar format
  for(i=0; i<NCCHAR; i++) {
    libccharptr->cg_num = ccidptr->cg_num;
    libccharptr->horiz_esc = ccmetricsptr->escapementX;
    libccharptr->vert_esc  = ccmetricsptr->escapementY;
    libccharptr->NPCC = ccharptr->NPCC;
    compoundcharsize += 8 + sizeof(libpart_type) * ccharptr->NPCC;
    partptr = ccharptr->parts;
    libpartptr = libccharptr->parts;
    
    for(j=0; j<ccharptr->NPCC; j++) {
      libpartptr->cg_num = code_to_cgnum(partptr->ccCharCode,tableptr);
      libpartptr->xoffset = partptr->offsets.x;
      libpartptr->yoffset = partptr->offsets.y;
      partptr++;
      libpartptr++;
    }

    libccharptr = (libcompoundchar_type far *)((BYTE far *)libccharptr + 
                  8 + sizeof(libpart_type) * libccharptr->NPCC);  
    ccharptr = (compoundchar_type far *)((BYTE far *)ccharptr +
                  4 + sizeof(part_type) * ccharptr->NPCC);  
    ccmetricsptr++;
    ccidptr++;
  }

  // sort the libcompoundchar structure by increasing cg numbers
  for(i=0; i<NCCHAR-1; i++) {

    ptr = (libcompoundchar_type far *)(libcompoundchar + 1);

    for(j=0; j<NCCHAR-1; j++) {

      nextptr = (libcompoundchar_type far *)((BYTE far *)ptr +
                 8 + sizeof(libpart_type)*ptr->NPCC);  
      if(ptr->cg_num > nextptr->cg_num) {

        tempptr->cg_num = ptr->cg_num;
        tempptr->horiz_esc = ptr->horiz_esc;
        tempptr->vert_esc = ptr->vert_esc;
        tempptr->NPCC = ptr->NPCC;

        tpartptr = tempptr->parts;
        libpartptr = ptr->parts;
        for(k=0; k<ptr->NPCC; k++) {
          tpartptr->cg_num = libpartptr->cg_num;
          tpartptr->xoffset = libpartptr->xoffset;
          tpartptr->yoffset = libpartptr->yoffset;
          tpartptr++;
          libpartptr++;
        } 
     
        ptr->cg_num = nextptr->cg_num;
        ptr->horiz_esc = nextptr->horiz_esc;
        ptr->vert_esc = nextptr->vert_esc;
        ptr->NPCC = nextptr->NPCC;

        libpartptr = ptr->parts;
        nextpartptr = nextptr->parts;
        numparts = nextptr->NPCC;
        for(k=0; k<numparts; k++) {
          libpartptr->cg_num = nextpartptr->cg_num;
          libpartptr->xoffset = nextpartptr->xoffset;
          libpartptr->yoffset = nextpartptr->yoffset;
          libpartptr++;
          nextpartptr++;
        } 

        nextptr = (libcompoundchar_type far *)((BYTE far *)ptr +
                   8 + sizeof(libpart_type) * ptr->NPCC);  

        nextptr->cg_num = tempptr->cg_num;
        nextptr->horiz_esc = tempptr->horiz_esc;
        nextptr->vert_esc = tempptr->vert_esc;
        nextptr->NPCC = tempptr->NPCC;

        libpartptr = nextptr->parts;
        tpartptr = tempptr->parts;
        for(k=0; k<tempptr->NPCC; k++) {
          libpartptr->cg_num = tpartptr->cg_num;
          libpartptr->xoffset = tpartptr->xoffset;
          libpartptr->yoffset = tpartptr->yoffset;
          libpartptr++;
          tpartptr++;
        } 
      }
      ptr = nextptr;
    }
  }
  return(compoundcharsize);
}

MLOCAL WORD
code_to_cgnum(code,table)
  UWORD    code;
  isocodetable_type far *table;

{
  WORD   i;
  isocodetable_type far *tableptr;
  
  tableptr = table;

  for(i=0;i<nchar_in_font;i++){
    if(tableptr->isoCode == code ) 
        return(tableptr->cgCode);
    tableptr++;
  }
  return((UWORD)FALSE);
}
