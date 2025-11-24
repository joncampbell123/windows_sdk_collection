//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  file.c
//**
//**  DESCRIPTION:
//**
//**
//**  HISTORY:
//**     06/14/93          created.
//**
//************************************************************************

#include "globals.h"
#include <commdlg.h>
#include <string.h>
#include "res.h"


//************************************************************************
//**
//**  AppGetFileName();
//**
//**  DESCRIPTION:
//**     This function is a wrapper for the Get[Open/Save]FileName commdlg
//**     chooser dialogs. Based on the fuFlags argument, this function 
//**     will display the appropriate chooser dialog and return the result.
//**
//**  ARGUMENTS:
//**     HWND    hwnd          - Handle to parent window.
//**     LPSTR   lpszFilePath  - Pointer to the buffer to receive the
//**                             the file path.
//**     LPSTR   lpszFileTitle - Pointer to the buffer to receive the 
//**                             file the file title, NULL if no title 
//**                             is wanted.
//**     BOOL     fSave        - TRUE if we are to save a file,
//**                             FALSE if we are to open the file.
//**
//**  RETURNS:
//**     BOOL  -  TRUE if a file was chosen. FALSE if the user canceled 
//**              the operation.
//**
//**  HISTORY:
//**     05/05/93       modified.
//**
//************************************************************************

BOOL FNLOCAL AppGetFileName(
    HWND    hwnd,
    LPSTR   lpszFilePath,
    LPSTR   lpszFileTitle,
    BOOL    fSave)
{
   OPENFILENAME   ofn;
   char           szExtDefault[4];
   char           szExtFilter[256];
   LPSTR          pstr;
   BOOL           f;

   //  Get the extension filter and default extension.
   //
   LoadString(ghinst, IDS_OFN_EXT_DEF, szExtDefault, sizeof(szExtDefault));
   LoadString(ghinst, IDS_OFN_EXT_FILTER, szExtFilter, sizeof(szExtFilter));

   // Parse the bang out of each string,
   // replace with a NULL.
   //
   for (pstr = szExtFilter; 
        '#' != *pstr;)
   {
      // Is it equal to a bang?
      //
      if ('!' == *pstr)
      {
         // Replace with a NULL.
         //
         *pstr = '\0';
         pstr++;
      }
      else
        pstr = AnsiNext(pstr);
   }

   // Reset the file's path.
   //
   lpszFilePath[0] = '\0';

   // If there is a title then reset it also.
   //
   if (lpszFileTitle)
      lpszFileTitle[0] = '\0';

   //  Initialize the OPENFILENAME structure elements.
   //
   memset(&ofn, 0, sizeof(ofn));

   ofn.lStructSize      =  sizeof(OPENFILENAME);
   ofn.hwndOwner        =  hwnd;
   ofn.lpstrFilter      =  szExtFilter;
   ofn.nFilterIndex     =  1L;
   ofn.lpstrFile        =  lpszFilePath;
   ofn.nMaxFile         =  MAX_PATH_LEN;
   ofn.lpstrFileTitle   =  lpszFileTitle;
   ofn.nMaxFileTitle    =  lpszFileTitle ? MAX_TITLE_LEN : 0;
   ofn.lpstrDefExt      =  szExtDefault;

   //  If the fSave is TRUE, then call GetSaveFileName()
   //  otherwise call GetOpenFileName().
   //
   if (fSave)
   {
      // Set the OPENFILENAME flags to save and prompt if we
      // will overwrite an existing file.
      //
      ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

      // Call the common dialog box function for saving a file.
      //
      f = GetSaveFileName(&ofn);
   }
   else
   {
      // Set the OPENFILENAME flags to open and the file 
      // must exist if we are opening.
      //
      ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

      // Call the common dialog box function for opening a file.
      //
      f = GetOpenFileName(&ofn);
   }

   return (f);
} //** AppGetFileName()


//************************************************************************
//**
//**  WriteHeaderChunk();
//**
//**  DESCRIPTION:
//**     This function will write the IDF header to the file.
//**
//**  ARGUMENTS:
//**     DWORD cInst   -  The instrument number to write.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful.
//**                 Otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL WriteHeaderChunk(
   DWORD cInst)
{
   MMRESULT    mmr;
   LPIDFHEADER pIDFHeader;
   MMCKINFO    chkSub;
   LPSTR       psz;
   DWORD       c;
   LONG        l;

   // Make the unique id for this instrument.
   //
   psz = (LPSTR)gpInst[cInst].pszProductASCII;

   // Store the product name with out spaces as the 
   // instrument's unique identifier.
   //
   c = 0;

   // psz will be NULL if they never specified a product name.
   //
   if (NULL != psz)
   {
       while (('\0' != *psz) && (c <= MAX_STR_LEN))
       {
           // If it's not a space then place it in the buffer.
           //
           if (*psz != ' ')
               gszbuf[c++] = *psz;

           // Get the next character.
           //
           psz++;
       }
   }
   
   // NULL terminate it.
   //
   gszbuf[c++] = '\0';

   // Allocate memory for the IDF header.
   //
   pIDFHeader= AllocPtr(GHND, sizeof(IDFHEADER) + c);
   if (NULL == pIDFHeader)
      return(IDFERR_NOMEM);

   // Fill the buffer with the IDFHEADER info.
   //
   pIDFHeader->cbStruct  = sizeof(IDFHEADER) + c;
   pIDFHeader->dwVersion = gpInst[cInst].dwVersion;
   pIDFHeader->dwCreator = gpInst[cInst].dwCreator;
   pIDFHeader->cbInstID  = c;
   lstrcpy((LPSTR)pIDFHeader->abInstID, gszbuf);
   

   // Setup the "hdr " chunk information.
   //
   chkSub.ckid = mmioFOURCC('h', 'd', 'r', ' ');
   chkSub.cksize = 0;
   
   //  Creating "fmt " Chunk in Destination File
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create header chunk.
      //
      FreePtr(&pIDFHeader);
      return(IDFERR_CANNOTCREATECHUNK);
   }
   
   // Write the "hdr " data to the file.
   //
   l = mmioWrite(ghmmio, (HPSTR)pIDFHeader, pIDFHeader->cbStruct);
   if (pIDFHeader->cbStruct != (DWORD)l)
   {
      // Error didn't write enough bytes.
      //
      FreePtr(&pIDFHeader);
      return(IDFERR_BADWRITE);
   }

   // Ascend out of the new chunk to fix up it's size.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   // Free the IDFHEADER that we allocated.
   //
   FreePtr(&pIDFHeader);

   return(MMSYSERR_NOERROR);
} //** WriteHeaderChunk()


