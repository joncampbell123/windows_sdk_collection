/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    imagedir.c

Abstract:

    The module contains the code to translate an image directory type to
    the address of the data for that entry.

Author:

    Steve Wood 18-Aug-1989

Environment:

    User Mode or Kernel Mode

Revision History:

--*/

#include <windows.h>


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

PIMAGE_NT_HEADERS
ImageNtHeader (
    IN PVOID Base
    )

/*++

Routine Description:

    This function returns the address of the NT Header.

Arguments:

    Base - Supplies the base of the image.

Return Value:

    Returns the address of the NT Header.

--*/

{

    PIMAGE_NT_HEADERS NtHeaders;

    if (Base != NULL &&
        Base != (PVOID)-1
       ) {
        try {
            if (((PIMAGE_DOS_HEADER)Base)->e_magic == IMAGE_DOS_SIGNATURE) {
                NtHeaders = (PIMAGE_NT_HEADERS)((PCHAR)Base + ((PIMAGE_DOS_HEADER)Base)->e_lfanew);
                if (NtHeaders->Signature == IMAGE_NT_SIGNATURE) {
                    return NtHeaders;
                }
            }

        } except(EXCEPTION_EXECUTE_HANDLER) {
            return NULL;
        }

    }

    return NULL;
}


PVOID
ImageDirectoryEntryToData (
    IN PVOID Base,
    IN BOOLEAN MappedAsImage,
    IN USHORT DirectoryEntry,
    OUT PULONG Size
    )

/*++

Routine Description:

    This function locates a Directory Entry within the image header
    and returns either the virtual address or seek address of the
    data the Directory describes.

Arguments:

    Base - Supplies the base of the image or data file.

    MappedAsImage - FALSE if the file is mapped as a data file.
                  - TRUE if the file is mapped as an image.

    DirectoryEntry - Supplies the directory entry to locate.

    Size - Return the size of the directory.

Return Value:

    NULL - The file does not contain data for the specified directory entry.

    NON-NULL - Returns the address of the raw data the directory describes.

--*/

{
    ULONG i, DirectoryAddress;
    PIMAGE_NT_HEADERS NtHeaders;
    PIMAGE_SECTION_HEADER NtSection;

    NtHeaders = ImageNtHeader(Base);

    if (DirectoryEntry >= NtHeaders->OptionalHeader.NumberOfRvaAndSizes) {
        return( NULL );
    }

    if (!(DirectoryAddress = NtHeaders->OptionalHeader.DataDirectory[ DirectoryEntry ].VirtualAddress)) {
        return( NULL );
    }
    *Size = NtHeaders->OptionalHeader.DataDirectory[ DirectoryEntry ].Size;
    if (MappedAsImage || DirectoryAddress < NtHeaders->OptionalHeader.SizeOfHeaders) {
        return( (PVOID)((ULONG)Base + DirectoryAddress) );
    }

    NtSection = (PIMAGE_SECTION_HEADER)((ULONG)NtHeaders +
                        sizeof(ULONG) +
                        sizeof(IMAGE_FILE_HEADER) +
                        NtHeaders->FileHeader.SizeOfOptionalHeader
                        );

    for (i=0; i<NtHeaders->FileHeader.NumberOfSections; i++) {
        if (DirectoryAddress >= NtSection->VirtualAddress &&
           DirectoryAddress < NtSection->VirtualAddress + NtSection->SizeOfRawData) {
            return( (PVOID)((ULONG)Base + (DirectoryAddress - NtSection->VirtualAddress) + NtSection->PointerToRawData) );
        }
        ++NtSection;
    }
    return( NULL );
}


USHORT
ChkSum(
    ULONG PartialSum,
    PUSHORT Source,
    ULONG Length
    )

/*++

Routine Description:

    Compute a partial checksum on a portion of an imagefile.

Arguments:

    PartialSum - Supplies the initial checksum value.

    Sources - Supplies a pointer to the array of words for which the
        checksum is computed.

    Length - Supplies the length of the array in words.

Return Value:

    The computed checksum value is returned as the function value.

--*/

{

    //
    // Compute the word wise checksum allowing carries to occur into the
    // high order half of the checksum longword.
    //

    while (Length--) {
        PartialSum += *Source++;
        PartialSum = (PartialSum >> 16) + (PartialSum & 0xffff);
    }

    //
    // Fold final carry into a single word result and return the resultant
    // value.
    //

    return (USHORT)(((PartialSum >> 16) + PartialSum) & 0xffff);
}
