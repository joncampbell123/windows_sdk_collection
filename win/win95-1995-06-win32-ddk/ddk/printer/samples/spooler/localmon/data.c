/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#include <windows.h>
#include <winspool.h>
#include <offsets.h>
#include <regstr.h>
#include "dialogs.h"


DWORD PortInfo1Strings[]={offsetof(LPPORT_INFO_1A, pName),
                          0xFFFFFFFF};

DWORD PortInfo2Strings[]={offsetof(LPPORT_INFO_2A, pPortName),
                          offsetof(LPPORT_INFO_2A, pMonitorName),
                          offsetof(LPPORT_INFO_2A, pDescription),
                          0xFFFFFFFF};

ERRORMAP tabError[] =
{
    { ERROR_OUT_OF_PAPER    , IDS_ERR_OUT_OF_PAPER        , MB_RETRYCANCEL | MB_ICONQUESTION},
    { ERROR_NOT_READY       , IDS_ERR_NOT_READY           , MB_RETRYCANCEL | MB_ICONQUESTION},
    { ERROR_IO_DEVICE       , IDS_ERR_IO_DEVICE           , MB_RETRYCANCEL | MB_ICONQUESTION},
    { ERROR_BAD_NET_RESP    , IDS_ERR_BAD_NET_RESP        , MB_OK | MB_ICONINFORMATION},
    { ERROR_BAD_UNIT        , IDS_ERR_BAD_UNIT            , MB_OK | MB_ICONINFORMATION},
    { ERROR_ACCESS_DENIED   , IDS_ERR_ACCESS_DENIED       , MB_OK | MB_ICONINFORMATION},
    { ERROR_BAD_DEVICE      , IDS_ERR_BAD_DEVICE          , MB_OK | MB_ICONINFORMATION},
    { ERROR_DEV_NOT_EXIST   , IDS_ERR_DEV_NOT_EXIST       , MB_OK | MB_ICONINFORMATION},
    { ERROR_FILE_NOT_FOUND  , IDS_ERR_FILE_NOT_FOUND      , MB_OK | MB_ICONINFORMATION},
    { ERROR_INVALID_HANDLE  , IDS_ERR_INVALID_HANDLE      , MB_OK | MB_ICONINFORMATION},
    { ERROR_NETWORK_BUSY    , IDS_ERR_NETWORK_BUSY        , MB_RETRYCANCEL | MB_ICONQUESTION},
    { ERROR_NET_WRITE_FAULT , IDS_ERR_NET_WRITE_FAULT     , MB_OK | MB_ICONINFORMATION},
    { ERROR_NO_NETWORK      , IDS_ERR_NO_NETWORK          , MB_OK | MB_ICONINFORMATION},
    { ERROR_NOT_CONNECTED   , IDS_ERR_NOT_CONNECTED       , MB_OK | MB_ICONINFORMATION},
    { ERROR_REM_NOT_LIST    , IDS_ERR_REM_NOT_LIST        , MB_OK | MB_ICONINFORMATION},
    { ERROR_REQ_NOT_ACCEP   , IDS_ERR_REQ_NOT_ACCEP       , MB_OK | MB_ICONINFORMATION},
    { ERROR_UNEXP_NET_ERR   , IDS_ERR_UNEXP_NET_ERR       , MB_OK | MB_ICONINFORMATION},
    { ERROR_DISK_FULL       , IDS_ERR_DISK_FULL           , MB_OK | MB_ICONINFORMATION},
    { ERROR_PRINTQ_FULL     , IDS_ERR_PRINTQ_FULL         , MB_OK | MB_ICONINFORMATION},
    { ERROR_REDIR_PAUSED    , IDS_ERR_REDIR_PAUSED        , MB_OK | MB_ICONINFORMATION},
    { ERROR_COUNTER_TIMEOUT , IDS_ERR_COUNTER_TIMEOUT     , MB_RETRYCANCEL | MB_ICONQUESTION},
    { ERROR_NOT_ENOUGH_SERVER_MEMORY, IDS_ERR_NOT_ENOUGH_SERVER_MEMORY, MB_OK | MB_ICONINFORMATION},
    { 0                     , 0 , MB_OK | MB_ICONINFORMATION},
};

TCHAR FAR *szDriverFile       = TEXT("Driver");
TCHAR FAR *szLocalPort  = TEXT("Local Port");
TCHAR FAR *szNull             = TEXT("");
TCHAR FAR *szPorts            = TEXT("ports");
TCHAR FAR *szRegistryPrinters = TEXT(REGSTR_PATH_PRINTERS);
