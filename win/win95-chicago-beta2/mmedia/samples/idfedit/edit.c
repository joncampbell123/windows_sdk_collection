//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  edit.c
//**
//**  DESCRIPTION:
//**     This file contains the code for the dialog box to enter new
//**     information for the IDF.
//**
//**  HISTORY:
//**     04/28/93       created.
//**
//************************************************************************


#include "globals.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "res.h"

//*****************************************************************************
//**
//**  GetDlgItemDword();
//**
//**  DESCRIPTION:
//**     This function will read text from the ID_EDIT_BOX field and convert
//**     it to a DWORD value.  I use this since GetDlgItemInt() only returns
//**     a UINT.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     DWORD 
//**
//**  HISTORY:
//**     09/08/93       created.
//**
//*****************************************************************************

DWORD FNLOCAL GetDlgItemDword(
   VOID)
{
   DWORD dw;

   // Get the text that the user entered in the list box.
   //
   GetDlgItemText(ghwndMain, ID_EDIT_BOX, gszbuf, MAX_STR_LEN);

   // Convert the text to a number.
   //
   dw = (DWORD)atol(gszbuf);

   wsprintf(gszbuf, "%lu", dw);

   return(dw);
} //** GetDlgItemDword()


//************************************************************************
//**
//**  IsTrue();
//**
//**  DESCRIPTION:
//**     This funtion will translate an expression passed to it to a 
//**     boolean value.
//**
//**  ARGUMENTS:
//**     char  *szExpression
//**
//**  RETURNS:
//**     BOOL  -  FALSE if the first character of the expression is:
//**              'F', '0' or '\0'. Anything else and the expression 
//**              will evaluate to false.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

BOOL FNLOCAL IsTrue(
   char  *szExpression)
{
   BOOL  f;
   char  c;

   // If the data the user entered was not of alpha or numeric 
   // origins then tell them what we actually look for.
   //
   if (isalnum(*szExpression))
   {
      // Convert the character, if it is one, 
      // to upper case for simplicity.
      //
      c = toupper(*szExpression);

      // Determine if the expression is false.
      // This can be either a 'F', '0' or a NULL.
      //
      f = !((c == 'F') || (*szExpression == '0') || (*szExpression == '\0'));
   }
   else
   {
      // Better let 'em know what we're expecting.
      //
      MessageBox(ghwndMain, 
                 "Please enter: 0, 1, TRUE or FALSE.", NULL, MB_OK);
   }

   return(f);
} //** IsTrue()


//*****************************************************************************
//**
//**  GetDlgItemBool();
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
//**     09/08/93             created.
//**
//*****************************************************************************

BOOL FNLOCAL GetDlgItemBool(
   VOID)
{
   BOOL  f;

   // Get the text form the edit box.
   //
   GetDlgItemText(ghwndMain, ID_EDIT_BOX, gszbuf, MAX_STR_LEN);

   // Determine it the expression is TRUE or FALSE.
   //
   f = IsTrue(gszbuf);

   return(f);
} //** GetDlgItemBool()

BOOL FNLOCAL GetDlgItemHex(
    LPDWORD     lpdwLength)
{
    PBYTE       pbSrc;
    PBYTE       pbDst;
    char        c;
    BYTE        b;
    
    GetDlgItemText(ghwndMain, ID_EDIT_BOX, gszbuf, MAX_STR_LEN);

    pbSrc = gszbuf;
    pbDst = gszbuf;

    for (;;)
    {
        while (*pbSrc && isspace(*pbSrc))
            ++pbSrc;

        // 0-length hex string is acceptable
        //
        if (!*pbSrc)
            break;

        b = 0;
        while (isxdigit(*pbSrc))
        {
            c = *pbSrc++;
            c = toupper(c);
            if ('0' <= c && '9' >= c)
                b = (b << 4) + (c - '0');
            else
                b = (b << 4) + (c - 'A' + 10);
        }

        if (*pbSrc != '\0' && !isspace(*pbSrc))
            return FALSE;

        *pbDst++ = b;
    }

    *lpdwLength = (pbDst - gszbuf);

    return TRUE;
}


//************************************************************************
//**
//**  UpdateHeaderInfo();
//**
//**  DESCRIPTION:
//**     This function will update the header information with what
//**     the user entered.
//**
//**  ARGUMENTS:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93       generalized.
//**
//************************************************************************

