/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    checksum.c

Abstract:

    This module implements a function for computing the checksum of an
    image file. It will also compute the checksum of other files as well.

Author:

    David N. Cutler 21-Mar-1993

Revision History:

--*/

#include <windows.h>
#include <imagehlp.h>


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

//
// Define checksum routine prototype.
//

USHORT
ChkSum(
    DWORD PartialSum,
    PUSHORT Source,
    DWORD Length
    );

PIMAGE_NT_HEADERS
CheckSumMappedFile (
    LPVOID BaseAddress,
    DWORD FileLength,
    LPDWORD HeaderSum,
    LPDWORD CheckSum
    )

/*++

Routine Description:

    This functions computes the checksum of a mapped file.

Arguments:

    BaseAddress - Supplies a pointer to the base of the mapped file.

    FileLength - Supplies the length of the file in bytes.

    HeaderSum - Suppllies a pointer to a variable that receives the checksum
        from the image file, or zero if the file is not an image file.

    CheckSum - Supplies a pointer to the variable that receive the computed
        checksum.

Return Value:

    None.

--*/

{

    PUSHORT AdjustSum;
    PIMAGE_NT_HEADERS NtHeaders;
    USHORT PartialSum;

    //
    // Compute the checksum of the file and zero the header checksum value.
    //

    *HeaderSum = 0;
    PartialSum = ChkSum(0, (PUSHORT)BaseAddress, (FileLength + 1) >> 1);

    //
    // If the file is an image file, then subtract the two checksum words
    // in the optional header from the computed checksum before adding
    // the file length, and set the value of the header checksum.
    //

    try {
	NtHeaders = ImageNtHeader(BaseAddress);

    } except(EXCEPTION_EXECUTE_HANDLER) {
        NtHeaders = NULL;
    }

    if ((NtHeaders != NULL) && (NtHeaders != BaseAddress)) {
        *HeaderSum = NtHeaders->OptionalHeader.CheckSum;
        AdjustSum = (PUSHORT)(&NtHeaders->OptionalHeader.CheckSum);
        PartialSum -= (PartialSum < AdjustSum[0]);
        PartialSum -= AdjustSum[0];
        PartialSum -= (PartialSum < AdjustSum[1]);
        PartialSum -= AdjustSum[1];
    }

    //
    // Compute the final checksum value as the sum of the paritial checksum
    // and the file length.
    //

    *CheckSum = (DWORD)PartialSum + FileLength;
    return NtHeaders;
}

DWORD
MapFileAndCheckSumW(
    PWSTR Filename,
    LPDWORD HeaderSum,
    LPDWORD CheckSum
    )

/*++

Routine Description:

    This functions maps the specified file and computes the checksum of
    the file.

Arguments:

    Filename - Supplies a pointer to the name of the file whose checksum
        is computed.

    HeaderSum - Supplies a pointer to a variable that receives the checksum
        from the image file, or zero if the file is not an image file.

    CheckSum - Supplies a pointer to the variable that receive the computed
        checksum.

Return Value:

    0 if successful, else error number.

--*/

{
    HANDLE FileHandle, MappingHandle;
    LPVOID BaseAddress;
    DWORD FileLength;

    //
    // Open the file for read access
    //

    FileHandle = CreateFileW(
                        Filename,
                        GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL );

    if (FileHandle == INVALID_HANDLE_VALUE) {
        return CHECKSUM_OPEN_FAILURE;
    }

    //
    //  Create a file mapping, map a view of the file into memory,
    //  and close the file mapping handle.
    //

    MappingHandle = CreateFileMapping(FileHandle,
                                      NULL,
                                      PAGE_READONLY,
                                      0,
                                      0,
                                      NULL);

    if (!MappingHandle) {
        CloseHandle( FileHandle );
        return CHECKSUM_MAP_FAILURE;
    }

    //
    // Map a view of the file
    //

    BaseAddress = MapViewOfFile(MappingHandle, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(MappingHandle);
    if (BaseAddress == NULL) {
        CloseHandle( FileHandle );
        return CHECKSUM_MAPVIEW_FAILURE;
    }

    //
    // Get the length of the file in bytes and compute the checksum.
    //
    FileLength = GetFileSize( FileHandle, NULL );
    CheckSumMappedFile(BaseAddress, FileLength, HeaderSum, CheckSum);

    //
    // Unmap the view of the file and close file handle.
    //

    UnmapViewOfFile(BaseAddress);
    CloseHandle( FileHandle );
    return CHECKSUM_SUCCESS;
}


ULONG
MapFileAndCheckSumA (
    LPSTR Filename,
    LPDWORD HeaderSum,
    LPDWORD CheckSum
    )

/*++

Routine Description:

    This functions maps the specified file and computes the checksum of
    the file.

Arguments:

    Filename - Supplies a pointer to the name of the file whose checksum
        is computed.

    HeaderSum - Supplies a pointer to a variable that receives the checksum
        from the image file, or zero if the file is not an image file.

    CheckSum - Supplies a pointer to the variable that receive the computed
        checksum.

Return Value:

    0 if successful, else error number.

--*/

{

    WCHAR   FileNameW[ MAX_PATH ];

    //
    //  Convert the file name to unicode and call the unicode version
    //  of this function.
    //

    if (MultiByteToWideChar(
                    CP_ACP,
                    MB_PRECOMPOSED,
                    Filename,
                    -1,
                    FileNameW,
                    MAX_PATH ) ) {

        return MapFileAndCheckSumW(FileNameW, HeaderSum, CheckSum);

    }

    return CHECKSUM_UNICODE_FAILURE;
}


BOOL
TouchFileTimes(
    HANDLE FileHandle,
    LPSYSTEMTIME lpSystemTime
    )
{
    SYSTEMTIME SystemTime;
    FILETIME SystemFileTime;

    if (lpSystemTime == NULL) {
        lpSystemTime = &SystemTime;
        GetSystemTime( lpSystemTime );
        }

    if (SystemTimeToFileTime( lpSystemTime, &SystemFileTime )) {
        return SetFileTime( FileHandle, NULL, NULL, &SystemFileTime );
        }
    else {
        return FALSE;
        }
}
