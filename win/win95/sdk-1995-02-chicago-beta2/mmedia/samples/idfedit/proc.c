//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  proc.c
//**
//**  DESCRIPTION:
//**     This file contains most of the code to the UI and memory 
//**     allocation.
//**
//**  HISTORY:
//**     04/22/93       created.
//**     09/07/93       rewrote almost everything.
//**
//**  NOTES:
//**     Added LoadStrings() everywhere for internationalization (wow!).
//**     Yes performance gains could be gotten by loading the most used
//**     strings into memory and then they wouldn't have to be loaded 
//**     all of the time.  Just remember one thing.... This is not a time 
//**     critical application.  i.e. the user should not notice so why 
//**     do it?
//**
//************************************************************************

#include "globals.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mmreg.h>
#include "res.h"

//************************************************************************
//**
//**  AllocPtr();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     UINT  fu
//**     DWORD cb
//**
//**  RETURNS:
//**     LPVOID 
//**
//**  HISTORY:
//**     07/09/93       
//**
//************************************************************************

LPVOID FNLOCAL AllocPtr(
   UINT  fu, 
   DWORD cb)
{
   HGLOBAL  hg;
   LPVOID   pv;

   // Try to allocate the requested memory.
   //
   hg = GlobalAlloc(fu, cb);
   if (NULL != hg)
   {
      // Convert the handle to a pointer.
      //
      pv = GlobalLock(hg);
   }
   else
   {
      // We failed to allocate the memory so
      // return NULL.
      //
      pv = NULL;
   }

   return(pv);
} //** AllocPtr()


//*****************************************************************************
//**
//**  ReAllocPtr();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     LPVOID   pv
//**     UINT     fu
//**     DWORD    cb
//**
//**  RETURNS:
//**     LPVOID 
//**
//**  HISTORY:
//**     08/25/93       
//**
//*****************************************************************************

LPVOID FNLOCAL ReAllocPtr(
   LPVOID   pv, 
   UINT     fu, 
   DWORD    cb)
{
   HGLOBAL  hg;

   // Get the selector for the pointer.
   //
#ifdef _WIN32
   hg = (HGLOBAL)GlobalHandle(pv);
#else
   hg = (HGLOBAL)SELECTOROF(pv);
#endif
   
   // Attempt to reallocate the memory.
   //
   hg = GlobalReAlloc(hg, cb, fu);
   if (NULL != hg)
   {
      // Get a pointer to the new memory object.
      //
      pv = GlobalLock(hg);
   }
   else
   {
      // The re-allocation failed.
      //
      pv = NULL;
   }

   return(pv);
} //** ReAllocPtr()


//************************************************************************
//**
//**  FreePtr();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     LPVOID   ptr
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     07/09/93             
//**
//************************************************************************

VOID FNLOCAL FreePtr(
   LPVOID FAR *ptr)
{
   if (NULL != *ptr)
   {
      // Free the memory.
      //
#ifdef _WIN32
      GlobalFree((HGLOBAL)GlobalHandle(*ptr));
#else
      GlobalFree((HGLOBAL)SELECTOROF(*ptr));
#endif
      
      // Initialize the pointer.
      //
      *ptr = NULL;
   }
} //** FreePtr()


//************************************************************************
//**
//**  SetWindowTitle();
//**
//**  DESCRIPTION:
//**     This function will set the window title bar to reflect the 
//**     currently loaded file.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     05/02/93       created.
//**
//************************************************************************

VOID FNLOCAL SetWindowTitle(
   VOID)
{
   char  achbuf[MAX_STR_LEN+1];

   // Build the title bar string.
   //    "IDFEDIT - <IDF title>"
   //
   wsprintf(achbuf, "%s - %s", (LPSTR)gszTitleBar, (LPSTR)gszIDFTitle);
   SetWindowText(ghwndMain, achbuf);

} //** SetWindowTitle()



//************************************************************************
//**
//**  SetupDialog();
//**
//**  DESCRIPTION:
//**     This function sets up the dialog box for a new IDF file.
//**     It enables windows, if necessary, sets the window title,
//**     sets any defaults, and updates the controls.
//**
//**  ARGUMENTS:
//**     NONE 
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

VOID FNLOCAL SetupDialog()
{
   HMENU hmenu;

   hmenu = GetMenu(ghwndMain);

   // If this is our first edit, enable main child windows
   //
   if (gfFirstEdit)
   {
      // Enable the main child windows.
      //
      EnableWindow(HWND_IDF_CURRENT_SECTION, TRUE);
      EnableWindow(HWND_CURRENT_INSTRUMENT, TRUE);
      EnableWindow(HWND_EDIT_BOX, TRUE);
      EnableWindow(HWND_LIST_BOX, TRUE);
      EnableWindow(HWND_INSTRUMENT_SCROLL, TRUE);
      EnableMenuItem(hmenu, IDM_NEW_INSTRUMENT, MF_BYCOMMAND | MF_ENABLED);
      
      gfFirstEdit=FALSE;
   }

   // Reset gfChanged to FALSE
   //
   gfChanged = FALSE;

   // Gray these menu items until the IDF has changed
   //
   EnableMenuItem(hmenu, IDM_FILESAVE, MF_BYCOMMAND | MF_GRAYED);
   EnableMenuItem(hmenu, IDM_FILESAVEAS, MF_BYCOMMAND | MF_GRAYED);

   // Set the window title to that of the new .IDF file.
   //
   SetWindowTitle();

   // Set the current selection to the Header info.
   //
   ComboBox_SetCurSel(HWND_IDF_CURRENT_SECTION, 0);

   // Set the current instrument to the first one, and the 
   // default section to the header information.
   //
   gdwCurrInst     = 0;

   UpdateListBox();
   InvalidateRect(ghwndMain, NULL, TRUE);
}

