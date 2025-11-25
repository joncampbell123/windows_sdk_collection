//
//  Common header file for entire VidClip project.
//
#ifndef __VCPROJ_H__
#define __VCPROJ_H__

// Windows Header Files:
#include <windows.h>
#include <commctrl.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// Multi-Media stream header files
#include <amstream.h>

//
//  Required for ATL stuff
//  
#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED

#include "atlbase.h"
extern CComModule _Module;
#include "atlcom.h"

// Local Header Files
#include "vidclip.h"
#include "document.h"
#include "resource.h"

#endif  // #ifndef __VCPROJ_H__