/*++

Copyright (c) 1994  Microsoft Corporation

Module Name:

    bindi.c

Abstract:
    Implementation for the MapAndLoad API

Author:

Revision History:

--*/

#include <private.h>


BOOL
MapAndLoad(
    LPSTR ImageName,
    LPSTR DllPath,
    PLOADED_IMAGE LoadedImage,
    BOOL DotDll,
    BOOL ReadOnly
    )
{
    HANDLE hFile;
    HANDLE hMappedFile;
    CHAR SearchBuffer[MAX_PATH];
    DWORD dw;
    LPSTR FilePart;
    LPSTR OpenName;

    // open and map the file.
    // then fill in the loaded image descriptor

    LoadedImage->hFile = INVALID_HANDLE_VALUE;

    OpenName = ImageName;
    dw = 0;
retry:
    hFile = CreateFile(
                OpenName,
                ReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );


    if ( hFile == INVALID_HANDLE_VALUE ) {
        if ( !dw ) {
            //
            // open failed try to find the file on the search path
            //

            dw =   SearchPath(
                    DllPath,
                    ImageName,
                    DotDll ? ".dll" : ".exe",
                    MAX_PATH,
                    SearchBuffer,
                    &FilePart
                    );
            if ( dw && dw < MAX_PATH ) {
                OpenName = SearchBuffer;
                goto retry;
            }
        }
        return FALSE;
    }
    hMappedFile = CreateFileMapping(
                    hFile,
                    NULL,
                    ReadOnly ? PAGE_READONLY : PAGE_READWRITE,
                    0,
                    0,
                    NULL
                    );
    if ( !hMappedFile ) {
        CloseHandle(hFile);
        return FALSE;
    }

    LoadedImage->MappedAddress = (PUCHAR) MapViewOfFile(
                                    hMappedFile,
                                    ReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE,
                                    0,
                                    0,
                                    0
                                    );

    CloseHandle(hMappedFile);

    if (!LoadedImage->MappedAddress ||
        !CalculateImagePtrs(LoadedImage)) {
        CloseHandle(hFile);
        return(FALSE);
    }

    // If readonly, no need to keep the file open..

    if (ReadOnly) {
        CloseHandle(hFile);
        LoadedImage->SizeOfImage = 0;
    } else {
        LoadedImage->hFile = hFile;
        LoadedImage->SizeOfImage = GetFileSize(hFile, NULL);
    }

    return(TRUE);
}

BOOL
CalculateImagePtrs(
    PLOADED_IMAGE LoadedImage
    )
{
    PIMAGE_DOS_HEADER DosHeader;
    BOOL fRC;

    // Everything is mapped. Now check the image and find nt image headers

    fRC = TRUE;  // Assume the best

    __try {
        DosHeader = (PIMAGE_DOS_HEADER)LoadedImage->MappedAddress;

        if ((DosHeader->e_magic != IMAGE_DOS_SIGNATURE) &&
            (DosHeader->e_magic != IMAGE_NT_SIGNATURE)) {
            fRC = FALSE;
            goto tryout;
        }

        if (DosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
            if (DosHeader->e_lfanew == 0) {
                LoadedImage->fDOSImage = TRUE;
                fRC = FALSE;
                goto tryout;
            }
            LoadedImage->FileHeader = (PIMAGE_NT_HEADERS)((ULONG)DosHeader + DosHeader->e_lfanew);
        } else {

            // No DOS header indicates an image built w/o a dos stub

            LoadedImage->FileHeader = (PIMAGE_NT_HEADERS)((ULONG)DosHeader);
        }

        if ( LoadedImage->FileHeader->Signature != IMAGE_NT_SIGNATURE ) {
            if ( (USHORT)LoadedImage->FileHeader->Signature == (USHORT)IMAGE_OS2_SIGNATURE ||
                 (USHORT)LoadedImage->FileHeader->Signature == (USHORT)IMAGE_OS2_SIGNATURE_LE
               ) {
                LoadedImage->fDOSImage = TRUE;
            }

            fRC = FALSE;
            goto tryout;
        }

        // No optional header indicates an object...

        if ( !LoadedImage->FileHeader->FileHeader.SizeOfOptionalHeader ) {
            fRC = FALSE;
            goto tryout;
        }

        // Image Base > 0x80000000 indicates a system image

        if ( LoadedImage->FileHeader->OptionalHeader.ImageBase >= 0x80000000 ) {
            LoadedImage->fSystemImage = TRUE;
        }

        // Check for versions < 2.50

        if ( LoadedImage->FileHeader->OptionalHeader.MajorLinkerVersion < 3 &&
             LoadedImage->FileHeader->OptionalHeader.MinorLinkerVersion < 5 ) {
            fRC = FALSE;
            goto tryout;
        }

        InitializeListHead( &LoadedImage->Links );
        LoadedImage->Characteristics = LoadedImage->FileHeader->FileHeader.Characteristics;
        LoadedImage->NumberOfSections = LoadedImage->FileHeader->FileHeader.NumberOfSections;
        LoadedImage->Sections = (PIMAGE_SECTION_HEADER)((ULONG)LoadedImage->FileHeader + sizeof(IMAGE_NT_HEADERS));
        LoadedImage->LastRvaSection = LoadedImage->Sections;

tryout:
        if (fRC == FALSE) {
            UnmapViewOfFile(LoadedImage->MappedAddress);
        }

    } __except ( EXCEPTION_EXECUTE_HANDLER ) {
        fRC = FALSE;
    }

    return fRC;
}

