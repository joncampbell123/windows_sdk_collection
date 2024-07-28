;//**[f***************************************************************;***
;// * $dosutil.h;
;// *
;// * Copyright (C) 1988,1989 Aldus Corporation.
;// * Copyright (C) 1988-1990 Microsoft Corporation.
;// * Copyright (C) 1990,1991 Hewlett-Packard Company.
;// *     All rights reserved.
;// *     Company confidential.
;// *
;// **f]*****************************************************************/
  
;// History
  
;// 01 aug 89   peterbe     Use ';//' to begin comment-only lines
;//             (comments for both C and .ASM).
  
;// Mixed 'C' and assembly language header file for
;// dos interrupts implemented in assembly.
  
;/* Interesting return codes.
;
DOS_INVALPATH   equ 00003h
DOS_NOFILES equ 00012h
DOS_BADDRIVE    equ 0000Fh
  
if      0
; */
  
#define DOS_INVALPATH   0x03
#define DOS_NOFILES 0x12
#define DOS_BADDRIVE    0x0F
  
;/*
endif
; */
  
;/* structure used by _gtfree
;
DISKINFO struc  ;*/ typedef struct {                /*
  
    av_clusters     dw  0       ;*/         short int av_clusters;      /*
    tt_clusters     dw  0       ;*/         short int tt_clusters;      /*
    b_sector        dw  0       ;*/         short int b_sector;         /*
    s_cluster       dw  0       ;*/         short int s_cluster;        /*
  
DISKINFO ends       ;*/ } DISKINFO, FAR *LPDISKINFO /*
; */
  
  
;/* structures used by _opend and _readd
; *
; * DIRTIME (DIRectory TIME) structure
;
;*/     typedef struct {                        /*
    ;*/         WORD hh : 5;                        /*
    ;*/         WORD mm : 6;                        /*
    ;*/         WORD ss : 5;                        /*
;*/         } DIRTIME, FAR *LPDIRTIME;  /*
; */
  
;/* DIRDATE (DIRectory DATE) structure
;
;*/     typedef struct {                        /*
    ;*/         WORD yy : 7;                        /*
    ;*/         WORD mm : 4;                        /*
    ;*/         WORD dd : 5;                        /*
;*/         } DIRDATE, FAR *LPDIRDATE;  /*
; */
  
;/* DIRDATA (DIRectory DATA) structure
;
;*/     typedef struct {                        /*
    ;*/         BYTE fordos[21];                    /*
    ;*/         BYTE att;                       /*
    ;*/         DIRTIME time;                       /*
    ;*/         DIRDATE date;                       /*
    ;*/         DWORD size;                     /*
    ;*/         BYTE name[13];                      /*
;*/         } DIRDATA, FAR *LPDIRDATA;  /*
; */
  
;/* External definitions for 'C' files.
; */
; extern int FAR PASCAL dos_gtfree(BYTE, LPDISKINFO);
; extern int FAR PASCAL dos_opend(LPDIRDATA, LPSTR, WORD);
; extern int FAR PASCAL dos_readd(LPDIRDATA);
; extern int FAR PASCAL dos_delete(LPSTR);
; extern int FAR PASCAL dos_mkdir(LPSTR);
; extern int far pascal dos_write(short, LPSTR, WORD, WORD far *);
  
;/*  Definition of STDERR for the dos_write function
STDERR  equ 2
  
if      0
;*/
#define STDERR 2
;/*
endif
; */
