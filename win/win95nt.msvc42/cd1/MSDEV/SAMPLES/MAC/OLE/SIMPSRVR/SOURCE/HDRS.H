#ifdef _MSC_VER
#include <msvcmac.h>
#endif 

#ifdef _MSC_VER
#define PASCAL_CALL(return_type) return_type __pascal
#else
#define PASCAL_CALL(return_type) pascal return_type
#endif


#ifndef __SC__
    #include "mac.h"
	#ifndef __OLE2__
	#include <ole2.h>
	#endif
#endif

#include "ole2ui.h"
#include "AEvents.h"
#include "App.h"
#include "CObject.h"
#include "CShape.h"
#include "CToolbar.h"
#include "CWindow.h"
#include "Scodes.h"
#include "Doc.h"
#include "icf.h"
#include "ido.h"
#include "imf.h"
#include "ioo.h"
#include "ips.h"
#include "OleError.h"
#include "Debug.h"
#include "Util.h"
#include "stdio.h"
#include <gestaltequ.h>
#include <traps.h>
#include <segload.h>
