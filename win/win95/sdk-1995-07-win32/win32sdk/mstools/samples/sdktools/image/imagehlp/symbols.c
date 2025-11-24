/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    symbols.c

Abstract:

    This function implements a generic simple symbol handler.

Author:

    Wesley Witt (wesw) 1-Sep-1994

Environment:

    User Mode

--*/

#include <private.h>


//
// defines for symbol file searching
//
#define SYMBOL_PATH             "_NT_SYMBOL_PATH"
#define ALTERNATE_SYMBOL_PATH   "_NT_ALT_SYMBOL_PATH"
#define SYSTEMROOT              "SystemRoot"

//
// structures
//

typedef struct _PROCESS_ENTRY {
    LIST_ENTRY      ListEntry;
    LIST_ENTRY      ModuleList;
    ULONG           Count;
    HANDLE          hProcess;
    LPSTR           SymbolSearchPath;
} PROCESS_ENTRY, *PPROCESS_ENTRY;

typedef struct _OMAP {
    ULONG  rva;
    ULONG  rvaTo;
} OMAP, *POMAP;

typedef struct _OMAPLIST {
   struct _OMAPLIST *next;
   OMAP             omap;
   ULONG            cb;
} OMAPLIST, *POMAPLIST;

typedef struct _MODULE_ENTRY {
    LIST_ENTRY                      ListEntry;
    ULONG                           BaseOfDll;
    ULONG                           DllSize;
    ULONG                           TimeDateStamp;
    ULONG                           CheckSum;
    USHORT                          MachineType;
    PSTR                            ModuleName;
    PSTR                            ImageName;
    PSTR                            LoadedImageName;
    PIMAGEHLP_SYMBOL                symbolTable;
    ULONG                           numsyms;
    SYM_TYPE                        SymType;
    PVOID                           pdb;
    PVOID                           dbi;
    PVOID                           gsi;
    PIMAGE_SECTION_HEADER           SectionHdrs;
    ULONG                           NumSections;
    PVOID                           pCvData;        // codeview data
    PFPO_DATA                       pFpoData;       // pointer to fpo data (x86)
    PIMAGE_FUNCTION_ENTRY           pExceptionData; // pointer to pdata (risc)
    ULONG                           dwEntries;      // # of fpo or pdata recs
    POMAP                           pOmapFrom;      // pointer to omap data
    ULONG                           cOmapFrom;      // count of omap entries
    POMAP                           pOmapTo;        // pointer to omap data
    ULONG                           cOmapTo;        // count of omap entries
    PIMAGEHLP_SYMBOL                TmpSym;         // used only for pdb symbols
} MODULE_ENTRY, *PMODULE_ENTRY;

typedef struct _PDB_INFO {
    CHAR    Signature[4];   // "NBxx"
    ULONG   Offset;         // always zero
    ULONG   sig;
    ULONG   age;
    CHAR    PdbName[_MAX_PATH];
} PDB_INFO, *PPDB_INFO;

#define n_name          N.ShortName
#define n_zeroes        N.Name.Short
#define n_nptr          N.LongName[1]
#define n_offset        N.Name.Long

//
// globals
//
LIST_ENTRY      ProcessList;
BOOL            SymInitialized;


//
// internal prototypes
//
BOOL
GetProcessModules(
    HANDLE  hProcess
    );

VOID
FreeModuleEntry(
    PMODULE_ENTRY ModuleEntry
    );

PPROCESS_ENTRY
FindProcessEntry(
    HANDLE  hProcess
    );

VOID
GetSymName(
    PIMAGE_SYMBOL Symbol,
    PUCHAR        StringTable,
    LPSTR         s
    );

BOOL
ProcessOmapSymbol(
    PMODULE_ENTRY mi,
    PIMAGEHLP_SYMBOL       sym
    );

DWORD
ConvertOmapFromSrc(
    PMODULE_ENTRY  mi,
    DWORD          addr,
    LPDWORD        bias
    );

DWORD
ConvertOmapToSrc(
    PMODULE_ENTRY  mi,
    DWORD          addr,
    LPDWORD        bias
    );

POMAP
GetOmapEntry(
    PMODULE_ENTRY  mi,
    DWORD          addr
    );

VOID
ProcessOmapForModule(
    PMODULE_ENTRY mi
    );

BOOL
LoadCoffSymbols(
    HANDLE             hProcess,
    PMODULE_ENTRY      mi,
    PUCHAR             stringTable,
    PIMAGE_SYMBOL      allSymbols,
    DWORD              numberOfSymbols
    );

BOOL
LoadCodeViewSymbols(
    HANDLE                 hProcess,
    PMODULE_ENTRY          mi,
    PUCHAR                 pCvData,
    DWORD                  dwSize
    );

BOOL
LoadOmap(
    PMODULE_ENTRY               mi,
    PIMAGE_DEBUG_INFORMATION    di
    );

PMODULE_ENTRY
GetModuleForPC(
    PPROCESS_ENTRY  ProcessEntry,
    DWORD           dwPcAddr
    );

PIMAGEHLP_SYMBOL
GetSymFromAddr(
    DWORD           dwAddr,
    PDWORD          pdwDisplacement,
    PMODULE_ENTRY   mi
    );

LPSTR
StringDup(
    LPSTR str
    );

BOOL
InternalLoadModule(
    IN  HANDLE          hProcess,
    IN  PSTR            ImageName,
    IN  PSTR            ModuleName,
    IN  DWORD           BaseOfDll,
    IN  DWORD           SizeOfDll,
    IN  HANDLE          hFile
    );


BOOL
IMAGEAPI
SymInitialize(
    IN HANDLE   hProcess,
    IN LPSTR    UserSearchPath,
    IN BOOL     fInvadeProcess
    )
{
    PPROCESS_ENTRY  ProcessEntry;


    if (!SymInitialized) {
        SymInitialized = TRUE;
        InitializeListHead( &ProcessList );
    }

    if (FindProcessEntry( hProcess )) {
        return TRUE;
    }

    ProcessEntry = LocalAlloc( LPTR, sizeof(PROCESS_ENTRY) );
    if (!ProcessEntry) {
        return FALSE;
    }
    ZeroMemory( ProcessEntry, sizeof(PROCESS_ENTRY) );

    ProcessEntry->hProcess = hProcess;
    InitializeListHead( &ProcessEntry->ModuleList );
    InsertTailList( &ProcessList, &ProcessEntry->ListEntry );

    SymSetSearchPath( hProcess, UserSearchPath );

    if (fInvadeProcess) {
        GetProcessModules( hProcess );
    }

    return TRUE;
}