//************************************************************************
//**
//**  WriteInstChunk();
//**
//**  DESCRIPTION:
//**     This function will write the instrument info to the file.
//**
//**  ARGUMENTS:
//**     DWORD cInst   -  The instrument number to write.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful.
//**                 Otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL WriteInstChunk(
   DWORD cInst)
{
   MMRESULT       mmr;
   LPIDFINSTINFO  pIDFInstrumentInfo;
   MMCKINFO       chkSub;
   DWORD          cbInstrumentChunk;
   LONG           l;

   // Determine the size that the IDFINSTINFO will need to be.
   // We add 1 to each string length since we need to include
   // the NULL in our count.
   //
   cbInstrumentChunk = sizeof(IDFINSTINFO) +
                       lstrlen(gpInst[cInst].pszManufactASCII) + 1 +
                       lstrlen(gpInst[cInst].pszManufactUNICODE) + 1 +
                       lstrlen(gpInst[cInst].pszProductASCII) + 1 +
                       lstrlen(gpInst[cInst].pszProductUNICODE) + 1;

   // Allocate memory for the instrument information.
   //
   pIDFInstrumentInfo = AllocPtr(GHND, cbInstrumentChunk);
   if (NULL == pIDFInstrumentInfo)
      return(IDFERR_NOMEM);

   // Build the IDF inst info.
   //
   pIDFInstrumentInfo->cbStruct         = cbInstrumentChunk;
   pIDFInstrumentInfo->dwManufactID     = gpInst[cInst].dwManufactID;
   pIDFInstrumentInfo->dwProductID      = gpInst[cInst].dwProductID;
   pIDFInstrumentInfo->dwRevision       = gpInst[cInst].dwRevision;
   pIDFInstrumentInfo->cbManufactASCII  = lstrlen(gpInst[cInst].pszManufactASCII) + 1;
   // Commented out since string aren't being filled in 
   //pIDFInstrumentInfo->cbManufactUNICODE  = lstrlen(gpInst[cInst].pszManufactUNICODE) + 1;
   pIDFInstrumentInfo->cbProductASCII   = lstrlen(gpInst[cInst].pszProductASCII) + 1;
   // Commented out since string aren't being filled in 
   //pIDFInstrumentInfo->cbProductUNICODE   = lstrlen(gpInst[cInst].pszProductUNICODE) + 1;

   // Copy the manufacturer and product names into the buffer.
   //
   lstrcpy((LPSTR)pIDFInstrumentInfo->abData, gpInst[cInst].pszManufactASCII);
   lstrcpy((LPSTR)&pIDFInstrumentInfo->abData[pIDFInstrumentInfo->cbManufactASCII], 
           gpInst[cInst].pszManufactUNICODE);

   lstrcpy((LPSTR)&pIDFInstrumentInfo->abData[pIDFInstrumentInfo->cbManufactASCII + pIDFInstrumentInfo->cbManufactUNICODE], 
           gpInst[cInst].pszProductASCII);

   lstrcpy((LPSTR)&pIDFInstrumentInfo->abData[pIDFInstrumentInfo->cbManufactASCII + pIDFInstrumentInfo->cbManufactUNICODE + pIDFInstrumentInfo->cbProductASCII], 
           gpInst[cInst].pszProductUNICODE);

   // Setup the "inst" chunk information.
   //
   chkSub.ckid = mmioFOURCC('i', 'n', 's', 't');
   chkSub.cksize = 0L;
   
   //  Creating a "inst" chunk in file.
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create instrument chunk.
      //
      FreePtr(&pIDFInstrumentInfo);
      return(IDFERR_CANNOTCREATECHUNK);
   }
   
   // Write "inst" the data to the file.
   //
   l = mmioWrite(ghmmio, 
                 (HPSTR)pIDFInstrumentInfo, 
                 pIDFInstrumentInfo->cbStruct);
   if (pIDFInstrumentInfo->cbStruct != (DWORD)l)
   {
      // Error didn't write enough bytes.
      //
      FreePtr(&pIDFInstrumentInfo);
      return(IDFERR_BADWRITE);
   }

   // Ascend out of the new chunk to fix up it's size.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   // Free the IDFINSTINFO that we allocated.
   //
   FreePtr(&pIDFInstrumentInfo);

   return(MMSYSERR_NOERROR);
} //** WriteInstChunk()


//************************************************************************
//**
//**  WriteCapsChunk();
//**
//**  DESCRIPTION:
//**     This function will write the instrument capabilities to the file.
//**
//**  ARGUMENTS:
//**     DWORD cInst   -  The instrument number to write.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful.
//**                 Otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL WriteCapsChunk(
   DWORD cInst)
{
   MMRESULT       mmr;
   IDFINSTCAPS    IDFInstrumentCaps;
   MMCKINFO       chkSub;
   DWORD          dw;

   // Fill the structure to write to the file.
   //
   IDFInstrumentCaps.cbStruct              = sizeof(IDFInstrumentCaps);
   IDFInstrumentCaps.fdwFlags              = gpInst[cInst].fdwFlags;
   IDFInstrumentCaps.dwBasicChannel        = gpInst[cInst].dwBasicChannel;
   IDFInstrumentCaps.cNumChannels          = gpInst[cInst].cNumChannels;
   IDFInstrumentCaps.cInstrumentPolyphony  = gpInst[cInst].cInstPoly;
   IDFInstrumentCaps.cChannelPolyphony     = gpInst[cInst].cChannelPoly;

   // Set up the "caps" chunk information.
   //
   chkSub.ckid = mmioFOURCC('c', 'a', 'p', 's');
   chkSub.cksize = 0;
   
   //  Creating a "inst" chunk in file.
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create instrument chunk.
      //
      return(IDFERR_CANNOTCREATECHUNK);
   }

   // Write the "caps" data to the file.
   //
   dw = mmioWrite(ghmmio, 
                  (HPSTR)&IDFInstrumentCaps, 
                  IDFInstrumentCaps.cbStruct);
   if (IDFInstrumentCaps.cbStruct != dw)
   {
      // Error didn't write enough bytes.
      //
      return(IDFERR_BADWRITE);
   }

   // Fix the chunk size.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   return(MMSYSERR_NOERROR);
} //** WriteCapsChunk()


