/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This module implements functions for splitting debugging information
    out of an image file and into a separate .DBG file.

Author:

    Steven R. Wood 4-May-1993

Revision History:

--*/

#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

// Helper routines

PIMAGE_NT_HEADERS
ImageNtHeader (
    IN PVOID Base
    );

PVOID
ImageDirectoryEntryToData (
    IN PVOID Base,
    IN BOOLEAN MappedAsImage,
    IN USHORT DirectoryEntry,
    OUT PULONG Size
    );

BOOL
SplitSymbols(
    LPSTR ImageName,
    LPSTR SymbolsPath,
    LPSTR SymbolFilePath
    )
{
    HANDLE FileHandle, SymbolFileHandle;
    HANDLE hMappedFile;
    LPVOID ImageBase;
    PIMAGE_NT_HEADERS NtHeaders;
    LPSTR ImageFileName;
    LPVOID Symbols, SymbolsToFree;
    DWORD SizeOfSymbols, Unused, ImageNameOffset, cb;
    PIMAGE_SECTION_HEADER Sections;
    DWORD SectionNumber, BytesWritten, BytesRead, NewFileSize, HeaderSum, CheckSum;
    PIMAGE_DEBUG_DIRECTORY DebugDirectories;
    PIMAGE_DEBUG_DIRECTORY DebugDirectory;
    PIMAGE_DEBUG_DIRECTORY MiscDebugDirectory;
    PIMAGE_DEBUG_DIRECTORY FpoDebugDirectory;
    DWORD DebugDirectorySize, DbgFileHeaderSize, NumberOfDebugDirectories;
    IMAGE_SEPARATE_DEBUG_HEADER DbgFileHeader;
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    DWORD  ExportedNamesSize;
    LPDWORD pp;
    LPSTR ExportedNames, Src, Dst;
    DWORD i, RvaOffset, ExportDirectorySize;
    PFPO_DATA FpoTable;
    DWORD FpoTableSize;
    PIMAGE_RUNTIME_FUNCTION_ENTRY RuntimeFunctionTable, pSrc;
    DWORD RuntimeFunctionTableSize;
    PIMAGE_FUNCTION_ENTRY FunctionTable, pDst;
    DWORD FunctionTableSize;
    ULONG NumberOfFunctionTableEntries;
    IMAGE_DEBUG_DIRECTORY PhonyFPODbgDir;
    DWORD SavedErrorCode;
    BOOL InsertExtensionSubDir;
    LPSTR ImageFilePathToSaveInImage;

    ImageFileName = ImageName + strlen( ImageName );
    while (ImageFileName > ImageName) {
        if (ImageFileName[ -1 ] == '\\' ||
            ImageFileName[ -1 ] == '/' ||
            ImageFileName[ -1 ] == ':'
           ) {
            break;
            }
        else {
            ImageFileName -= 1;
            }
        }

    if (SymbolsPath == NULL ||
        SymbolsPath[ 0 ] == '\0' ||
        SymbolsPath[ 0 ] == '.'
       ) {
        strncpy( SymbolFilePath, ImageName, ImageFileName - ImageName );
        SymbolFilePath[ ImageFileName - ImageName ] = '\0';
        InsertExtensionSubDir = FALSE;
        }
    else {
        strcpy( SymbolFilePath, SymbolsPath );
        InsertExtensionSubDir = TRUE;
        }

    Dst = SymbolFilePath + strlen( SymbolFilePath );
    if (Dst > SymbolFilePath && Dst[-1] != '\\' && Dst[-1] != '/' && Dst[-1] != ':') {
        *Dst++ = '\\';
        }
    ImageFilePathToSaveInImage = Dst;
    Src = strrchr( ImageFileName, '.' );
    if (Src != NULL && InsertExtensionSubDir) {
        while (*Dst = *++Src) {
            Dst += 1;
            }
        *Dst++ = '\\';
        }
    strcpy( Dst, ImageFileName );
    Dst = strrchr( Dst, '.' );
    if (Dst == NULL) {
        Dst = SymbolFilePath + strlen( SymbolFilePath );
        }
    strcpy( Dst, ".DBG" );

    //
    // open and map the file.
    //
    FileHandle = CreateFile( ImageName,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL
                           );


    if (FileHandle == INVALID_HANDLE_VALUE) {
        return FALSE;
        }

    hMappedFile = CreateFileMapping( FileHandle,
                                     NULL,
                                     PAGE_READWRITE,
                                     0,
                                     0,
                                     NULL
                                   );
    if (!hMappedFile) {
        CloseHandle( FileHandle );
        return FALSE;
        }

    ImageBase = MapViewOfFile( hMappedFile,
                               FILE_MAP_WRITE,
                               0,
                               0,
                               0
                             );
    if (!ImageBase) {
        CloseHandle( hMappedFile );
        CloseHandle( FileHandle );
        return FALSE;
        }

    //
    // Everything is mapped. Now check the image and find nt image headers
    //

    NtHeaders = ImageNtHeader( ImageBase );
    if (NtHeaders == NULL) {
        UnmapViewOfFile( ImageBase );
        CloseHandle( hMappedFile );
        CloseHandle( FileHandle );
        SetLastError( ERROR_BAD_EXE_FORMAT );
        return FALSE;
        }

    if (NtHeaders->OptionalHeader.MajorLinkerVersion < 2 ||
        NtHeaders->OptionalHeader.MinorLinkerVersion < 5
       ) {
        UnmapViewOfFile( ImageBase );
        CloseHandle( hMappedFile );
        CloseHandle( FileHandle );
        SetLastError( ERROR_BAD_EXE_FORMAT );
        return FALSE;
        }

    if (NtHeaders->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED) {
        UnmapViewOfFile( ImageBase );
        CloseHandle( hMappedFile );
        CloseHandle( FileHandle );
        SetLastError( ERROR_ALREADY_ASSIGNED );
        return FALSE;
        }

    DebugDirectories = ImageDirectoryEntryToData( ImageBase,
                                                  FALSE,
                                                  IMAGE_DIRECTORY_ENTRY_DEBUG,
                                                  &DebugDirectorySize
                                                );
    if (DebugDirectories == NULL || DebugDirectorySize == 0) {
        UnmapViewOfFile( ImageBase );
        CloseHandle( hMappedFile );
        CloseHandle( FileHandle );
        SetLastError( ERROR_BAD_EXE_FORMAT );
        return FALSE;
        }
    NumberOfDebugDirectories = DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

    Sections = IMAGE_FIRST_SECTION( NtHeaders );
    for (SectionNumber = 0;
         SectionNumber < NtHeaders->FileHeader.NumberOfSections;
         SectionNumber++
        ) {
        if (Sections[ SectionNumber ].PointerToRawData != 0 &&
            !stricmp( Sections[ SectionNumber ].Name, ".debug" )
           ) {
            break;
            }
        }

    FpoTable = NULL;
    SymbolsToFree = NULL;
    ExportedNames = NULL;
    MiscDebugDirectory = NULL;
    FpoDebugDirectory = NULL;
    if (SectionNumber >= NtHeaders->FileHeader.NumberOfSections) {
        if (NtHeaders->FileHeader.PointerToSymbolTable == 0 ||
            NtHeaders->FileHeader.NumberOfSymbols == 0
           ) {
            goto nosyms;
            }

        NewFileSize = NtHeaders->FileHeader.PointerToSymbolTable;
        DebugDirectory = DebugDirectories;
        for (i=0; i<NumberOfDebugDirectories; i++) {
            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_MISC) {
                NewFileSize = DebugDirectory->PointerToRawData +
                              DebugDirectory->SizeOfData;
                MiscDebugDirectory = DebugDirectory;
                }
            else
            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO) {
                FpoTableSize = DebugDirectory->SizeOfData;
                FpoTable = VirtualAlloc( NULL,
                                         FpoTableSize,
                                         MEM_COMMIT,
                                         PAGE_READWRITE
                                       );
                if ( FpoTable == NULL) {
                    goto nosyms;
                    }
                if (SetFilePointer( FileHandle,
                                    DebugDirectory->PointerToRawData,
                                    NULL,
                                    FILE_BEGIN
                                  ) != DebugDirectory->PointerToRawData
                   ) {
                    goto nosyms;
                    }

                if (!ReadFile( FileHandle, FpoTable, FpoTableSize, &BytesRead, NULL ) ||
                    FpoTableSize != BytesRead
                   ) {
                    goto nosyms;
                    }
                }

            DebugDirectory += 1;
            }

        SizeOfSymbols = GetFileSize( FileHandle, &Unused ) - NewFileSize;
        Symbols = VirtualAlloc( NULL,
                                SizeOfSymbols,
                                MEM_COMMIT,
                                PAGE_READWRITE
                              );
        if (Symbols == NULL) {
            goto nosyms;
            }
        SymbolsToFree = Symbols;

        if (SetFilePointer( FileHandle,
                            NewFileSize,
                            NULL,
                            FILE_BEGIN
                          ) != NewFileSize
           ) {
            goto nosyms;
            }

        if (!ReadFile( FileHandle, Symbols, SizeOfSymbols, &BytesRead, NULL ) ||
            SizeOfSymbols != BytesRead
           ) {
            goto nosyms;
            }
        Sections = NULL;
        }
    else {
        NewFileSize = Sections[ SectionNumber ].PointerToRawData;
        DebugDirectory = DebugDirectories;
        for (i=0; i<NumberOfDebugDirectories; i++) {
            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_MISC) {
                NewFileSize = DebugDirectory->PointerToRawData +
                              DebugDirectory->SizeOfData;
                MiscDebugDirectory = DebugDirectory;
                }
            else
            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO) {
                FpoTableSize = DebugDirectory->SizeOfData;
                FpoTable = VirtualAlloc( NULL,
                                         FpoTableSize,
                                         MEM_COMMIT,
                                         PAGE_READWRITE
                                       );
                if ( FpoTable == NULL) {
                    goto nosyms;
                    }

                memmove( FpoTable,
                         ((PCHAR)ImageBase + DebugDirectory->PointerToRawData),
                         FpoTableSize
                       );
                }

            DebugDirectory += 1;
            }

        Symbols = (PCHAR)ImageBase + NewFileSize;
        SizeOfSymbols = GetFileSize( FileHandle, &Unused ) - NewFileSize;
        }

    ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)
        ImageDirectoryEntryToData( ImageBase,
                                   FALSE,
                                   IMAGE_DIRECTORY_ENTRY_EXPORT,
                                   &ExportDirectorySize
                                 );
    if (ExportDirectory) {
        //
        // This particular piece of magic gets us the RVA of the
        // EXPORT section.  Dont ask.
        //

        RvaOffset = (DWORD)
            ImageDirectoryEntryToData( ImageBase,
                                       TRUE,
                                       IMAGE_DIRECTORY_ENTRY_EXPORT,
                                       &ExportDirectorySize
                                     ) - (DWORD)ImageBase;

        pp = (LPDWORD)((DWORD)ExportDirectory +
                      (DWORD)ExportDirectory->AddressOfNames - RvaOffset
                     );

        ExportedNamesSize = 1;
        for (i=0; i<ExportDirectory->NumberOfNames; i++) {
            Src = (LPSTR)((DWORD)ExportDirectory + *pp++ - RvaOffset);
            ExportedNamesSize += strlen( Src ) + 1;
            }
        ExportedNamesSize = (ExportedNamesSize + 16) & ~15;

        Dst = (LPSTR)LocalAlloc( LPTR, ExportedNamesSize );
        if (Dst != NULL) {
            ExportedNames = Dst;
            pp = (LPDWORD)((DWORD)ExportDirectory +
                          (DWORD)ExportDirectory->AddressOfNames - RvaOffset
                         );
            for (i=0; i<ExportDirectory->NumberOfNames; i++) {
                Src = (LPSTR)((DWORD)ExportDirectory + *pp++ - RvaOffset);
                while (*Dst++ = *Src++) {
                    }
                }
            }
        }
    else {
        ExportedNamesSize = 0;
        }

    RuntimeFunctionTable = (PIMAGE_RUNTIME_FUNCTION_ENTRY)
        ImageDirectoryEntryToData( ImageBase,
                                   FALSE,
                                   IMAGE_DIRECTORY_ENTRY_EXCEPTION,
                                   &RuntimeFunctionTableSize
                                 );
    if (RuntimeFunctionTable == NULL) {
        RuntimeFunctionTableSize = 0;
        FunctionTableSize = 0;
        FunctionTable = NULL;
        }
    else {
        NumberOfFunctionTableEntries = RuntimeFunctionTableSize / sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY );
        FunctionTableSize = NumberOfFunctionTableEntries * sizeof( IMAGE_FUNCTION_ENTRY );
        FunctionTable = (PIMAGE_FUNCTION_ENTRY)VirtualAlloc( NULL,
                                                             FunctionTableSize,
                                                             MEM_COMMIT,
                                                             PAGE_READWRITE
                                                           );
        if (FunctionTable == NULL) {
            goto nosyms;
            }

        pSrc = RuntimeFunctionTable;
        pDst = FunctionTable;
        for (i=0; i<NumberOfFunctionTableEntries; i++) {
            //
            // Make .pdata entries in .DBG file relative.
            //
            pDst->StartingAddress = pSrc->BeginAddress - NtHeaders->OptionalHeader.ImageBase;
            pDst->EndingAddress = pSrc->EndAddress - NtHeaders->OptionalHeader.ImageBase;
            pDst->EndOfPrologue = pSrc->PrologEndAddress - NtHeaders->OptionalHeader.ImageBase;
            pSrc += 1;
            pDst += 1;
            }
        }

    if (!MakeSureDirectoryPathExists( SymbolFilePath )) {
        return FALSE;
        }

    SymbolFileHandle = CreateFile( SymbolFilePath,
                                   GENERIC_WRITE,
                                   0,
                                   NULL,
                                   CREATE_ALWAYS,
                                   0,
                                   NULL
                                 );
    if (SymbolFileHandle == INVALID_HANDLE_VALUE) {
        goto nosyms;
        }

    DbgFileHeaderSize = sizeof( DbgFileHeader ) +
                        (NtHeaders->FileHeader.NumberOfSections * sizeof( IMAGE_SECTION_HEADER )) +
                        ExportedNamesSize +
                        FunctionTableSize +
                        DebugDirectorySize;
    if (FunctionTable != NULL) {
        DbgFileHeaderSize += sizeof( PhonyFPODbgDir );
        memset( &PhonyFPODbgDir, 0, sizeof( PhonyFPODbgDir ) );
        PhonyFPODbgDir.Type = IMAGE_DEBUG_TYPE_EXCEPTION;
        PhonyFPODbgDir.SizeOfData = FunctionTableSize;
        PhonyFPODbgDir.PointerToRawData = DbgFileHeaderSize -
                                          FunctionTableSize;
        if (Sections != NULL) {
            PhonyFPODbgDir.PointerToRawData -= sizeof( IMAGE_SECTION_HEADER );
            }
        }
    DbgFileHeaderSize = ((DbgFileHeaderSize + 15) & ~15);

    if (SetFilePointer( SymbolFileHandle,
                        DbgFileHeaderSize,
                        NULL,
                        FILE_BEGIN
                      ) != DbgFileHeaderSize
       ) {
        BytesWritten = 0;
        }
    else {
        if (!WriteFile( SymbolFileHandle,
                        Symbols,
                        SizeOfSymbols,
                        &BytesWritten,
                        NULL
                      )
           ) {
            BytesWritten = 0;
            }
        }

    if (BytesWritten == SizeOfSymbols) {
        cb = DebugDirectorySize;
        DebugDirectory = DebugDirectories;
        while (cb >= sizeof( *DebugDirectory )) {
            if (DebugDirectory->AddressOfRawData) {
                DebugDirectory->AddressOfRawData = 0;
                }

            if (NewFileSize <= DebugDirectory->PointerToRawData) {
                DebugDirectory->PointerToRawData -= NewFileSize;
                DebugDirectory->PointerToRawData += DbgFileHeaderSize;
                }

            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO) {
                PhonyFPODbgDir = *DebugDirectory;
                DebugDirectory->PointerToRawData = NewFileSize;
                }

            DebugDirectory += 1;
            cb -= sizeof( *DebugDirectory );
            }

        NtHeaders->FileHeader.PointerToSymbolTable = 0;
        NtHeaders->FileHeader.Characteristics |= IMAGE_FILE_DEBUG_STRIPPED;

        if (Sections != NULL) {
            NtHeaders->OptionalHeader.SizeOfImage = Sections[ SectionNumber ].VirtualAddress;
            NtHeaders->OptionalHeader.SizeOfInitializedData -= Sections[ SectionNumber ].SizeOfRawData;
            NtHeaders->FileHeader.NumberOfSections -= 1;
            }

        if (MiscDebugDirectory != NULL && MiscDebugDirectory->Type == IMAGE_DEBUG_TYPE_MISC) {
            ImageNameOffset = MiscDebugDirectory->PointerToRawData +
                              FIELD_OFFSET( IMAGE_DEBUG_MISC, Data );
            if (SetFilePointer( FileHandle, ImageNameOffset, NULL, FILE_BEGIN ) == ImageNameOffset) {
                WriteFile( FileHandle, ImageFilePathToSaveInImage, strlen( ImageFilePathToSaveInImage ) + 1, &Unused, NULL );
                }
            }

        if (FpoTable) {
            memcpy( (PCHAR)ImageBase + NewFileSize,
                    FpoTable,
                    FpoTableSize
                  );

            NewFileSize += PhonyFPODbgDir.SizeOfData;
            }

        CheckSumMappedFile( ImageBase,
                            NewFileSize,
                            &HeaderSum,
                            &CheckSum
                          );
        NtHeaders->OptionalHeader.CheckSum = CheckSum;

        DbgFileHeader.Signature = IMAGE_SEPARATE_DEBUG_SIGNATURE;
        DbgFileHeader.Flags = 0;
        DbgFileHeader.Machine = NtHeaders->FileHeader.Machine;
        DbgFileHeader.Characteristics = NtHeaders->FileHeader.Characteristics;
        DbgFileHeader.TimeDateStamp = NtHeaders->FileHeader.TimeDateStamp;
        DbgFileHeader.CheckSum = CheckSum;
        DbgFileHeader.ImageBase = NtHeaders->OptionalHeader.ImageBase;
        DbgFileHeader.SizeOfImage = NtHeaders->OptionalHeader.SizeOfImage;
        DbgFileHeader.ExportedNamesSize = ExportedNamesSize;
        DbgFileHeader.DebugDirectorySize = DebugDirectorySize;
        DbgFileHeader.NumberOfSections = NtHeaders->FileHeader.NumberOfSections;
        memset( DbgFileHeader.Reserved, 0, sizeof( DbgFileHeader.Reserved ) );
        DebugDirectory = DebugDirectories;

        SetFilePointer( SymbolFileHandle, 0, NULL, FILE_BEGIN );
        WriteFile( SymbolFileHandle,
                   &DbgFileHeader,
                   sizeof( DbgFileHeader ),
                   &BytesWritten,
                   NULL
                 );
        WriteFile( SymbolFileHandle,
                   IMAGE_FIRST_SECTION( NtHeaders ),
                   sizeof( IMAGE_SECTION_HEADER ) * NtHeaders->FileHeader.NumberOfSections,
                   &BytesWritten,
                   NULL
                 );

        if (ExportedNamesSize) {
            WriteFile( SymbolFileHandle,
                       ExportedNames,
                       ExportedNamesSize,
                       &BytesWritten,
                       NULL
                     );
            }

        if (FunctionTable) {
            WriteFile( SymbolFileHandle,
                       &PhonyFPODbgDir,
                       sizeof( PhonyFPODbgDir ),
                       &BytesWritten,
                       NULL
                     );
            }
        else
        if (FpoTable) {
            WriteFile( SymbolFileHandle,
                       &PhonyFPODbgDir,
                       sizeof( PhonyFPODbgDir ),
                       &BytesWritten,
                       NULL
                     );
            DebugDirectory += 1;
            DebugDirectorySize -= sizeof( PhonyFPODbgDir );
            }

        WriteFile( SymbolFileHandle,
                   DebugDirectory,
                   DebugDirectorySize,
                   &BytesWritten,
                   NULL
                 );

        if (FunctionTable) {
            WriteFile( SymbolFileHandle,
                       FunctionTable,
                       FunctionTableSize,
                       &BytesWritten,
                       NULL
                     );
            }

        SetFilePointer( SymbolFileHandle, 0, NULL, FILE_END );
        CloseHandle( SymbolFileHandle );

        FlushViewOfFile( ImageBase, NewFileSize );
        UnmapViewOfFile( ImageBase );
        CloseHandle( hMappedFile );
        if (SetFilePointer( FileHandle, NewFileSize, NULL, FILE_BEGIN ) != NewFileSize ||
            !SetEndOfFile( FileHandle )
           ) {
            }

        TouchFileTimes( FileHandle, NULL );
        CloseHandle( FileHandle );

        if (ExportedNames != NULL) {
            LocalFree( ExportedNames );
            }

        if (FpoTable != NULL) {
            VirtualFree( FpoTable, 0, MEM_RELEASE );
            }

        if (SymbolsToFree != NULL) {
            VirtualFree( SymbolsToFree, 0, MEM_RELEASE );
            }

        if (FunctionTable != NULL) {
            VirtualFree( FunctionTable, 0, MEM_RELEASE );
            }
        return TRUE;
        }
    else {
        CloseHandle( SymbolFileHandle );
        DeleteFile( SymbolFilePath );
        }