VOID
FreeModuleEntry(
    PMODULE_ENTRY ModuleEntry
    )
{
    ULONG               i;
    PIMAGEHLP_SYMBOL    Sym;
    PIMAGEHLP_SYMBOL    SymNext;


    if (ModuleEntry->symbolTable) {
        for (i=0,Sym=ModuleEntry->symbolTable; i<ModuleEntry->numsyms; i++) {
            SymNext = Sym->next;
            LocalFree( Sym );
            Sym = SymNext;
        }
    }
    if (ModuleEntry->SymType == SymPdb) {
        DBIClose( ModuleEntry->dbi );
        PDBClose( ModuleEntry->pdb );
    }
    if (ModuleEntry->SectionHdrs) {
        LocalFree( ModuleEntry->SectionHdrs );
    }
    if (ModuleEntry->pCvData) {
        LocalFree( ModuleEntry->pCvData );
    }
    if (ModuleEntry->pFpoData) {
        LocalFree( ModuleEntry->pFpoData );
    }
    if (ModuleEntry->pExceptionData) {
        LocalFree( ModuleEntry->pExceptionData );
    }
    if (ModuleEntry->TmpSym) {
        LocalFree( ModuleEntry->TmpSym );
    }
    if (ModuleEntry->ModuleName) {
        LocalFree( ModuleEntry->ModuleName );
    }
    if (ModuleEntry->ImageName) {
        LocalFree( ModuleEntry->ImageName );
    }
    if (ModuleEntry->LoadedImageName) {
        LocalFree( ModuleEntry->LoadedImageName );
    }
    LocalFree( ModuleEntry );
}


BOOL
IMAGEAPI
SymCleanup(
    HANDLE hProcess
    )
{
    PPROCESS_ENTRY      ProcessEntry;
    PLIST_ENTRY         Next;
    PMODULE_ENTRY       ModuleEntry;

    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (Next) {
        while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
            ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
            Next = ModuleEntry->ListEntry.Flink;
            FreeModuleEntry( ModuleEntry );
        }
    }

    if (ProcessEntry->SymbolSearchPath) {
        LocalFree( ProcessEntry->SymbolSearchPath );
    }

    RemoveEntryList( &ProcessEntry->ListEntry );
    LocalFree( ProcessEntry );

    return TRUE;
}


BOOL
IMAGEAPI
SymEnumerateModules(
    IN HANDLE                       hProcess,
    IN PSYM_ENUMMODULES_CALLBACK    EnumModulesCallback
    )
{
    PPROCESS_ENTRY  ProcessEntry;
    PLIST_ENTRY     Next;
    PMODULE_ENTRY   ModuleEntry;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (Next) {
        while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
            ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
            Next = ModuleEntry->ListEntry.Flink;
            if (!EnumModulesCallback( ModuleEntry->ModuleName, ModuleEntry->BaseOfDll )) {
                break;
            }
        }
    }

    return TRUE;
}

BOOL
IMAGEAPI
SymEnumerateSymbols(
    IN HANDLE                       hProcess,
    IN ULONG                        BaseOfDll,
    IN PSYM_ENUMSYMBOLS_CALLBACK    EnumSymbolsCallback
    )
{
    PPROCESS_ENTRY      ProcessEntry;
    PLIST_ENTRY         Next;
    PMODULE_ENTRY       ModuleEntry;
    DWORD               i;
    PIMAGEHLP_SYMBOL    sym;
    LPSTR               szSymName;



    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (Next) {
        while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
            ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
            Next = ModuleEntry->ListEntry.Flink;
            if (ModuleEntry->BaseOfDll == BaseOfDll) {
                if (ModuleEntry->SymType == SymPdb) {
                    DATASYM32 *dataSym = (DATASYM32*)GSINextSym( ModuleEntry->gsi, NULL );
                    PIMAGE_SECTION_HEADER sh;
                    DWORD addr;
                    CHAR SymName[512];
                    ULONG k;
                    while( dataSym ) {
                        for (k=0,addr=0,sh=ModuleEntry->SectionHdrs; k<ModuleEntry->NumSections; k++, sh++) {
                            if (k+1 == dataSym->seg) {
                                addr = sh->VirtualAddress + (dataSym->off + ModuleEntry->BaseOfDll);
                                break;
                            }
                        }
                        if (addr) {
                            if (dataSym->name[1] == '?' && dataSym->name[2] == '?' &&
                                dataSym->name[3] == '_' && dataSym->name[4] == 'C'    ) {
                                //
                                // ignore strings
                                //
                            } else {
                                strncpy( SymName, &dataSym->name[1], dataSym->name[0] );
                                dataSym->name[dataSym->name[0]] = 0;
                                if (!EnumSymbolsCallback( SymName, addr, 0 )) {
                                    break;
                                }
                            }
                        }
                        dataSym = (DATASYM32*)GSINextSym( ModuleEntry->gsi, (PUCHAR)dataSym );
                    }
                    return TRUE;
                }
                for (i=0,sym=ModuleEntry->symbolTable; i<ModuleEntry->numsyms; i++,sym=sym->next) {
                    szSymName = SymUnDName( sym );
                    if (!szSymName) {
                        szSymName = &sym->szName[1];
                    }
                    if (!EnumSymbolsCallback( szSymName, sym->addr, sym->size )) {
                        break;
                    }
                }
                break;
            }
        }
    }

    return TRUE;
}


PIMAGEHLP_SYMBOL
IMAGEAPI
SymGetSymFromAddr(
    IN  HANDLE          hProcess,
    IN  DWORD           dwAddr,
    OUT PDWORD          pdwDisplacement
    )
{
    PPROCESS_ENTRY  ProcessEntry;
    PMODULE_ENTRY   mi;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return NULL;
    }

    mi = GetModuleForPC( ProcessEntry, dwAddr );
    if (mi == NULL) {
        return NULL;
    }

    return GetSymFromAddr( dwAddr, pdwDisplacement, mi );
}


PIMAGEHLP_SYMBOL
IMAGEAPI
SymGetSymFromName(
    IN  HANDLE      hProcess,
    IN  LPSTR       name
    )
{
    PIMAGEHLP_SYMBOL    sym = NULL;
    DWORD           i;
    char            buf[256];
    LPSTR           p;
    PPROCESS_ENTRY  ProcessEntry;
    PMODULE_ENTRY   mi;
    PLIST_ENTRY     Next;

    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return NULL;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (!Next) {
        return NULL;
    }

    if (Next != &ProcessEntry->ModuleList) {
        mi = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
        if (mi->SymType == SymPdb) {
            return NULL;
        }
    }

    while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
        mi = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
        Next = mi->ListEntry.Flink;
        for (i=0,sym=mi->symbolTable; i<mi->numsyms; i++,sym=sym->next) {
            if (sym->szName[1] == '_') {
                strcpy(buf,&sym->szName[2]);
                p = strchr(buf, '@');
                if (p) {
                    *p = '\0';
                }
                if (strcmp(buf,name)==0) {
                    return sym;
                }
            } else {
                if (strncmp(&sym->szName[1],name,(DWORD)sym->szName[0])==0) {
                    return sym;
                }
            }
        }
    }

    return NULL;
}


