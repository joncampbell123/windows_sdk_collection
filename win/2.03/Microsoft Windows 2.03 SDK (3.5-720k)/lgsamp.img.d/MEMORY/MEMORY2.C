/****************************************************************************
    MODULE:  memory2.c

    FUNCTION: MemoryInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

#include "windows.h"
#include "memory.h"

BOOL MemoryInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) NULL;
    pWndClass->lpszClassName = (LPSTR) "Memory";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = MemoryWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}
