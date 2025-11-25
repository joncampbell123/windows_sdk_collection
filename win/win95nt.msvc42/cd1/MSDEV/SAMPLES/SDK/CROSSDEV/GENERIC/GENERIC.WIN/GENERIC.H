#include "resource.h"

#define APPNAME "Generic"

// Makes it easier to determine appropriate code paths:
#if defined (WIN32)
	#define IS_WIN32 TRUE
#else
	#define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_CHICAGO (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

//---------------------------------------------------------------------------
//  GENERIC.H
//---------------------------------------------------------------------------

#define IDC_STATIC -1

#define DLG_VERFIRST        400
#define IDC_COMPANY			DLG_VERFIRST
#define IDC_FILEDESC       	DLG_VERFIRST+1
#define IDC_PRODVER         DLG_VERFIRST+2
#define IDC_COPYRIGHT       DLG_VERFIRST+3
#define IDC_OSVERSION       DLG_VERFIRST+4
#define IDC_TRADEMARK       DLG_VERFIRST+5
#define DLG_VERLAST         DLG_VERFIRST+5

#define IDC_LABEL           DLG_VERLAST+1
