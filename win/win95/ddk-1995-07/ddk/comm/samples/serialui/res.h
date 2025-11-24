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

//
// res.h: Declares data, defines and struct types for common code
//				  module.
//
// This file contains only #define directives, thus it can be
// included in the resource script.
//

#ifndef __RES_H__
#define __RES_H__

/////////////////////////////////////////////////////  DIALOG CONSTANTS
//
// IDD_ naming conventions: IDD_xxdd*
//
// where xx defines the control type:
//   ST: static text		
//   LB: listbox
//   CB: combobox
//   CH: checkbox
//   PB: pushbutton
//   ED: edit control
//   GB: group box
//   RB: radio button
//   IC: icon
//
// and dd is a two-letter initial for the dialog;
// and * is any other qualifier to better explain the name of control.
//


#include "resids.h"
#include "dlgids.h"


#endif // __RES_H__