PIMAGE_FUNCTION_ENTRY
LookupFunctionEntry (
    PIMAGE_FUNCTION_ENTRY           FunctionTable,
    DWORD                           NumberOfFunctions,
    DWORD                           ControlPc
    )
{

    PIMAGE_FUNCTION_ENTRY           FunctionEntry;
    LONG                            High;
    LONG                            Low;
    LONG                            Middle;

    //
    // Initialize search indicies.
    //

    Low = 0;
    High = NumberOfFunctions - 1;

    //
    // Perform binary search on the function table for a function table
    // entry that subsumes the specified PC.
    //

    while (High >= Low) {

        //
        // Compute next probe index and test entry. If the specified PC
        // is greater than of equal to the beginning address and less
        // than the ending address of the function table entry, then
        // return the address of the function table entry. Otherwise,
        // continue the search.
        //

        Middle = (Low + High) >> 1;
        FunctionEntry = &FunctionTable[Middle];
        if (ControlPc < FunctionEntry->StartingAddress) {
            High = Middle - 1;

        } else if (ControlPc >= FunctionEntry->EndingAddress) {
            Low = Middle + 1;

        } else {
            return FunctionEntry;
        }
    }

    //
    // A function table entry for the specified PC was not found.
    //

    return NULL;
}

PFPO_DATA
SwSearchFpoData(
    DWORD     key,
    PFPO_DATA base,
    DWORD     num
    )
{
        PFPO_DATA  lo = base;
        PFPO_DATA  hi = base + (num - 1);
        PFPO_DATA  mid;
        DWORD      half;

        while (lo <= hi) {
                if (half = num / 2) {
                        mid = lo + ((num & 1) ? half : (half - 1));
                        if ((key >= mid->ulOffStart)&&(key < (mid->ulOffStart+mid->cbProcSize))) {
                            return mid;
                        }
                        if (key < mid->ulOffStart) {
                                hi = mid - 1;
                                num = (num & 1) ? half : half-1;
                        }
                        else {
                                lo = mid + 1;
                                num = half;
                        }
                }
                else
                if (num) {
                    if ((key >= lo->ulOffStart)&&(key < (lo->ulOffStart+lo->cbProcSize))) {
                        return lo;
                    }
                    else {
                        break;
                    }
                }
                else {
                        break;
                }
        }
        return(NULL);
}


LPVOID
IMAGEAPI
SymFunctionTableAccess(
    HANDLE  hProcess,
    DWORD   AddrBase
    )
{
    PPROCESS_ENTRY  ProcessEntry;
    PMODULE_ENTRY   mi;
    PVOID           rtf;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return NULL;
    }

    mi = GetModuleForPC( ProcessEntry, AddrBase );
    if (mi == NULL) {
        return NULL;
    }

    if (mi->pFpoData) {
        rtf = SwSearchFpoData( AddrBase - mi->BaseOfDll, mi->pFpoData, mi->dwEntries );
    } else {
        rtf = LookupFunctionEntry( mi->pExceptionData, mi->dwEntries, AddrBase );
    }

    return rtf;
}


BOOL
IMAGEAPI
SymGetModuleInfo(
    IN  HANDLE              hProcess,
    IN  DWORD               dwAddr,
    OUT PIMAGEHLP_MODULE    ModuleInfo
    )
{
    PPROCESS_ENTRY          ProcessEntry;
    PMODULE_ENTRY           mi;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    mi = GetModuleForPC( ProcessEntry, dwAddr );
    if (mi == NULL) {
        return FALSE;
    }

    ModuleInfo->BaseOfImage = mi->BaseOfDll;
    ModuleInfo->ImageSize = mi->DllSize;
    ModuleInfo->NumSyms = mi->numsyms;
    ModuleInfo->CheckSum = mi->CheckSum;
    ModuleInfo->TimeDateStamp = mi->TimeDateStamp;
    ModuleInfo->SymType = mi->SymType;
    strcpy( ModuleInfo->ModuleName, mi->ModuleName );
    strcpy( ModuleInfo->ImageName, mi->ImageName );
    strcpy( ModuleInfo->LoadedImageName, mi->LoadedImageName );

    return TRUE;
}


DWORD
IMAGEAPI
SymGetModuleBase(
    IN  HANDLE              hProcess,
    IN  DWORD               dwAddr
    )
{
    PPROCESS_ENTRY          ProcessEntry;
    PMODULE_ENTRY           mi;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return 0;
    }

    mi = GetModuleForPC( ProcessEntry, dwAddr );
    if (mi == NULL) {
        return 0;
    }

    return mi->BaseOfDll;
}


BOOL
IMAGEAPI
SymUnloadModule(
    IN  HANDLE      hProcess,
    IN  DWORD       BaseOfDll
    )

/*++

Routine Description:

    Remove the symbols for an image from a process' symbol table.

Arguments:

    hProcess - Supplies the token which refers to the process

    BaseOfDll - Supplies the offset to the image as supplies by the
        LOAD_DLL_DEBUG_EVENT and UNLOAD_DLL_DEBUG_EVENT.

Return Value:

    Returns TRUE if the module's symbols were successfully unloaded.
    Returns FALSE if the symbol handler does not recognize hProcess or
    no image was loaded at the given offset.

--*/

{
    PPROCESS_ENTRY  ProcessEntry;
    PLIST_ENTRY     Next;
    PMODULE_ENTRY   ModuleEntry;

    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (Next) {
        while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
            ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
            if (ModuleEntry->BaseOfDll == BaseOfDll) {
                RemoveEntryList(Next);
                FreeModuleEntry(ModuleEntry);
                return TRUE;
            }
            Next = ModuleEntry->ListEntry.Flink;
        }
    }
    return FALSE;

}

DWORD
IMAGEAPI
SymLoadModule(
    IN  HANDLE          hProcess,
    IN  PSTR            ImageName,
    IN  PSTR            ModuleName,
    IN  DWORD           BaseOfDll,
    IN  DWORD           DllSize
    )

/*++

Routine Description:

    Loads the symbols for an image for use by the other Sym functions.

Arguments:

    hProcess - Supplies unique process identifier.

    ImageName - Supplies the name of the image file.

    ModuleName - ???? Supplies the module name that will be returned by
            enumeration functions ????

    BaseOfDll - Supplies loaded base address of image.


Return Value:


--*/

{
    return InternalLoadModule(hProcess, ImageName, ModuleName, BaseOfDll, DllSize, NULL);
}

DWORD
IMAGEAPI
SymLoadModuleFromFile(
    IN  HANDLE          hProcess,
    IN  HANDLE          hFile,
    IN  DWORD           BaseOfDll,
    IN  DWORD           DllSize
    )

/*++

Routine Description:

    Loads the symbols for an image for use by the other Sym functions.

Arguments:

    hProcess - Supplies unique process identifier.

    hFile - Supplies image file handle, as from LOAD_DLL_DEBUG_EVENT.

    BaseOfDll - Supplies loaded base address of image.


Return Value:


--*/

{
    return InternalLoadModule(hProcess, NULL, NULL, BaseOfDll, DllSize, hFile);
}