//************************************************************************
//**
//**  NotifyUser();
//**
//**  DESCRIPTION:
//**     This function will display a message to the user.
//**
//**  ARGUMENTS:
//**     UINT  u  -  The resouce string id of the message to display.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

VOID FNLOCAL NotifyUser(
   UINT  u)
{
   char  szTitle[MAX_STR_LEN+1];
   int   cb;

   // Load the string resource.
   //
   cb = LoadString(ghinst, u, gszbuf, MAX_STR_LEN);
   if (0 != cb)
   {
      // Load the title bar string.
      //
      LoadString(ghinst, IDS_MESSAGE_BOX_TITLE, szTitle, MAX_STR_LEN);

      // Display the message to the user.
      //
      MessageBox(ghwndMain, gszbuf, szTitle, MB_OK);
   }
} //** NotifyUser()


//*****************************************************************************
//**
//**  QuerySave();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     BOOL 
//**
//**  HISTORY:
//**     09/07/93      created.
//**
//*****************************************************************************

int FNLOCAL QuerySave(
   VOID)
{
   char  szTitle[MAX_STR_LEN+1];
   int   i;

   // Load the string resources.
   //
   LoadString(ghinst, IDS_SAVE_CHANGES, gszbuf, MAX_STR_LEN);
   LoadString(ghinst, IDS_MESSAGE_BOX_TITLE, szTitle, MAX_STR_LEN);

   // Popup a message box with "YES", "NO" and "CANCEL" buttons
   // and ask user if they want to save the file....
   //
   i = MessageBox(ghwndMain, gszbuf, szTitle, MB_YESNOCANCEL);
   return(i);
} //** QuerySave()


//************************************************************************
//**
//**  CleanUp();
//**
//**  DESCRIPTION:
//**     This function will close a file if it is open, and free any
//**     memory that is being used by the application.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/27/93       created.
//**
//************************************************************************

VOID FNLOCAL CleanUp(
   VOID)
{
   DWORD c;
   UINT  iChannel;

   // If there is a file currently opened, then close it and
   // NULL out the handle.
   //
   if (NULL != ghmmio)
   {
      mmioClose(ghmmio, 0);
      ghmmio = NULL;
   }

   // Free the instruments.
   //
   if (NULL != gpInst)
   {
      for ( c = 0;
            c < gdwNumInsts;
            c++)
      {
         // Free the ASCII version of the instrument's
         // manufacturer name.
         //
         if (NULL != gpInst[c].pszManufactASCII)
            FreePtr(&gpInst[c].pszManufactASCII);
            
         // Free the UNICODE version of the instrument's
         // manufacturer name.
         //
         if (NULL != gpInst[c].pszManufactUNICODE)
            FreePtr(&gpInst[c].pszManufactUNICODE);

         // Free the ASCII version of the instrument's
         // product name.
         //
         if (NULL != gpInst[c].pszProductASCII)
            FreePtr(&gpInst[c].pszProductASCII);

         // Free the UNICODE version of the instrument's
         // product name.
         //
         if (NULL != gpInst[c].pszProductUNICODE)
            FreePtr(&gpInst[c].pszProductUNICODE);

         // Free any channel structures that have been allocated
         //
         for (iChannel = 0; iChannel < MAX_CHANNELS; iChannel++)
             if (NULL != gpInst[c].alpChannels[iChannel])
                 FreePtr(&gpInst[c].alpChannels[iChannel]);
      }

      // Free all of the instruments.
      //
      FreePtr(&gpInst);
   }

   // Initialize the current number of instruments and the
   // current instrument.
   //
   gdwNumInsts = 0;
   gdwCurrInst = 0;

} //** CleanUp()


//*****************************************************************************
//**
//**  SetListBoxTitleBar();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     UINT  uTitleBarID
//**
//**  RETURNS:
//**     MMRESULT 
//**
//**  HISTORY:
//**     09/06/93             created.
//**
//*****************************************************************************

MMRESULT FNLOCAL SetListBoxTitleBar(
   UINT  uTitleBarID)
{
   LPSTR psz;
   
   // Get the string for the title bar.
   //
   LoadString(ghinst, uTitleBarID, gszbuf, MAX_STR_LEN);

   // Parse our the left and right strings.
   // 
   for ( psz = gszbuf;
         *psz != '!';
         psz = AnsiNext(psz))
      ;

   *psz++ = '\0';

   // Set the title bar for the left side.
   //
   SetDlgItemText(ghwndMain, ID_HEADER_TEXT_LEFT, gszbuf);

   // Set the title bar for the right side.
   //
   SetDlgItemText(ghwndMain, ID_HEADER_TEXT_RIGHT, psz);

   return(MMSYSERR_NOERROR);
} //** SetListBoxTitleBar()


