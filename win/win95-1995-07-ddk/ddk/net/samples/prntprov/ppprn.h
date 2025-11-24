/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

//
// PPPRN.H
//
// Prototypes for private functions in PPPRN.C
//

BOOL ValidatePrinterHandle(HANDLE pPrinter);
BOOL IsServerHandle(HANDLE hPrinter);

BOOL PrintFileIsOpen(HANDLE Printer);

HANDLE CreatePrinterHandle(LPSTR lpServerName,
                           LPSTR lpDeviceName,
                           BOOL  bConnected,
                           LPPRINTER_DEFAULTS pDefaults);

BOOL DestroyPrinterHandle(HANDLE hPrinter);