//************************************************************************
//**
//**  WriteChannelChunk();
//**
//**  DESCRIPTION:
//**     This function will write the channel type info to the file.
//**
//**  ARGUMENTS:
//**     DWORD cInst   -  The instrument number to write.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful.
//**                 Otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL WriteChannelChunk(
   DWORD cInst)
{
   MMRESULT          mmr;
   IDFCHANNELHDR     IDFChannelHeader;
   LPCHANNEL         pChannel;
   IDFCHANNELINFO    IDFChannelInfo;
   MMCKINFO          chkSub;
   DWORD             c;
   LONG              l;
   DWORD             cbGeneralInitData;
   DWORD             cbDrumInitData;
   DWORD             cbIDFchnlinfo;
   LPIDFCHANNELINFO  lpIDFchnlinfo;
   DWORD             cbStruct;

   cbIDFchnlinfo = 0;
   lpIDFchnlinfo = NULL;
   
   // Initialize structures.
   //
   _fmemset(&IDFChannelHeader, 0, sizeof(IDFChannelHeader));
   _fmemset(&chkSub, 0, sizeof(chkSub));

   // Set the size of the channel header.
   //
   IDFChannelHeader.cbStruct = sizeof(IDFCHANNELHDR);

   // Map any undefined channels to GM.
   // BUGBUG - This should be an option!!!
   //
   IDFChannelHeader.fdwFlags |= IDFCHANNELHDR_F_GENERAL_MIDI;

   // How many non-GM channels do we have?
   //
   for ( c = 0;
         c < MAX_CHANNELS;
         c++)
   {
      if (NULL != gpInst[cInst].alpChannels[c])
      {
          IDFChannelHeader.cNumChannels++;
      }
   }

   // Setup the "chnl" chunk information.
   //
   chkSub.ckid = mmioFOURCC('c', 'h', 'n', 'l');
   
   //  Creating a "chnl" chunk in file.
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create instrument chunk.
      //
      return(IDFERR_CANNOTCREATECHUNK);
   }
   
   // Write the header to the file.
   //
   l = mmioWrite(ghmmio, (HPSTR)&IDFChannelHeader, IDFChannelHeader.cbStruct);
   if (IDFChannelHeader.cbStruct != (DWORD)l)
   {
      // Error didn't write enough bytes.
      //
      return(IDFERR_BADWRITE);
   }

   // Set the size for the channel info,
   // it will be the same for all channels.
   //
   IDFChannelInfo.cbStruct = sizeof(IDFCHANNELINFO);

   // Write the channel information to the file.
   // IFF the channel type is not GM.
   //
   for ( c = 0;
         c < MAX_CHANNELS; 
         c++)
   {
      if (NULL != (pChannel = gpInst[cInst].alpChannels[c]))
      {
          // Build a IDFCHANNELINFO and write it.
          //

          cbGeneralInitData = DWORD_ROUND(pChannel->cbGeneralInitData);
          cbDrumInitData    = DWORD_ROUND(pChannel->cbDrumInitData);

          cbStruct =
              sizeof(IDFCHANNELINFO) + cbGeneralInitData + cbDrumInitData;

          if (cbStruct > cbIDFchnlinfo)
          {
              if (0 == cbIDFchnlinfo)
              {
                  lpIDFchnlinfo = AllocPtr(GHND, cbStruct);
              }
              else
              {
                  lpIDFchnlinfo = ReAllocPtr(lpIDFchnlinfo, GHND, cbStruct);
              }

              if (NULL == lpIDFchnlinfo)
              {
                  return IDFERR_NOMEM;
              }
          }

          lpIDFchnlinfo->cbStruct           = cbStruct;
          lpIDFchnlinfo->dwChannel          = c;
          lpIDFchnlinfo->fdwChannel         = pChannel->fdwChannel;
          lpIDFchnlinfo->cbGeneralInitData  = pChannel->cbGeneralInitData;
          lpIDFchnlinfo->cbDrumInitData     = pChannel->cbDrumInitData;

          _fmemcpy(
              lpIDFchnlinfo->abData,
              pChannel->lpGeneralInitData,
              (size_t)cbGeneralInitData);

          _fmemcpy(
              lpIDFchnlinfo->abData + cbGeneralInitData,
              pChannel->lpDrumInitData,
              (size_t)cbDrumInitData);

          l = mmioWrite(ghmmio, (HPSTR)lpIDFchnlinfo, cbStruct);
          if ((DWORD)l != cbStruct)
          {
              FreePtr(&lpIDFchnlinfo);
              return IDFERR_BADWRITE;
          }
      }
   }

   // Ascend the chunk to correct the size info.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   FreePtr(&lpIDFchnlinfo);

   return(MMSYSERR_NOERROR);
} //** WriteChannelChunk()


//************************************************************************
//**
//**  WritePatchMapChunk();
//**
//**  DESCRIPTION:
//**     This function will write the patch maps to the file.
//**
//**  ARGUMENTS:
//**     DWORD cInst   -  The instrument number to write.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful.
//**                 Otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL WritePatchMapChunk(
   DWORD cInst)
{
   MMRESULT          mmr;
   IDFPATCHMAPHDR    IDFPatchMapHeader;
   LPBYTE            pbPatchMap;
   MMCKINFO          chkSub;
   LONG              l;

   // Get a nice pointer to the patch map information.
   //
   pbPatchMap = gpInst[cInst].aPatchMaps;

   // Initialize the structures.
   //
   _fmemset(&chkSub, 0, sizeof(chkSub));

   // Set the size of the patch map header.
   //
   IDFPatchMapHeader.cbStruct = sizeof(IDFPatchMapHeader);

   _fmemcpy(
       IDFPatchMapHeader.abPatchMap,
       pbPatchMap,
       sizeof(IDFPatchMapHeader.abPatchMap));
   
   // Setup the "pmap" chunk information.
   //
   chkSub.ckid = mmioFOURCC('p', 'm', 'a', 'p');

   //  Creating a "pmap" chunk in file.
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create instrument chunk.
      //
      return(IDFERR_CANNOTCREATECHUNK);
   }
   
   // Write the header to the file.
   //
   l = mmioWrite(ghmmio, (HPSTR)&IDFPatchMapHeader, IDFPatchMapHeader.cbStruct);
   if (IDFPatchMapHeader.cbStruct != (DWORD)l)
   {
      // Error didn't write enough bytes.
      //
      return(IDFERR_BADWRITE);
   }

   // Ascend the chunk to correct the size info.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   return(MMSYSERR_NOERROR);
} //** WritePatchMapChunk()


//*****************************************************************************
//**
//**  WriteKeyMapChunk();
//**
//**  DESCRIPTION:
//**
//**
//**  ARGUMENTS:
//**     DWORD cInst
//**
//**  RETURNS:
//**     MMRESULT 
//**
//**  HISTORY:
//**     09/10/93       created.
//**
//*****************************************************************************

