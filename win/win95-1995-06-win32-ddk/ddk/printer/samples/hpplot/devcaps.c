//*************************************************************
//  File name: devcaps.c
//
//  Description:
//
//*************************************************************

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>
#include "device.h"
#include "driver.h"
#include "color.h"
#include "control.h"
#include "dialog.h"
#include "glib.h"
#include "initlib.h"
#include "object.h"
#include "output.h"
#include "print.h"
#include "profile.h"
#include "text.h"
#include "utils.h"
#include "devmode.h"
#include "data.h"
#include <memory.h>

//*************************************************************
//
//  DeviceCapabilities
//
//  Purpose: Responsible for reporting what the driver supports.
//              
//
//
//  Parameters:
//      lpDevName
//      lpPort
//      wIndex
//      lpOutput
//      lpdm
//
//
//  Return: (DWORD FAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD FAR PASCAL DeviceCapabilities(lpDevName, lpPort, wIndex, lpOutput, lpdm)
LPSTR       lpDevName;   // the device name.
LPSTR       lpPort;      // the device's associated port.
WORD        wIndex;      // index of the device capability to query.
LPSTR       lpOutput;    // an array of bytes to store the output.
LPDEVMODE lpdm;     // current device mode.
{
    ENVIRONMENT dmTemp; // temporary storage for the default device mode
    DWORD nRetValue;

    // Handle cases that always return constants. Note that we don't
    // validate the lpDevName, so if lpDevName isn't a valid printer
    // name, these calls will succeed, even though others will fail.
    // (AddPrinter will require that DC_VERSION succeeds, no matter
    // what lpDevName is, since this is how AddPrinter finds out
    // what version the printer driver is, and whether or not the
    // driver can handle friendly names)
    
    switch(wIndex)
    {
	case DC_SIZE:
	    return sizeof(DEVMODE);

	case DC_EXTRA:
	    return (sizeof(ENVIRONMENT) - sizeof(DEVMODE));

	case DC_VERSION:
	    return 0x400;

	case DC_DRIVER:
	    return 100;
    }

    // check to see if input DEVMODE was passed in.  if not get the
    // current state of the driver
    if (lpdm == NULL)
    {
	if (ExtDeviceMode((HWND)NULL, NULL, &dmTemp, lpDevName, lpPort,
			  NULL, (LPSTR)NULL, DM_COPY) == -1)
	    return (0);

	lpdm = (LPDEVMODE)&dmTemp.dm;
    }

    switch (wIndex)
    {
      case DC_FIELDS:
	 // return the DEVMODE's bitfield
	      nRetValue = (DWORD)lpdm->dmFields;
	      break;

      case DC_PAPERNAMES:
      case DC_PAPERSIZE:
		case DC_PAPERS:
	 return (GetPaperInfo((LPENVIRONMENT)lpdm, lpOutput, wIndex));

		case DC_BINS:
		case DC_BINNAMES:
	 return (GetBinInfo((LPENVIRONMENT)lpdm, lpOutput, wIndex));

	   case DC_ENUMRESOLUTIONS:
	      // return the list of resolutions supported by the device.
	      // Each resolution is represented by a pair of DWORD's for
	      // the X and Y values respectively.
	      // If lpOutput is null, simply return the # of resolution supported.

	 if (lpOutput)
	 {
	    LPLONG lVal = (LPLONG)lpOutput;

	    *(lVal++) = 203;
	    *lVal     = 203;
	 }
	 nRetValue = 1;
	      break;

	   case DC_ORIENTATION:
	 // return degrees rotated for landscape
		   nRetValue = (DWORD)90;
	      break;

	   case DC_COPIES:
	      // return the maximum number of copies the device can print.
		   nRetValue = (DWORD) 1;
	      break;

	   default:
	      return (DWORD)-1L;
	}
   return(nRetValue);

} //*** DeviceCapabilities

//*************************************************************
//
//  GetPaperInfo
//
//  Purpose: Fills the DC_PAPERS, DC_PAPERSIZE, or
//           DC_PAPERNAMES output buffers with the appropriate
//           info
//
//
//  Parameters:
//      LPENVIRONMENT lpEnv
//      LPSTR lpBuf
//      WORD wIndex
//
//
//  Return: (DWORD NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD NEAR PASCAL GetPaperInfo(LPENVIRONMENT lpEnv, LPSTR lpBuf, WORD wIndex)
{
   int i;
   DWORD dwCount = 0;

   // find the number of supported paper sizes
   for (i=0; i < MAX_MEDIA_SIZES; i++)
   {
      if (DeviceInfo[lpEnv->Plotter].MediaSizeSupport[i])
      {
	 dwCount++;
	 if (lpBuf)
	 {
	    switch(wIndex)
	    {
	       case DC_PAPERNAMES:
		  lstrcpy(lpBuf, (LPSTR)GetString(SIZE_A + i));
		  lpBuf += CCHPAPERNAME;
		  break;

	       case DC_PAPERS:
	       {
		  LPWORD lpID = (LPWORD)lpBuf;

		  *lpID = SIZE_A + i;
		  lpBuf += sizeof(WORD);
	       }
	       break;

	       case DC_PAPERSIZE:
		  _fmemcpy(lpBuf, (LPSTR)&PaperSize[i], sizeof(POINT));
		  lpBuf += sizeof(POINT);
		  break;

	       default:
		  break;
	    }
	 }
      }
   }
   return dwCount;

} //*** GetPaperInfo

//*************************************************************
//
//  GetBinInfo
//
//  Purpose: Fills the DC_BINS or DC_BINNAMES output buffers
//           with the appropriate info
//
//
//  Parameters:
//      LPENVIRONMENT lpEnv
//      LPSTR lpBuf
//      WORD wIndex
//
//
//  Return: (DWORD NEAR PASCAL)
//
//
//  Comments:
//
//
//  History:    Date       Author     Comment
//
//*************************************************************

DWORD NEAR PASCAL GetBinInfo(LPENVIRONMENT lpEnv, LPSTR lpBuf, WORD wIndex)
{
   int i;
   DWORD dwCount = 0;

   for (i=0; i < MAX_FEEDS; i++)
   {
      if ((i == 0) || (i == DeviceInfo[lpEnv->Plotter].PaperFeedSupport))
      {
	 dwCount++;
	 if (lpBuf)
	 {
	    switch(wIndex)
	    {
	       case DC_BINS:
	       {
		  LPWORD lpID = (LPWORD)lpBuf;

		  *lpID = MANUAL + i;
		  lpBuf += sizeof(WORD);
	       }
	       break;

	       case DC_BINNAMES:
		  lstrcpy(lpBuf, (LPSTR)GetString (MANUAL + i));
		  lpBuf += CCHBINNAME;
		  break;

	       default:
		  break;
	    }
	 }
      }
   }
   return dwCount;

} //*** GetBinInfo

/*** EOF: devcaps.c ***/