BOOL
InternalLoadModule(
    IN  HANDLE          hProcess,
    IN  PSTR            ImageName,
    IN  PSTR            ModuleName,
    IN  DWORD           BaseOfDll,
    IN  DWORD           DllSize,
    IN  HANDLE          hFile
    )
{
    PIMAGE_DEBUG_INFORMATION    di;
    PPROCESS_ENTRY              ProcessEntry;
    PMODULE_ENTRY               mi;
    PIMAGE_COFF_SYMBOLS_HEADER  lpDebugInfo;
    PIMAGE_SYMBOL               lpSymbolEntry;
    PUCHAR                      lpStringTable;
    PUCHAR                      p;


    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return FALSE;
    }

    di = MapDebugInformation(
        hFile,
        ImageName,
        ProcessEntry->SymbolSearchPath,
        BaseOfDll
        );

    if (!di) {
        return FALSE;
    }

    mi = LocalAlloc( LPTR, sizeof(MODULE_ENTRY) );
    if (!mi) {
        return FALSE;
    }
    ZeroMemory( mi, sizeof(MODULE_ENTRY) );

    mi->BaseOfDll        = BaseOfDll ? BaseOfDll : di->ImageBase;
    mi->DllSize          = DllSize ? DllSize : di->SizeOfImage;
    mi->TimeDateStamp    = 0;
    mi->CheckSum         = di->CheckSum;
    mi->MachineType      = di->Machine;
    if (ModuleName) {
        mi->ModuleName = StringDup(ModuleName);
    } else {
        mi->ModuleName = StringDup(di->ImageFileName);
        p = strchr( mi->ModuleName, '.' );
        if (p) {
            *p = 0;
        }
    }
    if (ImageName) {
        mi->ImageName = StringDup(ImageName);
    } else {
        mi->ImageName = StringDup(di->ImageFileName);
    }
    mi->LoadedImageName  = StringDup(di->DebugFilePath);

    if (di->NumberOfFunctionTableEntries) {
        mi->pExceptionData = LocalAlloc(
            LPTR,
            sizeof(IMAGE_FUNCTION_ENTRY) * di->NumberOfFunctionTableEntries
            );
        if (mi->pExceptionData) {
            mi->dwEntries = di->NumberOfFunctionTableEntries;
            CopyMemory(
                mi->pExceptionData,
                di->FunctionTableEntries,
                sizeof(IMAGE_FUNCTION_ENTRY) * di->NumberOfFunctionTableEntries
                );
        }
    } else if (di->NumberOfFpoTableEntries) {
        mi->pFpoData = LocalAlloc(
            LPTR,
            sizeof(FPO_DATA) * di->NumberOfFpoTableEntries
            );
        if (mi->pFpoData) {
            mi->dwEntries = di->NumberOfFpoTableEntries;
            CopyMemory(
                mi->pFpoData,
                di->FpoTableEntries,
                sizeof(FPO_DATA) * di->NumberOfFpoTableEntries
                );
        }
    }

    mi->NumSections = di->NumberOfSections;
    mi->SectionHdrs = LocalAlloc(
        LPTR,
        sizeof(IMAGE_SECTION_HEADER) * di->NumberOfSections
        );
    if (mi->SectionHdrs) {
        CopyMemory(
            mi->SectionHdrs,
            di->Sections,
            sizeof(IMAGE_SECTION_HEADER) * di->NumberOfSections
            );
    }

    mi->TmpSym = LocalAlloc( LPTR, 4096 );

    if (di->SizeOfCodeViewSymbols) {
        LoadCodeViewSymbols(
            hProcess,
            mi,
            di->CodeViewSymbols,
            di->SizeOfCodeViewSymbols
            );
    } else if (di->SizeOfCoffSymbols) {
        lpDebugInfo = di->CoffSymbols;
        lpSymbolEntry = (PIMAGE_SYMBOL)((ULONG)lpDebugInfo +
                                               lpDebugInfo->LvaToFirstSymbol);
        lpStringTable = (PUCHAR)((ULONG)lpDebugInfo +
                                        lpDebugInfo->LvaToFirstSymbol +
                                        lpDebugInfo->NumberOfSymbols * IMAGE_SIZEOF_SYMBOL);
        LoadCoffSymbols(
            hProcess,
            mi,
            lpStringTable,
            lpSymbolEntry,
            lpDebugInfo->NumberOfSymbols
            );
    }

    LoadOmap( mi, di );

    ProcessOmapForModule( mi );

    UnmapDebugInformation( di );

    ProcessEntry->Count += 1;

    InsertTailList( &ProcessEntry->ModuleList, &mi->ListEntry, );

    return mi->BaseOfDll;
}


PPROCESS_ENTRY
FindProcessEntry(
    HANDLE  hProcess
    )
{
    PLIST_ENTRY                 Next;
    PPROCESS_ENTRY              ProcessEntry;


    Next = ProcessList.Flink;
    if (!Next) {
        return NULL;
    }

    while ((ULONG)Next != (ULONG)&ProcessList) {
        ProcessEntry = CONTAINING_RECORD( Next, PROCESS_ENTRY, ListEntry );
        Next = ProcessEntry->ListEntry.Flink;
        if (ProcessEntry->hProcess == hProcess) {
            return ProcessEntry;
        }
    }

    return NULL;
}


PIMAGEHLP_SYMBOL
GetSymFromAddr(
    DWORD           dwAddr,
    PDWORD          pdwDisplacement,
    PMODULE_ENTRY   mi
    )
{
    PIMAGEHLP_SYMBOL        sym;
    PIMAGE_SECTION_HEADER   sh;
    DWORD                   i;
    DATASYM32               *dataSym;


    if (mi == NULL) {
        return NULL;
    }

    if (mi->SymType == SymPdb) {
        DWORD Bias;
        DWORD OptimizedSymAddr = ConvertOmapFromSrc( mi, dwAddr, &Bias );
        if (!OptimizedSymAddr) {
            //
            // No equivalent address
            //
            dwAddr = 0;
        } else if (OptimizedSymAddr != dwAddr) {
            //
            // We have successfully converted
            //
            dwAddr = OptimizedSymAddr + Bias - mi->BaseOfDll;
        }

        //
        // locate the section that the address resides in
        //
        for (i=0,sh=mi->SectionHdrs; i<mi->NumSections; i++, sh++) {
            if (dwAddr - mi->BaseOfDll >= sh->VirtualAddress &&
                dwAddr - mi->BaseOfDll <  sh->VirtualAddress + sh->SizeOfRawData) {
                //
                // found the section
                //
                break;
            }
        }

        if (i == mi->NumSections) {
            return NULL;
        }

        dataSym = (DATASYM32*)GSINearestSym(
            mi->gsi,
            (USHORT)(i+1),
            dwAddr - mi->BaseOfDll - sh->VirtualAddress,
            pdwDisplacement
            );

        if (dataSym) {
            mi->TmpSym->next  = NULL;
            mi->TmpSym->prev  = NULL;
            mi->TmpSym->size  = 0;
            mi->TmpSym->flags = 0;
            mi->TmpSym->addr  = dwAddr;
            memcpy( mi->TmpSym->szName, dataSym->name, (UCHAR)dataSym->name[0]+1 );
            mi->TmpSym->szName[dataSym->name[0] + 1] = 0;
            return mi->TmpSym;
        }

        return NULL;
    }

    for (i=0,sym=mi->symbolTable; i<mi->numsyms; i++,sym=sym->next) {
        if (dwAddr >= sym->addr && dwAddr < sym->addr + sym->size) {
            if (pdwDisplacement) {
                *pdwDisplacement = dwAddr - sym->addr;
            }
            return sym;
        }
    }

    return NULL;
}


