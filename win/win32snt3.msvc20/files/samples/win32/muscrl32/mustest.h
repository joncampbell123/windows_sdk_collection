/*
 * MUSTEST.H
 *
 * Contains definitions specific to the MicroScroll test
 * application, such as control identifiers, menu item
 * identifiers, and internal function prototypes.
 *
 * Win32 & Win32 control format port, April 1994
 *     Tarn Faulkner, Microsoft Corporation
 *
 * Version 1.1, September 1991
 *     Kraig Brockschmidt, Microsoft Corporation
 *
 * Version 1.0, October 1990
 *     Kraig Brockschmidt, Microsoft Corporation
 */

#include "mustres.h"

//Control Identifiers.
#define ID_NULL             -1
#define ID_VERTEDIT         300
#define ID_HORZEDIT         301
#define ID_HORZSCROLL       302
#define ID_VERTSCROLL       303


//Special style used for MicroScroll on a 'spin' button.
#define MSS_SPIN (MSS_VERTICAL | MSS_TEXTHASRANGE | MSS_INVERTRANGE)


//Function prototypes.
LRESULT CALLBACK MusTestWndProc(HWND, UINT, WPARAM, LPARAM);
