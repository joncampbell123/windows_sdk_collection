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

/* CALDLG.c: Calibrate Dialog 

	Calibrate Dialog.

		This module contains the Calibrate Dialog WndProc and related
		functions for the top-level dialog.

*/

/******************* Includes and Controlling Defines ***********************/
#include "cal.h"
#include "calerr.h"
#include "_caldlg.h"

#include <winerror.h>

/******************* Defines ************************************************/
#define Myabs(i)	((i < 0) ? (-i) : i)

#define idTimer				2		// timer to terminate calibration
#define uTimeOut			20		// seconds to terminate in case of no response

#define REGSTR_PATH_PENDRV\
	"System\\CurrentControlSet\\Services\\VxD\\VPEND"
#define REGSTR_VAL_MODEL	"Model"
#define REGSTR_VAL_HWINFO	"HardwareInfo"

// The following two structures are copied from pendrv.h in the unidrv 
// source.
#define HW_CALIBRATE		0x00010000

typedef struct DRV_CalbStruct
	{
	DWORD dwOffsetX;
	DWORD dwOffsetY;
	DWORD dwDistinctWidth;
	DWORD dwDistinctHeight;
	}
	DRV_CALBSTRUCT;

typedef struct tag_HARDWAREINFO /*_hardwareinfo*/
	{
	DWORD ddCom_Port;
	DWORD com_base;
	DWORD ddIRQ_Number;
	DWORD ddPauseTime;
	DWORD ddReserved;
	DWORD ddOrientation;
	DRV_CALBSTRUCT calibrate;
	DWORD ddBufferZoneX;
	DWORD ddBufferZoneY;
	DWORD dwHardwarePacketSize;
	DWORD dwComPortSettings;
	DWORD dwHwFlags;
	DWORD ddHardwareType;
	DWORD ddTimerTickRate;
	DWORD ddDelayTime;
	DWORD VpenD_IRQ_Handle;
	DWORD SystemVMHandle;
	}
	_HARDWAREINFO, FAR *LP_HARDWAREINFO;

/******************* Macros *************************************************/

/******************* Typedefs ***********************************************/

/******************* Variables **********************************************/
HWND vhDlgCal = NULL;
HWND vhDlgVerify = NULL;

static WORD 	wCorner=0;
static POINT 	rgptCross[4];
static POINT 	rgptTab[4];
static POINT 	rgdpnt[4];
static WORD 	wOrientation;
static CALBSTRUCT csPen, csPenNew, csPenOrig;
static HANDLE 	hDriverPen;
static PENINFO pi;
static char 	szInstructionsTemp[STR_MAX_LEN];
static char 	szInstructions[INST_STR_MAX_LEN];

static char CODECONST szDisplayDriver[] = "Display Driver";
static char CODECONST szDisplayOrientation[] = "DisplayOrientation";

BOOL fDrawCursor = FALSE;
static BOOL fEatNextMouseEvent = FALSE;

#ifdef DEBUG
char szd[STR_MAX_LEN];
#endif // DEBUG

/******************* Local prototypes ***************************************/
BOOL PRIVATE UpdateRegCS(VOID);

#ifdef DEBUG
VOID PRIVATE DumpCalibStruct(LPCALBSTRUCT, LPSTR);
VOID PRIVATE DumpPenInfo(LPPENINFO, LPSTR);
#endif // DEBUG

extern UINT PRIVATE UMulDiv(UINT, UINT, UINT);

/******************* EXPORT FUNCTIONS ***************************************/