nosyms:
    SavedErrorCode = GetLastError();
    if (ExportedNames != NULL) {
        LocalFree( ExportedNames );
        }

    if (FpoTable != NULL) {
        VirtualFree( FpoTable, 0, MEM_RELEASE );
        }

    if (SymbolsToFree != NULL) {
        VirtualFree( SymbolsToFree, 0, MEM_RELEASE );
        }

    if (FunctionTable != NULL) {
        VirtualFree( FunctionTable, 0, MEM_RELEASE );
        }

    UnmapViewOfFile( ImageBase );
    CloseHandle( hMappedFile );
    CloseHandle( FileHandle );
    SetLastError( SavedErrorCode );
    return FALSE;
}

BOOL
SearchTreeForFile(
    LPSTR RootPath,
    PCHAR InputPathName,
    PCHAR OutputPathBuffer
    );

HANDLE
FindExecutableImage(
    LPSTR FileName,
    LPSTR SymbolPath,
    LPSTR ImageFilePath
    );

BOOL
GetImageNameFromMiscDebugData(
    HANDLE FileHandle,
    PVOID MappedBase,
    PIMAGE_NT_HEADERS NtHeaders,
    PIMAGE_DEBUG_DIRECTORY DebugDirectories,
    ULONG NumberOfDebugDirectories,
    LPSTR ImageFilePath
    );