//*****************************************************************************
//**
//**  DrawKeyMaps();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     09/07/93            created.
//**
//*****************************************************************************

VOID FNLOCAL DrawKeyMaps(
   VOID)
{
   UINT  c;

   UINT  cKeyMap;
   char  szKeyMapName[MAX_STR_LEN+1];

   // Get the channel type.
   //
   c = ComboBox_GetCurSel(HWND_DRUM_CHANNELS);
   if (CB_ERR == c)
       return;

   c = (UINT)ComboBox_GetItemData(HWND_DRUM_CHANNELS, c);

   switch(c)
   {
       case CHAN_GENERAL:
           // Key map for general only shows key numbers
           //
           for ( cKeyMap = 0;
                 cKeyMap < MAX_KEY_MAPS;
                 cKeyMap++)
           {
               wsprintf(gszbuf, 
                        gszUU, 
                        cKeyMap + 1,
                        gpInst[gdwCurrInst].aKeyMaps[c][cKeyMap]);
               
               // Add the string to the list box.
               //
               ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
           }
           break;

       case CHAN_DRUM:
           for ( cKeyMap = 0;
                 cKeyMap < MAX_KEY_MAPS;
                 cKeyMap++)
           {
               if ((cKeyMap < DRUM_KEY_START) || 
                   (cKeyMap > DRUM_KEY_END))
               {
                   wsprintf(gszbuf, 
                            gszUU, 
                            cKeyMap + 1,
                            gpInst[gdwCurrInst].aKeyMaps[c][cKeyMap]);
               }
               else
               {
                   // Get the name of the patch from the string table.
                   //
                   LoadString(ghinst, 
                              (UINT)DRUM_KEY + cKeyMap, 
                              szKeyMapName, 
                              MAX_STR_LEN);

                   // Build the string to display in the list box.
                   //
                   wsprintf(gszbuf, 
                            gszSU, 
                            (LPSTR)szKeyMapName, 
                            CURR_KEY_MAP(c, cKeyMap) + 1);
               }
               
               // Add the string to the list box.
               //
               ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
           }
           break;

   }
} //** DrawKeyMaps()


//************************************************************************
//**
//**  DrawPatchMapInfo();
//**
//**  DESCRIPTION:
//**     This funciton will display the current instrument's patch map
//**     information in the list box.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/28/93       created.
//**
//************************************************************************

VOID FNLOCAL DrawPatchMapInfo(
   VOID)
{
   UINT  c;
   char  szPatchName[MAX_STR_LEN+1];
   
   // Set the title bar for patch maps.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_PATCHES);

   // Fill the list box with the patch map information.
   //
   for ( c = 0; 
         c < MAX_PATCHES; 
         c++)
   {
      // Get the name of the patch from the string table.
      //
      LoadString(ghinst, 
                 (UINT)(GENERAL_PATCH + c),
                 szPatchName, 
                 MAX_STR_LEN);

      // Build the string to display in the list box.
      //
      wsprintf(gszbuf, gszSU, (LPSTR)szPatchName, CURR_PATCH_MAP(c) + 1);

      // Add the string to the list box.
      //
      ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
   }
   
} //** DrawPatchMapInfo()


//************************************************************************
//**
//**  DrawChannelTypes();
//**
//**  DESCRIPTION:
//**     This function will display the current instrument's channel
//**     type information in the list box.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/28/93       created.
//**
//************************************************************************

