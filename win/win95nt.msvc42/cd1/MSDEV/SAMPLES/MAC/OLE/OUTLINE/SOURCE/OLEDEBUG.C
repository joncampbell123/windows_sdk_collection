#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#if qOleDebug

#include "OleDebug.h"
Boolean		gOleBreakInterfaceCalls = false;			// set this flag if you want to break on
														// entry to Ole interfaces

#endif