/*+-------------------------------------------------------------------------*/
/*
PURPOSE: Dialog procedure for the top-level calibration dialog.
RETURN: 
GLOBALS: vhDlgCal
CONDITIONS: 
*/
BOOL FAR PASCAL _loadds CalDlg(
	HWND	hDlg,
	WORD	wMessage,
	WORD	wParam,
	LONG	lParam)
	{
	DWORD	dwReg = REG_DWORD;
	DWORD	dwBuf;
	DWORD	dwOrient;
	HKEY	hk;

	switch(wMessage)
		{
		case WM_INITDIALOG:
			vhDlgCal = hDlg;
			wCorner = 0;

			LoadString(vhInst, rsInstructions1, (LPSTR)szInstructionsTemp,
				STR_MAX_LEN);
			lstrcpy(szInstructions, szInstructionsTemp);
			LoadString(vhInst, rsInstructions2, (LPSTR)szInstructionsTemp,
				STR_MAX_LEN);
			lstrcat(szInstructions, szInstructionsTemp);
			LoadString(vhInst, rsInstructions3, (LPSTR)szInstructionsTemp,
				STR_MAX_LEN);
			lstrcat(szInstructions, szInstructionsTemp);

			if ((hDriverPen = OpenDriver("pen", NULL, NULL)) == 0L)
				{
				CalError(CALERR_CANTOPENPENDRIVER);
				return FALSE;
				}
			if (SendDriverMessage(hDriverPen, DRV_GetPenInfo,
					(DWORD)(LPPENINFO)&pi, NULL) == 0L)
				{
				CalError(CALERR_NOPEN);
				return FALSE;
				}

#ifdef DEBUG
			DumpPenInfo(&pi, "WM_INITDIALOG: PenInfo");
#endif

			if ((pi.lPdc & PDC_INTEGRATED) == 0)
				{
				CalError(CALERR_OPAQUE);
				return FALSE;
				}
			if (SendDriverMessage(hDriverPen, DRV_GetCalibration,
					(DWORD)(LPCALBSTRUCT)&csPenOrig, NULL) == 0L)
				{
				CalError(CALERR_OLDPENDRIVER);
				return FALSE;
				}

#ifdef DEBUG
			DumpCalibStruct(&csPenOrig, "WM_INITDIALOG: CalibStruct\r\n");
#endif

			csPen = csPenOrig;

			wOrientation = 0;		// default
			if (RegOpenKey(HKEY_LOCAL_MACHINE, (LPSTR)REGSTR_PATH_PENDRV,
					&hk) == ERROR_SUCCESS)
				{
				if (RegQueryValueEx(hk, (LPSTR)szDisplayDriver, NULL, &dwReg,
						(LPBYTE)&dwOrient, &dwBuf) == ERROR_SUCCESS)
					wOrientation = (WORD)dwOrient;
				else
					wOrientation = 0;
				RegCloseKey(hk);
				}
			else
				{
				DbgS("Error in opening the Registry for Orientation.\r\n");
				}

			SetWindowPos(vhDlgCal, NULL, 0, 0,
				GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
				SWP_NOZORDER);
			BLOCK
				{
				RECT rect;

				GetClientRect(vhDlgCal, (LPRECT)&rect);
				rgptCross[0].x = rect.left + dxOffset;
				rgptCross[0].y = rect.top + dyOffset;
				rgptCross[1].x = rect.right - dxOffset;
				rgptCross[1].y = rect.top + dyOffset;
				rgptCross[2].x = rect.left + dxOffset;
				rgptCross[2].y = rect.bottom - dyOffset;
				rgptCross[3].x = rect.right - dxOffset;
				rgptCross[3].y = rect.bottom - dyOffset;
				}
			PostMessage(hDlg, WM_RunCalibrate, NULL, NULL);
			return FALSE;

		case WM_RunCalibrate:
			SendDriverMessage(hDriverPen, DRV_GetCalibration,
				(DWORD)(LPCALBSTRUCT)&csPen, NULL);

#ifdef DEBUG
			DumpCalibStruct(&csPen, "WM_RunCalibrate: CalibStruct\r\n");
#endif
			wCorner = 0;
			InvalidateRect(hDlg, NULL, FALSE);
			return TRUE;

		case WM_AcceptChanges:
			AcceptChanges();
			PostMessage(hDlg, WM_CLOSE, NULL, NULL);
			return TRUE;

		case WM_RefuseChanges:
			SendDriverMessage(hDriverPen, DRV_SetCalibration,
				(DWORD)(LPCALBSTRUCT)&csPenOrig, NULL);
			PostMessage(hDlg, WM_CLOSE, NULL, NULL);
			return TRUE;

		case WM_PAINT:
			RepaintMe(hDlg);
			return TRUE;

		case WM_SETCURSOR:
			if (fDrawCursor)
				{
				return FALSE;
				}
			else
				{
				POINT pnt;

				GetCursorPos((LPPOINT)&pnt);
				ScreenToClient(hDlg, (LPPOINT)&pnt);

				if (
					(wCorner < 4) &&
					(pnt.x >= (rgptCross[wCorner].x - dxLine)) &&
					(pnt.x < (rgptCross[wCorner].x + dxLine)) &&
					(pnt.y >= (rgptCross[wCorner].y - dyLine)) &&
					(pnt.y < (rgptCross[wCorner].y + dyLine))
					)
					{
					// The cursor is near the cross hairs, so don't draw it.

					SetCursor(NULL);
					return TRUE;
					}
				else
					{
					return FALSE;
					}
				}

		case WM_ACTIVATE:
			switch (wParam)
				{
				case 2:
					fEatNextMouseEvent = TRUE;
					// FALLS THROUGH TO OTHER ACTIVATE HANDLER...
				case 1:
					fDrawCursor = FALSE;
					break;
				case 0:
					fDrawCursor = TRUE;
					break;
				}
			return TRUE;

		case WM_LBUTTONDOWN:
			if (fEatNextMouseEvent)
				{
				fEatNextMouseEvent = FALSE;
				}
			else if (wCorner<4)
				{
				DWORD dwInfo = GetMessageExtraInfo();

				if (IsPenEvent(wMessage, dwInfo))
					{
					HandlePenDown(vhDlgCal, LOWORD(dwInfo), lParam);
					}
				else
					{
					CalError(CALERR_UNUSUALINPUT);  // need some way to terminate
					}
				}
			return TRUE;

		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (fEatNextMouseEvent)
				{
				fEatNextMouseEvent = FALSE;
				return TRUE;
				}

			// ELSE FALL THROUGH TO UNUSUAL INPUT HANDLER
		case WM_COMMAND:
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		case WM_CHAR:
			CalError(CALERR_UNUSUALINPUT);  // need some way to terminate
			return TRUE;

		case WM_CLOSE:
			if (hDriverPen)
				{
				CloseDriver(hDriverPen, NULL, NULL);
				hDriverPen = 0;
				}
			ReleaseCapture();
			EndDialog(vhDlgCal, FALSE);
			vhDlgCal = NULL;
			return TRUE;

		default:
			return FALSE;
		}
	}