VOID FNLOCAL DrawChannelTypes(
   VOID)
{
   UINT         c;
   UINT         i;
   LPCHANNEL    pChannel;
   LPSTR        ps;
   static char  szHexWork[256];

   // Set the title bar for the list box.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_CHANNELS);

   // Figure out what the selected channel is in the channel select
   // dropdown.
   //
   c = ComboBox_GetCurSel(HWND_DRUM_CHANNELS);

   if (CB_ERR == c)
       return;

   // Item data of this index is the actual (zero-based) channel number
   //
   c = (UINT)ComboBox_GetItemData(HWND_DRUM_CHANNELS, c);

   // If there is no channel data, this is a new channel - allocate a
   // channel structure and fill it in with defaults
   // BUGBUG - how do they add/delete channels from CB?
   //
   if (NULL == (pChannel = gpInst[gdwCurrInst].alpChannels[c]))
   {
       if (NULL == (pChannel = AllocPtr(GHND, sizeof(CHANNEL))))
       {
           // BUGBUG - Figure out how to notify no-mem err
           //
           return;
       }

       pChannel->fdwChannel = IDFCHANNELINFO_F_GENERAL_CHANNEL;
       pChannel->cbGeneralInitData = 0;
       pChannel->lpGeneralInitData = NULL;
       pChannel->cbDrumInitData = 0;
       pChannel->lpDrumInitData = NULL;

       gpInst[gdwCurrInst].alpChannels[c] = pChannel;
   }

   wsprintf(gszbuf,
            gszSS,
            (LPSTR)gszChannelTypes[CHAN_GENERAL],
            (LPSTR)(pChannel->fdwChannel & IDFCHANNELINFO_F_GENERAL_CHANNEL ? gszYes : gszNo));

   c = ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
   if (LB_ERR != c && LB_ERRSPACE != c)
       ListBox_SetItemData(HWND_LIST_BOX, c, CT_IS_GENERAL);

   *szHexWork = '\0';
   if (pChannel->fdwChannel & IDFCHANNELINFO_F_GENERAL_CHANNEL)
   {
       c = (UINT)min(HEX_DIGITS_TO_SHOW, pChannel->cbGeneralInitData);
       ps = szHexWork;
       for (i=0; i < c; i++)
           ps += wsprintf(ps, gszHexByte, pChannel->lpGeneralInitData[i]&0xFF);

       if (pChannel->cbGeneralInitData > HEX_DIGITS_TO_SHOW)
           lstrcpy(ps, gszEllipsis);

       wsprintf(gszbuf, gszSS, (LPSTR)gszGeneralInit, (LPSTR)szHexWork);
       c = ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
       if (LB_ERR != c && LB_ERRSPACE != c)
           ListBox_SetItemData(HWND_LIST_BOX, c, CT_GENERAL_INIT_STR);
   }
  
   wsprintf(gszbuf,
            gszSS,
            (LPSTR)gszChannelTypes[CHAN_DRUM],
            (LPSTR)(pChannel->fdwChannel & IDFCHANNELINFO_F_DRUM_CHANNEL ? gszYes : gszNo));

   c = ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
   if (LB_ERR != c && LB_ERRSPACE != c)
       ListBox_SetItemData(HWND_LIST_BOX, c, CT_IS_DRUM);
   
   *szHexWork = '\0';
   if (pChannel->fdwChannel & IDFCHANNELINFO_F_DRUM_CHANNEL)
   {
       c = (UINT)min(HEX_DIGITS_TO_SHOW, pChannel->cbDrumInitData);
       ps = szHexWork;
       for (i=0; i < c; i++)
           ps += wsprintf(ps, gszHexByte, pChannel->lpDrumInitData[i]&0xFF);
       
       if (pChannel->cbDrumInitData > HEX_DIGITS_TO_SHOW)
           lstrcpy(ps, gszEllipsis);

       wsprintf(gszbuf, gszSS, (LPSTR)gszDrumInit, (LPSTR)szHexWork);
       c = ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
       if (LB_ERR != c && LB_ERRSPACE != c)
           ListBox_SetItemData(HWND_LIST_BOX, c, CT_DRUM_INIT_STR);
   }
   
} //** DrawChannelTypes()


//************************************************************************
//**
//**  DrawInstCaps();
//**
//**  DESCRIPTION:
//**     This function will display the current instrument's 
//**     capabilities in the list box.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/28/93       created.
//**
//************************************************************************

VOID FNLOCAL DrawInstCaps(
   VOID)
{
   BOOL  f;
   char  szCapability[MAX_STR_LEN+1];

   // Set the title bar for the list box.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_INSTCAPS);

   // Display the base channel.
   //
   LoadString(ghinst, IDS_CAPS_BASE, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, gszSU,
            (LPSTR)szCapability, 
            (UINT)gpInst[gdwCurrInst].dwBasicChannel);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the number of channels.
   //
   LoadString(ghinst, IDS_CAPS_BASE + 1, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU,
            (LPSTR)szCapability, 
            (UINT)gpInst[gdwCurrInst].cNumChannels);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the instrument's polyphony.
   //
   LoadString(ghinst, IDS_CAPS_BASE + 2, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU,
            (LPSTR)szCapability, 
            gpInst[gdwCurrInst].cInstPoly);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the channel polyphony.
   //
   LoadString(ghinst, IDS_CAPS_BASE + 3, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU,
            (LPSTR)szCapability, 
            gpInst[gdwCurrInst].cChannelPoly);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the general midi compatibility.
   //
   f = (BOOL)(gpInst[gdwCurrInst].fdwFlags & IDFINSTCAPS_F_GENERAL_MIDI);

   LoadString(ghinst, IDS_CAPS_BASE + 4, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSS,
            (LPSTR)szCapability, 
            (LPSTR)(f ? "TRUE" : "FALSE"));
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Does the instrument support sysex messages?
   //
   f = (BOOL)(gpInst[gdwCurrInst].fdwFlags & IDFINSTCAPS_F_SYSTEMEXCLUSIVE);

   LoadString(ghinst, IDS_CAPS_BASE + 5, szCapability, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSS,
            (LPSTR)szCapability,
            (LPSTR)(f ? "TRUE" : "FALSE"));
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
} //** DrawInstCaps()


//************************************************************************
//**
//**  DrawInstrumentInfo();
//**
//**  DESCRIPTION:
//**     This function will display the current instrument's instrument 
//**     information in the list box.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/28/93       created.
//**     09/07/93       rewrote to use string tables.
//**
//************************************************************************