MMRESULT FNLOCAL WriteKeyMapChunk(
   DWORD cInst)
{
   MMRESULT             mmr;
   IDFKEYMAPHDR         IDFKeyMapHeader;
   IDFKEYMAP            IDFKeyMap;
   MMCKINFO             chkSub;
   DWORD                cKeyMap;
   DWORD                dw;
   UINT                 iKeyMap;
   UINT                 iKeyValue;
   DWORD                dwKeyMapSaveMask;

   // Initialize the structures to 0.
   //
   _fmemset(&IDFKeyMapHeader, 0, sizeof(IDFKeyMapHeader));
   _fmemset(&IDFKeyMap, 0, sizeof(IDFKeyMap));
   _fmemset(&chkSub, 0, sizeof(chkSub));

   // Determine the number of key maps that are not 1:1 mappings (i.e.
   // anything work storing).
   //
   cKeyMap = 0;
   dwKeyMapSaveMask = 0;
   
   for (iKeyMap = 0; iKeyMap < CHAN_TYPES; iKeyMap++)
   {
       LPBYTE   pThisMap = gpInst[cInst].aKeyMaps[iKeyMap];
       
       for (iKeyValue = 0; iKeyValue < MAX_KEY_MAPS; iKeyValue++)
       {
           if (pThisMap[iKeyValue] != (BYTE)iKeyValue)
           {
               ++cKeyMap;
               dwKeyMapSaveMask |= (1L << iKeyMap);
               break;
           }
       }
   }

   // Ok, we know how many - so we can build the key map header
   // and write it.
   //
   IDFKeyMapHeader.cbStruct     = sizeof(IDFKeyMapHeader);
   IDFKeyMapHeader.cNumKeyMaps  = cKeyMap;
   IDFKeyMapHeader.cbKeyMap     = sizeof(IDFKeyMap);

   // Setup the "key " chunk information.
   //
   chkSub.ckid = mmioFOURCC('k', 'e', 'y', ' ');
   
   //  Creating chunk in the file.
   //
   mmr = mmioCreateChunk(ghmmio, &chkSub, 0);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Error could not create instrument chunk.
      //
      return(IDFERR_CANNOTCREATECHUNK);
   }
   
   // Write the header to the file.
   //
   dw = mmioWrite(ghmmio, (HPSTR)&IDFKeyMapHeader, IDFKeyMapHeader.cbStruct);
   if (IDFKeyMapHeader.cbStruct != dw)
   {
      // Error didn't write enough bytes.
      //
      return(IDFERR_BADWRITE);
   }
   
   // Enumerate the key maps we need to save and save them
   //
   IDFKeyMap.cbStruct = sizeof(IDFKeyMap);
   
   for (iKeyMap = 0; iKeyMap < CHAN_TYPES; iKeyMap++)
   {
       LPBYTE   pThisMap = gpInst[cInst].aKeyMaps[iKeyMap];
       
       if (dwKeyMapSaveMask | (1L << iKeyMap))
       {
           switch(iKeyMap)
           {
               case CHAN_GENERAL:
                   IDFKeyMap.fdwKeyMap = IDFKEYMAP_F_GENERAL_CHANNEL;
                   break;

               case CHAN_DRUM:
                   IDFKeyMap.fdwKeyMap = IDFKEYMAP_F_DRUM_CHANNEL;
                   break;

               default:
                   return IDFERR_BADNUMBER;
           }

           _fmemcpy(
               IDFKeyMap.abKeyMap,
               pThisMap,
               sizeof(IDFKeyMap.abKeyMap));     

           dw = mmioWrite(ghmmio, (HPSTR)&IDFKeyMap, IDFKeyMap.cbStruct);
           if (IDFKeyMap.cbStruct != dw)
           {
               // Error didn't write enough bytes.
               //
               return(IDFERR_BADWRITE);
           }
       }
   }
                                  
   // Ascend the chunk to correct the size info.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   return(MMSYSERR_NOERROR);
} //** WriteKeyMapChunk()


//************************************************************************
//**
//**  SaveConfigFile();
//**
//**  DESCRIPTION:
//**     This function will save the current IDF file in memory under 
//**     the name that is currently in gszIDFName.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if the file was successfully saved.
//**                 Otherwise an error code is returned.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL SaveConfigFile(
   VOID)
{
   MMRESULT mmr;
   MMCKINFO chkRIFF;
   MMCKINFO chkList;
   DWORD    c;

   // Attempt to open the file, create it if necessary.
   //
   ghmmio = mmioOpen(gszIDFName, 
                     NULL, 
                     MMIO_CREATE | MMIO_ALLOCBUF | MMIO_READWRITE);
   if (NULL == ghmmio)
   {
      // Error could not create the file.
      //
      return(IDFERR_CANNOTCREATEFILE);
   }

   // Create the main RIFF chunk
   //
   chkRIFF.fccType = mmioFOURCC('I', 'D', 'F', ' ');
   chkRIFF.cksize = 0L;
   
   mmr = mmioCreateChunk(ghmmio, &chkRIFF, MMIO_CREATERIFF);
   if (MMSYSERR_NOERROR != mmr)
   {
       mmioClose(ghmmio, 0);
       return IDFERR_CANNOTCREATECHUNK; 
   }

   // Loop through each instrument and save the data.
   //
   for ( c = 0; 
         c < gdwNumInsts; 
         c++)
   {
      // Set the LIST chunk type.
      //
      chkList.fccType = mmioFOURCC('M', 'M', 'A', 'P');

      // Initialize the size to zero.
      //
      chkList.cksize = 0L;

      // Create the list chunk "MMAP".
      //
      mmr = mmioCreateChunk(ghmmio, &chkList, MMIO_CREATELIST);
      if (MMSYSERR_NOERROR != mmr)
      {
         // Return an error code that we have a message for.
         //
         mmr = IDFERR_CANNOTCREATECHUNK;
         break;
      }
      
      // Write header chunk.
      // 
      mmr = WriteHeaderChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Write instrument information chunk.
      //
      mmr = WriteInstChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Write the instrument capabilities chunk.
      //
      mmr = WriteCapsChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Write the channel types chunk.
      //
      mmr = WriteChannelChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Write the patch map chunk.
      //
      mmr = WritePatchMapChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Write the key map chunk.
      //
      mmr = WriteKeyMapChunk(c);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Ascend chunk to fix chunk size.
      //
      mmioAscend(ghmmio, &chkList, 0);
   }

   // Ascend out of the main RIFF chunk
   //
   mmioAscend(ghmmio, &chkRIFF, 0);

   // Close the file.
   //
   mmioClose(ghmmio, 0);
   ghmmio = NULL;

   return(mmr);
} //** SaveConfigFile()


//************************************************************************
//**
//**  SaveConfigFileAs();
//**
//**  DESCRIPTION:
//**     This function will allow the user to save the current IDF file
//**     under a different name.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if the function either saves the
//**                 file correctly or if the user does _NOT_ choose a
//**                 name to save the file as.
//**
//**  HISTORY:
//**     04/29/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL SaveConfigFileAs(
   VOID)
{
   MMRESULT mmr;
   char     szTitle[MAX_TITLE_LEN+1];
   char     szName[MAX_PATH_LEN+1];
   BOOL     f;

   // Get the name to save the file as.
   //
   f = AppGetFileName(ghwndMain, szName, szTitle, TRUE);
   if (f)
   {
      // Save the temp's.
      //
      lstrcpy(gszIDFTitle, szTitle);
      lstrcpy(gszIDFName, szName);

      // Try and save the file.
      //
      mmr = SaveConfigFile();
      if (MMSYSERR_NOERROR != mmr)
         return(mmr);

      // Set the windows title.
      //
      SetWindowTitle();
   }

   return(MMSYSERR_NOERROR);
} //** SaveConfigFileAs()