VOID PRIVATE RepaintMe(
	HWND hwnd)
	{
	PAINTSTRUCT	ps;
	RECT 		rect, rectText;
	int 		rop, nWidthText, nHeightText;
	HDC 		hdc = BeginPaint(hwnd, &ps);
	HPEN 		hpen = SelectObject(hdc, (HPEN) GetStockObject(BLACK_PEN));

	HBRUSH 	hbrush = SelectObject(hdc, (HBRUSH) GetStockObject(BLACK_BRUSH));

	// first erase whole screen...
	GetClientRect(hwnd, (LPRECT) &rect);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

	// then draw the text and graphics if necessary
	SelectObject(hdc, (HPEN) GetStockObject(WHITE_PEN));
	rop = SetROP2(hdc, R2_XORPEN);
	if (wCorner<4)
		{
		// cross
		MoveTo(hdc, rgptCross[wCorner].x, rgptCross[wCorner].y - dyLine);
		LineTo(hdc, rgptCross[wCorner].x, rgptCross[wCorner].y + dyLine);
		MoveTo(hdc, rgptCross[wCorner].x - dxLine, rgptCross[wCorner].y);
		LineTo(hdc, rgptCross[wCorner].x + dxLine, rgptCross[wCorner].y);

		// instructions
		SetBkColor(hdc, RGB(0,0,0));
		SetTextColor(hdc, RGB(255,255,255));

		rectText.left = rgptCross[0].x + (2 * dxLine);
		rectText.top = rectText.bottom = 0;
		rectText.right = rgptCross[3].x - (2 * dxLine);

		nHeightText = DrawText(hdc, szInstructions, -1, (LPRECT)&rectText,
			DT_CALCRECT | DT_WORDBREAK);
		nWidthText = rectText.right - rectText.left;

		rectText.left = (rect.right + rect.left - nWidthText) / 2;
		rectText.right = (rect.right + rect.left + nWidthText) / 2;
		rectText.top = (rect.top + rect.bottom - nHeightText) / 2;
		rectText.bottom = (rect.top + rect.bottom + nHeightText) / 2;

		DrawText(hdc, szInstructions, -1, (LPRECT)&rectText, DT_WORDBREAK);
		}

	SetROP2(hdc, rop);
	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);
	EndPaint(hwnd, &ps);
	}