VOID FNLOCAL DrawInstrumentInfo(
   VOID)
{
   char  szInstrumentInfo[MAX_STR_LEN+1];

   // Set the title bar for the list box.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_INSTINFO);

   // Display the manufacturer.
   //
   LoadString(ghinst, IDS_INST_BASE, szInstrumentInfo, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSS, 
            (LPSTR)szInstrumentInfo, 
            (LPSTR)gpInst[gdwCurrInst].pszManufactASCII);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the product.
   //
   LoadString(ghinst, IDS_INST_BASE + 1, szInstrumentInfo, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSS, 
            (LPSTR)szInstrumentInfo, 
            (LPSTR)gpInst[gdwCurrInst].pszProductASCII);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the Manufacturer's ID.
   //
   LoadString(ghinst, IDS_INST_BASE + 2, szInstrumentInfo, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU, 
            (LPSTR)szInstrumentInfo, 
            (UINT)gpInst[gdwCurrInst].dwManufactID);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the product's ID.
   //
   LoadString(ghinst, IDS_INST_BASE + 3, szInstrumentInfo, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU,
            (LPSTR)szInstrumentInfo, 
            (UINT)gpInst[gdwCurrInst].dwProductID);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Display the Revision.
   //
   LoadString(ghinst, IDS_INST_BASE  + 4, szInstrumentInfo, MAX_STR_LEN);
   wsprintf(gszbuf, 
            gszSU,
            (LPSTR)szInstrumentInfo, 
            (UINT)gpInst[gdwCurrInst].dwRevision);
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
} //** DrawInstrumentInfo()


//************************************************************************
//**
//**  DrawHeader();
//**
//**  DESCRIPTION:
//**     This function will display the current instruments header 
//**     information in the list box.
//**     
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/28/93       created.
//**
//************************************************************************

VOID FNLOCAL DrawHeader(
   VOID)
{
   char  szHeaderString[MAX_STR_LEN+1];

   // Set the title bar for the list box.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_HEADER);

   // Get the string resource.
   //
   LoadString(ghinst, IDS_HEADER_BASE, szHeaderString, MAX_STR_LEN);

   // Build the string to display.
   //
   wsprintf(gszbuf,
            gszSX,
            (LPSTR)szHeaderString, 
            (UINT)gpInst[gdwCurrInst].dwVersion);

   // Add the string resource to the list box.
   //
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);

   // Get the string resource.
   //
   LoadString(ghinst, IDS_HEADER_BASE + 1, szHeaderString, MAX_STR_LEN);

   // Build the string to display.
   //
   wsprintf(gszbuf, 
            gszSX,
            (LPSTR)szHeaderString, 
            (UINT)gpInst[gdwCurrInst].dwCreator);

   // Add the string resource to the list box.
   //
   ListBox_AddString(HWND_LIST_BOX, (LPARAM)(LPSTR)gszbuf);
} //** DrawHeader()


//*****************************************************************************
//**
//**  EnableChannelListBox();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     09/07/93             created.
//**
//*****************************************************************************

VOID FNLOCAL EnableChannelListBox(
   VOID)
{
   char  szChannel[MAX_STR_LEN+1];
   DWORD cChannel;
   UINT  uChanType;
   DWORD dwIndex;

   // Enable the drop down list box.
   //
   EnableWindow(HWND_DRUM_CHANNELS, TRUE);

   // Show the drop down list box.
   //
   ShowWindow(HWND_DRUM_CHANNELS, SW_SHOW);

   // Reset the contents of the list box.
   //
   ComboBox_ResetContent(HWND_DRUM_CHANNELS);

   // Set the title bar for patch maps.
   //
   SetListBoxTitleBar(IDS_TITLE_BAR_KEYMAPS);

   // Get the string from our string table.
   //
   LoadString(ghinst, IDS_CHANNEL, szChannel, MAX_STR_LEN);

   if (guCurrSelection == IDF_CHANNEL_TYPE)
   {
       // Fill the drop down list box with the drum channels.
       // BUGBUG - Eventually add only those channels with data
       // and have a way to add/delete them
       //
       for ( cChannel = 0;
       cChannel < MAX_CHANNELS;
       cChannel++)
       {
           // Build the string to display.
           //
           wsprintf(gszbuf, "%s: %u", (LPSTR)szChannel, cChannel+1);

           // Add the string to the list box.
           //
           dwIndex = ComboBox_AddString(HWND_DRUM_CHANNELS, (LPARAM)(LPCSTR)gszbuf);
           if (CB_ERRSPACE != dwIndex)
               ComboBox_SetItemData(HWND_DRUM_CHANNELS, (WPARAM)dwIndex, cChannel);
       }
   }
   else
   {
       // IDF_KEY_MAPS - put channel types in
       //
       for (uChanType = 0; uChanType < CHAN_TYPES; uChanType++)
       {
           dwIndex = ComboBox_AddString(HWND_DRUM_CHANNELS, (LPARAM)(LPSTR)gszChannelTypes[uChanType]);
           if (CB_ERRSPACE != dwIndex)
               ComboBox_SetItemData(HWND_DRUM_CHANNELS, (WPARAM)dwIndex, uChanType);
       }
   }
   
   // Set the current selection to the first drum channel.
   //
   ComboBox_SetCurSel(HWND_DRUM_CHANNELS, 0);
} //** EnableChannelListBox()


//*****************************************************************************
//**
//**  DisableChannelListBox();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     09/07/93           created.
//**
//*****************************************************************************

VOID FNLOCAL DisableChannelListBox(
   VOID)
{
   // Disable the drop down list box.
   //
   EnableWindow(HWND_DRUM_CHANNELS, FALSE);

   // Hide the drop down list box.
   //
   ShowWindow(HWND_DRUM_CHANNELS, SW_HIDE);

} //** DisableChannelListBox()


