/*
 * DLLFUNCS.C
 *
 * Contains entry and exit points for the DLL implementation
 * of the OLE 2.0 User Interface Support Library.
 *
 * This file is not needed if we are linking the static library
 * version of this library.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */

#include "precomp.h"
#include "common.h"
#include "uiclass.h"

OLEDBGDATA_MAIN(TEXT(LIBNAME))

/*
 * DllMain
 *
 * Purpose:
 *  DLL-specific entry point called from LibEntry.
 */

#pragma code_seg(".text$initseg")

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD Reason, LPVOID lpv)
{
	if (Reason == DLL_PROCESS_DETACH)
	{
		OleDbgOut2(TEXT("DllMain: OLE2UI.DLL unloaded\r\n"));

		OleUIUnInitialize();
	}
	else if (Reason == DLL_PROCESS_ATTACH)
	{
		OleDbgOut2(TEXT("DllMain: OLE2UI.DLL loaded\r\n"));

		DisableThreadLibraryCalls(hInst);

		OleUIInitialize(hInst, (HINSTANCE)0,
			TEXT(SZCLASSICONBOX), TEXT(SZCLASSRESULTIMAGE));
	}
	return TRUE;
}

#pragma code_seg()