//************************************************************************
//**
//**  ReadHeaderChunk();
//**
//**  DESCRIPTION:
//**     This function will read the IDF header from the file.
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent   -  Pointer to the parent chunk.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful. Otherwise it will
//**                 return an error code.
//**
//**  HISTORY:
//**     05/03/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL ReadHeaderChunk(
   LPMMCKINFO pchkParent)
{
   LPIDFHEADER pIDFHeader;
   MMRESULT    mmr;
   MMCKINFO    chkSub;
   LONG        l;

   // We are looking for the instruments header chunk.
   //
   chkSub.ckid = mmioFOURCC('h', 'd', 'r', ' ');
   
   // Descend to the "hdr " chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "hdr " chunk, now check it's size and
   // see if it is one that we can read.
   // We check to make sure that the size of the chunks is
   // greater than a IDFHEADER, this ensures that the IDF
   // has some sort of unique name at the end.
   //
   if (sizeof(IDFHEADER) >= chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADCHUNK);
   }

   // Allocate memory for the header.
   //
   pIDFHeader = AllocPtr(GHND, chkSub.cksize);
   if (NULL == pIDFHeader)
   {
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_NOMEM);
   }

   // Read in the whole chunk into our buffer.
   //
   l = mmioRead(ghmmio, (HPSTR)pIDFHeader, chkSub.cksize);
   if (chkSub.cksize != (DWORD)l)
   {
      // We didn't read in the amount of data that was
      // expected, return in error.
      //
      FreePtr(&pIDFHeader);
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADREAD);
   }

   // Save the header info in the current instrument.
   //
   gpInst[gdwCurrInst].dwVersion = pIDFHeader->dwVersion;
   gpInst[gdwCurrInst].dwCreator = pIDFHeader->dwCreator;

   // Currently we only deal with MAX_STR_LEN number of bytes
   // for the unique name even though it is possible that 
   // the unique name can be longer.  Someone might update this
   // sometime....
   //
   _fstrncpy((LPSTR)gpInst[gdwCurrInst].szInstID,
             (LPSTR)pIDFHeader->abInstID, 
             (UINT)min(pIDFHeader->cbInstID, MAX_STR_LEN));

   // The strcpy that we use won't add this if our array
   // is to small for the whole name.
   //
   gpInst[gdwCurrInst].szInstID[MAX_STR_LEN] = '\0';

   // Ascend out of the chunk.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   // Free the memory used by the IDFHEADER.
   //
   FreePtr(&pIDFHeader);

   // Return success.
   //
   return(MMSYSERR_NOERROR);
} //** ReadHeaderChunk()


//************************************************************************
//**
//**  ReadInstChunk();
//**
//**  DESCRIPTION:
//**     This function will read the instrument information chunk from
//**     the IDF file.
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent   -  Pointer to the parent chunk.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful. Otherwise it will
//**                 return an error code.
//**
//**  HISTORY:
//**     05/03/93       created.
//**     09/10/93       updated this and that.
//**
//************************************************************************

MMRESULT FNLOCAL ReadInstChunk(
   LPMMCKINFO pchkParent)
{
   IDFINSTINFO    IDFInstInfo;
   LPSTR          pszManufactASCII      = NULL;
   LPSTR          pszManufactUNICODE    = NULL;
   LPSTR          pszProductASCII       = NULL;
   LPSTR          pszProductUNICODE     = NULL;
   MMRESULT       mmr;
   MMCKINFO       chkSub;
   LONG           l;

   // We are looking for the instrument information chunk.
   //
   chkSub.ckid = mmioFOURCC('i', 'n', 's', 't');
   
   // Descend to the "inst" chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "inst" chunk, now check it's size and
   // see if it is one that we can read.
   //
   if (sizeof(IDFINSTINFO) > chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADCHUNK);
   }

   // Read in the standard instrument data,
   // don't read in the name bytes yet.
   //
   l = mmioRead(ghmmio, (HPSTR)&IDFInstInfo, sizeof(IDFInstInfo));
   if (sizeof(IDFInstInfo) != (DWORD)l)
   {
      // We didn't read in the amount of data that was
      // expected, return in error.
      //
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADREAD);
   }

   // Build the inst info struct.
   //
   gpInst[gdwCurrInst].dwManufactID = IDFInstInfo.dwManufactID;
   gpInst[gdwCurrInst].dwProductID  = IDFInstInfo.dwProductID;
   gpInst[gdwCurrInst].dwRevision   = IDFInstInfo.dwRevision;

   // Allocate memory for the ASCII version of the
   // instrument's manufacturer name.
   //

   if (0 != IDFInstInfo.cbManufactASCII)
   {
      pszManufactASCII = AllocPtr(GHND, IDFInstInfo.cbManufactASCII);
      if (NULL == pszManufactASCII)
      {
         // We could not allocate memory for the string.
         //
         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_NOMEM);
      }
   }

   // Allocate memory for the UNICODE version of the
   // instrument's manufacturer name.
   //

   // Added if statement.  No IDFs have unicode strings yet so this was
   // always failing 
   if (0 != IDFInstInfo.cbManufactUNICODE)
   {
       pszManufactUNICODE = AllocPtr(GHND, IDFInstInfo.cbManufactUNICODE);
       if (NULL == pszManufactUNICODE)
       {
           // We could not allocate memory for the string.
           //
           FreePtr(&pszManufactASCII);

           mmioAscend(ghmmio, &chkSub, 0);
           return(IDFERR_NOMEM);
       }
   }

   // Allocate memory for the ASCII version of the
   // instrument's product name.
   //

   if (0 != IDFInstInfo.cbProductASCII)
   {
      pszProductASCII = AllocPtr(GHND, IDFInstInfo.cbProductASCII);
      if (NULL == pszProductASCII)
      {
         // We could not allocate memory for the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_NOMEM);
      }
   }

   // Allocate memory for the UNICODE version of the
   // instrument's product name.
   //
   // Added if statement.  No IDFs have unicode strings yet so this was
   // always failing when reading in IDFs 
   if (0 != IDFInstInfo.cbProductUNICODE)
   {
      pszProductUNICODE = AllocPtr(GHND, IDFInstInfo.cbProductUNICODE);
      if (NULL == pszProductUNICODE)
      {
         // We could not allocate memory for the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);
         FreePtr(&pszProductASCII);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_NOMEM);
      }
   }


   // Read in the string for the ASCII version of the 
   // instrument's manufacturer name.
   //
   if (0 != IDFInstInfo.cbManufactASCII)
   {
      l = mmioRead(ghmmio, (HPSTR)pszManufactASCII, IDFInstInfo.cbManufactASCII);
      if (IDFInstInfo.cbManufactASCII != (DWORD)l)
      {
         // We could not read in the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);
         FreePtr(&pszProductASCII);
         FreePtr(&pszProductUNICODE);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_BADREAD);
      }
   }

   // Read in the string for the UNICODE version of the 
   // instrument's manufacturer name.
   //
   // Added if statement.  No IDFs have unicode strings yet so this was
   // always failing 
   if (0 != IDFInstInfo.cbManufactUNICODE)
   {
      l = mmioRead(ghmmio, (HPSTR)pszManufactUNICODE, IDFInstInfo.cbManufactUNICODE);
      if (IDFInstInfo.cbManufactUNICODE != (DWORD)l)
      {
         // We could not read in the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);
         FreePtr(&pszProductASCII);
         FreePtr(&pszProductUNICODE);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_BADREAD);
       }
   }

   // Read in the string for the ASCII version of the 
   // instrument's product name.
   //
   if (0 != IDFInstInfo.cbProductASCII)
   {
      l = mmioRead(ghmmio, (HPSTR)pszProductASCII, IDFInstInfo.cbProductASCII);
      if (IDFInstInfo.cbProductASCII != (DWORD)l)
      {
         // We could not read in the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);
         FreePtr(&pszProductASCII);
         FreePtr(&pszProductUNICODE);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_BADREAD);
      }
   }

   // Read in the string for the UNICODE version of the 
   // instrument's product name.
   //
   // Added if statement.  No IDFs have unicode strings yet so this was
   // always failing when reading in IDFs 
   if (0 != IDFInstInfo.cbProductUNICODE)
   {
      l = mmioRead(ghmmio, (HPSTR)pszProductUNICODE, IDFInstInfo.cbProductUNICODE);
      if (IDFInstInfo.cbProductUNICODE != (DWORD)l)
      {
         // We could not read in the string.
         //
         FreePtr(&pszManufactASCII);
         FreePtr(&pszManufactUNICODE);
         FreePtr(&pszProductASCII);
         FreePtr(&pszProductUNICODE);

         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_BADREAD);
      }
   }

   // Complete the inst info struct. 
   //
   gpInst[gdwCurrInst].pszManufactASCII = pszManufactASCII;
   gpInst[gdwCurrInst].pszManufactUNICODE = pszManufactUNICODE;
   gpInst[gdwCurrInst].pszProductASCII = pszProductASCII;
   gpInst[gdwCurrInst].pszProductUNICODE = pszProductUNICODE;

   mmioAscend(ghmmio, &chkSub, 0);

   return(MMSYSERR_NOERROR);
} //** ReadInstChunk()