PIMAGE_DEBUG_INFORMATION
MapDebugInformation(
    HANDLE FileHandle,
    LPSTR FileName,
    LPSTR SymbolPath,
    ULONG ImageBase
    )
{
    ULONG NumberOfHandlesToClose;
    HANDLE HandlesToClose[ 4 ];
    HANDLE MappingHandle;
    PVOID MappedBase, Next;
    BOOL SeparateSymbols;
    UCHAR ImageFilePath[ MAX_PATH ];
    UCHAR DebugFilePath[ MAX_PATH ];
    PIMAGE_DEBUG_INFORMATION DebugInfo;
    PIMAGE_NT_HEADERS NtHeaders;
    PIMAGE_DEBUG_DIRECTORY DebugDirectories;
    PIMAGE_DEBUG_DIRECTORY DebugDirectory;
    PIMAGE_SEPARATE_DEBUG_HEADER DebugFileHeader;
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    PIMAGE_RUNTIME_FUNCTION_ENTRY RuntimeFunctionTable;
    PIMAGE_FUNCTION_ENTRY FunctionTable;
    ULONG NumberOfFunctionTableEntries, FunctionTableSize;
    PVOID DebugData;
    LPSTR Src, Dst;
    PULONG pp;
    ULONG RvaOffset;
    ULONG i, j;
    ULONG ExportedNamesSize;
    ULONG DebugInfoHeaderSize;
    ULONG DebugInfoSize;
    ULONG Size;
    ULONG NumberOfDebugDirectories;
    LONG BaseOffset;
    HANDLE SavedImageFileHandle;

    if (FileHandle == NULL && (FileName == NULL || FileName[ 0 ] == '\0')) {
        return NULL;
        }

    DebugInfo = NULL;
    NumberOfHandlesToClose = 0;
    MappedBase = NULL;
    SeparateSymbols = FALSE;
    SavedImageFileHandle = NULL;
    ImageFilePath[ 0 ] = '\0';
    NumberOfFunctionTableEntries = 0;
    FunctionTableSize = 0;
    FunctionTable = NULL; 
    try {
        try {
            if (FileHandle == NULL) {
                FileHandle = FindExecutableImage( FileName, SymbolPath, ImageFilePath );
                if (FileHandle == NULL) {
                    strcpy( ImageFilePath, FileName );
getDebugFile:
                    FileHandle = FindDebugInfoFile( FileName, SymbolPath, DebugFilePath );
                    if (FileHandle == NULL) {
                        if (SavedImageFileHandle != NULL) {
                            FileHandle = SavedImageFileHandle;
                            goto noDebugFile;
                            }
                        else {
                            leave;
                            }
                        }
                    else {
                        SeparateSymbols = TRUE;
                        }
                    }
                else {
                    SavedImageFileHandle = FileHandle;
                    }

                HandlesToClose[ NumberOfHandlesToClose++ ] = FileHandle;
                }
            else {
                SavedImageFileHandle = FileHandle;
                }

            //
            //  map image file and process enough to get the image name and capture
            //  stuff from header.
            //

            MappingHandle = CreateFileMapping( FileHandle,
                                          NULL,
                                          PAGE_READONLY,
                                          0,
                                          0,
                                          NULL
                                        );
            if (MappingHandle == NULL) {
                leave;
                }
            HandlesToClose[ NumberOfHandlesToClose++ ] = MappingHandle;

            MappedBase = MapViewOfFile( MappingHandle,
                                        FILE_MAP_READ,
                                        0,
                                        0,
                                        0
                                      );
            if (MappedBase == NULL) {
                leave;
                }

            DebugInfoSize = sizeof( *DebugInfo ) + strlen( ImageFilePath ) + 1;
            if (SeparateSymbols) {
                DebugInfoSize += strlen( DebugFilePath ) + 1;
                }

            if (!SeparateSymbols) {
                NtHeaders = ImageNtHeader( MappedBase );
                if (NtHeaders == NULL) {
                    leave;
                    }

                DebugDirectories = (PIMAGE_DEBUG_DIRECTORY)
                    ImageDirectoryEntryToData( MappedBase,
                                               FALSE,
                                               IMAGE_DIRECTORY_ENTRY_DEBUG,
                                               &Size
                                             );
                if (DebugDirectories != NULL) {
                    NumberOfDebugDirectories = Size / sizeof( IMAGE_DEBUG_DIRECTORY );
                    }
                else {
                    NumberOfDebugDirectories = 0;
                    }

                if (FileName == NULL &&
                    GetImageNameFromMiscDebugData( FileHandle,
                                                   MappedBase,
                                                   NtHeaders,
                                                   DebugDirectories,
                                                   NumberOfDebugDirectories,
                                                   ImageFilePath
                                                 )
                   ) {
                    FileName = ImageFilePath;
                    DebugInfoSize += strlen( ImageFilePath );
                    }

                DebugInfoSize = (DebugInfoSize + 16) & ~15;
                DebugInfoHeaderSize = DebugInfoSize;

                if (NtHeaders->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED) {
                    goto getDebugFile;
                    }

noDebugFile:
                ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)
                    ImageDirectoryEntryToData( MappedBase,
                                               FALSE,
                                               IMAGE_DIRECTORY_ENTRY_EXPORT,
                                               &Size
                                             );
                if (ExportDirectory) {
                    //
                    // This particular piece of magic gets us the RVA of the
                    // EXPORT section.  Dont ask.
                    //

                    RvaOffset = (ULONG)
                        ImageDirectoryEntryToData( MappedBase,
                                                   TRUE,
                                                   IMAGE_DIRECTORY_ENTRY_EXPORT,
                                                   &Size
                                                 ) - (DWORD)MappedBase;

                    pp = (PULONG)((ULONG)ExportDirectory +
                                  (ULONG)ExportDirectory->AddressOfNames - RvaOffset
                                 );

                    ExportedNamesSize = 1;
                    for (i=0; i<ExportDirectory->NumberOfNames; i++) {
                        Src = (LPSTR)((ULONG)ExportDirectory + *pp++ - RvaOffset);
                        ExportedNamesSize += strlen( Src ) + 1;
                        }
                    ExportedNamesSize = (ExportedNamesSize + 16) & ~15;
                    DebugInfoSize += ExportedNamesSize;
                    }
                else {
                    ExportedNamesSize = 0;
                    }

                RuntimeFunctionTable = (PIMAGE_RUNTIME_FUNCTION_ENTRY)
                    ImageDirectoryEntryToData( MappedBase,
                                               FALSE,
                                               IMAGE_DIRECTORY_ENTRY_EXCEPTION,
                                               &Size
                                             );
                if (RuntimeFunctionTable != NULL) {
                    NumberOfFunctionTableEntries = Size / sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY );
                    FunctionTableSize = NumberOfFunctionTableEntries *
                                        sizeof( IMAGE_FUNCTION_ENTRY );

                    DebugInfoSize += FunctionTableSize;
                    }

                if (NumberOfDebugDirectories != 0) {
                    DebugDirectory = DebugDirectories;
                    for (i=0; i<NumberOfDebugDirectories; i++) {
                        if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO ||
                            DebugDirectory->Type == IMAGE_DEBUG_TYPE_COFF
                           ) {
                            if (DebugDirectory->AddressOfRawData == 0) {
                                DebugInfoSize += DebugDirectory->SizeOfData;
                                }
                            }

                        DebugDirectory += 1;
                        }
                    }
                }
            else {
                DebugFileHeader = (PIMAGE_SEPARATE_DEBUG_HEADER)MappedBase;
                Next = (PVOID)((PIMAGE_SECTION_HEADER)(DebugFileHeader + 1) + DebugFileHeader->NumberOfSections);
                if (DebugFileHeader->ExportedNamesSize != 0) {
                    Next = (PVOID)((PCHAR)Next + DebugFileHeader->ExportedNamesSize);
                    }
                DebugDirectories = (PIMAGE_DEBUG_DIRECTORY)Next;
                DebugDirectory = DebugDirectories;
                NumberOfDebugDirectories = DebugFileHeader->DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

                for (i=0; i<NumberOfDebugDirectories; i++) {
                    if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_EXCEPTION) {
                        FunctionTableSize = DebugDirectory->SizeOfData;
                        NumberOfFunctionTableEntries = FunctionTableSize / sizeof( IMAGE_FUNCTION_ENTRY );
                        break;
                        }

                    DebugDirectory += 1;
                    }

                DebugInfoSize = (DebugInfoSize + 16) & ~15;
                DebugInfoHeaderSize = DebugInfoSize;
                DebugInfoSize += FunctionTableSize;
                }

            DebugInfo = VirtualAlloc( NULL, DebugInfoSize, MEM_COMMIT, PAGE_READWRITE );
            if (DebugInfo == NULL) {
                leave;
                }

            DebugInfo->Size = DebugInfoSize;
            DebugInfo->ImageFilePath = (LPSTR)(DebugInfo + 1);
            strcpy( DebugInfo->ImageFilePath, ImageFilePath );
            Src = strchr( DebugInfo->ImageFilePath, '\0' );
            while (Src > DebugInfo->ImageFilePath) {
                if (Src[ -1 ] == '\\' || Src[ -1 ] == '/' || Src[ -1 ] == ':') {
                    break;
                    }
                else {
                    Src -= 1;
                    }
                }
            DebugInfo->ImageFileName = Src;
            DebugInfo->DebugFilePath = DebugInfo->ImageFilePath;
            if (SeparateSymbols) {
                DebugInfo->DebugFilePath += strlen( DebugInfo->ImageFilePath ) + 1;
                strcpy( DebugInfo->DebugFilePath, DebugFilePath );
                }

            DebugInfo->MappedBase = MappedBase;
            if (SeparateSymbols) {
                DebugInfo->Machine = DebugFileHeader->Machine;
                DebugInfo->Characteristics = DebugFileHeader->Characteristics;
                DebugInfo->TimeDateStamp = DebugFileHeader->TimeDateStamp;
                DebugInfo->CheckSum = DebugFileHeader->CheckSum;
                DebugInfo->ImageBase = DebugFileHeader->ImageBase;
                DebugInfo->SizeOfImage = DebugFileHeader->SizeOfImage;
                DebugInfo->NumberOfSections = DebugFileHeader->NumberOfSections;
                DebugInfo->Sections = (PIMAGE_SECTION_HEADER)(DebugFileHeader + 1);
                Next = (PVOID)(DebugInfo->Sections + DebugInfo->NumberOfSections);

                DebugInfo->ExportedNamesSize = DebugFileHeader->ExportedNamesSize;
                if (DebugInfo->ExportedNamesSize) {
                    DebugInfo->ExportedNames = (LPSTR)Next;
                    Next = (PVOID)((PCHAR)Next + DebugInfo->ExportedNamesSize);
                    }
                DebugDirectory = DebugDirectories;
                NumberOfDebugDirectories = DebugFileHeader->DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );
                Next = (PVOID)((PCHAR)Next + DebugFileHeader->DebugDirectorySize);
                for (i=0; i<NumberOfDebugDirectories; i++) {
                    if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_EXCEPTION) {
                        DebugInfo->NumberOfFunctionTableEntries = NumberOfFunctionTableEntries;
                        DebugInfo->FunctionTableEntries = (PIMAGE_FUNCTION_ENTRY)
                            ((PCHAR)MappedBase + DebugDirectory->PointerToRawData);

                        BaseOffset = ImageBase;
                        FunctionTable = (PIMAGE_FUNCTION_ENTRY)((ULONG)DebugInfo + DebugInfoHeaderSize);
                        memmove( FunctionTable, DebugInfo->FunctionTableEntries, FunctionTableSize );
                        DebugInfo->FunctionTableEntries = FunctionTable;
                        DebugInfo->LowestFunctionStartingAddress = (ULONG)0xFFFFFFFF;
                        DebugInfo->HighestFunctionEndingAddress = 0;
                        for (j=0; j<DebugInfo->NumberOfFunctionTableEntries; j++) {
                            FunctionTable->StartingAddress += BaseOffset;
                            if (FunctionTable->StartingAddress < DebugInfo->LowestFunctionStartingAddress) {
                                DebugInfo->LowestFunctionStartingAddress = FunctionTable->StartingAddress;
                                }

                            FunctionTable->EndingAddress += BaseOffset;
                            if (FunctionTable->EndingAddress > DebugInfo->HighestFunctionEndingAddress) {
                                DebugInfo->HighestFunctionEndingAddress = FunctionTable->EndingAddress;
                                }

                            FunctionTable->EndOfPrologue += BaseOffset;
                            FunctionTable += 1;
                            }
                        }
                    else
                    if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO) {
                        DebugInfo->NumberOfFpoTableEntries =
                            DebugDirectory->SizeOfData / sizeof( FPO_DATA );
                        DebugInfo->FpoTableEntries = (PFPO_DATA)
                            ((PCHAR)MappedBase + DebugDirectory->PointerToRawData);
                        }
                    else
                    if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_COFF) {
                        DebugInfo->SizeOfCoffSymbols = DebugDirectory->SizeOfData;
                        DebugInfo->CoffSymbols = (PIMAGE_COFF_SYMBOLS_HEADER)
                            ((PCHAR)MappedBase + DebugDirectory->PointerToRawData);
                        }

                    DebugDirectory += 1;
                    }
                }
            else {
                DebugInfo->Machine = NtHeaders->FileHeader.Machine;
                DebugInfo->Characteristics = NtHeaders->FileHeader.Characteristics;
                DebugInfo->TimeDateStamp = NtHeaders->FileHeader.TimeDateStamp;
                DebugInfo->CheckSum = NtHeaders->OptionalHeader.CheckSum;
                DebugInfo->ImageBase = NtHeaders->OptionalHeader.ImageBase;
                DebugInfo->SizeOfImage = NtHeaders->OptionalHeader.SizeOfImage;
                DebugInfo->NumberOfSections = NtHeaders->FileHeader.NumberOfSections;
                DebugInfo->Sections = IMAGE_FIRST_SECTION( NtHeaders );
                Next = (PVOID)((ULONG)DebugInfo + DebugInfoHeaderSize);

                DebugInfo->ExportedNamesSize = ExportedNamesSize;
                if (DebugInfo->ExportedNamesSize) {
                    DebugInfo->ExportedNames = (LPSTR)Next;
                    Next = (PVOID)((LPSTR)Next + ExportedNamesSize);

                    pp = (PULONG)((ULONG)ExportDirectory +
                                  (ULONG)ExportDirectory->AddressOfNames - RvaOffset
                                 );

                    Dst = DebugInfo->ExportedNames;
                    for (i=0; i<ExportDirectory->NumberOfNames; i++) {
                        Src = (LPSTR)((ULONG)ExportDirectory + *pp++ - RvaOffset);
                        while (*Dst++ = *Src++) {
                            }
                        }
                    }

                if (RuntimeFunctionTable != NULL) {
                    BaseOffset = ImageBase - DebugInfo->ImageBase;
                    DebugInfo->FunctionTableEntries = (PIMAGE_FUNCTION_ENTRY)Next;
                    DebugInfo->NumberOfFunctionTableEntries = NumberOfFunctionTableEntries;
                    Next = (PVOID)((LPSTR)Next + FunctionTableSize);

                    DebugInfo->LowestFunctionStartingAddress = (ULONG)0xFFFFFFFF;
                    DebugInfo->HighestFunctionEndingAddress = 0;
                    FunctionTable = DebugInfo->FunctionTableEntries;
                    for (i=0; i<NumberOfFunctionTableEntries; i++) {
                        FunctionTable->StartingAddress = RuntimeFunctionTable->BeginAddress + BaseOffset;
                        if (FunctionTable->StartingAddress < DebugInfo->LowestFunctionStartingAddress) {
                            DebugInfo->LowestFunctionStartingAddress = FunctionTable->StartingAddress;
                            }

                        FunctionTable->EndingAddress = RuntimeFunctionTable->EndAddress + BaseOffset;
                        if (FunctionTable->EndingAddress > DebugInfo->HighestFunctionEndingAddress) {
                            DebugInfo->HighestFunctionEndingAddress = FunctionTable->EndingAddress;
                            }

                        FunctionTable->EndOfPrologue = RuntimeFunctionTable->PrologEndAddress + BaseOffset;
                        RuntimeFunctionTable += 1;
                        FunctionTable += 1;
                        }
                    }

                DebugDirectory = DebugDirectories;
                for (i=0; i<NumberOfDebugDirectories; i++) {
                    if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO ||
                        ((DebugDirectory->Type == IMAGE_DEBUG_TYPE_COFF) &&
                         !(NtHeaders->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED)
                        )
                       ) {
                        DebugData = NULL;
                        if (DebugDirectory->AddressOfRawData == 0) {
                            if (SetFilePointer( FileHandle,
                                                DebugDirectory->PointerToRawData,
                                                NULL,
                                                FILE_BEGIN
                                              ) == DebugDirectory->PointerToRawData
                               ) {
                                if (ReadFile( FileHandle,
                                              Next,
                                              DebugDirectory->SizeOfData,
                                              &Size,
                                              NULL
                                            ) &&
                                    DebugDirectory->SizeOfData == Size
                                   ) {
                                    DebugData = Next;
                                    Next = (PVOID)((LPSTR)Next + Size);
                                    }
                                }
                            }
                        else {
                            DebugData = (LPSTR)MappedBase + DebugDirectory->PointerToRawData;
                            Size = DebugDirectory->SizeOfData;
                            }

                        if (DebugData != NULL) {
                            if (DebugDirectory->Type == IMAGE_DEBUG_TYPE_FPO) {
                                DebugInfo->FpoTableEntries = DebugData;
                                DebugInfo->NumberOfFpoTableEntries = Size / sizeof( FPO_DATA );
                                }
                            else {
                                DebugInfo->CoffSymbols = DebugData;
                                DebugInfo->SizeOfCoffSymbols = Size;
                                }
                            }
                        }

                    DebugDirectory += 1;
                    }
                }
            }
        except( EXCEPTION_EXECUTE_HANDLER ) {
            if (DebugInfo != NULL) {
                VirtualFree( DebugInfo, 0, MEM_RELEASE );
                DebugInfo = NULL;
                }
            }
        }
    finally {
        if (DebugInfo == NULL) {
            if (MappedBase != NULL) {
                UnmapViewOfFile( MappedBase );
                }
            }

        while (NumberOfHandlesToClose--) {
            CloseHandle( HandlesToClose[ NumberOfHandlesToClose ] );
            }
        }

    return DebugInfo;
}