VOID FNLOCAL UpdateHeaderInfo(
   UINT  uEntry)   
{  
   DWORD dwNewValue;

   // Get the integer data that the user entered.
   //
   dwNewValue = GetDlgItemDword();

   // Update the appropriate entry.
   //
   switch (uEntry)
   {
      case IDF_VERSION:
         // Save the IDF format version.
         //
         gpInst[gdwCurrInst].dwVersion = dwNewValue;

         break;

      case IDF_CREATOR:
         // Save the IDF creator (manufacturer).
         //
         gpInst[gdwCurrInst].dwCreator = dwNewValue;

         break;
   }

} //** UpdateHeaderInfo()


//************************************************************************
//**
//**  UpdateInstrumentInfo();
//**
//**  DESCRIPTION:
//**     This function will update the instrument information with
//**     what the user entered.
//**
//**  ARGUMENTS:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93       updated.
//**
//************************************************************************

VOID FNLOCAL UpdateInstrumentInfo(
   UINT  uEntry)
{
   DWORD cbTextLength;
   LPSTR psz;

   // Update the instrument information structure accordingly.
   //
   switch (uEntry)
   {
      case INST_MANUFACT:
      case INST_PRODUCT:

         // Get the length of the entered text.    
         //
         cbTextLength = Edit_GetTextLength(HWND_EDIT_BOX);

         // Allocate a new buffer for the name.
         //
         psz = AllocPtr(GHND, cbTextLength + 1);   
         if (NULL == psz)
         {
            // We could not allocate memory for the new name.
            //
            NotifyUser(IDFERR_NOMEM);
            break;
         }

         // Get the name entered into the buffer.
         //
         GetDlgItemText(ghwndMain, ID_EDIT_BOX, psz, (UINT)cbTextLength + 1); 

         // Which string did they edit?
         //
         if (INST_MANUFACT == uEntry)
         {
            // Free the old name.
            //
            FreePtr(&gpInst[gdwCurrInst].pszManufactASCII);
         
            // Save the new name in our instrument structure.
            //
            gpInst[gdwCurrInst].pszManufactASCII = psz;
         }
         else
         {
            // Free the old name.
            //
            FreePtr(&gpInst[gdwCurrInst].pszProductASCII);
         
            // Save the new name in our instrument structure.
            //
            gpInst[gdwCurrInst].pszProductASCII = psz;
         }

         break;

      case INST_MID:
         // Save the manufacturer ID.
         //
         gpInst[gdwCurrInst].dwManufactID = GetDlgItemDword();
         
         break;

      case INST_PID:
         // Save the product ID.
         //
         gpInst[gdwCurrInst].dwProductID = GetDlgItemDword();

         break;

      case INST_REV:
         // Save the revision
         //
         gpInst[gdwCurrInst].dwRevision = GetDlgItemDword();

         break;
   }

} //** UpdateInstrumentInfo()


//************************************************************************
//**
//**  UpdateInstCaps();
//**
//**  DESCRIPTION:
//**     This function will update the instrument capabilities 
//**     information with what the user entered.
//**
//**  ARGUMENTS:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93             added a few things.
//**
//************************************************************************

VOID FNLOCAL UpdateInstCaps(
   UINT  uEntry)
{
   BOOL  f;

   // Update the instrument caps struct accordingly.
   //
   switch (uEntry)
   {
      case CAPS_BASECHANL:
         // Save the new base channel.
         //
         gpInst[gdwCurrInst].dwBasicChannel = GetDlgItemDword();

         break;

      case CAPS_NUMCHANL:
         // Save the new number of channels.
         //
         gpInst[gdwCurrInst].cNumChannels =  GetDlgItemDword();

         break;

      case CAPS_INSTPOLY:
         // Save the instrument's polyphony.
         //
         gpInst[gdwCurrInst].cInstPoly = GetDlgItemDword();

         break;

      case CAPS_CHANLPOLY:
         // Save the channel polyphony.
         //
         gpInst[gdwCurrInst].cChannelPoly = GetDlgItemDword();

         break;
      
      case CAPS_GENERAL:
         // Did the user enter a TRUE or FALSE expression?
         //
         f = GetDlgItemBool();

         // Is the instrument General MIDI compliant?
         //
         if (f)
            gpInst[gdwCurrInst].fdwFlags |= IDFINSTCAPS_F_GENERAL_MIDI;
         else
            gpInst[gdwCurrInst].fdwFlags &= ~IDFINSTCAPS_F_GENERAL_MIDI;

         break;

      case CAPS_SYSEX:
         // Did the user enter a TRUE or FALSE expression?
         //
         f = GetDlgItemBool();

         // Does the instrument support system exclusive messages?
         //
         if (f)
            gpInst[gdwCurrInst].fdwFlags |= IDFINSTCAPS_F_SYSTEMEXCLUSIVE;
         else
            gpInst[gdwCurrInst].fdwFlags &= ~IDFINSTCAPS_F_SYSTEMEXCLUSIVE;

         break;
   }

} //** UpdateInstCaps()

