/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: color.cpp
 *
 ***************************************************************************/

#include <d3drmwin.h>
#include "viewer.h"
#include <windows.h>
#include <string.h>
#include <d3drm.h>

int ChooseNewColor(HWND win, D3DCOLOR* current)
{
    CHOOSECOLOR cc;
    COLORREF clr;
    COLORREF aclrCust[16];
    int i;

    for (i = 0; i < 16; i++)
        aclrCust[i] = RGB(255, 255, 255);

    clr =
        RGB
        (   (int) (255 * D3DRMColorGetRed(*current)),
            (int) (255 * D3DRMColorGetGreen(*current)),
            (int) (255 * D3DRMColorGetBlue(*current))
        );

    memset(&cc, 0, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = win;
    cc.rgbResult = clr;
    cc.lpCustColors = aclrCust;
    cc.Flags = CC_RGBINIT|CC_FULLOPEN;

    if (ChooseColor(&cc))
    {   *current =
            D3DRMCreateColorRGB
            (   D3DVAL(GetRValue(cc.rgbResult) / D3DVAL(255.0)),
                D3DVAL(GetGValue(cc.rgbResult) / D3DVAL(255.0)),
                D3DVAL(GetBValue(cc.rgbResult) / D3DVAL(255.0))
            );
        return TRUE;
    }
    else return FALSE;
}
