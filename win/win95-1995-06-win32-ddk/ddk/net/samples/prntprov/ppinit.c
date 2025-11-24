/*****************************************************************/
/**               Microsoft Windows for Workgroups              **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPINIT.C
 *  
 * Initialization functions and static data for SMB print provider
 *
 */         


#include "mspp.h"

#include <winsplp.h>
#include <netlib.h>
#include "stubs.h"

//
// Description strings for this provider and its execution environment.
//
char PROVIDER_NAME[]      = "Microsoft Networks Print Provider";
char ENVIRONMENT_STRING[] = "Windows 4.0";
char EMPTY_STRING[]       = "";
char BACKSLASH[]          = "\\";

//
// Table of entry points for network printing services.  This table is passed to the
// request router by the InitializePrintProvidor function.
//
// CODEWORK - this table is declared as an array of LPVOIDs until we
// move to Win32 because the PRINTPROVIDOR structure in WINSPLP.H was
// taken directly from NT and does not declare these functions as
// WINAPI, which they must be for Win16.
//
#pragma PROCESS_LOCAL_BEGIN

LPVOID PrintProvidor[] =      {PPOpenPrinter,
                               PPSetJob,
                               PPGetJob,
                               PPEnumJobs,
                               NULL,          // PPAddPrinter,
                               NULL,          // PPDeletePrinter,
                               PPSetPrinter,
                               PPGetPrinter,
                               PPEnumPrinters,
                               stubAddPrinterDriver,
                               stubEnumPrinterDrivers,
                               PPGetPrinterDriver,
                               PPGetPrinterDriverDirectory,
                               stubDeletePrinterDriver,
                               stubAddPrintProcessor,
                               stubEnumPrintProcessors,
                               PPGetPrintProcessorDirectory,
                               stubDeletePrintProcessor,
                               stubEnumPrintProcessorDatatypes,
                               PPStartDocPrinter,
                               PPStartPagePrinter,
                               PPWritePrinter,
                               PPEndPagePrinter,
                               PPAbortPrinter,
                               PPReadPrinter,
                               PPEndDocPrinter,
                               PPAddJob,
                               PPScheduleJob,
                               stubGetPrinterData,
                               stubSetPrinterData,
                               stubWaitForPrinterChange,
                               PPClosePrinter,
                               NULL,          // AddForm,
                               NULL,          // DeleteForm,
                               NULL,          // GetForm,
                               NULL,          // SetForm,
                               NULL,          // EnumForms,
                               stubEnumMonitors,
                               PPEnumPorts,   
                               stubAddPort,
                               stubConfigurePort,
                               stubDeletePort,
                               stubCreatePrinterIC,
                               stubPlayGdiScriptOnPrinterIC,
                               stubDeletePrinterIC,
                               stubAddPrinterConnection,
                               stubDeletePrinterConnection,
                               stubPrinterMessageBox,
                               stubAddMonitor,
                               stubDeleteMonitor
                             };

#pragma PROCESS_LOCAL_END

                               
/////////////////////////////////////////////////////////////////////////////                               
// InitializePrintProvidor
//
// Called by SPOOLEXE to get a copy of our table of print provider
// entry points.  Returns TRUE to indicate success.
//
/////////////////////////////////////////////////////////////////////////////
BOOL WINAPI InitializePrintProvidor(LPPRINTPROVIDOR pPrintProvidor,
                                    DWORD    cbPrintProvidor,        
                                    LPTSTR   pFullRegistryPath) {

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.InitializePrintProvidor\n"));

  memcpyf(pPrintProvidor, (const void *) &PrintProvidor, min(sizeof(PrintProvidor),
          (int) cbPrintProvidor));

  return TRUE;
}