VOID PRIVATE HandlePenDown(
	HWND 	hwnd,
	WORD 	wEventRef,
	LONG 	lParam)
	{
	STROKEINFO si;
	int ix = rgptCross[wCorner].x - LOWORD(lParam);
	int iy = rgptCross[wCorner].y - HIWORD(lParam);

	MessageBeep(0);

	// If user did not click inside the cross, the click location cannot be used.
	if ((Myabs(ix) > 3*dxLine) || (Myabs(iy) > 3*dyLine))
		{
		SendDriverMessage(hDriverPen, DRV_SetCalibration,
			(DWORD)(LPCALBSTRUCT)&csPenOrig, NULL);
		CalError(CALERR_UNUSUALINPUT);
		return;
		}

	// Get the click location in ultra-fine resolution tablet coordinates.
	if (GetPenHwEventData(wEventRef, wEventRef,
			&rgptTab[wCorner], NULL, 1, &si) != REC_OK)
		{
		PostMessage(hwnd, WM_CLOSE, NULL, (long) NULL);
		return;
		}

	// convert into raw coordinates
#if SIGNED_MULDIV
	rgdpnt[wCorner].x = MulDiv(rgptTab[wCorner].x, csPen.wDistinctWidth,
		(int)pi.cxRawWidth) - csPen.wOffsetX;
	rgdpnt[wCorner].y = MulDiv(rgptTab[wCorner].y, csPen.wDistinctHeight,
		(int)pi.cyRawHeight) - csPen.wOffsetY;
#else	// unsigned MulDiv
	rgdpnt[wCorner].x = (int)UMulDiv((UINT)rgptTab[wCorner].x,
		(UINT)csPen.wDistinctWidth, pi.cxRawWidth) - csPen.wOffsetX;
	rgdpnt[wCorner].y = UMulDiv((UINT)rgptTab[wCorner].y,
		(UINT)csPen.wDistinctHeight, pi.cyRawHeight) - csPen.wOffsetY;
#endif	// SIGNED_MULDIV

	wCorner++;
	InvalidateRect(hwnd, NULL, FALSE);
	if (wCorner<4)
		return;

	// make rgdpnt[0] and rgdpnt[3] the low and high averages
	rgdpnt[0].x = (rgdpnt[0].x + rgdpnt[2].x)/2;
	rgdpnt[0].y = (rgdpnt[0].y + rgdpnt[1].y)/2;
	rgdpnt[3].x = (rgdpnt[1].x + rgdpnt[3].x)/2;
	rgdpnt[3].y = (rgdpnt[2].y + rgdpnt[3].y)/2;

	// compute new offset
	csPenNew.wOffsetX =
		(WORD)(((rgdpnt[3].x*rgptCross[0].x) - (rgdpnt[0].x*rgptCross[3].x))
		/ (rgptCross[3].x - rgptCross[0].x));
	csPenNew.wOffsetY =
		(WORD)(((rgdpnt[3].y*rgptCross[0].y) - (rgdpnt[0].y*rgptCross[3].y))
		/ (rgptCross[3].y - rgptCross[0].y));

	// compute new width & height
	csPenNew.wDistinctWidth = MulDiv((rgdpnt[0].x + csPenNew.wOffsetX),
		GetSystemMetrics(SM_CXSCREEN), rgptCross[0].x);

	csPenNew.wDistinctHeight = MulDiv((rgdpnt[0].y + csPenNew.wOffsetY),
		GetSystemMetrics(SM_CYSCREEN), rgptCross[0].y);

	// set the new tablet values
	SendDriverMessage(hDriverPen, DRV_SetCalibration,
		(DWORD)(LPCALBSTRUCT)&csPenNew, NULL);

#ifdef DEBUG
	DumpCalibStruct(&csPenNew, "HandlePenDown: CalibNew\r\n");
#endif

	BLOCK
		{
		// verify the new tablet settings
		FARPROC lpfn = MakeProcInstance((FARPROC)VerifyChangesDlg, vhInst);

		DialogBox(vhInst, (LPSTR)MAKEINTRESOURCE(iddVerifyChanges),
			(HWND)vhDlgCal, (DLGPROC)lpfn);
		FreeProcInstance(lpfn);
		}
	}


