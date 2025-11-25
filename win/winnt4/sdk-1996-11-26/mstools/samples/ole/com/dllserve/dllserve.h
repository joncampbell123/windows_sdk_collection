/*+==========================================================================
  File:      DLLSERVE.H

  Summary:   Include file for the DLLSERVE.DLL dynamic link library.
             Intended primarily for external users of this DLL who
             exploit it via static linkage to the DllRegisterServer and
             DllUnregisterServer exported service calls.

             For a comprehensive tutorial code tour of DLLSERVE's
             contents and offerings see the accompanying DLLSERVE.TXT file.
             For more specific technical details on the internal workings
             see the comments dispersed throughout the DLLSERVE source code.

  Classes:   none.

  Functions: DllRegisterServer, DllUnregisterServer.

  Origin:    9-11-95: atrent - Editor-inheritance from COMOBJ.H in
               the COMOBJ OLE Tutorial Code Sample.

----------------------------------------------------------------------------
  This file is part of the Microsoft OLE Tutorial Code Samples.

  Copyright (C) Microsoft Corporation, 1996.  All rights reserved.

  This source code is intended only as a supplement to Microsoft
  Development Tools and/or on-line documentation.  See these other
  materials for detailed information regarding Microsoft code samples.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
  PARTICULAR PURPOSE.
==========================================================================+*/

#if !defined(DLLSERVE_H)
#define DLLSERVE_H

#if !defined(RC_INCLUDE)

#if !defined(_DLLEXPORT_)

// If _DLLEXPORT_ is NOT defined then the default is to import.
#define DLLENTRY EXTERN_C __declspec(dllimport)
#define STDENTRY EXTERN_C __declspec(dllimport) HRESULT STDAPICALLTYPE
#define STDENTRY_(type) EXTERN_C __declspec(dllimport) type STDAPICALLTYPE

// Here is the list of server APIs offered by the DLL (using the
// appropriate entry API declaration macros just #defined above).

STDENTRY DllRegisterServer(void);

STDENTRY DllUnregisterServer(void);

#else  // _DLLEXPORT_

// Else if _DLLEXPORT_ is indeed defined then we've been told to export.
#define DLLENTRY EXTERN_C __declspec(dllexport)
#define STDENTRY EXTERN_C __declspec(dllexport) HRESULT STDAPICALLTYPE
#define STDENTRY_(type) EXTERN_C __declspec(dllexport) type STDAPICALLTYPE

#endif // _DLLEXPORT_

#endif // RC_INCLUDE

#endif // DLLSERVE_H
