/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPUTIL.C .
 *   
 * MS Net print provider utility functions.
 *
 */          
 
#include "mspp.h"
#include <netlib.h>

static CRITICAL_SECTION PPCriticalSection;

//static BOOL bLibLoaded = FALSE;


/*****************************************************************
 PPInitCritical

 Sets up the critical section object for the MS Network print
 provider. 
*****************************************************************/
void PPInitCritical() {

  InitializeCriticalSection(&PPCriticalSection);

}

/*****************************************************************
 PPDeleteCritical

 Frees all system resources associated with the PP's critical
 section.

*****************************************************************/
void PPDeleteCritical() {

  DeleteCriticalSection(&PPCriticalSection);

}

/*****************************************************************
 PPEnterCritical

 Attempts to enter the print provider's critical section or blocks
 until the critical section is available.

*****************************************************************/
void PPEnterCritical() {

  EnterCriticalSection(&PPCriticalSection);

}

/*****************************************************************
 PPLeaveCritical

 Releases ownership of the print provider's critical section.

*****************************************************************/
void PPLeaveCritical() {
  
  LeaveCriticalSection(&PPCriticalSection);

}
//
// Additional debugging code for memory allocator. If DEBUG is 
// defined, we keep a count of allocs and frees.  When shutting
// down the MSPP DLL, a call to PPMemLeakCheck will verify
// that no unfreed blocks remain.
//
#ifdef DEBUG  

static DWORD dwPPAllocCount = 0;
#define IncAllocCount() PPEnterCritical();\
                        dwPPAllocCount++;\
                        PPLeaveCritical();

#define DecAllocCount() PPEnterCritical();\
                        dwPPAllocCount--;\
                        PPLeaveCritical();

/*******************************************************************
  _pp_mem_leak_check

  To be called before unloading MSPP.DLL, after freeing all 
  allocated memory.  Checks to make sure that the number of 
  calls to PPAllocMem matches the number of calls to PPFreeMem.

  Returns TRUE if the numbers match, FALSE if they don't, which
  indicates a memory leak. 

*******************************************************************/
BOOL _pp_mem_leak_check() {

  return (dwPPAllocCount == 0);

}

#else

#define IncAllocCount()
#define DecAllocCount()

#endif

/*******************************************************************
  PPInitHeap
  
  Allocates a heap for this DLL's local use.  Returns TRUE if 
  successful, or if a heap has already been allocated, FALSE
  if an error occurs while attempting to allocate the heap.

*******************************************************************/
BOOL PPInitHeap() {

    return TRUE;		/* no longer need to initialize shell's heap */

}

/*******************************************************************
  PPAllocMem

  Allocates dwCount bytes of memory for use by the print provider 
  DLL.  May allocate extra memory and fill with debugging information
  if DEBUG is defined.

  Returns a pointer to the allocated memory if successfull, NULL
  if unable to allocate the memory.  Extended error status is 
  available from GetLastError.

******************************************************************/
LPVOID PPAllocMem(DWORD dwCount) {
    LPDWORD  pMem;
    DWORD    cbNew;

//
// Give us a little room for debugging info and make sure that our
// size in bytes results in a whole number of DWORDs.
//
    cbNew = dwCount + 2 * sizeof(DWORD);

    if (cbNew & 3)
        cbNew += sizeof(DWORD) - (cbNew & 3);

//
// Attempt to allocate the memory
//
    pMem = (LPDWORD) MemAlloc(cbNew);

    if (!pMem) {
      DBGMSG(DBG_LEV_ERROR,
             ("MSPP.PPAllocMem failed on request for %ld bytes.\n",
             cbNew));

      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return NULL;
    }

//
// Set up extra tracking information -- region size at the front
// and an easily distinguishable constant at the back to help
// us detect overwrites. (constant value from ccteng)
//
    *pMem = dwCount;
    *(LPDWORD)((LPBYTE)pMem + cbNew - sizeof(DWORD))=0xdeadbeef;

    IncAllocCount();

    return (LPVOID) (pMem+1);
}

/*******************************************************************
  PPValidateMem

  Checks memory blocks allocated by PPAllocMem. These blocks contain
  debugging information that helps to check for pointer overruns and
  underruns.

  Returns TRUE if the region is OK, FALSE if any problems are
  detected. 

*******************************************************************/
BOOL PPValidateMem(LPVOID pMem, DWORD cb) {
  LPDWORD pBlock;
  DWORD  cbNew;

  if (pMem == NULL) {

    DBGMSG(DBG_LEV_ERROR, 
           ("MSPP.PPValidateMem - ERROR.  pMem is NULL.\n"));

    return FALSE;
  }

  pBlock = (LPDWORD) pMem;
  pBlock--;

//
// Calculate the "real" size of our allocated block and round it up to
// an even number of DWORDs.
//
  cbNew = cb + 2 * sizeof(DWORD);

  if (cbNew & 3)
        cbNew += sizeof(DWORD) - (cbNew & 3);

//
// Compare the values that PPAllocMem stored at the beginning and end
// of the block
//
  if ( (*pBlock != cb) ||
    (*(LPDWORD)((LPBYTE)pBlock + cbNew - sizeof(DWORD)) != 0xdeadbeef)) {

    DBGMSG(DBG_LEV_ERROR, 
           ("ERR: MSPP.PPValidateMem - bad memory block at %0lx\n,pMem"));

    return FALSE;

  }

//
// The block passes all our tests, so it MUST be OK. Right?
//
  return TRUE;
}