BOOL FAR PASCAL _loadds VerifyChangesDlg(
	HWND 	hDlg,
	WORD 	message,
	WORD 	wParam,
	LONG 	lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			BLOCK
				{
				RECT 	rectParent, rectMe;
				POINT pntCenter;
				int 	nWidthMe, nHeightMe;

				vhDlgVerify = hDlg;
				GetWindowRect(hDlg, (LPRECT) &rectMe);
				rectParent.left = rectParent.top = 0;
				rectParent.right  = GetSystemMetrics(SM_CXSCREEN);
				rectParent.bottom = GetSystemMetrics(SM_CYSCREEN);
				pntCenter.x = (rectParent.left + rectParent.right) / 2;
				pntCenter.y = (rectParent.top + rectParent.bottom) / 2;
				nWidthMe = rectMe.right - rectMe.left;
				nHeightMe = rectMe.bottom - rectMe.top;

				SetWindowPos(hDlg, NULL,
					pntCenter.x - (nWidthMe/2), pntCenter.y - (nHeightMe/2),
					nWidthMe, nHeightMe, SWP_NOZORDER);
				}
			SetTimer(hDlg, idTimer, uTimeOut*1000, NULL);
			return TRUE;
		case WM_COMMAND:
			switch (wParam)
				{
				case IDOK:
					KillTimer(hDlg, idTimer);
					PostMessage(vhDlgCal, WM_AcceptChanges, NULL, NULL);
					EndDialog(hDlg, 0);
					break;
				case IDCANCEL:
					KillTimer(hDlg, idTimer);
					PostMessage(vhDlgCal, WM_RefuseChanges, NULL, NULL);
					EndDialog(hDlg, 0);
					break;
				case idcRecalibrate:
					KillTimer(hDlg, idTimer);
					PostMessage(vhDlgCal, WM_RunCalibrate, NULL, NULL);
					EndDialog(hDlg, 0);
					break;
				}
			return TRUE;
		case WM_CLOSE:
			EndDialog(vhDlgVerify, FALSE);
			vhDlgVerify = NULL;
			return TRUE;
		case WM_TIMER:
			MessageBeep(0);
			KillTimer(hDlg, idTimer);
			PostMessage(vhDlgCal, WM_RefuseChanges, NULL, NULL);
			EndDialog(hDlg, 0);
			return TRUE;
		}
	return FALSE;
	}


VOID PRIVATE AcceptChanges(
	VOID)
	{
	if (wOrientation != 0)
		{
		if (wOrientation >= 2)
			{
			csPenNew.wOffsetX = -csPenNew.wOffsetX;
			}
		if (wOrientation <= 2)
			{
			csPenNew.wOffsetY = -csPenNew.wOffsetY;
			}
		if (wOrientation % 2)
			{
			int temp=csPenNew.wDistinctWidth;
			csPenNew.wDistinctWidth = csPenNew.wDistinctHeight;
			csPenNew.wDistinctHeight = temp;

			temp = csPenNew.wOffsetX;
			csPenNew.wOffsetX = csPenNew.wOffsetY;
			csPenNew.wOffsetY = temp;
			}
		}

	UpdateRegCS();
	return;
	}


