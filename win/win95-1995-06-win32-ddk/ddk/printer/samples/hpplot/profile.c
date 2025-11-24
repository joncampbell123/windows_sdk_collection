/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

                                  profile

*******************************************************************************
*******************************************************************************

This unit defines all functions that deal with the configuration of the current
environment. */

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "driver.h"
#include "dialog.h"
#include "profile.h"
#include "utils.h"

/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL static

/* **************************** Local Data ********************************* */

typedef struct {
    char  PenInfo [120],         /* Pen information */
          ActiveCarousels [12],  /* Active carousels */
          Size[8];               /* Media size */
    short CurrentCarousel,       /* Current carousel */
          PaperFeed,             /* Paper feed */
          Preloaded,             /* Preloaded */
          Draft,                 /* Draft */
          Orientation;           /* Orientation */
} DEFPROFILE;

LOCAL DEFPROFILE DefaultDeviceInfo[NUM_PLOTTERS] =
{
    /* ColorPro */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "0",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* ColorPro with GEC */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "0",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7470A */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "0",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7475A */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "0",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7550A */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "0",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7580A */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "3",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7585A */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7580B */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "3",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7585B */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP 7586B */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* DraftPro */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "3",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP DRAFTPRO DXL */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "3",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* HP DRAFTPRO EXL */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* DraftMaster I */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                    /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    },
    /* DraftMaster II */
    {
        "10 0 0 0 0 0 3 0 0 0 0 0 4 0 0 0 0 0 1 0 0 0 0 0 9 0 0 0 0 0 5 0 0 0 0 0 6 0 0 0 0 0 2 0 0 0 0 0",
        "1",                   /* Active carousels */
        "4",                    /* Media size */
        0,                      /* Current carousel */
        0,                      /* Paper Feed */
        0,                      /* Preloaded */
        1,                      /* Orientation */
        0                       /* Draft */
    }
};

LOCAL char NoPen[] = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";

LOCAL char ProfileString [140];   /* Current profile string read */

/* *************************** Exported Data ******************************* */
/* *************************** Local Routines ****************************** */

LOCAL void NEAR PASCAL breakdown_string (String,Values,MaxValues)
  /* This function will convert a series of space seperated character strings
     into their integer counterparts and place them into an integer array
     pointed to by Values.  If the end of the string is reached prior to the
     array being filled, the remainder of the array is filled with zeros. */
LPSTR String;
LPINT Values;
short MaxValues;
{
    short nValues = 0;

    while (nValues < MaxValues)
    {
        if (*(String) != 0)

        // Shell has a function called StrToInt so we can't have a private
        // function of the same name.  Since I don't know exactly how
        // StrToInt works, I'll have to make a guess.  I will eat whitespace
        // before and after the number to convert.  MikeSh 30Aug93.
        {
            int i;

            while (*String == ' ')
                String++;
            i = 0;
            while (*String >= '0' && *String <= '9')
                i = 10 * i + *String++ - '0';
                
            *Values++ = i;
            while (*String == ' ')
                String++;
        }

        else
            *(Values++) = 0;
        nValues++;
    }
}


void FAR PASCAL get_defaults (LPENVIRONMENT lpSetup, BOOL bPaperDlg)
  /* This function will return the defaults for the currently selected device.
     The defaults are equivilant to the defaults used for a reading a devices
     profile when none exists in the WIN.INI file. */
{

   if (bPaperDlg)
   {
    // this section needs to make sure that the device independent part of
    // the DEVMODE is set in conjunction with the device dependent part

    lpSetup->PaperFeed = DefaultDeviceInfo [lpSetup->Plotter].PaperFeed;
    lpSetup->dm.dmDefaultSource = MANUAL + lpSetup->PaperFeed;

    lpSetup->Orientation = DefaultDeviceInfo [lpSetup->Plotter].Orientation;
    lpSetup->dm.dmOrientation = (lpSetup->Orientation == 0) ? 
                             DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

    lpSetup->Draft = DefaultDeviceInfo [lpSetup->Plotter].Draft;
    lpSetup->dm.dmPrintQuality = (lpSetup->Draft == 1) ? DMRES_DRAFT : DMRES_HIGH;

    breakdown_string ((LPSTR) DefaultDeviceInfo [lpSetup->Plotter].Size,(LPINT)
      &lpSetup->Size,2);

    lpSetup->dm.dmPaperSize = SIZE_A + lpSetup->Size;

    lpSetup->Preloaded = DefaultDeviceInfo [lpSetup->Plotter].Preloaded;
   }
   else
   {
      short Index;

      lpSetup->CurrentCarousel = DefaultDeviceInfo [lpSetup->Plotter].CurrentCarousel;

      breakdown_string((LPSTR) DefaultDeviceInfo [lpSetup->Plotter].
                        ActiveCarousels,(LPINT) lpSetup->ActiveCarousel,6);

      breakdown_string((LPSTR) DefaultDeviceInfo [lpSetup->Plotter].PenInfo,
                       (LPINT) &lpSetup->Carousel [0].Pen [0].Color,48);
      for (Index = 1; Index < MAX_CAROUSELS; Index++)
         breakdown_string((LPSTR) NoPen,(LPINT) &lpSetup->Carousel [Index].Pen
                          [0].Color,48);
   }
}

