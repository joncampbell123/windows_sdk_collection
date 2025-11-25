#ifdef _MSC_VER
#include "msvcmac.h"
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
#include "Util.h"
#include "CErrors.h"
#include "App.h"
#include "ias.h"
#include "site.h"
#include "iocs.h"
#include "cwindow.h"
#include "debug.h"
#include "mycoguid.h"
#include "imf.h"
#include "scodes.h"
#include <gestaltequ.h>
#include "traps.h"
#include "oleguid.h"
#include "segload.h"