//************************************************************************
//**
//**  UpdateListBox();
//**
//**  DESCRIPTION:
//**     This function will redraw the contents of the list box depending
//**     on what guCurrSelection says is the currently viewed section.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     05/01/93       created.
//**
//************************************************************************

VOID FNLOCAL UpdateListBox(
   VOID)
{
   UINT  uPrevSelection;
   BOOL  fDisable;
   BOOL  fEnable;

   // Disable drawing of the list box.
   //
   SetWindowRedraw(HWND_LIST_BOX, FALSE);

   // Clear the current list box's contents.
   //
   ListBox_ResetContent(HWND_LIST_BOX);

   // Clear the edit box.
   //
   SetDlgItemText(ghwndMain, ID_EDIT_BOX, gszNULL);

   // Disable the enter button.
   //
   EnableWindow(HWND_EDIT_ENTER, FALSE);

   // Save the previous selection.
   //
   uPrevSelection = guCurrSelection;

   // Get the current selection in the combo-box.
   //
   guCurrSelection = ComboBox_GetCurSel(HWND_IDF_CURRENT_SECTION);

   if (guCurrSelection != uPrevSelection)
   {
       // Enable or disable the drop-down channel list box as needed.
       //
       fDisable = (IDF_KEY_MAPS == uPrevSelection || IDF_CHANNEL_TYPE == uPrevSelection);
       fEnable  = (IDF_KEY_MAPS == guCurrSelection || IDF_CHANNEL_TYPE == guCurrSelection);

       if (fDisable && !fEnable)
           DisableChannelListBox();

       if (fEnable)
           EnableChannelListBox();
   }

   // Draw the correct section info.
   //
   switch (guCurrSelection)
   {
      case IDF_HEADER_INFO:
         // Redraw the header info.
         //
         DrawHeader();

         break;

      case IDF_INSTRUMENT_INFO:
         // Update the instrument information.
         //
         DrawInstrumentInfo();

         break;

      case IDF_INSTRUMENT_CAPS:
         // Update the midi capabilities information.
         //
         DrawInstCaps();

         break;

      case IDF_CHANNEL_TYPE:
         // Update the channel type.
         //
         DrawChannelTypes();

         break;

      case IDF_PATCH_MAPS:
         // Update the patch map.
         //
         DrawPatchMapInfo();

         break;

      case IDF_KEY_MAPS:
         // Update hte key map.
         //
         DrawKeyMaps();
         
         break;
   }

   // Set the current selection to the first string.
   //
   SetFocus(HWND_LIST_BOX);
   ListBox_SetCurSel(HWND_LIST_BOX, 0);

   // Enable redrawing of the list box.
   //
   SetWindowRedraw(HWND_LIST_BOX, TRUE);
} //** UpdateListBox()


//************************************************************************
//**
//**  AllocateInst();
//**
//**  DESCRIPTION:
//**     This function will allocate or reallocate the IDF pointer.
//**     If the gpInst pointer is NULL then it will allocate memory for
//**     a new instrument, if gpIDF is _NOT_ NULL then it will reallocate
//**     the pointer so that it will contain space for one more instrument.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID
//**
//**  HISTORY:
//**     05/01/93       created.
//**
//************************************************************************

BOOL FNLOCAL AllocateInst(
   VOID)
{
   LPVOID   pv;

   // Do we already have instruments allocated?
   //
   if (NULL == gpInst)
   {
      // Nope, so allocate memory for one.
      //
      pv = AllocPtr(GHND, sizeof(INSTRUMENT) * (gdwNumInsts + 1));
   }
   else
   {
      // Yep, so reallocate memory for another.
      //
      pv = ReAllocPtr(gpInst, GHND, sizeof(INSTRUMENT) * (gdwNumInsts + 1));
   }

   // Did we allocate anything, new or otherwise?
   //
   if (NULL != pv)
   {
      // We've got another instrument allocated.
      //
      gpInst = pv;
      gdwCurrInst = gdwNumInsts;
      gdwNumInsts++;
      return(TRUE);
   }

   return(FALSE);
} //** AllocateInst()