BOOL
UnmapDebugInformation(
    PIMAGE_DEBUG_INFORMATION DebugInfo
    )
{
    if (DebugInfo != NULL) {
        try {
            UnmapViewOfFile( DebugInfo->MappedBase );
            memset( DebugInfo, 0, sizeof( *DebugInfo ) );
            VirtualFree( DebugInfo, 0, MEM_RELEASE );
            }
        except( EXCEPTION_EXECUTE_HANDLER ) {
            return FALSE;
            }
        }

    return TRUE;
}

HANDLE
FindExecutableImage(
    LPSTR FileName,
    LPSTR SymbolPath,
    LPSTR ImageFilePath
    )
{
    LPSTR Start, End;
    HANDLE FileHandle;
    UCHAR DirectoryPath[ MAX_PATH ];

    Start = SymbolPath;
    while (Start && *Start != '\0') {
        if (End = strchr( Start, ';' )) {
            strncpy( DirectoryPath, Start, End - Start );
            DirectoryPath[ End - Start ] = '\0';
            End += 1;
            }
        else {
            strcpy( DirectoryPath, Start );
            }

        if (SearchTreeForFile( DirectoryPath, FileName, ImageFilePath )) {
            FileHandle = CreateFile( ImageFilePath,
                                     GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL
                                   );
            if (FileHandle != INVALID_HANDLE_VALUE) {
                return FileHandle;
                }
            }

        Start = End;
        }

    return NULL;
}