/*******************************************************************
  PPFreeMem

  Frees memory allocated with PPAllocMem and reports any 
  pointer overruns.
*******************************************************************/
BOOL PPFreeMem(LPVOID pMem, DWORD  cb ) {
  LPDWORD pNewMem;

//
// Try to at least make sure it's our memory and that no pointers have
// gone astray in it.
//
  if (!PPValidateMem(pMem,cb)) {

    return FALSE;

  }

//
// Point to the true beginning of our allocated block and free it.
//
  pNewMem = (LPDWORD) pMem;
  pNewMem--;

  MemFree(pNewMem);

  DecAllocCount();

  return TRUE;
}

/*******************************************************************
  PPAllocString

  Allocates enough memory to store the specified string, then copies
  the string to the newly allocated memory.  Returns a pointer to 
  the new string if successful, NULL if unable to allocate sufficient
  memory.

*******************************************************************/
LPSTR PPAllocString(LPSTR pStr) {
   LPSTR pMem;

//
// Make sure we got passed a valid pointer
//
   if (pStr == NULL) {

     return NULL;

   }

//
// Allocate memory for the string, including room for the terminator
//
   pMem = (LPSTR) PPAllocMem((1 + lstrlen(pStr)) * sizeof(TCHAR));

   if (pMem != NULL) {

     lstrcpy(pMem,pStr);

   }

   return pMem;
}

/*******************************************************************
  PPFreeString

  Frees a string allocated with PPAllocString. Returns TRUE if
  successful, FALSE otherwise.

*******************************************************************/
BOOL PPFreeString(LPSTR pStr) {
  BOOL result;

  result = PPFreeMem(pStr,(1 + lstrlen(pStr)) * sizeof(TCHAR));

  return result;

}

/*******************************************************************
  nls_strchr

  Returns a pointer to the first occurrence of ch in lpString.  
  Uses AnsiNext to work correctly with DBCS strings.

*******************************************************************/
LPSTR nls_strchr(LPSTR lpString,TCHAR ch) {
 
   while (*lpString != 0) {

     if (*lpString == ch)
       return lpString;

     lpString = AnsiNext(lpString);

   }

   return NULL;
}

/*******************************************************************
  lstrsize

  Returns the number of bytes needed to store a string, including
  its trailing 0.  Returns 0 if the supplied string pointer is
  NULL.

*******************************************************************/
int lstrsize(LPSTR string) {

  if (string == NULL) return 0;

  return 1 + lstrlen(string);

}

/*******************************************************************
  nls_lstrcpyn
  
  Copies as much of a string as possible into a buffer <n> bytes
  long. The resulting string is 0 terminated, regardless of 
  whether we got the whole thing or not. 

*******************************************************************/
void nls_lstrcpyn(LPSTR dest,LPSTR src,DWORD n) {
  DWORD Count;


  Count = 0;
  n = min(n - 1,lstrlen(src));

  while (Count < n) {

    if (*src == 0) break;

//
// If it's a lead byte and we have room, copy two bytes. If we
// don't have room for the whole character, we'll stop here.
//
    if (IsDBCSLeadByte(*src)) {

      if ((n - Count) < 2) break;

      *dest++ = *src++;
      *dest++ = *src++;
      Count += 2;

    }

//
// If it's a single byte character, just copy it.
//
    else {

      *dest++ = *src++;
      Count++;
    }
  }

  dest[Count] = 0;

}

/*******************************************************************
  is_unc_name

  Returns TRUE if the specified string follows the format of a
  valid UNC name, FALSE otherwise.  Remember that just because
  a name follows the UNC format doesn't mean it's actually a
  valid name.

*******************************************************************/
BOOL is_unc_name(LPSTR lpName) {
  LPSTR lpSlash;
  int len;

//
// Name must begin with \\something. Just \\ isn't valid.
//
  if (lstrlen(lpName) >= 3) {

    if ((*lpName == '\\') && (*(lpName + 1) == '\\')) {

//
// We have a double backslash.  Now check to make sure that the
// server name portion of the name is about the right length.
//
      lpSlash = nls_strchr(lpName + 2,'\\');

      if (lpSlash != NULL) {

        len = lpSlash - lpName;

      } 
      else {

        len = lstrlen(lpName);

      }

      if (len <= 18) {

        DBGMSG(DBG_LEV_VERBOSE,("MSPP.is_unc_name: %s is a UNC name.",lpName));

        return TRUE;
      }
    }
  }

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.is_unc_name: %s is NOT a UNC name.",lpName));

  return FALSE;
}