VOID
UnMapAndLoad(
    PLOADED_IMAGE pLi
    )
{
    DWORD HeaderSum, CheckSum;

    // Test for read-only
    if (pLi->hFile == INVALID_HANDLE_VALUE) {
        FlushViewOfFile(pLi->MappedAddress, pLi->SizeOfImage);
        UnmapViewOfFile(pLi->MappedAddress);
    } else {
        CheckSumMappedFile( pLi->MappedAddress,
                            pLi->SizeOfImage,
                            &HeaderSum,
                            &CheckSum
                          );
        pLi->FileHeader->OptionalHeader.CheckSum = CheckSum;

        FlushViewOfFile(pLi->MappedAddress, pLi->SizeOfImage);
        UnmapViewOfFile(pLi->MappedAddress);

        if (pLi->SizeOfImage != GetFileSize(pLi->hFile, NULL)) {
            SetFilePointer(pLi->hFile, pLi->SizeOfImage, NULL, FILE_BEGIN);
            SetEndOfFile(pLi->hFile);
        }

        CloseHandle(pLi->hFile);
    }
}


BOOL
GetImageConfigInformation(
    PLOADED_IMAGE LoadedImage,
    PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation
    )
{
    PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigData;
    ULONG i;

    ImageConfigData = ImageDirectoryEntryToData( LoadedImage->MappedAddress,
                                                 FALSE,
                                                 IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG,
                                                 &i
                                               );
    if (ImageConfigData != NULL && i == sizeof( *ImageConfigData )) {
        memcpy( ImageConfigInformation, ImageConfigData, sizeof( *ImageConfigData ) );
        return TRUE;
        }
    else {
        return FALSE;
        }
}

BOOL
SetImageConfigInformation(
    PLOADED_IMAGE LoadedImage,
    PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation
    )
{
    PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigData;
    ULONG i, DirectoryAddress;

    if (LoadedImage->hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
        }

    ImageConfigData = ImageDirectoryEntryToData( LoadedImage->MappedAddress,
                                                 FALSE,
                                                 IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG,
                                                 &i
                                               );
    if (ImageConfigData != NULL && i == sizeof( *ImageConfigData )) {
        memcpy( ImageConfigData, ImageConfigInformation, sizeof( *ImageConfigData ) );
        return TRUE;
        }

    DirectoryAddress = GetImageUnusedHeaderBytes( LoadedImage, &i );
    if (i < sizeof(*ImageConfigData)) {
        return FALSE;
        }

    LoadedImage->FileHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG ].VirtualAddress = DirectoryAddress;
    LoadedImage->FileHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG ].Size = sizeof( *ImageConfigData );
    ImageConfigData = (PIMAGE_LOAD_CONFIG_DIRECTORY)
        ((PCHAR)LoadedImage->MappedAddress + DirectoryAddress);
    memcpy( ImageConfigData, ImageConfigInformation, sizeof( *ImageConfigData ) );
    return TRUE;
}


BOOLEAN ImageLoadInit;
LIST_ENTRY ImageLoadList;

PLOADED_IMAGE
ImageLoad(
    LPSTR DllName,
    LPSTR DllPath
    )
{
    PLIST_ENTRY Head,Next;
    PLOADED_IMAGE LoadedImage;

    if (!ImageLoadInit) {
        InitializeListHead( &ImageLoadList );
        ImageLoadInit = TRUE;
        }

    Head = &ImageLoadList;
    Next = Head->Flink;

    while (Next != Head) {
        LoadedImage = CONTAINING_RECORD( Next, LOADED_IMAGE, Links );
        if (!stricmp( DllName, LoadedImage->ModuleName )) {
            return LoadedImage;
            }

        Next = Next->Flink;
        }

    LoadedImage = (PLOADED_IMAGE)HeapAlloc( GetProcessHeap(),
                                            HEAP_ZERO_MEMORY,
                                            sizeof( *LoadedImage ) + strlen( DllName ) + 1
                                          );
    if (LoadedImage != NULL) {
        LoadedImage->ModuleName = (LPSTR)(LoadedImage + 1);
        strcpy( LoadedImage->ModuleName, DllName );
        if (MapAndLoad( DllName, DllPath, LoadedImage, TRUE, TRUE )) {
            InsertTailList( &ImageLoadList, &LoadedImage->Links );
            return LoadedImage;
            }

        HeapFree( GetProcessHeap(), 0, LoadedImage );
        LoadedImage = NULL;
        }

    return LoadedImage;
}

BOOL
ImageUnload(
    PLOADED_IMAGE LoadedImage
    )
{
    if (!IsListEmpty( &LoadedImage->Links )) {
        RemoveEntryList( &LoadedImage->Links );
        }

    UnMapAndLoad( LoadedImage );
    return HeapFree( GetProcessHeap(), 0, LoadedImage );
}