HANDLE
FindDebugInfoFile(
    LPSTR FileName,
    LPSTR SymbolPath,
    LPSTR DebugFilePath
    )
{
    HANDLE FileHandle;
    LPSTR s;
    LPSTR Start, End;
    UCHAR BaseName[ MAX_PATH ];
    DWORD n;

    if (!(s = strrchr( FileName, '.' )) || stricmp( s, ".DBG" )) {
        if (s != NULL) {
            strcpy( BaseName, s+1 );
            strcat( BaseName, "\\" );
            }
        else {
            BaseName[ 0 ] = '\0';
            }

        s = FileName + strlen( FileName );
        while (s > FileName) {
            if (*--s == '\\' || *s == '/' || *s == ':') {
                s += 1;
                break;
                }
            }
        strcat( BaseName, s );
        if (!(s = strrchr( BaseName, '.' ))) {
            s = strchr( BaseName, '\0' );
            }
        strcpy( s, ".DBG" );
        }
    else {
        strcpy( BaseName, FileName );
        }

    Start = SymbolPath;
    while (Start && *Start != '\0') {
        if (End = strchr( Start, ';' )) {
            *End = '\0';
            }

        n = GetFullPathName( Start, MAX_PATH, DebugFilePath, &s );
        if (End) {
            *End++ = ';';
            }
        Start = End;
        if (n == 0) {
            continue;
            }

        if (s != NULL && !stricmp( s, "Symbols" )) {
            strcat( DebugFilePath, "\\" );
            }
        else {
            strcat( DebugFilePath, "\\Symbols\\" );
            }
        strcat( DebugFilePath, BaseName );

        FileHandle = CreateFile( DebugFilePath,
                                 GENERIC_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 0,
                                 NULL
                               );
        if (FileHandle != INVALID_HANDLE_VALUE) {
            return FileHandle;
            }
        }

    return NULL;
}