//************************************************************************
//**
//**  NewInstrument();
//**
//**  DESCRIPTION:
//**     This function will allocate and initialize the memory for an
//**     instrument the IDF.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR on success or IDFERR_NOMEM if 
//**                 it cannot allocate memory for the instrument.
//**
//**  HISTORY:
//**     04/27/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL NewInstrument(
   VOID)
{
   UINT  cPatch;
   UINT  cChannel;
   UINT  cKeyMap;


   // Allocate a new IDF struct.
   //
   if (!AllocateInst())
      return(IDFERR_NOMEM);

   // Fill in the instruments fields.
   //
   gpInst[gdwCurrInst].dwVersion       = IDF_FMT_VERSION;
   gpInst[gdwCurrInst].dwCreator       = MM_MICROSOFT;
   gpInst[gdwCurrInst].fdwFlags        |= IDFINSTCAPS_F_GENERAL_MIDI;
   gpInst[gdwCurrInst].dwBasicChannel  = 1;
   gpInst[gdwCurrInst].cNumChannels    = 16;
   gpInst[gdwCurrInst].cInstPoly       = 16;
   gpInst[gdwCurrInst].cChannelPoly    = 16;

   for ( cChannel = 0;
         cChannel < MAX_CHANNELS;
         cChannel++ )
   {
       gpInst[gdwCurrInst].alpChannels[cChannel] = NULL;
   }
              

   // Initialize the patch maps.
   //
   for ( cPatch = 0; 
         cPatch < MAX_PATCHES; 
         cPatch++)
   {
      // Set up each patch as General MIDI.
      //
      CURR_PATCH_MAP(cPatch) = cPatch;
   }

   // Initialize the key maps.
   //
   for ( cChannel = 0;
         cChannel < CHAN_TYPES;
         cChannel++)
   {
      for ( cKeyMap = 0;
            cKeyMap < MAX_KEY_MAPS;
            cKeyMap++)
      {
         // Start out with General MIDI.
         //
         CURR_KEY_MAP(cChannel, cKeyMap) = cKeyMap;
      }
   }

   // Place the number of the current instrument in the 
   // edit box. Add one to the index to give a nice readable
   // number to non-computer types.
   //
   SetDlgItemInt(ghwndMain, 
                 ID_CURRENT_INSTRUMENT, 
                 (UINT)gdwCurrInst + 1, 
                 FALSE);

   // Reset the scroll bar's range.
   //
   SetScrollRange(HWND_CURRENT_INSTRUMENT,
                  SB_CTL, 
                  0, 
                  (UINT)gdwCurrInst, 
                  FALSE);

   // Set the scroll bar position to the new instrument.
   //
   SetScrollPos(HWND_CURRENT_INSTRUMENT,
                SB_CTL,
                (UINT)gdwCurrInst, 
                TRUE);

   // Set return value to success.
   //
   return(MMSYSERR_NOERROR);
} //** NewInstrument()


//************************************************************************
//**
//**  NewConfigFile();
//**
//**  DESCRIPTION:
//**     This function will clear any IDF that is currently being
//**     edited and create an new IDF and a default instrument to edit.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR upon success, 
//**                 IDFERR_NOMEM if it could not allcoate a new 
//**                 instrument.
//**
//**  HISTORY:
//**     04/26/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL NewConfigFile(
   VOID)
{
   MMRESULT mmr;

   // Dump any current information.
   //
   CleanUp();

   // Allocate memory for an instrument.
   //
   mmr = NewInstrument();
   if (MMSYSERR_NOERROR != mmr)
      return(mmr);

   // Load the default file name.
   //
   LoadString(ghinst, IDS_DEF_FILE_NAME, gszIDFTitle, MAX_TITLE_LEN);

   // Initialize the path name.
   //
   _fmemset(gszIDFName, 0, MAX_PATH_LEN + 1);

   // Setup the dialog box for the new IDF file
   //
   SetupDialog();

   return(MMSYSERR_NOERROR);
} //** NewConfigFile()


void FNLOCAL idfeditOnPaint(
    HWND            hwnd)                               
{
    PAINTSTRUCT     ps;
    
    BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
}

void FNLOCAL idfeditOnVScroll(
    HWND            hwnd,
    HWND            hwndCtl,
    UINT            code,
    int             pos)
{

    // Did they want to switch instruments?
    //
    if (HWND_INSTRUMENT_SCROLL == hwndCtl)
    {

        // What did the user do this time?
        //
        switch (code)
        {
            case SB_LINEUP:
                // Subtract one from the current instrument.
                // If we are at the first instrument the wrap 
                // to the last instrument.
                //
                if (0 == gdwCurrInst)
                {
                    // Remeber that gdwCurrInst is an index whereas
                    // gdwNumInsts is a count.  So we need to subtract
                    // one from the count to get the index.
                    // 
                    gdwCurrInst = gdwNumInsts - 1;
                }
                else
                {
                    // Get the previous instrument.
                    //
                    gdwCurrInst--;
                }

                break;

            case SB_LINEDOWN:
                // Add one to the current instrument.
                // If we are at the last instrument then wrap
                // to the first instrument.
                //
                if (gdwNumInsts == gdwCurrInst+1)
                {
                    // Set the current instrument to the 
                    // first instrument in our instrument array.
                    //
                    gdwCurrInst = 0;
                }
                else
                {
                    // Get the next instrument.
                    //
                    gdwCurrInst++;
                }

                break;

        }


        // Update the edit field.
        // We add one to give a nice readable number to
        // non-computer types....
        //
        SetDlgItemInt(ghwndMain, 
                      ID_CURRENT_INSTRUMENT, 
                      (UINT)gdwCurrInst + 1, 
                      FALSE);

        // Update the contents of the dialog.
        //
        UpdateListBox();
    }
}