//************************************************************************
//**
//**  UpdateChannelType();
//**
//**  DESCRIPTION:
//**     This function will update the channel type information with 
//**     what user entered.
//**
//**  ARGUMENTS:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93             added a few things.
//**
//************************************************************************

VOID FNLOCAL UpdateChannelType(
   UINT  uEntry)
{
   BOOL  f;
   UINT  uChan;
   UINT  uItem;
   DWORD dwIndex;
   DWORD cbHex;
   LPVOID pv;
   
   // Update channel entry based on selected channel and item changed
   //

   dwIndex = ComboBox_GetCurSel(HWND_DRUM_CHANNELS);
   if (CB_ERR == dwIndex)
       return;

   uChan = (UINT)ComboBox_GetItemData(HWND_DRUM_CHANNELS, (WPARAM)dwIndex);

   uItem = ListBox_GetCurSel(HWND_LIST_BOX);
   if (CB_ERR == uItem)
       return;

   uItem = (UINT)ListBox_GetItemData(HWND_LIST_BOX, uItem);

   switch(uItem)
   {
       case CT_IS_GENERAL:
           f = GetDlgItemBool();
           if (f)
               CURR_CHANNEL(uChan)->fdwChannel |= IDFCHANNELINFO_F_GENERAL_CHANNEL;
           else
               CURR_CHANNEL(uChan)->fdwChannel &= ~IDFCHANNELINFO_F_GENERAL_CHANNEL;
           return;
           
       case CT_IS_DRUM:
           f = GetDlgItemBool();
           if (f)
               CURR_CHANNEL(uChan)->fdwChannel |= IDFCHANNELINFO_F_DRUM_CHANNEL;
           else
               CURR_CHANNEL(uChan)->fdwChannel &= ~IDFCHANNELINFO_F_DRUM_CHANNEL;
           return;
           
       case CT_GENERAL_INIT_STR:
           if (!GetDlgItemHex(&cbHex))
           {
               if (0 == LoadString(ghinst, IDS_BAD_HEX_INPUT, gszbuf, MAX_STR_LEN+1))
                   *gszbuf = '\0';
               
               MessageBox(ghwndMain, gszbuf, gszTitleBar, MB_ICONEXCLAMATION|MB_OK);
               return;
           }

           if (NULL == CURR_CHANNEL(uChan)->lpGeneralInitData)
           {
               pv = AllocPtr(GHND, cbHex);
           }
           else
           {
               pv = ReAllocPtr(CURR_CHANNEL(uChan)->lpGeneralInitData, GHND, cbHex);
           }

           if (NULL == pv)
           {
               if (0 == LoadString(ghinst, IDFERR_NOMEM, gszbuf, MAX_STR_LEN+1))
                   *gszbuf = '\0';
               
               MessageBox(ghwndMain, gszbuf, gszTitleBar, MB_ICONEXCLAMATION|MB_OK);
               return;
           }

           CURR_CHANNEL(uChan)->lpGeneralInitData = pv;
           CURR_CHANNEL(uChan)->cbGeneralInitData = cbHex;
           _fmemcpy(CURR_CHANNEL(uChan)->lpGeneralInitData, gszbuf, (UINT)cbHex);
           return;
               
       case CT_DRUM_INIT_STR:
           if (!GetDlgItemHex(&cbHex))
           {
               if (0 == LoadString(ghinst, IDS_BAD_HEX_INPUT, gszbuf, MAX_STR_LEN+1))
                   *gszbuf = '\0';
               
               MessageBox(ghwndMain, gszbuf, gszTitleBar, MB_ICONEXCLAMATION|MB_OK);
               return;
           }

           if (NULL == CURR_CHANNEL(uChan)->lpDrumInitData)
           {
               pv = AllocPtr(GHND, cbHex);
           }
           else
           {
               pv = ReAllocPtr(CURR_CHANNEL(uChan)->lpDrumInitData, GHND, cbHex);
           }

           if (NULL == pv)
           {
               if (0 == LoadString(ghinst, IDFERR_NOMEM, gszbuf, MAX_STR_LEN+1))
                   *gszbuf = '\0';
               
               MessageBox(ghwndMain, gszbuf, gszTitleBar, MB_ICONEXCLAMATION|MB_OK);
               return;
           }

           CURR_CHANNEL(uChan)->lpDrumInitData = pv;
           CURR_CHANNEL(uChan)->cbDrumInitData = cbHex;
           _fmemcpy(CURR_CHANNEL(uChan)->lpDrumInitData, gszbuf, (UINT)cbHex);
           return;
           
       default:
           return;
   }   

#if 0
   // Get the text the user entered.
   //
   GetDlgItemText(ghwndMain, ID_EDIT_BOX, gszbuf, MAX_STR_LEN);

   // Do we know about this channel type?
   //
   for ( c = 0, f = FALSE;
         !f && (NUM_CHANNELS_DEFINED > c);
         c++)
   {
      // Is it in our array?
      //
      if (0 == lstrcmpi(gszChannelTypes[c], gszbuf))
      {
         // We found it.
         //
         f = TRUE;

         // Save the type index.
         //
         CURR_CHANNEL_TYPE(uEntry) = c;
      }
   }
#endif
} //** UpdateChannelType()


