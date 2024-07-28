/****************************************************************************
 *
 *   initc.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#define NOSTR  /* to avoid redefining strings */
#include "sndblst.h"

#define DEF_MIDIINPERSISTENCE   50      /* default MIDI in persistence factor */

/*****************************************************************************

    internal function prototypes

 ****************************************************************************/ 

static WORD NEAR PASCAL GetProfileHex(LPSTR szApp, LPSTR szEntry, WORD wDef);

/****************************************************************************

    strings

    most strings are only accessed at init time, so they will be placed
    in the _INIT code segment.

 ***************************************************************************/

#define BCODE _based(_segname("_CODE"))
#define BDATA _based(_segname("_DATA"))

/* non-localized strings */
char BCODE STR_PORT[]           = "port";
char BCODE STR_INT[]            = "int";
char BCODE STR_DRIVERNAME[]     = "sndblst.drv";
char BCODE STR_INIFILE[]        = "system.ini";
char BCODE STR_PRODUCTNAME[]    = "Sound Blaster";
static char BCODE STR_NULL[]        = "";
static char BCODE STR_DMACHANNEL[]  = "dmachannel";
static char BCODE STR_PERSISTENCE[] = "midiinpersistence";
static char BCODE STR_NOWARNING[]   = "nowarning";
static char BCODE STR_VERIFYINT[]   = "verifyint";
static char BCODE STR_EVILTWIN[]    = "SNDBLST1";

#ifdef DEBUG
    static char BCODE STR_SNDBLST[] = "sndblst";
    static char BCODE STR_MMDEBUG[] = "mmdebug";

    /* these strings are accessed at interrupt time, so they must be in */
    /* the fixed DS */
    char BDATA STR_CRLF[]       = "\r\nSNDBLST: ";
    char BDATA STR_SPACE[]      = " ";
#endif

/****************************************************************************

    public data

 ***************************************************************************/

HANDLE  ghModule        = NULL;         /* our module handle */
WORD    gwErrorStringId = 0;            /* if initialization fails, string id */
BYTE    gfVerifyInt     = FALSE;

#ifdef DEBUG
WORD    wDebugLevel = 0;        /* debug level */
#endif

/***************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | GetProfileHex | Get a profile hex value.
 *
 * @parm LPSTR | szAppName | The app name.
 *
 * @parm LPSTR | szEntry | The entry name.
 *
 * @parm WORD | wDefault | The default value.
 *
 * @rdesc Returns the profile value or the default if none is present.
 ***************************************************************************/
static WORD NEAR PASCAL GetProfileHex(LPSTR szApp, LPSTR szEntry, WORD wDef)
{
BYTE  buf[20];
WORD  n;
BYTE  b;
int   i;

    n = GetPrivateProfileString(szApp, szEntry, STR_NULL, buf, sizeof(buf), STR_INIFILE);
    if (n < 1) return wDef;

    for (n = 0, i = 0; b = buf[i]; i++) {
        if (b > 'Z') b -= 'a' - 'A';
        b -= '0';
        if (b > 9) b -= 7;
        if (b > 15)
            break;
        n = n * 16 + b;
    }
    return n;
}

/***************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | ConfigGetPortBase |
 *
 * @rdesc Returns the port base from SYSTEM.INI
 ***************************************************************************/
WORD NEAR PASCAL ConfigGetPortBase(void)
{
WORD    wPort;

    /* read szIniFile and get the board configuration information. */

    wPort = GetProfileHex(STR_DRIVERNAME, STR_PORT, -1);
    switch (wPort) {
        case 0x200:
        case 0x210:
        case 0x220:
        case 0x230:
        case 0x240:
        case 0x250:
        case 0x260:
        case 0x270:
            break;

        default:
            wPort = -1;
            D1("driver PORT not configured");
    }

    return (wPort);
}


/***************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | ConfigGetIRQ |
 *
 * @rdesc Returns the IRQ ('int') from SYSTEM.INI
 ***************************************************************************/
BYTE NEAR PASCAL ConfigGetIRQ(void)
{
BYTE    bInt;

    bInt = (BYTE)GetPrivateProfileInt(STR_DRIVERNAME, STR_INT, -1, STR_INIFILE);

    switch (bInt) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 10:
            break;

        case 2:
            bInt = 9;
            break;

        default:
            bInt = -1;
            D1("driver INT not configured");
            break;
    }

    return (bInt);
}


/***************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | ConfigGetDMAChannel |
 *
 * @rdesc Returns the DMA channel from SYSTEM.INI
 ***************************************************************************/
BYTE NEAR PASCAL ConfigGetDMAChannel(void)
{
BYTE    bDMAChannel;

    /* get the DMA channel that the card is using... */
    bDMAChannel = (BYTE)GetPrivateProfileInt(STR_DRIVERNAME, STR_DMACHANNEL,
                                -1, STR_INIFILE);

    switch (bDMAChannel) {
        case 0:
        case 1:
        case 3:
            break;

        default:
            bDMAChannel = 1;
    }

    return (bDMAChannel);
}