void FNLOCAL idfeditOnCommand(
    HWND            hwnd,
    int             id,
    HWND            hwndCtl,
    UINT            codeNotify                                      
)
{
    MMRESULT        mmr;
    HMENU           hmenu;

    switch (id)
    {
        case IDM_FILENEW:
            // Setup a new configuration file.
            //
            mmr = NewConfigFile();
            if (MMSYSERR_NOERROR != mmr)
                NotifyUser(mmr);

            break;

        case IDM_FILEOPEN:
            // Open an existing configuration file.
            //
            mmr = OpenConfigFile();
            if (MMSYSERR_NOERROR != mmr)
                NotifyUser(mmr);

            break;

        case IDM_FILESAVE:
            // Save the current configuration file.
            //
            if ('\0' == gszIDFName[0])
                mmr = SaveConfigFileAs();
            else
                mmr = SaveConfigFile();

            if (MMSYSERR_NOERROR != mmr)
                NotifyUser(mmr);

            break;

        case IDM_FILESAVEAS:
            // Save the current configuration file as something....
            //
            mmr = SaveConfigFileAs();
            if (MMSYSERR_NOERROR != mmr)
                NotifyUser(mmr);

            break;

        case ID_IDF_CURRENT_SECTION:
            // Did the selection change?
            //
            if (CBN_CLOSEUP == codeNotify)
                UpdateListBox();

            break;

        case ID_DRUM_CHANNELS:
            // Check to see if the user selected a different drum channel.
            //
            if (CBN_CLOSEUP == codeNotify)
                UpdateListBox();

            break;

        case ID_EDIT_BOX:
            // Has something been typed?
            //
            if (EN_CHANGE == codeNotify)
            {
                // Enable the Enter button.
                //
                EnableWindow(HWND_EDIT_ENTER, TRUE);
            }

            break;

        case ID_EDIT_ENTER:
            // Was the enter button clicked?
            //
            if (BN_CLICKED == codeNotify)
            {
                // Update the instrument's information with
                // the new data the the user has entered.
                //
                UpdateInstrument();
            }

            // Is this the first change??  
            // If so, activate the save menu.
            //
            if (!gfChanged)
            {
                hmenu = GetMenu(ghwndMain);
                EnableMenuItem(hmenu, IDM_FILESAVE, MF_BYCOMMAND | MF_ENABLED);
                EnableMenuItem(hmenu, IDM_FILESAVEAS, MF_BYCOMMAND | MF_ENABLED);

                // A change has been made.
                //
                gfChanged = TRUE;
            }
            break;

        case IDM_NEW_INSTRUMENT:
            // Add a new instrument to the whole thing.
            //
            mmr = NewInstrument();
            if (MMSYSERR_NOERROR != mmr)
            {
                // There was an error while trying to create a new instrument.
                //
                NotifyUser(mmr);
            }

            // Update the list box with the new instruments settings.
            //
            UpdateListBox();

            break;

        case IDM_FILEEXIT:
            // Destroy the window.
            //
            PostMessage(ghwndMain, WM_CLOSE, 0, 0L);

            break;

        case IDM_ABOUT:
            // Show the about box.
            //
            About();

            break;
    }
}

void FNLOCAL idfeditOnClose(
    HWND            hwnd)
{
    int             i;
    MMRESULT        mmr;

    // Were any modifications made to the file?
    //
    if (gfChanged)
    {
        // Query the user for save.
        //
        i = QuerySave();

        // Did they decide to continue?
        //
        if (IDCANCEL == i)
            return;

        // Do they wish to save the current IDF?
        //
        if (IDYES == i)
        {
            // Save the current IDF.
            //
            if ('\0' == gszIDFName[0])
                mmr = SaveConfigFileAs();
            else
                mmr = SaveConfigFile();

            // We're we able to save the file?
            //
            if (MMSYSERR_NOERROR != mmr)
                NotifyUser(mmr);
        }

        //
        // By default if the user selects "YES" or "NO" we
        // fall through and destroy the window and consequently
        // exit the program.
        //
    }

    // Destroy main window.
    //
    DestroyWindow(ghwndMain);
}

void FNLOCAL idfeditOnDestroy(
    HWND            hwnd)
{
    // We're done so clean up.
    //
    CleanUp();

    // Post our quit message.
    //
    PostQuitMessage(0);
}

#if _WIN32
HBRUSH FNLOCAL idfeditOnCtlColor(
    HWND            hwnd,
    HDC             hdc,
    HWND            hwndChild,
    int             type)
{
    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

    return (HBRUSH)GetStockObject(NULL_BRUSH);
}
#endif

//************************************************************************
//**
//**  MainProc();
//**
//**  DESCRIPTION:
//**     Processes the messages for the application.
//** 
//**  ARGUMENTS:
//**     HWND     hwnd     -  Window that we process messages for.
//**     UINT     umsg     -  Message ID.
//**     WPARAM   wParam   -  Message dependant info.
//**     LPARAM   lParam   -  Message dependant info.
//**
//**  RETURNS:
//**     LRESULT  -  Return value depends upon the message.
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

LRESULT FNEXPORT MainProc(
   HWND     hwnd,
   UINT     umsg,
   WPARAM   wParam,
   LPARAM   lParam)
{
   switch (umsg)
   {
       HANDLE_MSG(hwnd, WM_PAINT,           idfeditOnPaint);
       HANDLE_MSG(hwnd, WM_VSCROLL,         idfeditOnVScroll);
       HANDLE_MSG(hwnd, WM_COMMAND,         idfeditOnCommand);
       HANDLE_MSG(hwnd, WM_CLOSE,           idfeditOnClose);
       HANDLE_MSG(hwnd, WM_DESTROY,         idfeditOnDestroy);
#ifdef _WIN32
       HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC,  idfeditOnCtlColor);
#endif

      default:
         // Take care of everything else.
         //
         return(DefWindowProc(hwnd, umsg, wParam, lParam));
   }
} //** MainProc()