/*+-------------------------------------------------------------------------*/
/*
PURPOSE: To update the HardwareInfo structure of the Pen driver section
	 in the registry.
RETURN:  TRUE if successful and FALSE otherwise.
GLOBALS: csPenNew
CONDITIONS: Only for the Microsoft Windows 95 version.
*/
BOOL PRIVATE
	UpdateRegCS(
	VOID)
	{
	HKEY	hkVPenD;		// handle to the pen driver key
	HKEY	hkModel;		// handle to the Model key
	char	szModel[cchResMax];	// buffer to get the current model
	DWORD	dwType = REG_SZ;
	DWORD	dwBuf = cchResMax;
	BOOL	fRet = FALSE;		// return value

	// Open the pen driver key.
	if (RegOpenKey(HKEY_LOCAL_MACHINE, (LPSTR)REGSTR_PATH_PENDRV, &hkVPenD)
			== ERROR_SUCCESS)
		{
		// Get the current model name.
		if (RegQueryValueEx(hkVPenD, (LPSTR)REGSTR_VAL_MODEL, NULL, &dwType,
				(LPBYTE)(LPSTR)&szModel, &dwBuf) == ERROR_SUCCESS)
			{
			// Open the current model key.
			if (RegOpenKey(hkVPenD, (LPSTR)szModel, &hkModel) == ERROR_SUCCESS)
				{
				_HARDWAREINFO	HWInfo;
				
				dwType = REG_DWORD;
				dwBuf = sizeof(_HARDWAREINFO);

				// Read the current Hardware info structure.
				if (RegQueryValueEx(hkModel, (LPSTR)REGSTR_VAL_HWINFO, NULL,
						&dwType, (LPBYTE)(LPSTR)&HWInfo, &dwBuf) == ERROR_SUCCESS)
					{
					// Update the Hardware info structure.
					HWInfo.calibrate.dwOffsetX = (DWORD)(csPenNew.wOffsetX);
					HWInfo.calibrate.dwOffsetY = (DWORD)(csPenNew.wOffsetY);
					HWInfo.calibrate.dwDistinctWidth =
						(DWORD)csPenNew.wDistinctWidth;
					HWInfo.calibrate.dwDistinctHeight =
						(DWORD)csPenNew.wDistinctHeight;
					HWInfo.dwHwFlags |= HW_CALIBRATE;

					// Write the updated Hardware info structure to the registry.
					if (RegSetValueEx(hkModel, (LPSTR)REGSTR_VAL_HWINFO, NULL,
							REG_DWORD, (LPBYTE)(LPSTR)&HWInfo, dwBuf)
							!= ERROR_SUCCESS)
						{
						DbgS("Error in setting calibration.\r\n");
						}
					else
						fRet = TRUE;
					}
				else
					{
					DbgS("Error in querying calibration from the registry\r\n");
					}
				RegCloseKey(hkModel);
				}
			else
				{
				DbgS("Error in opening the Model key.\r\n");
				}
			}
		else
			{
			DbgS("Error in querying Model name.\r\n");
			}
		RegCloseKey(hkVPenD);
		}
	else
		{
		DbgS("Error in opening the Pen Driver key.\r\n");
		}

	return fRet;
	}


#ifdef DEBUG
VOID PRIVATE DumpCalibStruct(
	LPCALBSTRUCT 	lpcs,
	LPSTR				lpstr)
	{
	DbgS((lpstr));
	wsprintf(szd, "\twOffsetX = %d, wOffsetY = %d\r\n",
		lpcs->wOffsetX, lpcs->wOffsetY);
	DbgS(szd);
	wsprintf(szd, "\twDistinctWidth = %d, wDistinctHeight = %d\r\n",
		lpcs->wDistinctWidth, lpcs->wDistinctHeight);
	DbgS(szd);
	}


VOID PRIVATE DumpPenInfo(
	LPPENINFO	lppi,
	LPSTR			lpstr)
	{
	DbgS((lpstr));
	wsprintf(szd, "\tcxRawWidth = %u, cyRawHeight = %u\r\n",
		lppi->cxRawWidth, lppi->cyRawHeight);
	DbgS(szd);
	wsprintf(szd, "\twDistinctWidth = %u, wDistinctHeight = %u\r\n",
		lppi->wDistinctWidth, lppi->wDistinctHeight);
	DbgS(szd);
	wsprintf(szd, "\tnSamplingRate = %d, nSamplingDist = %d\r\n",
		lppi->nSamplingRate, lppi->nSamplingDist);
	DbgS(szd);
	}
#endif