//--------------------------*MakeAppName*-----------------------------------//
// Action:  concatenate lpFirst and lpSecond in the firm First,Second. Or,
//          simply copy Second to the target is lpFirst is NULL. Either way,
//          strip the trailing colon.
//
//      Return the length of the total string. Return -1 if fails.
//
//--------------------------------------------------------------------------//

short FAR PASCAL MakeAppName(LPSTR lpTarget,
                             LPSTR lpFirst,
                             LPSTR lpSecond,
                             short max)
{
    short length=0;
    LPSTR lpTmp=lpTarget;

    if(lpFirst)
        length+=lstrlen(lpFirst);

    length+=lstrlen(lpSecond);

    if(!length || length > (max - 2))
        return -1;

    if(lpFirst && *lpFirst)
    {
        while(*lpTmp++ = *lpFirst++)
            ;

        *(lpTmp-1)=',';
        length++;
    }

    while(*lpTmp++ = *lpSecond++)
        ;

    // Strip the trailing colon, if any--subtract 2, since lpTmp was
    // incremented after copying the terminating NULL.
    if(':' == *(lpTmp-2))
    {
        *(lpTmp-2)='\0';
        length--;
    }

    return length;
}


int FAR PASCAL get_profile (lpDevice,lpSetup,lpPort,lpProfile)
  /* This function will read the current environment configuration for the
     given device.  lpDevice is a string pointer to the plotter name.  If the
     current environment is not configured for the given device, the device
     configuration will be retrieved from the WIN.INI file. */
LPSTR lpDevice;
LPENVIRONMENT lpSetup;
LPSTR lpPort;
LPSTR lpProfile;
{
    short SelDevice = 0;       /* Selected device */
    char  Keyword [5];         /* Carousel/pen keyword construct */

    // char szAppName[MAX_STRING_LENGTH];

    while (lstrcmp ((LPSTR) lpDevice,(LPSTR) GetString (COLORPRO + SelDevice))
      && SelDevice < NUM_PLOTTERS)
        SelDevice++;

    // if device out of range use ColorPro as default
    if (SelDevice >= NUM_PLOTTERS)
      SelDevice = 0;

    // make driver look in [modelname,port]
    // MakeAppName(szAppName, lpDevice, lpPort, sizeof(szAppName));
    // well, good thought, but apparently the 3.1 driver is just modelname

    lpSetup->Plotter = SelDevice;

    lpSetup->CurrentCarousel = GetOneProfileInt ((LPSTR) lpDevice,
      (LPSTR) "CurrentCarousel", -1,
      lpProfile);

    if (lpSetup->CurrentCarousel==-1)
        return -1;

    lpSetup->PaperFeed = GetOneProfileInt ((LPSTR) lpDevice,
      (LPSTR) "PaperFeed", DefaultDeviceInfo[SelDevice].PaperFeed,lpProfile);

    lpSetup->Preloaded = GetOneProfileInt ((LPSTR) lpDevice,
      (LPSTR) "Preloaded", DefaultDeviceInfo[SelDevice].Preloaded, lpProfile);

    lpSetup->Orientation = GetOneProfileInt ((LPSTR) lpDevice,
      (LPSTR) "Orientation", DefaultDeviceInfo[SelDevice].Orientation,lpProfile);

    lpSetup->Draft = GetOneProfileInt ((LPSTR) lpDevice,(LPSTR) "Draft",
      DefaultDeviceInfo[SelDevice].Draft,lpProfile);

    GetOneProfileString ((LPSTR) lpDevice,(LPSTR) "ActiveCarousels",
      (LPSTR) DefaultDeviceInfo[SelDevice].ActiveCarousels,
      (LPSTR) ProfileString, 20,lpProfile);
    breakdown_string ((LPSTR) ProfileString,(LPINT) lpSetup->ActiveCarousel, 6);

    GetOneProfileString ((LPSTR) lpDevice,(LPSTR) "Size",
      (LPSTR) DefaultDeviceInfo[SelDevice].Size,
      (LPSTR) ProfileString, 6,lpProfile);
    breakdown_string ((LPSTR) ProfileString,(LPINT) &lpSetup->Size, 2);

    {
        short Carousel;
        LPSTR lpDefProfile;

        Keyword[0] = 'C';
        for (Carousel = 0; Carousel < MAX_CAROUSELS; Carousel++)
        {
            ltoa((LPSTR) &Keyword[1],Carousel+1);
            if (Carousel == 0)
                lpDefProfile = DefaultDeviceInfo[SelDevice].PenInfo;
            else
                lpDefProfile = NoPen;
            GetOneProfileString ((LPSTR) lpDevice, (LPSTR) Keyword,
              (LPSTR) lpDefProfile, (LPSTR) ProfileString, 139,lpProfile);
            breakdown_string ((LPSTR) ProfileString,
              (LPINT) &lpSetup->Carousel[Carousel].Pen[0].Color,48);
        }
    }

    // get rid of this, too

    WriteProfileString("HPPLOT", NULL, NULL);
    return 1;
}

