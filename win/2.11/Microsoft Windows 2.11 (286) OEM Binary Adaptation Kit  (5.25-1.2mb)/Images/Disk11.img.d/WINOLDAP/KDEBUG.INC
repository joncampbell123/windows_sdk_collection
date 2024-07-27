; Debugging include file for KERNEL
;

if1
ifndef WINDEBUG
    KDEBUG = 0
else
    SEGDEBUG = 1
    KDEBUG = 1
endif
outif KDEBUG,0,<Kernel debug support enabled>
ifndef SEGDEBUG
    SDEBUG = 0
else
    SDEBUG = 1
endif
outif SDEBUG,0,<Kernel segment debugger support enabled>
endif

ERR_LMEM        =   00100h          ; Local memory errors
ERR_LMEMCRIT    =   00140h          ; Local heap is busy
ERR_LMEMHANDLE  =   00180h          ; Invalid local handle
ERR_LMEMLOCK    =   001C0h          ; LocalLock count overflow
ERR_LMEMUNLOCK  =   001F0h          ; LocalUnlock count underflow

ERR_GMEM        =   00200h          ; global memory errors
ERR_GMEMCRIT    =   00240h          ; Critical section problems
ERR_GMEMHANDLE  =   00280h          ; Invalid global handle
ERR_GMEMLOCK    =   002C0h          ; globalLock count overflow
ERR_GMEMUNLOCK  =   002F0h          ; globalUnlock count underflow

ERR_TASK        =   00300h          ; task schedule errors
ERR_TASKID      =   00301h          ; Invalid task ID
ERR_TASKEXIT    =   00302h          ; Invalid exit system call
ERR_TASKFRAME   =   00303h          ; Invalid BP chain

ERR_LD          =   00400h          ; dynamic loader/linker errors
ERR_LDBOOT      =   00401h          ; Error during boot process
ERR_LDLOAD      =   00402h          ; Error loading a module
ERR_LDORD       =   00403h          ; Invalid ordinal reference
ERR_LDNAME      =   00404h          ; Invalid entry name reference
ERR_LDPROC      =   00405h          ; Invalid start proc
ERR_LDMODULE    =   00406h          ; Invalid module handle
ERR_LDRELOC     =   00407h          ; Invalid relocation record
ERR_LDFWDREF    =   00408h          ; Error saving forward reference
ERR_LDREADSEG   =   00409h          ; Error reading segment contents
ERR_LDREADREL   =   00410h          ; Error reading segment contents
ERR_LDDISK      =   00411h          ; Insert disk for specified file
ERR_LDNRTABLE   =   00412h          ; Error reading non-resident table
ERR_LDINT3F     =   004FFh          ; INT 3F handler unable to load segment

ERR_RESMAN      =   00500h          ; resource manager/user profile errors
ERR_MISSRES     =   00501h          ; Missing resource table
ERR_BADRESTYPE  =   00502h          ; Bad resource type
ERR_BADRESNAME  =   00503h          ; Bad resource name
ERR_BADRESFILE  =   00504h          ; Bad resource file
ERR_BADRESREAD  =   00505h          ; Error reading resource

ERR_ATOM        =   00600h          ; atom manager errors

ERR_IO          =   00700h          ; I/O package errors

if KDEBUG
externNP    <KernelError>
kerror  macro   errcode,msg,segarg,offarg
        local   a,b
        ifnb    <errcode>
        mov     ax,errcode
        endif
        push    ax
        mov     ax,codeOffset a
        push    cs
        push    ax
        ifnb    <segarg>
        push    segarg
        push    offarg
        else
        xor     ax,ax
        push    ax
        push    ax
        endif
        call    KernelError
        jmp     short b
a:
        ifnb    <msg>
        DB      '&msg'
        else
        DB      '*** Fatal Kernel Error ***'
        endif
        DB      0,'$'
b:
        endm
else

if1
outif   ?RIPAUX,0
endif

ife ?RIPAUX                  ; Dont declare external in RIPAUX.ASM
externFP    <FatalExit>
endif
kerror  macro   errcode,msg,segarg,offarg
        mov     ax,errcode
        cCall   FatalExit,<ax>
        endm
endif

if1
outif   ?LDDEBUG,0
endif

if KDEBUG
ife ?LDDEBUG
externNP    <KernelDbgMsg>
endif
dbgmsg  macro   msg
        call    KernelDbgMsg
        ifnb    <msg>
        DB      '&msg'
        endif
        DB      0
        endm
else
dbgmsg  macro   msg
        endm
endif
