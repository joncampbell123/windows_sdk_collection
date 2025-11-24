// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:     mixline.h
//
//  PURPOSE:  Header file for mixline.c
//


TV_ITEM          tvi;                          // TreeView Item
TV_INSERTSTRUCT  tvins;                        // TreeView Insert Struct
HTREEITEM htiControl;
   
BOOL GetMixerControls(HWND hwnd, LPMIXERLINE lpMixerLine, HTREEITEM htiLine);
void ExpandChildren(HTREEITEM htiParent);
BOOL ItemHasChild(HTREEITEM);