BOOL
GetImageNameFromMiscDebugData(
    HANDLE FileHandle,
    PVOID MappedBase,
    PIMAGE_NT_HEADERS NtHeaders,
    PIMAGE_DEBUG_DIRECTORY DebugDirectories,
    ULONG NumberOfDebugDirectories,
    LPSTR ImageFilePath
    )
{
    IMAGE_DEBUG_MISC TempMiscData;
    PIMAGE_DEBUG_MISC DebugMiscData;
    ULONG BytesToRead, BytesRead;
    BOOLEAN FoundImageName;
    LPSTR ImageName;

    while (NumberOfDebugDirectories) {
        if (DebugDirectories->Type == IMAGE_DEBUG_TYPE_MISC) {
            break;
            }
        else {
            DebugDirectories += 1;
            NumberOfDebugDirectories -= 1;
            }
        }

    if (NumberOfDebugDirectories == 0) {
        return FALSE;
        }

    if (NtHeaders->OptionalHeader.MinorLinkerVersion < 36) {
        BytesToRead = FIELD_OFFSET( IMAGE_DEBUG_MISC, Reserved );
        }
    else {
        BytesToRead = FIELD_OFFSET( IMAGE_DEBUG_MISC, Data );
        }

    DebugMiscData = NULL;
    if (DebugDirectories->AddressOfRawData == 0) {
        if (SetFilePointer( FileHandle,
                            DebugDirectories->PointerToRawData,
                            NULL,
                            FILE_BEGIN
                          ) == DebugDirectories->PointerToRawData
           ) {
            if (ReadFile( FileHandle,
                          &TempMiscData,
                          BytesToRead,
                          &BytesRead,
                          NULL
                        ) &&
                BytesRead == BytesToRead
               ) {
                DebugMiscData = &TempMiscData;
                }
            }
        }
    else {
        DebugMiscData = (PIMAGE_DEBUG_MISC)((PCHAR)MappedBase +
                                            DebugDirectories->PointerToRawData
                                           );
        }

    FoundImageName = FALSE;
    if (DebugMiscData != NULL && DebugMiscData->DataType == IMAGE_DEBUG_MISC_EXENAME) {
        if (DebugMiscData == &TempMiscData) {
            BytesToRead = DebugMiscData->Length - BytesToRead;
            if (ReadFile( FileHandle,
                          ImageFilePath,
                          BytesToRead,
                          &BytesRead,
                          NULL
                        ) &&
                BytesRead == BytesToRead
               ) {
                FoundImageName = TRUE;
                }
            }
        else {
            ImageName = (PCHAR)DebugMiscData + BytesToRead;
            BytesToRead = DebugMiscData->Length - BytesToRead;
            if (*ImageName != '\0' ) {
                memcpy( ImageFilePath, ImageName, BytesToRead );
                FoundImageName = TRUE;
                }
            }
        }

    return FoundImageName;
}