//************************************************************************
//**
//**  UpdatePatchMap();
//**
//**  DESCRIPTION:
//**     This function will update the patch map information.
//**
//**  ARGUMENTS:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93             added a few things.
//**
//************************************************************************

VOID FNLOCAL UpdatePatchMap(
   UINT  uEntry)
{
   UINT  uPatch;

   // Get the new patch.
   //
   uPatch = GetDlgItemInt(ghwndMain, ID_EDIT_BOX, NULL, FALSE);

   // Get the new patch map that the user enterd.
   //
   CURR_PATCH_MAP(uEntry) = (BYTE)uPatch;

} //** UpdatePatchMap()


//*****************************************************************************
//**
//**  UpdateKeyMap();
//**
//**  DESCRIPTION:
//**     UINT  uEntry   -  The item in the list box that we are editing.
//**
//**  ARGUMENTS:
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     09/08/93            created.
//**
//*****************************************************************************

VOID FNLOCAL UpdateKeyMap(
   UINT  uEntry)
{
   DWORD dwIndex;
   UINT  uType;

   // Update the key map for the correct channel type
   //

   // Get the channel type (this will be an index into the key map array)
   //

   dwIndex = ComboBox_GetCurSel(HWND_DRUM_CHANNELS);
   if (CB_ERR == (UINT)dwIndex)
       return;

   uType = (UINT)ComboBox_GetItemData(HWND_DRUM_CHANNELS, (WPARAM)dwIndex);

   // Update
   //
   CURR_KEY_MAP(uType, uEntry) = (BYTE)GetDlgItemInt(ghwndMain,
                                                 ID_EDIT_BOX, 
                                                 NULL, 
                                                 FALSE);
} //** UpdateKeyMap()

//************************************************************************
//**
//**  UpdateInstrument();
//**
//**  DESCRIPTION:
//**     This function will update the IDF file acording to the changes
//**     made by the user.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     VOID 
//**
//**  HISTORY:
//**     04/29/93       created.
//**     09/08/93       updated.
//**
//************************************************************************

VOID FNLOCAL UpdateInstrument(
   VOID)
{
   UINT  uEntry;

   // Disable the redrawing of the list box until we're done.
   //
   SetWindowRedraw(HWND_LIST_BOX, FALSE);

   // Which entry are we editing?
   //
   uEntry = ListBox_GetCurSel(HWND_LIST_BOX);

   // What section of the IDF are we editing?
   //
   switch (guCurrSelection)
   {
      case IDF_HEADER_INFO:
         // Update the header information.
         //
         UpdateHeaderInfo(uEntry);

         break;

      case IDF_INSTRUMENT_INFO:
         // Update the instrument information.
         //
         UpdateInstrumentInfo(uEntry);

         break;

      case IDF_INSTRUMENT_CAPS:
         // Update the midi capabilities information.
         //
         UpdateInstCaps(uEntry);

         break;

      case IDF_CHANNEL_TYPE:
         // Update the channel type.
         //
         UpdateChannelType(uEntry);

         break;

      case IDF_PATCH_MAPS:
         // Update the patch map.
         //
         UpdatePatchMap(uEntry);

         break;

      case IDF_KEY_MAPS:
         // Update the appropriate key map.
         //
         UpdateKeyMap(uEntry);
         
         break;
   }

   // Clear the current items from the list box.
   //
   ListBox_ResetContent(HWND_LIST_BOX);

   // Update the section.
   //
   UpdateListBox();

   // Set the current selection to the one we changed.
   //
   SetFocus(HWND_LIST_BOX);
   ListBox_SetCurSel(HWND_LIST_BOX, uEntry);

   // Enable the redrawing of the list box and invalidate 
   // it so that it does a re-draw.
   //
   
   SetWindowRedraw(HWND_LIST_BOX, TRUE);
   InvalidateRect(ghwndMain, NULL, FALSE);

} //** UpdateInstrument()

                     

