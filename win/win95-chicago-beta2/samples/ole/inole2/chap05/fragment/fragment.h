/*
 * FRAGMENT.H
 * Fragmented File Generator Chapter 5
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_


#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <initguid.h>
#include <ole2ver.h>
#include <string.h>
#include <book1632.h>

#define CSTREAMS    26
#define CCHPATHMAX  256


class CFragment
    {
    private:
        BOOL        m_fInitialized;         //CoInitilize?
        IMalloc    *m_pIMalloc;             //Task allocator
        char       *m_pch;                  //Char array (8 bits)
        TCHAR      *m_pszScratch;           //Scratch space

        UINT        m_cch;                  //Stream size
        UINT        m_cStreams;             //Stream count.
        TCHAR       m_szFile[CCHPATHMAX];   //File in use.

        HCURSOR     m_hCur;                 //Saved cursor.


    public:
        CFragment::CFragment(void);
        CFragment::~CFragment(void);

        BOOL        Init(void);
        BOOL        AllocCharArrays(void);
        BOOL        CreateFragmentedFile(void);
        void        FreeSpaceInFile(void);
        void        DefragmentFile(void);

    private:
        void        StreamCreate(IStorage *, UINT, IStream **);
        void        Message(LPTSTR);

       #ifndef WIN32
        //Substitute for Win32 MoveFile under Win16
        void        MyMoveFile(char *, char *);
       #endif
    };

#endif //_FRAGMENT_H_
