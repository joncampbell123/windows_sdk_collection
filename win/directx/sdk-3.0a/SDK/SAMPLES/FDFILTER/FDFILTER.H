#ifndef __FDFILTER_H__
#define __FDFILTER_H__


#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <d3dtypes.h>

#include "debug.h"
#include "resource.h"

class CFilter;
class CGargle;

#define REG_SETTINGS_KEY            TEXT("\\Software\\Microsoft\\DFilter")
#define REG_SETTING_INPUTDEVICE     TEXT("InputDevice");
#define REG_SETTING_OUTPUTDEVICE    TEXT("OutputDevice");
#define REG_SETTING_INFORMAT        TEXT("InputFormat");
#define REG_SETTING_OUTFORMAT       TEXT("OutputFormat");

typedef struct tag_dsdevdesc
{
    GUID    guDevice;               // Device GUID
    PSTR    pszDeviceDesc;          // Description string
    struct tag_dsdevdesc *pNext;
} DSDEVICEDESC, *PDSDEVICEDESC;


typedef struct tag_wavedevdesc
{
    UINT    uID;                        // Device ID
    char    szDeviceDesc[MAXPNAMELEN];  // Description string
    DWORD   dwFormats;
    WORD    nChannels;
    struct tag_wavedevdesc *pNext;
} WAVEDEVICEDESC, *PWAVEDEVICEDESC;

typedef struct tag_fd
{
    DWORD   dwCode;
    BOOL    fEnabled;
} FORMATDATA, *PFORMATDATA;

#define NUM_FORMATCODES     (16)

#define FC_GETFREQCODE(fc)      ((fc) / 1000)
#define FC_GETBITS(fc)          ((fc) % 100)
#define FC_GETCHANNELS(fc)      (((fc) % 1000) / 100)

#define NUM_BUFFERS             (16)


#ifdef __DEFINE_GLOBAL_VARIABLES__

FORMATDATA   aInputFormats[NUM_FORMATCODES] = {{ 8108, TRUE }, { 8208, TRUE },
                                    { 8116, TRUE }, { 8216, TRUE },
                                    { 11108, TRUE }, { 11208, TRUE },
                                    { 11116, TRUE }, { 11216, TRUE },
                                    { 22108, TRUE }, { 22208, TRUE },
                                    { 22116, TRUE }, { 22216, TRUE },
                                    { 44108, TRUE }, { 44208, TRUE },
                                    { 44116, TRUE }, { 44216, TRUE }};

FORMATDATA   aOutputFormats[NUM_FORMATCODES] = {{ 8108, TRUE }, { 8208, TRUE },
                                    { 8116, TRUE }, { 8216, TRUE },
                                    { 11108, TRUE }, { 11208, TRUE },
                                    { 11116, TRUE }, { 11216, TRUE },
                                    { 22108, TRUE }, { 22208, TRUE },
                                    { 22116, TRUE }, { 22216, TRUE },
                                    { 44108, TRUE }, { 44208, TRUE },
                                    { 44116, TRUE }, { 44216, TRUE }};

LPDIRECTSOUND       gpds;
LPDIRECTSOUNDBUFFER gpdsbPrimary;
LPWAVEFORMATEX      gpwfxInput, gpwfxOutput;


char                gszAppName[] = "FDFilter";
char                gszAppDescription[] = "Full-Duplex Filter Sample";
HINSTANCE           ghInst;
HWND                ghMainWnd;
HWAVEIN             ghWaveIn;
HANDLE              ghCallbackFinished;

DWORD               gdwOutputFormat, gdwInputFormat, gcbBufferSize;
PWAVEDEVICEDESC     gpwddInputDevices, gpwddIn;
PDSDEVICEDESC       gpdsddOutputDevices, gpdsddOut;

CRITICAL_SECTION    gcsBufferData;
int                 gnBuffersOut;
PWAVEHDR            gawhHeaders;
PBYTE               gpbBufferData;
LONG                gfBuffersInitialized;
LPDIRECTSOUNDBUFFER gpdsbOutput;
DSBUFFERDESC        gdsbdOutput;
CFilter *           gpFilter;

#else

extern FORMATDATA           aInputFormats[], aOutputFormats[];
extern LPDIRECTSOUND        gpds;
extern LPDIRECTSOUNDBUFFER  gpdsbPrimary;
extern LPWAVEFORMATEX       gpwfxInput, gpwfxOutput;


extern char                 gszAppName[];
extern char                 gszAppDescription[];
extern HINSTANCE            ghInst;
extern HWND                 ghMainWnd;
extern HWAVEIN              ghWaveIn;
extern HANDLE               ghCallbackFinished;

extern DWORD                gdwOutputFormat, gdwInputFormat, gcbBufferSize;
extern PWAVEDEVICEDESC      gpwddInputDevices, gpwddIn;
extern PDSDEVICEDESC        gpdsddOutputDevices, gpdsddOut;

extern CRITICAL_SECTION     gcsBufferData;
extern int                  gnBuffersOut;
extern PWAVEHDR             gawhHeaders;
extern PBYTE                gpbBufferData;
extern LONG                 gfBuffersInitialized;
extern LPDIRECTSOUNDBUFFER  gpdsbOutput;
extern DSBUFFERDESC         gdsbdOutput;
extern CFilter *            gpFilter;

#endif



int PASCAL WinMain( HINSTANCE, HINSTANCE, LPSTR, int );
BOOL CALLBACK MainDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK AboutDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK CancelWarningDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK SelectDevicesDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK SelectFormatsDlgProc( HWND, UINT, WPARAM, LPARAM );


void FillFormatListBox( PFORMATDATA, HWND );

#endif  // __FDFILTER_H__


