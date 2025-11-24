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

typedef struct
{
    DWORD   id;         // Error id from GetLast Error
    DWORD   ids;
    DWORD   mbStyle;    // message box style
}   ERRORMAP, * PERRORMAP;

extern ERRORMAP tabError[];

// ------------------------------------------------------------
// LOCALMON
#define DLG_PORTNAME                200
#define IDD_PN_EF_PORTNAME          201

#define DLG_CONFIGURE_LPT           300
#define IDD_CL_PORTNAME             301
#define IDD_CL_CC_DOSSPOOL          302

#define DLG_PRINTTOFILE             400
#define IDD_PF_EF_OUTPUTFILENAME    401

#define IDS_LOCALMONITOR            500
#define IDS_INVALIDPORTNAME_S       501
#define IDS_PORTALREADYEXISTS_S     502
#define IDS_NOTHING_TO_CONFIGURE    503
#define IDS_UNKNOWN_LOCAL_PORT      504
#define IDS_FILE_PORT               505

#define IDS_OVERWRITE_FILE          510
#define IDS_FILE_CAPTION            511
// ------------------------------------------------------------

// ------------------------------------------------------------
// WINPRINT
#define IDS_BANNERTITLE1            521
#define IDS_BANNERTITLE2            522
#define IDS_BANNERJOB               523
#define IDS_BANNERUSER              524
#define IDS_BANNERDATE              525
#define IDS_BANNERSIMPLE            526
#define IDS_BANNERFULL              527
// ------------------------------------------------------------

#define IDS_ERR_OUT_OF_PAPER        600
#define IDS_ERR_NOT_READY           601
#define IDS_ERR_IO_DEVICE           602
#define IDS_ERR_BAD_NET_RESP        603
#define IDS_ERR_BAD_UNIT            604
#define IDS_ERR_ACCESS_DENIED       605
#define IDS_ERR_BAD_DEVICE          606
#define IDS_ERR_DEV_NOT_EXIST       607
#define IDS_ERR_FILE_NOT_FOUND      608
#define IDS_ERR_INVALID_HANDLE      609
#define IDS_ERR_NETWORK_BUSY        610
#define IDS_ERR_NETNAME_DELETED     611
#define IDS_ERR_NET_WRITE_FAULT     612
#define IDS_ERR_NO_NETWORK          613
#define IDS_ERR_NOT_CONNECTED       614
#define IDS_ERR_REM_NOT_LIST        615
#define IDS_ERR_REQ_NOT_ACCEP       616
#define IDS_ERR_UNEXP_NET_ERR       617
#define IDS_ERR_DISK_FULL           618
#define IDS_ERR_PRINTQ_FULL         619
#define IDS_ERR_REDIR_PAUSED        620
#define IDS_ERR_COUNTER_TIMEOUT     621
#define IDS_ERR_NOT_ENOUGH_SERVER_MEMORY    622

#define IDS_ERR_UNKNOWN             650

#define IDS_STATUS_PAPER_JAM        660
#define IDS_STATUS_PAPER_OUT        661
#define IDS_STATUS_PAPER_PROBLEM    662
#define IDS_STATUS_MANUAL_FEED      663
#define IDS_STATUS_OFFLINE          664
#define IDS_STATUS_WARMING_UP       665
#define IDS_STATUS_TONER_LOW        666
#define IDS_STATUS_OUT_OF_MEMORY    667
#define IDS_STATUS_DOOR_OPEN        668
#define IDS_STATUS_POWER_SAVE       669

#define IDS_WIN32_SPOOLER           700

#define IDS_WRITING_ERROR           850
#define IDS_RETRY_CANCEL            851
#define IDS_DISK_ERROR              852
#define IDS_OPEN_ERROR              853

#define IDS_SHADOW_PRINT            900
#define IDS_SHADOW_QUERY1           901

// ------------------------------------------------------------
// WINPRINT
#define IDC_STANDBAN    600
#define RT_CLIPFILE     601
// ------------------------------------------------------------