PMODULE_ENTRY
GetModuleForPC(
    PPROCESS_ENTRY  ProcessEntry,
    DWORD           dwPcAddr
    )
{
    static PLIST_ENTRY          Next = NULL;
    PMODULE_ENTRY               ModuleEntry;


    if (dwPcAddr == (DWORD)-1) {
        if ((ULONG)Next == (ULONG)&ProcessEntry->ModuleList) {
            return NULL;
        }
        ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
        Next = ModuleEntry->ListEntry.Flink;
        return ModuleEntry;
    }

    Next = ProcessEntry->ModuleList.Flink;
    if (!Next) {
        return NULL;
    }

    while ((ULONG)Next != (ULONG)&ProcessEntry->ModuleList) {
        ModuleEntry = CONTAINING_RECORD( Next, MODULE_ENTRY, ListEntry );
        Next = ModuleEntry->ListEntry.Flink;
        if (dwPcAddr == 0) {
            return ModuleEntry;
        }
        if ((dwPcAddr >= ModuleEntry->BaseOfDll) &&
            (dwPcAddr  < ModuleEntry->BaseOfDll + ModuleEntry->DllSize)) {
               return ModuleEntry;
        }
    }

    return NULL;
}


PIMAGEHLP_SYMBOL
GetSymFromAddrAllContexts(
    DWORD           dwAddr,
    PDWORD          pdwDisplacement,
    PPROCESS_ENTRY  ProcessEntry
    )
{
    PMODULE_ENTRY mi = GetModuleForPC( ProcessEntry, dwAddr );
    if (mi == NULL) {
        return NULL;
    }
    return GetSymFromAddr( dwAddr, pdwDisplacement, mi );
}


PIMAGEHLP_SYMBOL
AllocSym(
    PMODULE_ENTRY   mi,
    DWORD           addr,
    LPSTR           name
    )
{
    PIMAGEHLP_SYMBOL sym;
    PIMAGEHLP_SYMBOL symS;
    DWORD   i;


    //
    // allocate memory for the symbol entry
    //
    i = sizeof(IMAGEHLP_SYMBOL) + (UCHAR)name[0] + 2;
    sym = (PIMAGEHLP_SYMBOL) LocalAlloc( LPTR, i );
    if (!sym) {
        return NULL;
    }
    ZeroMemory( sym, i );

    mi->numsyms++;

    //
    // initialize the symbol entry
    //
    sym->addr = addr;
    memcpy( sym->szName, name, (UCHAR)name[0]+1 );
    sym->size = 0;
    sym->next = NULL;
    sym->prev = NULL;
    sym->flags = 0;

    //
    // get the list head
    //
    symS = mi->symbolTable;

    if (!symS) {
        //
        // add the first list entry
        //
        mi->symbolTable = sym;
        return sym;
    }

    if (symS->addr > sym->addr) {
        //
        // replace the list head
        //
        mi->symbolTable = sym;
        sym->next = symS;
        sym->prev = (symS->prev) ? symS->prev : symS;
        sym->prev->next = sym;
        symS->prev = sym;
        return sym;
    }

    if ((symS->prev == NULL && sym->addr >= symS->addr) ||
        (symS->prev && sym->addr > symS->prev->addr)) {
        //
        // the entry goes at the end of the list
        //
        symS = (symS->prev) ? symS->prev : symS;
        sym->next = (symS->next) ? symS->next : symS;
        sym->prev = symS;
        symS->next = sym;
        symS->prev = (symS->prev) ? symS->prev : sym;
        mi->symbolTable->prev = sym;
        return sym;
    }

    //
    // the symbol goes somewhere in the middle
    //
    for (i=0; i<mi->numsyms; i++,symS=symS->next) {
        if (symS->addr > sym->addr) {
            sym->next = symS;
            sym->prev = symS->prev;
            symS->prev->next = sym;
            symS->prev = sym;
            return sym;
        }
    }

    mi->numsyms--;
    LocalFree( sym );

    return NULL;
}


BOOL
LoadOmap(
    PMODULE_ENTRY               mi,
    PIMAGE_DEBUG_INFORMATION    di
    )
{
    PIMAGE_DEBUG_DIRECTORY pDebugDir;
    ULONG                  ulDebugDirCount;
    ULONG                  cb;
    PVOID                  pv;


    pDebugDir = di->DebugDirectory;
    ulDebugDirCount = di->NumberOfDebugDirectories;

    while (ulDebugDirCount--) {
        cb = pDebugDir->SizeOfData;
        pv = (PVOID) ((DWORD)di->MappedBase + pDebugDir->PointerToRawData);
        switch (pDebugDir->Type) {
            case IMAGE_DEBUG_TYPE_OMAP_TO_SRC:
                mi->pOmapTo = (POMAP) LocalAlloc( LPTR, cb );
                RtlCopyMemory( mi->pOmapTo, pv, cb);
                mi->cOmapTo = cb / sizeof(OMAP);
                break;

            case IMAGE_DEBUG_TYPE_OMAP_FROM_SRC:
                mi->pOmapFrom = (POMAP) LocalAlloc( LPTR, cb );
                RtlCopyMemory( mi->pOmapFrom, pv, cb);
                mi->cOmapFrom = cb / sizeof(OMAP);
                break;
        }
        pDebugDir++;
    }

    return TRUE;
}


BOOL
LoadCoffSymbols(
    HANDLE             hProcess,
    PMODULE_ENTRY      mi,
    PUCHAR             stringTable,
    PIMAGE_SYMBOL      allSymbols,
    DWORD              numberOfSymbols
    )
{
    PIMAGE_SYMBOL       NextSymbol;
    PIMAGE_SYMBOL       Symbol;
    PIMAGE_AUX_SYMBOL   AuxSymbol;
    PIMAGEHLP_SYMBOL             sym;
    CHAR                szSymName[256];
    DWORD               numaux;
    DWORD               i;
    DWORD               j;
    DWORD               addr;


    mi->symbolTable = NULL;
    NextSymbol = allSymbols;
    for (i= 0; i < numberOfSymbols; i++) {
        Symbol = NextSymbol++;
        if (Symbol->StorageClass == IMAGE_SYM_CLASS_EXTERNAL && Symbol->SectionNumber > 0) {
            GetSymName( Symbol, stringTable, &szSymName[1] );
            addr = Symbol->Value + mi->BaseOfDll;
            (UCHAR)szSymName[0] = strlen(&szSymName[1]);
            if (szSymName[1] == '?' && szSymName[2] == '?' &&
                szSymName[3] == '_' && szSymName[4] == 'C'    ) {
                //
                // ignore strings
                //
            } else {
                AllocSym( mi, addr, szSymName );
            }
        }
        if (numaux = Symbol->NumberOfAuxSymbols) {
            for (j=numaux; j; --j) {
                AuxSymbol = (PIMAGE_AUX_SYMBOL) NextSymbol;
                NextSymbol++;
                ++i;
            }
        }
    }

    //
    // calculate the size of each symbol
    //
    for (i=0,sym=mi->symbolTable; i<mi->numsyms; i++,sym=sym->next) {
        if (i+1 < mi->numsyms) {
            sym->size = sym->next->addr - sym->addr;
        }
    }

    mi->SymType = SymCoff;

    return TRUE;
}

