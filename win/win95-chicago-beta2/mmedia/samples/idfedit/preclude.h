//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  preclude.h
//**
//**  DESCRIPTION:
//**    Gets rid of un-needed definitions in system header files.
//**
//**  HISTORY:
//**     06/28/93          created.
//**
//************************************************************************


//
// MMSYSTEM precludes
//
 
#define     MMNOSOUND
#define     MMNOWAVE
#define     MMNOSEQ
#define     MMNOTIMER
#define     MMNOJOY
#define     MMNOMCI
#define     MMNOTASK

//
// MMDDK precludes
//
#define     MMNOWAVEDEV
#define     MMNOAUXDEV
#define     MMNOTIMERDEV
#define     MMNOJOYDEV
#define     MMNOMCIDEV
#define     MMNOTASKDEV

//
// WINDOWS precludes
//

#define     NOGDICAPMASKS        //- CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define     NOVIRTUALKEYCODES    //- VK_*
#define     NOKEYSTATES          //- MK_*
#define     NOSYSCOMMANDS        //- SC_*
#define     NORASTEROPS          //- Binary and Tertiary raster ops
#define     OEMRESOURCE          //- OEM Resource values
#define     NOCLIPBOARD          //- Clipboard routines
#define     NOMETAFILE           //- typedef METAFILEPICT
#define     NOSOUND              //- Sound driver routines
#define     NOWH                 //- SetWindowsHook and WH_*
#define     NOCOMM               //- COMM driver routines
#define     NOKANJI              //- Kanji support stuff.
#define     NOPROFILER           //- Profiler interface.