#define MAX_DEPTH 32

BOOL
SearchTreeForFile(
    LPSTR RootPath,
    LPSTR InputPathName,
    LPSTR OutputPathBuffer
    )
{
    PCHAR FileName;
    PUCHAR Prefix = "";
    CHAR PathBuffer[ MAX_PATH ];
    ULONG Depth;
    PCHAR PathTail[ MAX_DEPTH ];
    PCHAR FindHandle[ MAX_DEPTH ];
    LPWIN32_FIND_DATA FindFileData;
    UCHAR FindFileBuffer[ MAX_PATH + sizeof( WIN32_FIND_DATA ) ];
    BOOL Result;

    strcpy( PathBuffer, RootPath );
    FileName = InputPathName;
    while (*InputPathName) {
        if (*InputPathName == ':' || *InputPathName == '\\' || *InputPathName == '/') {
            FileName = ++InputPathName;
            }
        else {
            InputPathName++;
            }
        }
    FindFileData = (LPWIN32_FIND_DATA)FindFileBuffer;
    Depth = 0;
    Result = FALSE;
    while (TRUE) {
startDirectorySearch:
        PathTail[ Depth ] = strchr( PathBuffer, '\0' );
        if (PathTail[ Depth ] > PathBuffer && PathTail[ Depth ][ -1 ] != '\\') {
            *(PathTail[ Depth ])++ = '\\';
            }

        strcpy( PathTail[ Depth ], "*.*" );
        FindHandle[ Depth ] = FindFirstFile( PathBuffer, FindFileData );
        if (FindHandle[ Depth ] != INVALID_HANDLE_VALUE) {
            do {
                if (FindFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (strcmp( FindFileData->cFileName, "." ) &&
                        strcmp( FindFileData->cFileName, ".." ) &&
                        Depth < MAX_DEPTH
                       ) {
                        sprintf( PathTail[ Depth ], "%s\\", FindFileData->cFileName );
                        Depth++;
                        goto startDirectorySearch;
                        }
                    }
                else
                if (!stricmp( FindFileData->cFileName, FileName )) {
                    strcpy( PathTail[ Depth ], FindFileData->cFileName );
                    strcpy( OutputPathBuffer, PathBuffer );
                    Result = TRUE;
                    }

restartDirectorySearch:
                if (Result) {
                    break;
                    }
                }
            while (FindNextFile( FindHandle[ Depth ], FindFileData ));
            FindClose( FindHandle[ Depth ] );

            if (Depth == 0) {
                break;
                }

            Depth--;
            goto restartDirectorySearch;
            }
        }

    return Result;
}