BOOL
LoadCodeViewSymbols(
    HANDLE                  hProcess,
    PMODULE_ENTRY           mi,
    PUCHAR                  pCvData,
    DWORD                   dwSize
    )
{
    CHAR                    ErrorText[1024];
    OMFSignature            *omfSig;
    OMFDirHeader            *omfDirHdr;
    OMFDirEntry             *omfDirEntry;
    DATASYM32               *dataSym;
    OMFSymHash              *omfSymHash;
    PIMAGEHLP_SYMBOL        sym;
    DWORD                   i;
    DWORD                   j;
    DWORD                   k;
    DWORD                   addr;
    PIMAGE_SECTION_HEADER   sh;
    PPDB_INFO               PdbInfo;
    CHAR                    Path[MAX_PATH];
    CHAR                    PdbName[MAX_PATH];
    CHAR                    Fname[MAX_PATH];
    CHAR                    Ext[_MAX_EXT];
    CHAR                    Drive[_MAX_DRIVE];
    CHAR                    Dir[_MAX_DIR];
    HANDLE                  hPdb;


    mi->pCvData = (LPVOID) LocalAlloc( LPTR, dwSize );
    if (!mi->pCvData) {
        return FALSE;
    }
    memcpy( mi->pCvData, pCvData, dwSize );

    omfSig = (OMFSignature*) mi->pCvData;
    if ((strncmp( omfSig->Signature, "NB08", 4 ) != 0) &&
        (strncmp( omfSig->Signature, "NB09", 4 ) != 0) &&
        (strncmp( omfSig->Signature, "NB10", 4 ) != 0)) {
        return FALSE;
    }

    if (strncmp( omfSig->Signature, "NB10", 4 ) == 0) {
        PdbInfo = (PPDB_INFO) mi->pCvData;
        _splitpath( mi->LoadedImageName, Drive, Dir, NULL, NULL );
        _makepath( Path, Drive, Dir, NULL, NULL );
        if (!PDBOpenValidate(
                PdbInfo->PdbName,
                Path,
                "r",
                PdbInfo->sig,
                PdbInfo->age,
                &i,
                ErrorText,
                &mi->pdb )) {
            //
            // either the pdb could not be found or
            // it's signature/age did not match
            //
            // now lets look down the _nt_symbol_path
            //
            _splitpath( PdbInfo->PdbName, NULL, NULL, Fname, Ext );
            strcat( Fname, Ext );
            hPdb = FindExecutableImage(
                Fname,
                SymGetSearchPath(hProcess),
                PdbName
                );
            if (!hPdb) {
                return FALSE;
            }
            CloseHandle( hPdb );
            if (!PDBOpenValidate(
                    PdbName,
                    NULL,
                    "r",
                    PdbInfo->sig,
                    PdbInfo->age,
                    &i,
                    ErrorText,
                    &mi->pdb )) {
                //
                // the pdb we found along the symbol path is bad too!
                //
                return FALSE;
            }
        }

        if (!PDBOpenDBI( mi->pdb, "r", "", &mi->dbi )) {
            PDBClose( mi->pdb );
            return FALSE;
        }

        if (!DBIOpenPublics( mi->dbi, &mi->gsi )) {
            DBIClose( mi->dbi );
            PDBClose( mi->pdb );
            return FALSE;
        }

        mi->SymType = SymPdb;

    } else {

        omfDirHdr = (OMFDirHeader*) ((DWORD)omfSig + (DWORD)omfSig->filepos);
        omfDirEntry = (OMFDirEntry*) ((DWORD)omfDirHdr + sizeof(OMFDirHeader));

        for (i=0; i<omfDirHdr->cDir; i++,omfDirEntry++) {
            if (omfDirEntry->SubSection == sstGlobalPub) {
                omfSymHash = (OMFSymHash*) ((DWORD)omfSig + omfDirEntry->lfo);
                dataSym = (DATASYM32*) ((DWORD)omfSig + omfDirEntry->lfo + sizeof(OMFSymHash));
                for (j=sizeof(OMFSymHash); j<=omfSymHash->cbSymbol; ) {
                    addr = 0;
                    for (k=0,addr=0,sh=mi->SectionHdrs; k<mi->NumSections; k++, sh++) {
                        if (k+1 == dataSym->seg) {
                            addr = sh->VirtualAddress + (dataSym->off + mi->BaseOfDll);
                            break;
                        }
                    }
                    if (addr) {
                        if (dataSym->name[1] == '?' && dataSym->name[2] == '?' &&
                            dataSym->name[3] == '_' && dataSym->name[4] == 'C'    ) {
                            //
                            // ignore strings
                            //
                        } else {
                            AllocSym( mi, addr, dataSym->name );
                        }
                    }
                    j += dataSym->reclen + 2;
                    dataSym = (DATASYM32*) ((DWORD)dataSym + dataSym->reclen + 2);
                }
                break;
            }
        }
        mi->SymType = SymCv;
    }

    //
    // calculate the size of each symbol
    //
    for (i=0,sym=mi->symbolTable; i<mi->numsyms; i++,sym=sym->next) {
        if (i+1 < mi->numsyms) {
            sym->size = sym->next->addr - sym->addr;
        }
    }

    return TRUE;
}

VOID
GetSymName(
    PIMAGE_SYMBOL Symbol,
    PUCHAR        StringTable,
    LPSTR         s
    )
{
    DWORD i;

    if (Symbol->n_zeroes) {
        for (i=0; i<8; i++) {
            if ((Symbol->n_name[i]>0x1f) && (Symbol->n_name[i]<0x7f)) {
                *s++ = Symbol->n_name[i];
            }
        }
        *s = 0;
    }
    else {
        strcpy( s, &StringTable[Symbol->n_offset] );
    }
}

LPSTR
IMAGEAPI
SymUnDName(
    PIMAGEHLP_SYMBOL sym
    )
{
    static char outBuf[1024];
    LPSTR dname = &sym->szName[1];
    DWORD len = (DWORD)sym->szName[0];
    LPSTR p;


    if (dname[0] == '?') {
        if(UnDecorateSymbolName( dname,
                                 outBuf,
                                 sizeof(outBuf),
                                 UNDNAME_NAME_ONLY ) == 0 ) {
            memcpy( outBuf, dname, len );
            outBuf[len] = 0;
        }
    } else {
        if (dname[0] == '_' || dname[0] == '@') {
            dname += 1;
        }
        if (dname[0] == '.' && dname[1] == '.') {
            dname += 2;
        }
        p = strchr( dname, '@' );
        if (p) {
            *p = 0;
        }
        strcpy( outBuf, dname );
    }

    return outBuf;
}