/***************************************************************************/

static WORD NEAR PASCAL GetWindowsVersionCorrectly()
{
WORD w;

    w = LOWORD(GetVersion());

    return (w << 8) | (w >> 8);
}

/***************************************************************************/

static void NEAR PASCAL HardErrorMsgBox(WORD wStringId)
{
char szErrorBuffer[MAX_ERR_STRING]; /* buffer for error messages */

    /*  Starting with Windows 3.1, it is ok to bring up a _hard system modal_ */
    /*  message box during LibInit.  In Windows 3.0, this will not work! */
    if (GetWindowsVersionCorrectly() >= 0x30A) {
        LoadString(ghModule, wStringId, szErrorBuffer, sizeof(szErrorBuffer));
        MessageBox(NULL, szErrorBuffer, STR_PRODUCTNAME, MB_OK|MB_SYSTEMMODAL|MB_ICONHAND);
    }
}

BOOL FAR PASCAL InitDisplayConfigErrors(void)
{
char szErrorBuffer[MAX_ERR_STRING]; /* buffer for error messages */

    if (gwErrorStringId) {
        LoadString(ghModule, gwErrorStringId, szErrorBuffer, sizeof(szErrorBuffer));
        MessageBox(NULL, szErrorBuffer, STR_PRODUCTNAME, MB_OK|MB_SYSTEMMODAL|MB_ICONHAND);
        gwErrorStringId = 0;
    }

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | LibMain | Library initialization code.
 *
 * @parm HANDLE | hModule | Our module handle.
 *
 * @parm WORD | wHeapSize | The heap size from the .def file.
 *
 * @parm LPSTR | lpCmdLine | The command line.
 *
 * @rdesc Returns 1 if the initialization was successful and 0 otherwise.
 ***************************************************************************/
int NEAR PASCAL LibMain(HANDLE hModule, WORD wHeapSize, LPSTR lpCmdLine)
{
WORD    wPort;
BYTE    bInt;
BYTE    bDMAChannel;
WORD    wMidiInPersistence;
WORD    wError;
DWORD   dw;

#ifdef DEBUG
    /* get debug level - default is 0 */
    wDebugLevel = GetProfileInt(STR_MMDEBUG, STR_SNDBLST, 0);
#endif

    D1("LibMain");

    /* save our module handle */
    ghModule = hModule;

    /* we have a good chance of hanging if two sound blaster drivers are */
    /* enabled */
    if (GetModuleHandle(STR_EVILTWIN)) {
        D1("EVIL TWIN SNDBLST1 LOCATED!!  WE WILL NOT ENABLE!!!");
        wError = IDS_ERRTWODRIVERS;
        goto libmain_Severe_Error;
    }

    /* do preliminary stuff--and check for hardware problems... */
    if (wError = InitPreliminary())
        goto libmain_Severe_Error;


    /* get the DMA channel that the card is using... */
    wPort = ConfigGetPortBase();
    bInt = ConfigGetIRQ();
    bDMAChannel = ConfigGetDMAChannel();

    /* over-ride switch in case someone has problems... */
    gfVerifyInt = (BYTE)GetPrivateProfileInt(STR_DRIVERNAME, STR_VERIFYINT, 1, STR_INIFILE);


    /* verify port, IRQ, DMA channel and DSP version
     *
     *  The return value is:
     *      LOWORD() =  error code. it is 0 if no error, otherwise it is
     *                  the IDS_xxxxxx error define (and hi-word will be
     *                  zero).
     *      HIWORD() =  DSP version.
     */
    dw = InitVerifyConfiguration(wPort, bInt, bDMAChannel);

    /* was there an error? */
    if (LOWORD(dw)) {
        /*  display error, but allow load to succeed.  we will NOT enable,
         *  but we need to allow someone to configure us in the drivers applet.
         */
        gwErrorStringId = IDS_ERRBADCONFIG;
        return 1;
    }

    /*  iff the DSP version is too old, then do NOT load driver! otherwise */
    /*  let it be loaded so it can be configured (but not enabled). */
    wError = HIWORD(dw);
    if (wError < DSP_VERSION_REQD) {
        wError = IDS_ERRBADVERSION;

libmain_Severe_Error:

        /* display error and FAIL to load... */
        HardErrorMsgBox(wError);
        return 0;
    }

    /* get MIDI input persistence... */
    wMidiInPersistence = GetPrivateProfileInt(STR_DRIVERNAME, STR_PERSISTENCE,
                                DEF_MIDIINPERSISTENCE, STR_INIFILE);

    /* set the configuration */
    InitSetConfiguration(wPort, bInt, bDMAChannel, wMidiInPersistence);

    /* load driver (but maybe not enable if configuration is wrong). */
    return 1;
}