//************************************************************************
//**
//**  ReadCapsChunk();
//**
//**  DESCRIPTION:
//**     This function will read the instrument capabilities chunk from
//**     the IDF file.
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent   -  Pointer to the parent chunk.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful. Otherwise it will
//**                 return an error code.
//**
//**  HISTORY:
//**     05/03/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL ReadCapsChunk(
   LPMMCKINFO pchkParent)
{
   IDFINSTCAPS    IDFInstCaps;
   MMRESULT       mmr;
   MMCKINFO       chkSub;
   LONG           l;

   // We are looking for the instrument capabilities chunk.
   //
   chkSub.ckid = mmioFOURCC('c', 'a', 'p', 's');
   
   // Descend to the "caps" chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "caps" chunk, now check it's size and
   // see if it is one that we can read.
   //
   if (sizeof(IDFInstCaps) != chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADCHUNK);
   }

   // Read the instrument's capabilities from the file.
   //
   l = mmioRead(ghmmio, (HPSTR)&IDFInstCaps, sizeof(IDFInstCaps));
   if (sizeof(IDFInstCaps) != l)
   {
      // We didn't read in the amount of data that was
      // expected, return in error.
      //
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADREAD);
   }

   // Save the IDF instrument capabilities.
   //
   gpInst[gdwCurrInst].dwBasicChannel = IDFInstCaps.dwBasicChannel;
   gpInst[gdwCurrInst].cNumChannels   = IDFInstCaps.cNumChannels;
   gpInst[gdwCurrInst].cInstPoly      = IDFInstCaps.cInstrumentPolyphony;
   gpInst[gdwCurrInst].cChannelPoly   = IDFInstCaps.cChannelPolyphony;
   gpInst[gdwCurrInst].fdwFlags       = IDFInstCaps.fdwFlags;

   // Ascend out of the capabilities chunk.
   //
   mmioAscend(ghmmio, &chkSub, 0);

   return(mmr);
} //** ReadCapsChunk()