VOID
ProcessOmapForModule(
    PMODULE_ENTRY mi
    )
{
    PIMAGEHLP_SYMBOL       sym;
    PIMAGEHLP_SYMBOL       symN;
    DWORD         i;


    if (!mi->pOmapFrom) {
        return;
    }

    if (!mi->symbolTable) {
        return;
    }

    if ((mi->SymType != SymCoff) && (mi->SymType != SymCv)) {
        return;
    }

    sym = mi->symbolTable;
    while (sym->next != mi->symbolTable) {
        symN = sym->next;
        ProcessOmapSymbol( mi, sym );
        sym = symN;
    }

    //
    // calculate the size of each symbol
    // this must be done again because the omap process
    // may have added symbols
    //
    for (i=0,sym=mi->symbolTable; i<mi->numsyms; i++,sym=sym->next) {
        if (i+1 < mi->numsyms) {
            sym->size = sym->next->addr - sym->addr;
        }
    }
}


BOOL
ProcessOmapSymbol(
    PMODULE_ENTRY mi,
    PIMAGEHLP_SYMBOL       sym
    )
{
    DWORD       bias;
    DWORD       OptimizedSymAddr;
    DWORD       rvaSym;
    POMAPLIST   pomaplistHead;
    DWORD       SymbolValue;
    DWORD       OrgSymAddr;
    POMAPLIST   pomaplistNew;
    POMAPLIST   pomaplistPrev;
    POMAPLIST   pomaplistCur;
    POMAPLIST   pomaplistNext;
    DWORD       rva;
    DWORD       rvaTo;
    DWORD       cb;
    DWORD       end;
    DWORD       rvaToNext;
    LPSTR       NewSymName;
    CHAR        Suffix[32];
    DWORD       addrNew;
    POMAP       pomap;
    PIMAGEHLP_SYMBOL     symOmap;


    if (sym->flags & SYMF_OMAP_GENERATED || sym->flags & SYMF_OMAP_MODIFIED) {
        return FALSE;
    }

    OrgSymAddr = SymbolValue = sym->addr;
    OptimizedSymAddr = ConvertOmapFromSrc( mi, SymbolValue, &bias );

    if (OptimizedSymAddr == 0) {
        //
        // No equivalent address
        //
        sym->addr = 0;
    } else if (OptimizedSymAddr != sym->addr) {
        //
        // We have successfully converted
        //
        sym->addr = OptimizedSymAddr + bias - mi->BaseOfDll;
    }

    if (!sym->addr) {
        goto exit;
    }

    rvaSym = SymbolValue - mi->BaseOfDll;
    SymbolValue = sym->addr + mi->BaseOfDll;

    pomap = GetOmapEntry( mi, OrgSymAddr );
    if (!pomap) {
        goto exit;
    }

    pomaplistHead = NULL;

    //
    // Look for all OMAP entries belonging to SymbolEntry
    //

    end = OrgSymAddr - mi->BaseOfDll + sym->size;

    while (pomap && (pomap->rva < end)) {

        if (pomap->rvaTo == 0) {
            pomap++;
            continue;
        }

        //
        // Allocate and initialize a new entry
        //
        pomaplistNew = (POMAPLIST) LocalAlloc( LPTR, sizeof(OMAPLIST) );
        if (!pomaplistNew) {
            return FALSE;
        }

        pomaplistNew->omap = *pomap;
        pomaplistNew->cb = pomap[1].rva - pomap->rva;

        pomaplistPrev = NULL;
        pomaplistCur = pomaplistHead;

        while (pomaplistCur != NULL) {
            if (pomap->rvaTo < pomaplistCur->omap.rvaTo) {
                //
                // Insert between Prev and Cur
                //
                break;
            }
            pomaplistPrev = pomaplistCur;
            pomaplistCur = pomaplistCur->next;
        }

        if (pomaplistPrev == NULL) {
            //
            // Insert in head position
            //
            pomaplistHead = pomaplistNew;
        } else {
            pomaplistPrev->next = pomaplistNew;
        }

        pomaplistNew->next = pomaplistCur;

        pomap++;
    }

    if (pomaplistHead == NULL) {
        goto exit;
    }

    pomaplistCur = pomaplistHead;
    pomaplistNext = pomaplistHead->next;

    //
    // we do have a list
    //
    while (pomaplistNext != NULL) {
        rva = pomaplistCur->omap.rva;
        rvaTo  = pomaplistCur->omap.rvaTo;
        cb = pomaplistCur->cb;
        rvaToNext = pomaplistNext->omap.rvaTo;

        if (rvaToNext == sym->addr) {
            //
            // Already inserted above
            //
        } else if (rvaToNext < (rvaTo + cb + 8)) {
            //
            // Adjacent to previous range
            //
        } else {
            addrNew = mi->BaseOfDll + rvaToNext;
            Suffix[0] = '_';
            ltoa( pomaplistNext->omap.rva - rvaSym, &Suffix[1], 10 );
            cb = strlen(Suffix) + (UCHAR)sym->szName[0] + 2;
            NewSymName = LocalAlloc( LPTR,  cb );
            if (!NewSymName) {
                return FALSE;
            }
            cb = (UCHAR)sym->szName[0] + 1;
            memcpy( NewSymName, sym->szName, cb );
            memcpy( &NewSymName[cb], Suffix, strlen(Suffix) );
            (UCHAR)NewSymName[0] += strlen(Suffix);
            symOmap = AllocSym( mi, addrNew, NewSymName );
            LocalFree( NewSymName );
            if (symOmap) {
                symOmap->flags |= SYMF_OMAP_GENERATED;
            }
        }

        LocalFree(pomaplistCur);

        pomaplistCur = pomaplistNext;
        pomaplistNext = pomaplistNext->next;
    }

    LocalFree(pomaplistCur);

exit:
    sym->addr += mi->BaseOfDll;
    if (sym->addr != OrgSymAddr) {
        sym->prev->next = sym->next;
        sym->next->prev = sym->prev;
        mi->numsyms--;
        if (sym == mi->symbolTable) {
            mi->symbolTable = sym->next;
        }
        symOmap = AllocSym( mi, sym->addr, sym->szName );
        if (symOmap) {
            symOmap->flags |= SYMF_OMAP_MODIFIED;
        }
        LocalFree( sym );
    }

    return TRUE;
}


