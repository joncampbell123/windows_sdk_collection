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
// err.h: Declares data, defines and struct types for error handling
//          module.
//

#ifndef __ERR_H__
#define __ERR_H__

// Requires comm.h to be included prior to this
//

// Messagebox type flags, used by MsgBox_* macros
//
#define MSG_ERROR       1
#define MSG_INFO        2
#define MSG_QUESTION    3


// Message box macros
//

//      int MsgBox_Err(HWND hwndParent, UINT ids, UINT idsCaption);
//          Invoke error message (with ! icon)
//
#define MsgBox_Err(hwnd, ids, idsCap)       MsgBoxIds(hwnd, ids, idsCap, MSG_ERROR)

//      int MsgBox_Info(HWND hwndParent, UINT ids, UINT idsCaption);
//          Invoke info message (with i icon)
//
#define MsgBox_Info(hwnd, ids, idsCap)      MsgBoxIds(hwnd, ids, idsCap, MSG_INFO)

//      int MsgBox_Question(HWND hwndParent, UINT ids, UINT idsCaption);
//          Invoke question message (with ? icon and Yes/No buttons)
//
#define MsgBox_Question(hwnd, ids, idsCap)  MsgBoxIds(hwnd, ids, idsCap, MSG_QUESTION)

//      int MsgBox_ErrSz(HWND hwndParent, LPCSTR psz, UINT idsCaption);
//          Invoke error message (with ! icon)
//
#define MsgBox_ErrSz(hwnd, lpsz, idsCap)        MsgBoxSz(hwnd, lpsz, idsCap, MSG_ERROR, NULL)

//      int MsgBox_InfoSz(HWND hwndParent, LPCSTR psz, UINT idsCaption);
//          Invoke info message (with i icon)
//
#define MsgBox_InfoSz(hwnd, lpsz, idsCap)       MsgBoxSz(hwnd, lpsz, idsCap, MSG_INFO, NULL)

//      int MsgBox_QuestionSz(HWND hwndParent, LPCSTR psz, UINT idsCaption);
//          Invoke question message (with ? icon and Yes/No buttons)
//
#define MsgBox_QuestionSz(hwnd, lpsz, idsCap)   MsgBoxSz(hwnd, lpsz, idsCap, MSG_QUESTION, NULL)


int PUBLIC MsgBoxIds (HWND hwndParent, UINT ids, UINT idsCaption, UINT nBoxType);
int PUBLIC MsgBoxSz (HWND hwndParent, LPCSTR lpsz, UINT idsCaption, UINT nBoxType, HANDLE hinst);

// Debugging macros
//

#ifdef DEBUG

#define ASSERTSEG

// Use this macro to declare message text that will be placed
// in the CODE segment (useful if DS is getting full)
//
// Ex: DEBUGTEXT(szMsg, "Invalid whatever: %d");
//
#define DEBUGTEXT(sz, msg)	/* ;Internal */ \
    static const char ASSERTSEG sz[] = msg;

void PUBLIC MyAssertFailed(LPCSTR szFile, int line);
void CPUBLIC MyAssertMsg(BOOL f, LPCSTR pszMsg, ...);

// ASSERT(f)  -- Generate "assertion failed in line x of file.c"
//               message if f is NOT true.
//
#define ASSERT(f)                                                       \
    {                                                                   \
        DEBUGTEXT(szFile, __FILE__);                                    \
        if (!(f))                                                       \
            MyAssertFailed(szFile, __LINE__);                          \
    }
#define ASSERT_E(f)  ASSERT(f)

// ASSERT_MSG(f, msg, args...)  -- Generate wsprintf-formatted msg w/params
//                          if f is NOT true.
//
#define ASSERT_MSG   MyAssertMsg

#else

#define ASSERT(f)
#define ASSERT_E(f)      (f)
#define ASSERT_MSG   1 ? (void)0 : (void)

#endif // DEBUG


#endif // __ERR_H__