BOOL
MakeSureDirectoryPathExists(
    LPSTR DirPath
    )
{
    LPSTR Dir,p;
    DWORD dw;

    Dir = DirPath;
    p = Dir+1;

    //
    //  If the second character in the path is "\", then this is a UNC
    //  path, and we should skip forward until we reach the 2nd \ in the
    //  path.
    //
    if (*p == '\\') {
        p++;            // Skip over the second \ in the name.

        //
        //  Skip until we hit the first "\" (\\Server\).
        //

        while (*p && *p != '\\') {
            p++;
            }

        if (*p) {
            *p++;
            }

        //
        //  Skip until we hit the second "\" (\\Server\Share\).
        //

        while (*p && *p != '\\') {
            p++;
            }

        //
        //  The directory to check is the "path" part of \\Server\Share\path
        //

        if (*p) {
            Dir = p-1;
            }
        }
    else if (*p == ':' ) {
        p++;
        p++;
        }

    while( *p ) {
        if ( *p == '\\' ) {
            *p = '\0';
            dw = GetFileAttributes(DirPath);
            if ( dw == 0xffffffff || dw & FILE_ATTRIBUTE_DIRECTORY != FILE_ATTRIBUTE_DIRECTORY ) {
                if ( dw == 0xffffffff ) {
                    if ( !CreateDirectory(DirPath,NULL) ) {
                        return FALSE;
                        }
                    }
                }

            *p = '\\';
            }
        p++;
        }

    return TRUE;
}


BOOL
UpdateDebugInfoFile(
    LPSTR ImageFileName,
    LPSTR SymbolPath,
    LPSTR DebugFilePath,
    PIMAGE_NT_HEADERS NtHeaders
    )
{
    HANDLE hDebugFile, hMappedFile;
    PVOID MappedAddress;
    PIMAGE_SEPARATE_DEBUG_HEADER DbgFileHeader;

    hDebugFile = FindDebugInfoFile(
                    ImageFileName,
                    SymbolPath,
                    DebugFilePath
                    );
    if ( hDebugFile == NULL ) {
        return FALSE;
        }
    CloseHandle(hDebugFile);

    hDebugFile = CreateFile( DebugFilePath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL
                           );
    if ( hDebugFile == INVALID_HANDLE_VALUE ) {
        return FALSE;
        }

    hMappedFile = CreateFileMapping(
                    hDebugFile,
                    NULL,
                    PAGE_READWRITE,
                    0,
                    0,
                    NULL
                    );
    if ( !hMappedFile ) {
        CloseHandle(hDebugFile);
        return FALSE;
        }

    MappedAddress = MapViewOfFile(hMappedFile,
                        FILE_MAP_WRITE,
                        0,
                        0,
                        0
                        );
    CloseHandle(hMappedFile);
    if ( !MappedAddress ) {
        CloseHandle(hDebugFile);
        return FALSE;
        }

    DbgFileHeader = (PIMAGE_SEPARATE_DEBUG_HEADER)MappedAddress;
    if (DbgFileHeader->ImageBase != NtHeaders->OptionalHeader.ImageBase ||
        DbgFileHeader->CheckSum != NtHeaders->OptionalHeader.CheckSum
       ) {
        DbgFileHeader->ImageBase = NtHeaders->OptionalHeader.ImageBase;
        DbgFileHeader->CheckSum = NtHeaders->OptionalHeader.CheckSum;
        UnmapViewOfFile(MappedAddress);
        FlushViewOfFile(MappedAddress,0);
        TouchFileTimes(hDebugFile,NULL);
        CloseHandle(hDebugFile);
        return TRUE;
        }
    else {
        UnmapViewOfFile(MappedAddress);
        FlushViewOfFile(MappedAddress,0);
        CloseHandle(hDebugFile);
        return FALSE;
        }
}