DWORD
ConvertOmapFromSrc(
    PMODULE_ENTRY  mi,
    DWORD          addr,
    LPDWORD        bias
    )
{
    DWORD   rva;
    DWORD   comap;
    POMAP   pomapLow;
    POMAP   pomapHigh;
    DWORD   comapHalf;
    POMAP   pomapMid;


    *bias = 0;

    if (!mi->pOmapFrom) {
        return addr;
    }

    rva = addr - mi->BaseOfDll;

    comap = mi->cOmapFrom;
    pomapLow = mi->pOmapFrom;
    pomapHigh = pomapLow + comap;

    while (pomapLow < pomapHigh) {

        comapHalf = comap / 2;

        pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

        if (rva == pomapMid->rva) {
            return mi->BaseOfDll + pomapMid->rvaTo;
        }

        if (rva < pomapMid->rva) {
            pomapHigh = pomapMid;
            comap = (comap & 1) ? comapHalf : (comapHalf - 1);
        } else {
            pomapLow = pomapMid + 1;
            comap = comapHalf;
        }
    }

    //
    // If no exact match, pomapLow points to the next higher address
    //
    if (pomapLow == mi->pOmapFrom) {
        //
        // This address was not found
        //
        return 0;
    }

    if (pomapLow[-1].rvaTo == 0) {
        //
        // This address is not translated so just return the original
        //
        return addr;
    }

    //
    // Return the closest address plus the bias
    //
    *bias = rva - pomapLow[-1].rva;

    return mi->BaseOfDll + pomapLow[-1].rvaTo;
}


DWORD
ConvertOmapToSrc(
    PMODULE_ENTRY  mi,
    DWORD          addr,
    LPDWORD        bias
    )
{
    DWORD   rva;
    DWORD   comap;
    POMAP   pomapLow;
    POMAP   pomapHigh;
    DWORD   comapHalf;
    POMAP   pomapMid;
    INT     i;


    *bias = 0;

    if (!mi->pOmapTo) {
        return 0;
    }

    rva = addr - mi->BaseOfDll;

    comap = mi->cOmapTo;
    pomapLow = mi->pOmapTo;
    pomapHigh = pomapLow + comap;

    while (pomapLow < pomapHigh) {

        comapHalf = comap / 2;

        pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

        if (rva == pomapMid->rva) {
            if (pomapMid->rvaTo == 0) {
                //
                // We are probably in the middle of a routine
                //
                i = -1;
                while ((&pomapMid[i] != mi->pOmapTo) && pomapMid[i].rvaTo == 0) {
                    //
                    // Keep on looping back until the beginning
                    //
                    i--;
                }
                return mi->BaseOfDll + pomapMid[i].rvaTo;
            } else {
                return mi->BaseOfDll + pomapMid->rvaTo;
            }
        }

        if (rva < pomapMid->rva) {
            pomapHigh = pomapMid;
            comap = (comap & 1) ? comapHalf : (comapHalf - 1);
        } else {
            pomapLow = pomapMid + 1;
            comap = comapHalf;
        }
    }

    //
    // If no exact match, pomapLow points to the next higher address
    //
    if (pomapLow == mi->pOmapTo) {
        //
        // This address was not found
        //
        return 0;
    }

    if (pomapLow[-1].rvaTo == 0) {
        return 0;
    }

    //
    // Return the new address plus the bias
    //
    *bias = rva - pomapLow[-1].rva;

    return mi->BaseOfDll + pomapLow[-1].rvaTo;
}

POMAP
GetOmapEntry(
    PMODULE_ENTRY  mi,
    DWORD          addr
    )
{
    DWORD   rva;
    DWORD   comap;
    POMAP   pomapLow;
    POMAP   pomapHigh;
    DWORD   comapHalf;
    POMAP   pomapMid;


    if (mi->pOmapFrom == NULL) {
        return NULL;
    }

    rva = addr - mi->BaseOfDll;

    comap = mi->cOmapFrom;
    pomapLow = mi->pOmapFrom;
    pomapHigh = pomapLow + comap;

    while (pomapLow < pomapHigh) {

        comapHalf = comap / 2;

        pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

        if (rva == pomapMid->rva) {
            return pomapMid;
        }

        if (rva < pomapMid->rva) {
            pomapHigh = pomapMid;
            comap = (comap & 1) ? comapHalf : (comapHalf - 1);
        } else {
            pomapLow = pomapMid + 1;
            comap = comapHalf;
        }
    }

    return NULL;
}


LPSTR
IMAGEAPI
SymGetSearchPath(
    HANDLE      hProcess
    )

/*++

Routine Description:

    This function looks up the symbol search path associated with a process.

Arguments:

    hProcess - Supplies the token associated with a process.

Return Value:

    A pointer to the search path.  Returns NULL if the process is not
    know to the symbol handler.

--*/

{
    PPROCESS_ENTRY ProcessEntry = FindProcessEntry( hProcess );

    if (!ProcessEntry) {
        return NULL;
    }

    return ProcessEntry->SymbolSearchPath;
}

LPSTR
IMAGEAPI
SymSetSearchPath(
    HANDLE      hProcess,
    LPSTR       UserSearchPath
    )

/*++

Routine Description:

    This functions sets the searh path to be used by the symbol loader
    for the given process.  If UserSearchPath is not supplied, a default
    path will be used.

Arguments:

    hProcess - Supplies the process token associated with a symbol table.

    UserSearchPath - Supplies the new search path to associate with the
        process. If this argument is NULL, the following path is generated:

        .;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%

        It is ok if any or all of the environment variables is missing.

Return Value:

    A pointer to the new search path.  The user should not modify this string.
    Returns NULL if the process is not known to the symbol handler.

--*/

{
    PPROCESS_ENTRY  ProcessEntry;
    LPSTR           p;
    DWORD           cbSymPath;
    DWORD           cb;

    ProcessEntry = FindProcessEntry( hProcess );
    if (!ProcessEntry) {
        return NULL;
    }

    if (ProcessEntry->SymbolSearchPath) {
        LocalFree(ProcessEntry->SymbolSearchPath);
    }

    if (UserSearchPath) {

        ProcessEntry->SymbolSearchPath = StringDup(UserSearchPath);

    } else {

        //
        // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%"
        //

        cbSymPath = 2;     // ".;"

        //
        // GetEnvironmentVariable returns the size of the string
        // INCLUDING the '\0' in this case.
        //
        cbSymPath += GetEnvironmentVariable( SYMBOL_PATH, NULL, 0 );
        cbSymPath += GetEnvironmentVariable( ALTERNATE_SYMBOL_PATH, NULL, 0 );
        cbSymPath += GetEnvironmentVariable( SYSTEMROOT, NULL, 0 );


        p = ProcessEntry->SymbolSearchPath = LocalAlloc( LPTR, cbSymPath );
        if (!p) {
            return NULL;
        }

        *p++ = '.';
        --cbSymPath;

        cb = GetEnvironmentVariable(SYMBOL_PATH, p+1, cbSymPath);
        if (cb) {
            *p = ';';
            p += cb+1;
            cbSymPath -= cb+1;
        }

        cb = GetEnvironmentVariable(ALTERNATE_SYMBOL_PATH, p+1, cbSymPath);
        if (cb) {
            *p = ';';
            p += cb+1;
            cbSymPath -= cb+1;
        }

        cb = GetEnvironmentVariable(SYSTEMROOT, p+1, cbSymPath);
        if (cb) {
            *p = ';';
            p += cb+1;
        }

        *p = 0;
    }

    return ProcessEntry->SymbolSearchPath;
}


LPSTR
StringDup(
    LPSTR str
    )
{
    LPSTR ds = LocalAlloc( LPTR, strlen(str) + 1 );
    if (ds) {
        strcpy( ds, str );
    }
    return ds;
}