/*******************************************************************
  ResolveDeviceName

  Given a string that may or may not be a valid network printer
  name, make a valiant attempt to translate it to a UNC path to
  a device, then to separate the server name from the device name.


  The incoming name can be in one of three formats (1) UNC - in
  which case, we have nothing to do, (2) DOS logical device, e.g. 
  LPT1:, in which case we must look it up in the redirection
  table, or (3) "friendly" name, in which case we do some, as yet
  unknown, thing.

                              ----------

  NOTE: For this version, the local provider should take care
  of all friendly names.  We will only get UNC paths and possibly
  DOS device names for redirected devices.


  Either lpServer or lpDevice can be NULL if you just want to
  extract part of a UNC name.

                              ----------

  Returns TRUE if the name was converted successfully, FALSE 
  otherwise.

*******************************************************************/
BOOL ResolveDeviceName(LPSTR lpInName,LPSTR lpServer, LPSTR lpDevice) {
  TCHAR szNetPath[MAX_PATH];
  TCHAR szTemp[MAX_PATH];
  LPSTR lpString;
  UINT uResult,uLength;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.ResolveDeviceName(%s)\n",lpInName));

//
// Make sure the incoming name string isn't too long for us to handle.
// If it's longer than MAX_PATH, it can't possibly be ours.
//
  if (lstrlen(lpInName) > MAX_PATH - 1) return FALSE;


//
// UNC paths must begin with a a double backslash. If we don't have
// that, we'll assume it's a DOS device name. In that case, we try
// to get the network to resolve it by calling WNetGetConnection.
//
  if (is_unc_name(lpInName)) {

    lstrcpy(szNetPath,lpInName);
    CharUpper(szNetPath);
//
// If there's a trailing colon, kill it.
//   
    lpString = nls_strchr(szNetPath,':');
    if (lpString != NULL) *lpString = 0;

  }
  else {

    DBGMSG(DBG_LEV_VERBOSE,("  Calling WNetGetConnection\n"));

//
// Just in case it is a local device name, kill the first colon we see.
// WNetGetConnection likes printer names without 'em.  Also, make sure
// the name is OEM format.
//

    lstrcpy(szTemp,lpInName);
    CharUpper(szTemp);

    lpString = nls_strchr(szTemp,':');
    if (lpString != NULL) *lpString = 0;

    uLength = MAX_PATH;

    uResult = WNetGetConnection(szTemp,szNetPath,(LPDWORD) &uLength);

    if (uResult != WN_SUCCESS) return FALSE;

    if (!is_unc_name(szNetPath)) return FALSE;
  }

//
// At this point, szNetPath contains a valid UNC path. Now we'll attempt
// to break it into separate server and device strings.
//
  lpString = nls_strchr(szNetPath + (2 * sizeof(TCHAR)),'\\');

//
// If the path consists only of a server name, arrange it so we have
// a pointer to a zero length "device name", just for neatness.
//
  if (lpString == NULL) {

    lpString = szNetPath + lstrlen(szNetPath) - 1;

  }
  else {

    *lpString = 0;

  }

  if (lpServer != NULL) lstrcpy(lpServer,szNetPath);
  if (lpDevice != NULL) lstrcpy(lpDevice,lpString + 1);

  DBGMSG(DBG_LEV_VERBOSE,("  %s resolved OK\n",lpInName));

  return TRUE;
}

/*******************************************************************
  PPSetString
  
  Copies strings into a Win32 format buffer -- a structure at the
  front of the buffer and strings packed into the end.
  
  On entry, *buf should point to the last available byte in the
  buffer.

*******************************************************************/
void PPSetString(LPSTR *dest,LPSTR src,LPSTR *buf) {
  
  if (src != NULL) {

// Place string at end of buffer.
//
    (*buf) -= lstrlen(src);
    lstrcpy(*buf,src);

// Place string address in structure and save pointer to new
// last available byte.
//
    *dest = *buf;
    (*buf)--;

  }
  else *dest = NULL;
}

/*******************************************************************
  PPCopyString

  Safely copies a string from one location to another. Checks for
  NULL source or destination.

*******************************************************************/
void PPCopyString(LPSTR dest,LPSTR src) {

  if ((src == NULL) || (dest == NULL)) return;
  lstrcpy(dest,src);

}

/*******************************************************************
  PPCopyMem

  Copies a block of memory into a Win32 format buffer -- a structure 
  at the front of the buffer and strings packed into the end.
  
  On entry, *buf should point to the last available byte in the
  buffer.

*******************************************************************/
void PPCopyMem(LPSTR *dest,LPSTR src,DWORD cbSize,LPSTR *buf) {
  
  if (src != NULL) {

// Place string at end of buffer.
//
    (*buf) -= cbSize + 1;
    memcpyf(*buf,src,cbSize);

// Place buffer address in structure and save pointer to new
// last available byte.
//
    *dest = *buf;
    (*buf)--;

  }
  else *dest = NULL;
}