LOCAL void NEAR PASCAL build_string (String,Values,MaxValues)
  /* This function will convert an integer array pointed to by Values into a
     series of space seperated character strings pointed to by String.  The
     integer array is MaxValues in length. */
LPSTR String;
LPINT Values;
short MaxValues;
{
    short nValues,
          length;

    for (nValues = 0; nValues < MaxValues; nValues++)
    {
        length = (short)ltoa((LPSTR) String, *(Values++));
        *(String + length) = ' ';
        String += (length + 1);
    }
    *(String) = '\0';
}
// This function will save the current environment table to the desired 
// profile. 

void FAR PASCAL save_profile (LPDIALOGINFO lpdi)
{
   short Carousel;
   char  Keyword [5];
   char  szAppName[MAX_STRING_LENGTH];

   MakeAppName(szAppName, lpdi->lpDM->dm.dmDeviceName, lpdi->szPort, sizeof(szAppName));

   ltoa((LPSTR) ProfileString,lpdi->lpDM->CurrentCarousel);
   SaveOneProfileString(szAppName, (LPSTR) "CurrentCarousel",(LPSTR) ProfileString, lpdi->lpProfile);

   ltoa((LPSTR) ProfileString,lpdi->lpDM->PaperFeed);
   SaveOneProfileString (szAppName, (LPSTR) "PaperFeed",(LPSTR) ProfileString, lpdi->lpProfile);

   ltoa((LPSTR) ProfileString,lpdi->lpDM->Preloaded);
   SaveOneProfileString (szAppName, (LPSTR) "Preloaded",(LPSTR) ProfileString, lpdi->lpProfile);

   ltoa((LPSTR) ProfileString,lpdi->lpDM->Orientation);
   SaveOneProfileString (szAppName, (LPSTR) "Orientation",(LPSTR) ProfileString, lpdi->lpProfile);

   ltoa((LPSTR) ProfileString,lpdi->lpDM->Draft);
   SaveOneProfileString (szAppName, (LPSTR) "Draft",(LPSTR) ProfileString, lpdi->lpProfile);

   build_string ((LPSTR) ProfileString,(LPINT) lpdi->lpDM->ActiveCarousel,6);
   SaveOneProfileString (szAppName, (LPSTR) "ActiveCarousels",(LPSTR) ProfileString, lpdi->lpProfile);

   build_string ((LPSTR) ProfileString,(LPINT) &lpdi->lpDM->Size,2);
   SaveOneProfileString (szAppName, (LPSTR) "Size",(LPSTR) ProfileString, lpdi->lpProfile);

   // write carousel info
   Keyword [0] = 'C';
   for (Carousel = 0; Carousel < MAX_CAROUSELS; Carousel++)
   {
      ltoa((LPSTR) &Keyword [1],Carousel+1);
      build_string ((LPSTR) ProfileString,
      (LPINT) &lpdi->lpDM->Carousel[Carousel].Pen[0].Color,48);
      SaveOneProfileString (szAppName, (LPSTR) Keyword,(LPSTR) ProfileString, lpdi->lpProfile);
   }
}


void SaveOneProfileString(LPSTR lpAppName, LPSTR lpEntryName,  LPSTR lpString, LPSTR lpProfile)
{
   if (lpProfile == NULL)
       // write to WIN.INI
       WriteProfileString(lpAppName, lpEntryName, lpString);
   else
       // write to the given profile
       WritePrivateProfileString(lpAppName, lpEntryName, lpString, lpProfile);
}

void GetOneProfileString(LPSTR lpAppName, LPSTR lpEntryName, LPSTR lpDefault,
                         LPSTR lpBuf, int iBufSize, LPSTR lpProfile)
{
   if (lpProfile == NULL)
       // read to WIN.INI
       GetProfileString(lpAppName, lpEntryName, lpDefault, lpBuf, iBufSize);
   else
       // read to the given profile
       GetPrivateProfileString(lpAppName, lpEntryName, lpDefault, lpBuf,
                               iBufSize, lpProfile);
}

int GetOneProfileInt(LPSTR lpAppName, LPSTR lpEntryName, int iDefault, LPSTR lpProfile)
{
   int iRet;

   if (lpProfile == NULL)
       // write to WIN.INI
       iRet = GetProfileInt(lpAppName, lpEntryName, iDefault);
   else
       // write to the given profile
       iRet = GetPrivateProfileInt(lpAppName, lpEntryName, iDefault, lpProfile);

   return (iRet);
}