//************************************************************************
//**
//**  ReadChannelChunk();
//**
//**  DESCRIPTION:
//**     This function will read the channel type chunk from the IDF file.
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent   -  Pointer to the parent chunk.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful. Otherwise it will
//**                 return an error code.
//**
//**  HISTORY:
//**     05/03/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL ReadChannelChunk(
   LPMMCKINFO pchkParent)
{
   MMRESULT          mmr;
   MMRESULT          mmrRet;
   MMCKINFO          chkSub;
   IDFCHANNELHDR     IDFChannelHeader;
   DWORD             cbStruct;
   DWORD             cbIDFchnlinfo;
   LPIDFCHANNELINFO  lpIDFchnlinfo;
   DWORD             dwChannel;
   DWORD             c;
   LONG              l;
   DWORD             cbGeneralInitData;
   DWORD             cbDrumInitData;
   LPVOID            pv;
   
   // Default return value
   //
   mmrRet = MMSYSERR_NOERROR;
   lpIDFchnlinfo = NULL;
   
   // We are looking for the instrument channel definitions.
   //
   chkSub.ckid = mmioFOURCC('c', 'h', 'n', 'l');
   
   // Descend to the "chnl" chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "chnl" chunk, now check it's size and
   // make sure it's at least as big as a IDFCHANNELHDR.
   //
   if (sizeof(IDFCHANNELHDR) > chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmrRet = IDFERR_BADCHUNK;
      goto Read_Channel_Chunk_Err;
   }

   // Read the channel header in.
   //
   l = mmioRead(ghmmio, (HPSTR)&IDFChannelHeader, sizeof(IDFChannelHeader));
   if (sizeof(IDFChannelHeader) != l)
   {
      // Couldn't read in all of the header.
      //
      mmrRet = IDFERR_BADREAD;
      goto Read_Channel_Chunk_Err;
   }

   // Are there channels defined in the IDF?
   //
   if (0 == IDFChannelHeader.cNumChannels)
   {
      // Nothing else to read here.
      //
      goto Read_Channel_Chunk_Err;
   }

   // Read all the channels that are defined in the IDF.
   //
   cbIDFchnlinfo = 0;
   for ( c = 0; 
         c < IDFChannelHeader.cNumChannels; 
         c++)
   {
      // Read the cbStruct of this channel so we know how much to grab
      //
      l = mmioRead(ghmmio, (HPSTR)&cbStruct, sizeof(cbStruct));
      if (sizeof(cbStruct) != l || cbStruct <= sizeof(cbStruct))
      {
          mmrRet = IDFERR_BADREAD;
          goto Read_Channel_Chunk_Err;
      }

      // Make sure temporary lpIDFchnlinfo is big enough to hold chunk
      //
      if (cbStruct > cbIDFchnlinfo)
      {
          if (0 == cbIDFchnlinfo)
          {
              pv = AllocPtr(GHND, cbStruct);
          }
          else
          {
              pv = ReAllocPtr(lpIDFchnlinfo, GHND, cbStruct);
          }
          if (NULL == pv)
          {
              if (NULL != lpIDFchnlinfo)
                  FreePtr(&lpIDFchnlinfo);
              
              mmrRet = IDFERR_NOMEM;
              goto Read_Channel_Chunk_Err;
          }

          lpIDFchnlinfo = pv;
      }
      
      // Read the channel type information from the IDF.
      //
      cbStruct -= sizeof(cbStruct);
      l = mmioRead(ghmmio,
                   ((HPSTR)lpIDFchnlinfo) + sizeof(cbStruct),
                   cbStruct);
      if (cbStruct != (DWORD)l)
      {
         // Didn't read the correct amount of information.
         //
         mmrRet = IDFERR_BADREAD;
         goto Read_Channel_Chunk_Err;
      }

      // What channel is the non-general type for?
      //
      dwChannel = lpIDFchnlinfo->dwChannel;
      
      // Allocate a block off of the instrument structure and store
      // the relevant information.
      //
      if (NULL != CURR_CHANNEL(dwChannel))
      {
          FreePtr(&(CURR_CHANNEL(dwChannel)));
      }

      cbGeneralInitData = DWORD_ROUND(lpIDFchnlinfo->cbGeneralInitData);
      cbDrumInitData    = DWORD_ROUND(lpIDFchnlinfo->cbDrumInitData);

      if (NULL == (CURR_CHANNEL(dwChannel) =
          AllocPtr(GHND, sizeof(CHANNEL) + cbGeneralInitData + cbDrumInitData)))
      {
          mmrRet = IDFERR_NOMEM;
          goto Read_Channel_Chunk_Err;
      }

      CURR_CHANNEL(dwChannel)->lpGeneralInitData =
          ((LPSTR)(CURR_CHANNEL(dwChannel))) + sizeof(CHANNEL);

      CURR_CHANNEL(dwChannel)->lpDrumInitData =
          CURR_CHANNEL(dwChannel)->lpGeneralInitData + cbGeneralInitData;

      _fmemcpy(
          CURR_CHANNEL(dwChannel)->lpGeneralInitData,
          lpIDFchnlinfo->abData,
          (size_t)cbGeneralInitData);
      
      _fmemcpy(
          CURR_CHANNEL(dwChannel)->lpDrumInitData,
          lpIDFchnlinfo->abData + cbGeneralInitData,
          (size_t)cbDrumInitData);

      CURR_CHANNEL(dwChannel)->fdwChannel        = lpIDFchnlinfo->fdwChannel;
      CURR_CHANNEL(dwChannel)->cbGeneralInitData = lpIDFchnlinfo->cbGeneralInitData;
      CURR_CHANNEL(dwChannel)->cbDrumInitData    = lpIDFchnlinfo->cbDrumInitData;
   }

Read_Channel_Chunk_Err:

   mmioAscend(ghmmio, &chkSub, 0);

   if (NULL != lpIDFchnlinfo)
   {
       FreePtr(&lpIDFchnlinfo);
   }
   
   return(mmrRet);
} //** ReadChannelChunk()


//************************************************************************
//**
//**  ReadPatchMapChunk();
//**
//**  DESCRIPTION:
//**     This function will read the patch maps chunk from the IDF file.
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent   -  Pointer to the parent chunk.
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if successful. Otherwise it will
//**                 return an error code.
//**
//**  HISTORY:
//**     05/03/93       created.
//**
//************************************************************************

MMRESULT FNLOCAL ReadPatchMapChunk(
   LPMMCKINFO pchkParent)
{
   MMRESULT          mmr;
   MMCKINFO          chkSub;
   IDFPATCHMAPHDR    IDFPatchMapHeader;
   LONG              l;

   // We are looking for the patch map for the instrument.
   //
   chkSub.ckid = mmioFOURCC('p', 'm', 'a', 'p');
   
   // Descend to the "pmap" chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "pmap" chunk, now check it's size and
   // make sure it's at least as big as a IDFPATCHMAPHDR.
   //
   if (sizeof(IDFPATCHMAPHDR) > chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADCHUNK);
   }

   // Read the channel header in.
   //
   l = mmioRead(ghmmio, (HPSTR)&IDFPatchMapHeader, sizeof(IDFPatchMapHeader));
   if (sizeof(IDFPatchMapHeader) != l)
   {
      // Couldn't read in all of the header.
      //
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADREAD);
   }

   _fmemcpy(
       gpInst[gdwCurrInst].aPatchMaps,
       IDFPatchMapHeader.abPatchMap,
       sizeof(gpInst[gdwCurrInst].aPatchMaps));
           

   mmioAscend(ghmmio, &chkSub, 0);

   // Return success.
   //
   return(MMSYSERR_NOERROR);
} //** ReadPatchMapChunk()


//*****************************************************************************
//**
//**  ReadKeyMapChunk();
//**
//**  DESCRIPTION:
//**     This function will read in the key maps for the drum channels.
//**
//**
//**  ARGUMENTS:
//**     LPMMCKINFO pchkParent
//**
//**  RETURNS:
//**     MMRESULT 
//**
//**  HISTORY:
//**     09/10/93       created.
//**
//*****************************************************************************

