/*
 * ICONBOX.C
 *
 * Implemenatation of an IconBox control for OLE 2.0 UI dialogs that we'll
 * use wherever a dialog needs an icon/label display.  Through the control's
 * interface we can change the image or control label visibility.
 *
 * The IconBox discusses images in CF_METAFILEPICT format.  When drawing
 * such a metafile, the entire aspect is centered in the IconBox, so long
 * labels are chopped at either end.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */

#include "precomp.h"
#include "iconbox.h"
#include "utility.h"

//Flag indicating if we've registered the class
static BOOL fRegistered;


/*
 * FIconBoxInitialize
 *
 * Purpose:
 *  Registers the IconBox control class.
 *
 * Parameters:
 *  hInst           HINSTANCE instance of the DLL.
 *
 *  hPrevInst       HINSTANCE of the previous instance.  Used to
 *                  determine whether to register window classes or not.
 *
 *  lpszClassName   LPSTR containing the class name to register the
 *                  IconBox control class with.
 *
 * Return Value:
 *  BOOL            TRUE if all initialization succeeded, FALSE otherwise.
 */

#pragma code_seg(".text$initseg")

BOOL FIconBoxInitialize(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszClassName)
	{
	WNDCLASS        wc;

	// Only register class if we're the first instance
	if (hPrevInst)
		fRegistered = TRUE;
	else
	{
		// Static flag fRegistered guards against calling this function more
		// than once
		if (!fRegistered)
		{
			wc.lpfnWndProc   =IconBoxWndProc;
			wc.cbClsExtra    =0;
			wc.cbWndExtra    =CBICONBOXWNDEXTRA;
			wc.hInstance     =hInst;
			wc.hIcon         =NULL;
			wc.hCursor       =LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground =(HBRUSH)NULL;
			wc.lpszMenuName  =NULL;
			wc.lpszClassName =lpszClassName;
			wc.style         =CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW;

			fRegistered = RegisterClass(&wc);
		}
	}
	return fRegistered;
}

#pragma code_seg()


/*
 * IconBoxUninitialize
 *
 * Purpose:
 *  Cleans up anything done in FIconBoxInitialize.  Currently there is
 *  nothing, but we do this for symmetry.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void IconBoxUninitialize(void)
{
	return;
}

/*
 * IconBoxWndProc
 *
 * Purpose:
 *  Window Procedure for the IconBox custom control.  Only handles
 *  WM_CREATE, WM_PAINT, and private messages to manipulate the image.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  Standard
 */

LONG CALLBACK EXPORT IconBoxWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HGLOBAL         hMF=NULL;
	PAINTSTRUCT     ps;
	HDC             hDC;
	RECT            rc;

	//Handle standard Windows messages.
	switch (iMsg)
	{
		case WM_CREATE:
			SetWindowLong(hWnd, IBWW_HIMAGE, 0);
			SetWindowWord(hWnd, IBWW_FLABEL, TRUE);
			return 0L;

		case WM_ERASEBKGND:
		{
			HBRUSH hBrush;
			RECT   Rect;
			POINT  point;

			GetClientRect(hWnd, &Rect);
			hBrush = (HBRUSH)SendMessage(GetParent(hWnd),
										WM_CTLCOLORDLG,
										wParam,
										(LPARAM)GetParent(hWnd));

			if (!hBrush)
				return FALSE;

			UnrealizeObject(hBrush);

			SetBrushOrgEx((HDC)wParam, 0, 0, &point);
			FillRect((HDC)wParam, &Rect, hBrush);

			return TRUE;
		}

		case WM_PAINT:
			hMF=(HGLOBAL)GetWindowLong(hWnd, IBWW_HIMAGE);

			//BeginPaint and EndPaint clear us even if hMF is NULL.
			hDC=BeginPaint(hWnd, &ps);

			if (NULL!=hMF)
			{
				BOOL            fLabel;

				//Now we get to paint the metafile, centered in our rect.
				GetClientRect(hWnd, &rc);

				/*
				 * If we're doing icon only, then place the metafile
				 * at the center of our box minus half the icon width.
				 * Top is top.
				 */
				fLabel=GetWindowWord(hWnd, IBWW_FLABEL);


				//Go draw where we decided to place it.
				OleUIMetafilePictIconDraw(hDC, &rc, hMF, !fLabel);
			}
			EndPaint(hWnd, &ps);
			break;

		case IBXM_IMAGESET:
			/*
			 * wParam is a flag to delete the old or not.
			 * lParam contains the new handle.
			 */
			hMF=(HGLOBAL)SetWindowLong(hWnd, IBWW_HIMAGE, lParam);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);

			//Delete the old handle if requested
			if (0L!=wParam)
			{
				OleUIMetafilePictIconFree(hMF);
				hMF=NULL;
			}
			return (LONG)(UINT)hMF;

		case IBXM_IMAGEGET:
			//Return the current index.
			hMF=(HGLOBAL)GetWindowLong(hWnd, IBWW_HIMAGE);
			return (LONG)(UINT)hMF;

		case IBXM_IMAGEFREE:
			//Free up whatever we're holding.
			hMF=(HGLOBAL)GetWindowLong(hWnd, IBWW_HIMAGE);
			OleUIMetafilePictIconFree(hMF);
			SetWindowLong(hWnd, IBWW_HIMAGE, 0);
			return 1L;

		case IBXM_LABELENABLE:
			//wParam has the new flag, returns the previous flag.
			return (LONG)SetWindowWord(hWnd, IBWW_FLABEL, (WORD)wParam);

		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}

	return 0L;
}
