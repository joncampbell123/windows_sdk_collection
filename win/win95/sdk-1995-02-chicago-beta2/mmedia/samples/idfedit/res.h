//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  res.h
//**
//**  DESCRIPTION:
//**
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

#ifndef __RES_H
#define __RES_H

#define  ICON_1               1
#define  ICON_2               2

//
// MENU DEFINITIONS
//
#define  IDFEDIT_MENU         1

#define  FILE_BASE            (10)
#define  IDM_FILENEW          (FILE_BASE)
#define  IDM_FILEOPEN         (FILE_BASE + 1)
#define  IDM_FILESAVE         (FILE_BASE + 2)
#define  IDM_FILESAVEAS       (FILE_BASE + 3)
#define  IDM_FILEEXIT         (FILE_BASE + 4)
#define  IDM_ABOUT            (FILE_BASE + 5)
#define  IDM_NEW_INSTRUMENT   (FILE_BASE + 6)

//
// STRING TABLE DEFINITIONS
//
#define  STRINGTABLE_BASE           (0)
#define  IDS_OFN_EXT_DEF            (STRINGTABLE_BASE + 1)
#define  IDS_OFN_EXT_FILTER         (STRINGTABLE_BASE + 2)
#define  IDS_DEF_FILE_NAME          (STRINGTABLE_BASE + 3)
#define  IDS_SAVE_CHANGES           (STRINGTABLE_BASE + 4)
#define  IDS_MESSAGE_BOX_TITLE      (STRINGTABLE_BASE + 5)
#define  IDS_CHANNEL                (STRINGTABLE_BASE + 6)
#define  IDS_CAPTION_BAR            (STRINGTABLE_BASE + 7)

#define  IDS_CHANNEL_TYPE_BASE      (STRINGTABLE_BASE + 10)
#define  IDS_CHANNEL_TYPE_GENERAL   (IDS_CHANNEL_TYPE_BASE + 0)
#define  IDS_CHANNEL_TYPE_DRUM      (IDS_CHANNEL_TYPE_BASE + 1)
#define  IDS_CHANNEL_TYPE_LIGHTING  (IDS_CHANNEL_TYPE_BASE + 2)

#define  IDS_YES                    (STRINGTABLE_BASE + 20)
#define  IDS_NO                     (STRINGTABLE_BASE + 21)

#define  IDS_GENERAL_INIT           (STRINGTABLE_BASE + 22)
#define  IDS_DRUM_INIT              (STRINGTABLE_BASE + 23)
//
// ERROR VALUES
//
#define  IDFERR_BASE                (STRINGTABLE_BASE + 50)
#define  IDFERR_BADREAD             (IDFERR_BASE + 0)
#define  IDFERR_NOMEM               (IDFERR_BASE + 1)
#define  IDFERR_BADCHUNK            (IDFERR_BASE + 2)
#define  IDFERR_CANNOTCREATEFILE    (IDFERR_BASE + 3)
#define  IDFERR_CANNOTFINDCHUNK     (IDFERR_BASE + 4)
#define  IDFERR_BADWRITE            (IDFERR_BASE + 5)
#define  IDFERR_BADNUMBER           (IDFERR_BASE + 6)
#define  IDFERR_CANNOTCREATECHUNK   (IDFERR_BASE + 7)

#define  IDS_BAD_HEX_INPUT          (IDFERR_BASE+30)

#define  IDS_SECTION_NAME_BASE   (STRINGTABLE_BASE + 100)

#define  IDS_TITLE_BAR_BASE      (STRINGTABLE_BASE + 150)
#define  IDS_TITLE_BAR_HEADER    (IDS_TITLE_BAR_BASE + 0)
#define  IDS_TITLE_BAR_INSTINFO  (IDS_TITLE_BAR_BASE + 1)
#define  IDS_TITLE_BAR_INSTCAPS  (IDS_TITLE_BAR_BASE + 2)
#define  IDS_TITLE_BAR_CHANNELS  (IDS_TITLE_BAR_BASE + 3)
#define  IDS_TITLE_BAR_PATCHES   (IDS_TITLE_BAR_BASE + 4)
#define  IDS_TITLE_BAR_KEYMAPS   (IDS_TITLE_BAR_BASE + 5)

#define  IDS_HEADER_BASE         (STRINGTABLE_BASE + 200)

#define  IDS_INST_BASE           (STRINGTABLE_BASE + 300)

#define  IDS_CAPS_BASE           (STRINGTABLE_BASE + 400)


#define  GENERAL_PATCH           (STRINGTABLE_BASE + 500)
#define  DRUM_KEY                (STRINGTABLE_BASE + 700)

// 
// DIALOG INCLUDE FILE's
//
#include "idfd.h"
#include "about.h"

#endif