MMRESULT FNLOCAL ReadKeyMapChunk(
   LPMMCKINFO pchkParent)
{
   MMRESULT             mmr;
   MMCKINFO             chkSub;
   IDFKEYMAPHDR         IDFKeyMapHeader;
   IDFKEYMAP            IDFKeyMap;
   DWORD                c;
   LONG                 l;
   UINT                 iKeyMap;
   UINT                 iKeyValue;
   
   // Initialize the in-memory key maps to default values before
   // we try reading anything. Default is a 1:1 nul mapping.
   //
   for (iKeyMap = 0; iKeyMap < CHAN_TYPES; iKeyMap++)
   {
       LPBYTE   pThisMap = gpInst[gdwCurrInst].aKeyMaps[iKeyMap];

       for (iKeyValue = 0; iKeyValue < MAX_KEY_MAPS; iKeyValue++)
       {
           *pThisMap++ = (BYTE)iKeyValue;
       }
   }

   // We are looking for the key map header for the instrument.
   //
   chkSub.ckid = mmioFOURCC('k', 'e', 'y', ' ');
   
   // Descend to the "key " chunk in this list.
   //
   mmr = mmioDescend(ghmmio, &chkSub, pchkParent, MMIO_FINDCHUNK);
   if (MMSYSERR_NOERROR != mmr)
   {
      // Could not find the chunk.
      //
      return(IDFERR_CANNOTFINDCHUNK);
   }

   // We found the "key " chunk, now check it's size and
   // make sure it's at least as big as a IDFKEYMAPHDR.
   //
   if (sizeof(IDFKEYMAPHDR) > chkSub.cksize)
   {
      // The sizeof the IDF header is not what we expected.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADCHUNK);
   }

   // Read the channel header in.
   //
   l = mmioRead(ghmmio, (HPSTR)&IDFKeyMapHeader, sizeof(IDFKeyMapHeader));
   if (sizeof(IDFKeyMapHeader) != l)
   {
      // Couldn't read in all of the header.
      //
      mmioAscend(ghmmio, &chkSub, 0);
      return(IDFERR_BADREAD);
   }

   // Are there any patches defined that we need to read?
   //
   if (0 == IDFKeyMapHeader.cNumKeyMaps)
   {
      // There are no key maps defined in the IDF.
      // 
      mmioAscend(ghmmio, &chkSub, 0);
      return(MMSYSERR_NOERROR);
   }

   // Read all the patch maps that are defined.
   //
   for ( c = 0; 
         c < IDFKeyMapHeader.cNumKeyMaps; 
         c++)
   {
      // Read the key map channel information.
      //
      l = mmioRead(ghmmio, 
                   (HPSTR)&IDFKeyMap, 
                   sizeof(IDFKeyMap));
      if (sizeof(IDFKeyMap) != l)
      {
         // We didn't read in the key map's channel information
         // correctly.
         //
         mmioAscend(ghmmio, &chkSub, 0);
         return(IDFERR_BADREAD);
      }

      // Figure out if this key map is of a type we know about.
      //
      switch(IDFKeyMap.fdwKeyMap)
      {
          case IDFKEYMAP_F_GENERAL_CHANNEL:
              iKeyMap = CHAN_GENERAL;
              break;
              
          case IDFKEYMAP_F_DRUM_CHANNEL:
              iKeyMap = CHAN_DRUM;
              break;

          default:
              mmioAscend(ghmmio, &chkSub, 0);
              return IDFERR_BADCHUNK;
      }

      // We understand the chunk, copy it.
      //
      _fmemcpy(
          gpInst[gdwCurrInst].aKeyMaps[iKeyMap],
          IDFKeyMap.abKeyMap,
          sizeof(gpInst[gdwCurrInst].aKeyMaps[iKeyMap]));
   }

   mmioAscend(ghmmio, &chkSub, 0);

   // Return success.
   //
   return(MMSYSERR_NOERROR);
} //** ReadKeyMapChunk()


//************************************************************************
//**
//**  OpenConfigFile();
//**
//**  DESCRIPTION:
//**     This function will open and read in a IDF file.
//**
//**  ARGUMENTS:
//**     VOID
//**
//**  RETURNS:
//**     MMRESULT -  MMSYSERR_NOERROR if there was not an error,
//**                 otherwise it will return an error code.
//**
//**  HISTORY:
//**     04/22/93       created.
//**     09/10/93       changed a few things.
//**
//************************************************************************

MMRESULT FNLOCAL OpenConfigFile(
   VOID)
{
   MMRESULT       mmr;
   char           szTitle[MAX_TITLE_LEN+1];
   char           szName[MAX_PATH_LEN+1];
   HMMIO          hmmio;
   MMCKINFO       chkRIFF;
   MMCKINFO       chkParent;
   BOOL           f;

   // Get the file name to open.
   //
   f = AppGetFileName(ghwndMain, szName, szTitle, FALSE);
   if (!f)
   {
      // We didn't get a name.
      // But this is _NOT_ always an error so let return in a good state.
      //
      return(MMSYSERR_NOERROR);
   }

   // Open the file for reading.
   //
   hmmio = mmioOpen(szName, NULL, MMIO_ALLOCBUF | MMIO_READ);
   if (NULL == hmmio)
   {
      // Error could not create the file.
      //
      return(IDFERR_CANNOTCREATEFILE);
   }

   // Disable redrawing of the list box while we load the 
   // new IDF.
   //
   SetWindowRedraw(HWND_LIST_BOX, FALSE);

   // Clear the list box of it's current contents
   // in preparation for the new file.
   //
   ListBox_ResetContent(HWND_LIST_BOX);

   // Clean up the current editing session.
   //
   CleanUp();

   // Now save our temp stuff.
   //
   ghmmio = hmmio;

   // Descend into the main "RIFF" chunk
   //
   chkRIFF.fccType = mmioFOURCC('I', 'D', 'F', ' ');
   mmr = mmioDescend(ghmmio, &chkRIFF, NULL, MMIO_FINDRIFF);
   if (MMSYSERR_NOERROR != mmr)
   {
       CleanUp();
       return(mmr);
   }

   // Now loop through all of the "MMAP" list chunks and load them in.
   //
   for (;;)
   {
      // Search for a "MMAP" list chunk.
      //
      chkParent.fccType = mmioFOURCC('M', 'M', 'A', 'P');

      mmr = mmioDescend(ghmmio, &chkParent, &chkRIFF, MMIO_FINDLIST);
      if (MMIOERR_CHUNKNOTFOUND == mmr)
      {
         // We couldn't find one so we are done.
         // We must reset the return code so that it doesn't show
         // an error.
         //
         mmr = MMSYSERR_NOERROR;
         break;
      }

      // Allocate a new IDF struct.
      //
      mmr = NewInstrument();
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the header chunk.
      //
      mmr = ReadHeaderChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the instrument info chunk.
      //
      mmr = ReadInstChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the instrument caps chunk.
      //
      mmr = ReadCapsChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the channel type chunk.
      //
      mmr = ReadChannelChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the patch map chunk.
      //
      mmr = ReadPatchMapChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      // Read in the key map chunk.
      //
      mmr = ReadKeyMapChunk(&chkParent);
      if (MMSYSERR_NOERROR != mmr)
         break;

      mmioAscend(ghmmio, &chkParent, 0);
   }

   // Did we succeed?
   //
   if (MMSYSERR_NOERROR != mmr)
   {
      // We failed somewhere.
      //
      CleanUp();
      return(mmr);
   }

   // Save the name of the file.
   //
   lstrcpy(gszIDFName, szName);
   lstrcpy(gszIDFTitle, szTitle);

   // Setup the dialog box for the new IDF file
   //
   SetupDialog();

   // Close the file.
   //
   mmioClose(ghmmio, 0);
   ghmmio = NULL;

   return(MMSYSERR_NOERROR);
} //** OpenConfigFile()




