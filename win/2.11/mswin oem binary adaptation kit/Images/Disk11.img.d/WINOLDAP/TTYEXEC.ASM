page ,132
;-------------------------------------------------------------------------
;
; MODULE TTYEXEC.ASM - Code for performing old app EXEC DOS function (4BH)
;			This is the code used when an old app EXECs another
;			old app with DOS call 4BH and is also the code used
;			by Windows to start an old app up from Windows.
;
; 			$Author:   tqn  $
; 			$Revision:   1.24  $
; 			$Date:   16 Nov 1987  0:39:34  $
;
.xlist
INCLUDE CMACROS.INC
NORASTOPS = 1		; dont include raster ops
NOVK	  = 1		; dont include virtual keys
NOTEXT	  = 1
NOMST	  = 1		; dont include misc structures
NOWM	  = 1		; don't include window messages
WOAOPT	  = 1		;~~tqn 050587 Short Windows.inc
INCLUDE WINDOWS.INC
INCLUDE PDB.INC
INCLUDE TTY.INC
include galloc.inc
.list
;
;========  HP MODIFICATION  ==================================================
;
;  - Add the size of HPSystem saved buffer to the total size of the allocated
;    partition. (routine AllocateMemory)
;  - Save and restore the state of HP-Systems upon entry and exit of the old
;    BAD application.
;
;    **************
;    * EXTENSIONS *
;    **************
;
;  - If there is not enough memory to allocate the required BAD DOS partition,
;    go to the extension module to swap Windows out and to reallocate the
;    Winoldap +Grabber modules as children of WIN.COM.
;
;  - If a BAD batch file requires more memory than currently available, swap
;    Windows out and execute that batch file by invoking the DOS EXEC function.
;
;  Author:	Tuan Nguyen
;
;  History:	Version 1.03
;		08/16/86
;		03/05/87	Integrate with the documented version of 
;				Winoldap 1.03
;
;==============================================================================

page

;
; Memory fudge factor for figuring old app memory requirements
;
SwapAreaSize    equ      800H		; Size in paras of area reserved for
					; use by Windows specific applications
SwapAreaEMS	equ	 280H		; Use this if EMS4.0 hardware is active



; The larger SwapArea is, the better Windows-specific applications can run
; with DOS applications. However, DOS applications will have less memory
; available to them and thus will have to be often swapped in and out of disk.


include pifoff.inc
page
include partition.inc
include	ems.inc
page
;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<OpenFile,BuildPDB,GetCurrentPDB,GetProfileInt>
externFP	<GlobalAlloc,GlobalFree,GlobalCompact,GlobalSize,GlobalFreeAll>
externFP	<GlobalRealloc,AnsiUpper,GetModuleHandle>
externFP	<GetCurrentTask,IsScreenGrab,GetWindowText>
externFP	<MessageBox,LoadString>
externFP	<ExitWindows,LoadModule>
externFP	<EnableWindow>

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<BeginExecution,EndExecution>
ExternNP	<EnableClipboard, DisableClipboard>
ExternNP	<EnableTopView, DisableTopView>
;
externNP	<GetBufferSizes,LoadGrab,PostWarning,HookGood>
externNP	<CheckCom,DestroyExit,TTYDefault>
externNP	<InitPartition,FreePartition,FindPartition>
externNP	<IsSwappable,ShowTerminate>

;
; Bit mask for device bit in DOS IOCTL get call
;
devid_ISDEV	EQU	80h

;
; EQUs for various DOS system calls we use
;
Set_DMA 	EQU	26
Close		EQU	62
Read		EQU	63
LSeek		EQU	66
IOCtl		EQU	68

;
; EXEC system call error codes
;
exec_invalid_function		EQU	1
exec_bad_format 		EQU	11
exec_not_enough_memory		EQU	8
exec_file_not_found		EQU	2
Msg_no_Floppy			equ	8CH ;!! HP

;
; The following get used as arguments to the EXEC system call.	They indicate
; whether or not the program is executed or whether or not a program header
; gets created.
;
exec_func_no_execute	EQU 1		; no execute bit
exec_func_overlay	EQU 2		; overlay bit

;
; These are the formats of the EXEC parameters for the various EXEC sub-functions
;
; Structure of EXEC call 0 parameters
;
Exec0		STRUC
Exec0_environ	DW	?		; seg addr of environment
Exec0_com_line	DD	?		; pointer to asciz command line
Exec0_5C_FCB	DD	?		; default fcb at 5C
Exec0_6C_FCB	DD	?		; default fcb at 6C
Exec0		ENDS
;
; Structure of EXEC call 1 parameters
;
Exec1		STRUC
Exec1_environ	DW	?		; seg addr of environment
Exec1_com_line	DD	?		; pointer to asciz command line
Exec1_5C_FCB	DD	?		; default fcb at 5C
Exec1_6C_FCB	DD	?		; default fcb at 6C
Exec1_SP	DW	?		; stack pointer of program
Exec1_SS	DW	?		; stack seg register of program
Exec1_IP	DW	?		; entry point IP
Exec1_CS	DW	?		; entry point CS
Exec1		ENDS
;
; Structure of EXEC call 3 parameters
;
Exec3		STRUC
Exec3_load_addr DW	?		; seg address of load point
Exec3_reloc_fac DW	?		; relocation factor
Exec3		ENDS

;
; Exit codes in upper byte of WAIT return code
;
Exit_terminate	    EQU     0
Exit_abort	    EQU     0
Exit_Ctrl_C	    EQU     1
Exit_Hard_Error     EQU     2
Exit_Keep_process   EQU     3

;
; EXE file header
;
EXE_file    STRUC
exe_signature	DW  ?			; must contain 4D5A  (yay zibo!)
exe_len_mod_512 DW  ?			; low 9 bits of length
exe_pages	DW  ?			; number of 512b pages in file
exe_rle_count	DW  ?			; count of reloc entries
exe_par_dir	DW  ?			; number of paragraphs before image
exe_min_BSS	DW  ?			; minimum number of para of BSS
exe_max_BSS	DW  ?			; max number of para of BSS
exe_SS		DW  ?			; stack of image
exe_SP		DW  ?			; SP of image
exe_chksum	DW  ?			; checksum  of file (ignored)
exe_IP		DW  ?			; IP of entry
exe_CS		DW  ?			; CS of entry
exe_rle_table	DW  ?			; byte offset of reloc table
exe_iov 	DW  ?			; overlay number (0 for root)
exe_sym_tab	DD  ?			; offset of symbol table in file
EXE_file    ENDS

;
; Valid values for exe_signature
;
exe_valid_signature	EQU 5A4Dh
exe_valid_old_signature EQU 4D5Ah

;
; Structure of EXE symbol table entries
;
symbol_entry	STRUC
sym_value	DD  ?
sym_type	DW  ?
sym_len 	DB  ?
sym_name	DB  255 dup (?)
symbol_entry	ENDS

page
;
; This is a copy of the DOS system call 4BH proc header
;
;	AL  Function
;	--  --------
;	 0  Load and execute the program.
;	 1  Load, create  the  program	header	but  do  not
;	    begin execution.
;	 3  Load overlay. No header created.
;
;	    AL = 0 -> load/execute program
;
;	    +---------------------------+
;	    | WORD segment address of	|
;	    | environment.		|
;	    +---------------------------+
;	    | DWORD pointer to ASCIZ	|
;	    | command line at 80h	|
;	    +---------------------------+
;	    | DWORD pointer to default	|
;	    | FCB to be passed at 5Ch	|
;	    +---------------------------+
;	    | DWORD pointer to default	|
;	    | FCB to be passed at 6Ch	|
;	    +---------------------------+
;
;	    AL = 1 -> load program
;
;	    +---------------------------+
;	    | WORD segment address of	|
;	    | environment.		|
;	    +---------------------------+
;	    | DWORD pointer to ASCIZ	|
;	    | command line at 80h	|
;	    +---------------------------+
;	    | DWORD pointer to default	|
;	    | FCB to be passed at 5Ch	|
;	    +---------------------------+
;	    | DWORD pointer to default	|
;	    | FCB to be passed at 6Ch	|
;	    +---------------------------+
;	    | DWORD returned value of	|
;	    | CS:IP			|
;	    +---------------------------+
;	    | DWORD returned value of	|
;	    | SS:IP			|
;	    +---------------------------+
;
;	    AL = 3 -> load overlay
;
;	    +---------------------------+
;	    | WORD segment address where|
;	    | file will be loaded.	|
;	    +---------------------------+
;	    | WORD relocation factor to |
;	    | be applied to the image.	|
;	    +---------------------------+
;
; Returns:
;	    AX = Exec_invalid_function
;	       = Exec_bad_format
;	       = Exec_bad_environment
;	       = Exec_not_enough_memory
;	       = Exec_file_not_found
;

;!!  HP
;
ExternFP	InquireSystem		; (SYSTEM)
ExternFP	<UpdateWindow, ShowWindow>
ExternFP	<A20Proc>
;
ExternW		HPPara			; Size of HPSystem Saved Area (TTYTASK)
ExternW		HPSeg			; Segment of HPSystem Save Ara (TTYTASK)
ExternW		TopPDB			; (TTYOEM)
ExternB		SwapName 		; Swap drive name ('A','B',..,'?')
ExternW 	fIcon
ExternW		EEMSBlock		; (TTYEMS)

ExternNP	<SaveHPSystem, RestoreHPSystem> ; (TTYSYS)
ExternNP	<DOSExec>			; (TTYEXT)
;
;******

page
;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	data
externB 	NoMemMsg	; Pointer to the pre-loaded memory error message
externW 	MyParmBlock	; EXEC parms storage for LoadModule
;
; The various PIF file variables loaded out of the PIF file
;
externW 	pifDrive		; Drive of program
externB 	pifPathname		; Name of program
externB 	pifTitle		; TITLE (caption) of program
externB 	pifInitialDir		; Initial directory
externW 	pifMaxMem		; Max memory
externW 	pifMinMem		; Min memory
externW 	pifInts 		; PIF specified INT swap range
externB 	pifIntLow		; Low byte of pifInts (lower bound)
externB 	pifIntHigh		; High byte of pifInts (upper bound)
externW 	pifSysMem		; System memory
externB 	pifBehavior		; Behavior bits
externB 	pifFlags		; Behavior bits
externB 	pifMsFlags		; Behavior bits

externB 	InBuf			; char input buffer
externB 	OutBuf			; char output buffer
externD 	pFileSpec		; Buffer for app name
externW 	OldApState		; OldApState flag
externW 	MyTopPDB		; WINOLDAP top PDB pointer
externW 	hTTYwnd 		; Handle of Window if good app
externW 	ShowParm		; Parm for ShowWindow of this app
externW 	ExecParm		; Storage for EXEC parametrs
externW		wReturnCode		; Top-Level App return code
;
; Various partition variables
;
externW 	Partition		; Addr of partition
externW 	SwitchPara		; Size of switch area
externW 	BufPara 		; Size of screen grab area
externW 	MousePara		; Size of mouse area

;!! HP
;
ExternW		COMMCount		; (TTYTASK)
ExternD		OrgInt21Proc
GlobalW		PifMinMem1,0		; Minimum size required in paras
 					; including environment and App PDB
GlobalW		PifMaxMem1,-1		; Maximum size desired:
					;  =0: All remaining free memory
					;  =-1:Force extended mode with Required
;*****

sEnd	data

page


sBegin	code
	assumes ds,data
	assumes cs,code

	public	ParseComLine,ShowError,ShowMessage,ShowMessage2
	public	Allocate,GetParagraphs
	public	SetCount,TTYquery,GetPureName
	public	GetUserProCS,GetBytes

	public	szPif, come_back_here

;!! HP
	public	GetBytes, MyAllocate, GetFreeMem	; Needed by TTYEXT.ASM
	public	Allocatememory				; Debugging
;*****

;-----------------------------------------------
;
; Global task data stored in WINOLDAP code segment
;

szMSDOS 	db	'MSDOS',0		; DOS executive to run if WIN.COM
						;   is execed
szPif		db	'PIF',0 		; PIF directory and PIF extension
szMemory	db	'MEMORY',0		; WIN.INI string for memory values
szComp		db	'COMPATIBLE',0		; WIN.INI string for behavior
szCommand	db	'COMMAND.COM',0 	; Name of DOS command interpreter
szComSpec	db	'COMSPEC='		; Correct command (full path) in envir
szTEMP		db	'TEMP=' 		; envir string for temp directory

GlobalW		AppCount, -2		; # oldapps *2 started (both chid/parents)
GlobalW 	A20ProcRet, 0

externB StartUpPathname 		; Windows start up PIF directory
externB GoodCount			; Count of good apps running
externB BadDepth			; Count of bad apps running
externB IconCount			; Count of ICONIC old apps
externW GrabSeg 			; Seg of grabber
externW GrabSize			; Size of grabber
externD Int3FVector			; WINDOWS INT 3F vector

;**
;
; WINEXEC - Windows EXEC an executable
;
;    Begin execution of a Standard application using PIF file info.
;
; ENTRY:
;	PathName - Long pointer to program name
;	ParameterBlock - Long pointer to parameter block.
;	LoadFunc
;	    Low byte - function: 0,1 or 3
;	    High bit of low byte set if it is KNOWN this isn't a Windows app
;		AND the pif data in DS has ALREADY been set for this app
;		Various error situations encountered will be handled differently
;		This bit being set indicates that the app is being run via
;		WINOLDAP from the Windows environment (the first old app in
;		an old app group). The bit being clear indicates that the
;		app is being EXECed by another old app (a subsequent old app
;		in some old app group).
;	DS = SS = Data of old app responsible for the EXEC
;	    NOTE: In the case of an old app which does not have
;		an old app parent, this DS is the initial
;		DS for the new old app
; EXIT:
;	If app is a Windows app
;	    App has started
;	    If AX == -1
;		App was loaded and is running
;	    else
;		EXEC failed and AX is error code to return in AX
;	Else
;	    App has started and run to completion (blocks on app EXIT)
;	    OR overlay or no start executable is loaded.
;	    If AX >= 64
;		AX is value to return in AX after successful EXEC
;	    else
;		EXEC failed and AX is error code to return in AX
; USES:
;	ALL but SS,DS,BP,SI,DI	 (C standard)
;
cProc	WinExec,<PUBLIC,NEAR>,<si,di>
	parmD	PathName		; Name to EXEC
	parmD	ParameterBlock		; EXEC parm block
	parmW	LoadFunc		; Type of EXEC

	localB	Exec_func		; Storage for LoadFunc
	localB	ParentPif		; Storage for parent PIF behavior
	localB	ParentPif2		; Storage for parent PIF behavior
	localW	ParentPartition 	; Storage for parent partition
	localW	Exec_PDB		; Storage for PDB of parent
	localW	EnvironParas		; number of segments in enviroment
	localW	Exec_Environ		; segment of enviroment
	localD	Exec_GlobalHeap 	; Global heap pointer for app
	localV	PathBuffer,128		; Buffer for name building

	localW	Exec_fh 		; File handle of exec file(s)
	localW	Exec_rel_fac		; Reloc factor from header or parms
	localW	Exec_res_len_para	; Length of EXE image
	localW	Exec_init_CS		; Initial CS:IP from header
	localW	Exec_init_IP
	localW	Exec_init_SS		; Initial SS:SP from header
	localW	Exec_init_SP
	localW	Exec_size		; Size for prog mem block
	localW	Exec_Load_Block 	; Misc storage
    ;
    ; THIS IS A DUPLICATE OF THE EXE FILE HEADER STRUCTURE ON THE STACK
    ;	this group of variables forms the internal buffer where the
    ;	EXE file header is read in, and it is used to buffer reloaction
    ;	table entries while performing fix ups
    ;
	localW	Exec_dma		; dma address
	localW	Exec_iov		; overlay number (0 for root)
	localW	Exec_rle_table		; byte offset of reloc table
	localW	Exec_CS 		; CS of entry
	localW	Exec_IP 		; IP of entry
	localW	Exec_chksum		; checksum  of file (ignored)
	localW	Exec_SP 		; SP of image
	localW	Exec_SS 		; stack of image
	localW	Exec_max_BSS		; max number of para of BSS
	localW	Exec_min_BSS		; minimum number of para of BSS
	localW	Exec_par_dir		; number of paragraphs before image
	localW	Exec_rle_count		; count of reloc entries
	localW	Exec_pages		; number of 512b pages in file
	localW	Exec_len_mod_512	; low 9 bits of length
	localW	Exec_signature		; must contain 4D5A
	localW	SwapArea		; fudge factor used in "MyAllocate"
	localW	MyAppCount		; local copy of AppCount

	Exec_internal_buffer_size   EQU 30  ; Size of above "buffer"

cBegin
    ;	Enable the A20 Line
	mov	ax, 1
	cCall	A20Proc,<ax>
	mov	cs:[A20ProcRet],ax

    ;	
    ;	Use a smaller fudge factor if EMS4.0 hardware is supported (i.e.
    ;	The EMS line is below 640 kb). It allows more room to run GOOD apps.
    ;
	mov	[SwapArea], SwapAreaSize
	mov	AX, CS:[EEMSBlock].Bankline
	or	AX, AX
	jz	WE_Cont
	cmp	AX, 0A000h
	jae	WE_Cont
	mov	[SwapArea], SwapAreaEMS

WE_Cont:
    ;
    ; Suck of various parametrs from old app DS of EXECor
    ;
	cld
	mov	al,[pifBehavior]
	mov	[ParentPif],al		; Parents PIF info
	mov	al,[pifMsFlags]
	mov	ParentPif2,al		; Parents PIF info
	mov	ax,[Partition]
	mov	ParentPartition,ax	; Parents partition
	cCall	GetCurrentPDB
	mov	CS:[TopPDB], dx		;!! HP
	mov	Exec_PDB,ax		; Parents PDB
    ;
    ; New app re-uses parent partition, so nul out partition
    ;	pointers
    ;
	xor	ax,ax
	mov	[BufPara],ax
	mov	[SwitchPara],ax
	mov	[MousePara],ax
    ;
    ; Convert pathname of file to Upper Case
    ;
	cCall	AnsiUpper,<Pathname>
    ;
    ; Reset DOS PDB to WINOLDAP PDB
    ;
	mov	bx,[MyTopPDB]
	mov	ah,50H			; set PDB
	int	21h
    ;
    ; See if input LoadFunc indicates PIF data in and not windows app
    ;
	mov	ax,LoadFunc
	mov	Exec_func,al
	test	al,80h			; Lots of work done?
	jz	ex1			; No, check
	jmp	ex4			; PIF stuff set, not Windows app

    ;
    ; Here if it MIGHT be a Windows app
    ;
ex1:	cmp	cs:[BadDepth],0 	; Is it bad?
	jnz	ex3aV			; Yes, can't run Windows apps
    ;
    ; See if the given file is a Windows app
    ;

    ;
    ; Set up parametrs for LoadModule at MyParmBlock
    ;
	lds	si,ParameterBlock
	push	ss
	pop	es
	mov	di,dataOffset MyParmBlock
	movsw			; move over enviroment
	movsw			; move over command line
	movsw
	mov	ax,dataOffset ExecParm
	stosw
	mov	ax,ss
	stosw
	xor	ax,ax
	stosw
	stosw
    ;
    ; Check if file extension is .COM or .BAT, if so we know it's
    ;	not a Windows app
    ;
	les	di,Pathname		; Point at name
	call	GetExtension
	jnz	ex3aT
	jmp	ex3a			; Extension is .BAT, cannot be Wind app
ex3aT:
	cmp	ax,'c.' 		; check for .COM file extension
	jnz	ex1a			; no match, check for .PIF
	cmp	dx,'mo'
	jnz	ex1a			; no, try .PIF
    ;
    ; We are EXECing a .COM file
    ; Check if EXECing WIN.COM, if so EXEC a new MSDOS.EXE
    ;
	les	di,Pathname
	call	GetPureName
	cmp	word ptr es:[di],"IW"
	jnz	ex3a			; Nope, is a regular .COM file
	cmp	word ptr es:[di+2],".N"
ex3aV:	jnz	ex3a			; Nope, is a regular .COM file
    ;
    ; EXECing WIN.COM, start up new MSDOS.EXE
    ;
	mov	ax,codeOffset szMSDOS
	regptr	csax,cs,ax
	cCall	GetModuleHandle,<csax>
	mov	dx,ax
	xor	ax,ax
	mov	ds,ax
	jmp	short ex2a		; Go start it

    ;
    ; Check if extension is .PIF, if so is not a Windows app
    ;
ex1a:	cmp	ax,'p.'
	jnz	ex2			; Not .PIF try Windows app
	cmp	dx,'fi'
	jz	ex3a			; Is .PIF, not Windows app
    ;
    ; App is NOT .COM .BAT .PIF, see if we can LoadModule it (Windows app)
    ;	 First lets see if the file even exists.
    ;
ex2:	push	ss
	pop	ds
	lea	si,PathBuffer		; try to open the file first
	mov	ax,OF_EXIST		; with no prompting
	regptr	dssi,ds,si
	cCall	OpenFile,<Pathname,dssi,ax>
	inc	ax			; Did it fail?
	mov	ax,[si+opXtra]		; grab the possible error code
	jz	ex2c			; Couldn't open, EXEC fails
	lea	dx,PathBuffer.opfile	; point at fully qualified name
    ;
    ; Try to LoadModule the app
    ;
ex2a:
 
;  We have to clean up our hooks temporarily before loadmodule
;
	push	DX
 	call	DisableTopView
	ccall	DisableClipBoard
	pop	DX
	mov	ax,dataOffset MyParmBlock
	regptr	dsdx,ds,dx
	regptr	ssax,ss,ax
	cCall	LoadModule,<dsdx,ssax>
;
	push	AX
	call	EnableClipBoard
	call	EnableTopView
	pop	AX
	cmp	ax,64		; check for error...
	jb	ex3		; Got an error
ex2b:	jmp	NoErrorRet	; It worked, we're done

    ;
    ; Could not LoadModule, if reason is bad format, might be a Standard App
    ;	 .EXE, else file is bogus, return error in AX
    ;
ex3:	cmp	ax,exec_bad_format	; Is it bad Windows EXE format?
	jz	ex3a			; Yes, try standard app load
ex2c:	jmp	WinReturn		; Done

    ;
    ;  App is not a Windows app, Run an old application
    ;  First find its .PIF information
    ;
ex3a:	push	ss			; OpenFile zaps DS, reset
	pop	ds
	cCall	GetPifData,<PathName>
	or	ax,ax			; Found PIF file?
	jz	ex2b			; can't find pif file, just return OK
    ;
    ; PIF data is now set in DS, OR in parent PIF, child has both its own
    ;	and its parents badness
    ;
	mov	al,[ParentPif]		; child inherits badness
	or	[pifBehavior],al
    ;
    ; If inbuf is empty (no parametrs from PIF file), put a CR LF in it
    ;
	mov	bx,dataOffset InBuf
	cmp	byte ptr [bx],-1	; Empty?
	jnz	ex4			; No
	mov	word ptr [bx],0D00h
    ;
    ; If the app requests unavailable COM resource, we can't run it
    ;
ex4:	mov	al,[pifMsFlags]
	call	CheckCOM		; Check COM resource
	jnc	ex4c			; Ok
	jmp	ErrorRet		; Can't run app

    ;
    ;  Check for .PIF extension, if found alter program name field
    ;  If extension is PIF, alter Pathname to point to PIF program name
    ;
ex4c:	les	di,Pathname
	call	GetExtension
	les	di,Pathname
	cmp	ax,'p.'
	jnz	ex5			; Not .PIF
	cmp	dx,'fi'
	jnz	ex5			; Not .PIF
    ;
    ; EXEC name is .PIF, change name to program name in PIF file
    ;
	push	ss
	pop	ds
	mov	bx,dataOffset pifPathname
	cmp	byte ptr ds:[bx+1],":"	; drive specified in PIF?
	jz	ex4a			; yes, don't modify
	mov	ax,es:[di]
	cmp	ah,":"			; Drive specified in EXEC name?
	jnz	ex4a			; No
    ;
    ; PIF name doesn't include drive, but EXEC name does, tack specified drive
    ;	onto PIF name so we look on the right drive
    ;
	mov	bx,dataOffset pifDrive
	mov	ds:[bx],ax
ex4a:	mov	SEG_Pathname,ds
	mov	OFF_Pathname,bx
	les	di,Pathname		; reload pointer to file name
    ;
    ; If extension is .BAT, handle in special way
    ;
ex5:	call	GetExtension		; batch file?
	jz	RunBatchFile		; yes, process differently
	jmp	ex6			; not a batch file

    ;
    ; Run a batch file by running COMMAND /C batchname
    ;
RunBatchFile:
	cCall	GetPifData,<PathName>
	or	ax,ax			; Found PIF file?
        jnz     rbf1                    ; yes - use information
                                        ; can't find pif file, use default
        mov     ax,40                   ; DOS40: command.com needs 40K to run in
					; used to be 32K.  tonyg
	and	[pifBehavior],(255-fKeyboard)	; IGNORE Keyboard bit
        mov     [pifMinMem],ax
        mov     [pifMaxMem],ax
rbf1:
	sub	sp,128			; Need 128 byte buffer
	push	ss
	pop	es
	mov	di,sp			; ES:DI points at temp buffer
					;    for command line
	mov	ax,'/ '
	stosw
	mov	ax,' c'
	stosw
	lds	si,Pathname		; make it /c foo %1 %2 %3 etc...
	mov	ah,30H			; get dos version number
	int	21h
	cmp	al,3			; if DOS 3.x or above
	jae	ex4b			; full path spec OK
    ;
    ; We are on a pre 3.0 DOS, if path given we can't use it
    ;
	cmp	byte ptr [si+1],":"
	jnz	ex20a			; No drive
	lodsw				; copy over drive spec
	stosw
ex20a:	xchg	di,si			; di = si, save current di
	push	ds
	pop	es
	call	GetPureName
	xchg	si,di			; ds:si points past directory stuff
	push	ss
	pop	es
    ;
    ; Transfer over name of batch file into command line
    ;
ex4b:	lodsb
	stosb
	cmp	al,'.'
	jnz	ex4b
	mov	byte ptr es:[di-1],' '
    ;
    ; Fill the input buffer up with the command line
    ;  and make the EXEC command line point to it
    ;
	les	bx,ParameterBlock
	lds	si,es:[bx+Exec0_com_line]
	inc	si				; skip past count
	mov	word ptr es:[bx+Exec0_com_line],dataOffset InBuf
	mov	word ptr es:[bx+Exec0_com_line+2],ss
	push	ss
	pop	es
	mov	cx,sp
	add	cx,128
	sub	cx,di
	rep	movsb				; Tack EXEC com line onto
	mov	si,sp				;  /c batchname built so far
	mov	di,dataOffset InBuf+1		; Final com line goes here
	push	ss
	pop	ds
	mov	cx,128-1
	rep	movsb				; Transfer comline to Inbuf
	add	sp,128			; Done with temp buffer
	call	SetCount		; Set command line count
	call	GetComSpec		; Point at COMMAND.COM in environment
	mov	SEG_Pathname,ds 	; COMMAND.COM is EXEC name
	mov	OFF_Pathname,si
    ;
    ; EXEC of standard app
    ;
ex6:	cCall	AnsiUpper,<PathName>	; Upper case the name
	push	ss			; Might get here with DS blasted
	pop	ds
	mov	si,dataOffset pifInitialDir
	cmp	byte ptr [si],0 	; is there an initial directory?
	jz	ex6c			; no, proceed normally
    ;
    ; There is an initial directory in the PIF file.
    ; If EXEC name doesn't have directory, look for app in Initial directory
    ;
	les	di,PathName
	call	GetPureName
	push	es			; Save PathName pointer
	push	di

	push	ss
	pop	es
	lea	di,PathBuffer.opFile
	cmp	byte ptr [si+1],":"
	jz	ex6a			; Drive speced
	mov	ah,19H
	int	21h
	add	al,"A"
	mov	ah,":"
	stosw				; Save default drive
ex6a:	lodsb				; copy over EXEC directory
	stosb
	or	al,al
	jnz	ex6a
    ;
    ; If no trailing path sep, add one
    ;
	mov	ax,"/\"
	dec	di
	cmp	es:[di-1],al
	jz	ex6aa
	cmp	es:[di-1],ah
	jz	ex6aa
	stosb
ex6aa:	pop	si		; recover pathname
	pop	ds

ex6b:	lodsb			; Transfer over EXEC name
	stosb
	or	al,al
	jnz	ex6b
	push	ss
	pop	ds
	lea	dx,PathBuffer.opFile	; try to open the file
	mov	ax,3D00H
	int	21H
	jnc	ex8		; Open worked, we have found the file
				;   in the PIF initial directory
ex6c:	lea	si,PathBuffer
	regptr	sssi,ss,si
	xor	ax,ax
	test	Exec_func,80h	; If work has been done,
	jz	ex7		; no prompt on open
	mov	ax,OF_PROMPT+OF_CANCEL
ex7:	cCall	OpenFile,<Pathname,sssi,ax>

	cmp	ax,-1			; Is it present?
	jz	NotFound		; No, error
    ;
    ; Save the file handle of the executable
    ;
ex8:	MOV	Exec_fh,AX
	MOV	BX,AX			; MAKE SURE!!!!! when we get here
					;  from the jump before ex6c BX IS NOT SET
    ;
    ; See if trying to EXEC a device
    ;
	MOV	AX,(ioctl SHL 8)	; Get device information.
	INT	21h
	TEST	DL,devid_ISDEV		; Was the file a disk file?
	JZ	Exec_read_header	; Yes, we're ok
NotFound:
	MOV	AX,Exec_file_not_found	; Set not found error
	jmp	QuitReturn

    ;
    ; Have the executable open, see if .COM or .EXE
    ;
Exec_read_header:
	lds	si,ParameterBlock
	call	GetEnviromentSize	     ; Set EnvironParas for later
	mov	Exec_Load_Block,0
    ;
    ; We read in the program header into the above data area and determine
    ; where in this memory the image will be located.
    ;
	PUSH	SS
	POP	DS			     ; Point DS to the stack.
	MOV	CX,Exec_internal_buffer_size ; Header size.
	MOV	BX,Exec_fh		     ; Executable file handle.
	LEA	DX,Exec_signature

	MOV	AH,READ
	INT	21h
	jnc	xrh1			     ; OK
Exec_bad_filej2:
	jmp	Exec_bad_file		     ; File bad

xrh1:	or	ax,ax			     ; Did we read 0 bytes?
	jz	Exec_bad_filej2 	     ; Yes, file is 0 length, bad format
	CMP	AX,Exec_internal_buffer_size ; Did we read the executable header?
	JNZ	Exec_com_filej		     ; No, Treat it as a .COM file

	MOV	AX,Exec_signature
	CMP	AX,exe_valid_signature	     ; Is signature valid?
	JZ	Exec_save_start 	     ; assume com file if no signature.
	CMP	AX,exe_valid_old_signature   ; Old signature word?
	JZ	Exec_save_start 	     ; assume com file if no signature.
    ;
    ; File does not have correct .EXE signature
    ;
Exec_com_filej:
	xor	ax,ax
	mov	Exec_min_BSS,ax
	not	ax
	mov	Exec_max_BSS,ax
	JMP	Exec_com_file

    ;
    ; We have the program header ... determine memory requirements.
    ;
Exec_save_start:
	MOV	AX,Exec_pages		; Get 512-byte pages.
	MOV	CL,5			; Convert to paragraphs.
	SHL	AX,CL			;
	SUB	AX,Exec_par_dir 	; AX = size of code image in paragraphs.
	MOV	Exec_res_len_para,AX	; Save the code size.
    ;
    ; Do we need to allocate memory?  Yes if function is not load-overlay.
    ;
	TEST	Exec_func, Exec_func_overlay
;!! HP
	jnz	Exec_Save_1
	Jmp	Exec_allocate		; Need to allocate space.
;
; Get load address from block.
;
Exec_Save_1:
;*****
    ;
    ; Load a .EXE format overlay
    ;

    ;
    ; Get load address from block.
    ;
	LES	DI,ParameterBlock
	MOV	AX,ES:[DI].exec3_load_addr
	MOV	Exec_dma,AX
	MOV	AX,ES:[DI].exec3_reloc_fac
	MOV	Exec_rel_fac,AX
	JMP	Exec_find_res

Exec_no_mem:
	mov	al,Exec_not_enough_memory
	test	ss:[pifBehavior],fResident
	jz	exec_bomb
Exec_bad_file:
	MOV	AL,Exec_bad_format
Exec_bomb:
	assume	DS:NOTHING,ES:NOTHING
	xor	ah,ah
	PUSH	AX
	MOV	BX,Exec_fh
	MOV	AH,CLOSE
	INT	21h
	POP	AX
    ;
    ;  All errors return through this point
    ;
ErrorRet:
	push	ss
	pop	ds

assumes ds,data
	test	Exec_func,80h		; guy with work done?
	jz	err1			; no, no error showing
	call	ShowError		; show the error
QuitReturn:
	test	Exec_func,80h		; Guy with work done?
	jz	err1			; No, all ok
	mov	ss:[OldApState],SF_TERMINATED; Mark app as terminated
err1:	jmp	WinReturn

;!! HP
;			 
Exec_ext:
	test 	Exec_Func, 80H		; Top level guy?
	jz 	Exec_no_mem 		; cannot swap Windows for a child
	mov 	AL, [pifBehavior]
	test	AL, BADBITS		; Only attempt to shrink if BAD apps
	jz	Exec_no_mem
	test	AL, fResident		; Cannot shrink Windows for a resident
	jnz	Exec_no_mem
	xor	AX, AX		
 	mov	AL, CS:[SwapName]

;  Cannot shrink Windows if swap drive is removable

	sub	AL, 'A'			; AX= media device (Drive A = 0)
	mov	BX, 1			; Inquire drive status command
	ccall	InquireSystem, <BX, AX>
	push	SS
	pop	DS
	cmp	AX, 3			; Is it a fixed drive
	je	Exec_ext1		; Yes
	cmp	AX, 1			; Is it mapped to a fixed drive?
	mov	AL, Msg_No_floppy	; Assume error: cannot swap to floppy
	jne	Exec_Bomb
	or	DX, DX			  
	jz	Exec_Bomb		; No, show error

Exec_ext1:
	mov	BX, Exec_fh		; Close the DOS .EXE file
	mov	AH, CLOSE
	int 	21H
 
	mov	DI, SS
	lea	SI, PathBuffer.opfile	; DI:SI -> Exec pathname (i.e. Pathbuffer)
	LES	BX, ParameterBlock
	
	regptr	lpPathN, DI, SI
	regptr	lpParmB, ES, BX
	ccall	DOSExec, <lpPathN, lpParmB> ; Try to shrink Windows and exec
	jnc	Exec_ext2
	push	AX			; CY set: Bomb !!!; Error code in AX
	push	DS
	mov	[fIcon], 1		; We are in Icon mode
	ccall	UpdateWindow, <[hTTYwnd]>
	pop	DS
	pop	AX			; Get back error code
	jmp	ErrorRet

Exec_ext2:		
	jmp	NoErrorRet
;******

Exec_allocate:
	call	AllocateMemory
;!! HP
	jnz	Exec_alloc_ok
        jmp     Exec_ext   		; try to swap Windows using the extensions

Exec_alloc_ok:
	cmp	bx,exec_res_len_para	; enough memory to load image
	jae	Exec_alloc1
	jmp	Exec_no_mem

Exec_alloc1:
;*****
	MOV	Exec_rel_fac,AX 	; new segment
    ;
    ; Determine the location in the file of the beginning of the resident.
    ;
Exec_find_res:
	MOV	DX,Exec_par_dir
	PUSH	DX
	MOV	CL,4
	SHL	DX,CL			; low word of location
	POP	AX
	MOV	CL,12
	SHR	AX,CL			; high word of location
	MOV	CX,AX			; CX <- high
    ;
    ; Read in the resident image (first, seek to it).
    ;
	MOV	BX,Exec_fh
	MOV	AX,(LSEEK SHL 8) + 0
	INT	21h
Exec_big_read:				; Read resident into memory.
	MOV	BX,Exec_res_len_para
	CMP	BX,1000h		; Too many bytes to read?
	JB	Exec_read_ok
	MOV	BX,0FE0h		; Get a chunk of FE00 bytes.
Exec_read_ok:
	SUB	Exec_res_len_para,BX	; Compute remaining size of exec.
	PUSH	BX
	MOV	CL,4
	SHL	BX,CL			; Get count in bytes from paras.
	MOV	CX,BX			; Count in correct register.
	MOV	BX,Exec_fh		; Handle in correct register.
	MOV	DS,Exec_dma		; Set up read buffer.
    assume  DS:NOTHING
	XOR	DX,DX
	PUSH	CX			; Save our count.

	MOV	AH,READ
	INT	21h

	POP	CX			; Fetch old count to verify.

	CMP	CX,AX			; Did we read enough?
	POP	BX			; Get paragraph count back.
	JNZ	Exec_do_reloc		; Do reloc if no more to read.
    ;
    ; We've read in CX bytes... bump DTA location
    ;
	ADD	Exec_dma,BX		; Bump dma address.
	CMP	Exec_res_len_para,0
	JNZ	Exec_big_read
    ;
    ; The image has now been read in.  We must perform relocation to
    ; the current location.
    ;
Exec_do_reloc:
	MOV	CX,Exec_rel_fac
	MOV	AX,Exec_SS		; Get initial SS.
	ADD	AX,CX			; Relocate.
	MOV	Exec_init_SS,AX

	MOV	AX,Exec_SP		; Initial SP.
	MOV	Exec_init_SP,AX

	LES	AX,DWORD PTR ( Exec_IP )
	MOV	Exec_init_IP,AX
	MOV	AX,ES
	ADD	AX,CX			; Relocated.
	MOV	Exec_init_CS,AX

	XOR	CX,CX
	MOV	DX,Exec_rle_table
	MOV	BX,Exec_fh

	MOV	AX,(LSEEK SHL 8) + 0
	INT	21h

	JNC	Exec_get_entries
Exec_bad_filej:
	JMP	Exec_bad_file

Exec_get_entries:
	MOV	DX,Exec_rle_count     ; Number of entries left.

Exec_read_reloc:
	assume	DS:NOTHING
	PUSH	DX

	LEA	DX,Exec_signature
	MOV	CX,((Exec_internal_buffer_size)/4)*4
	MOV	BX,Exec_fh
	push	ss
	pop	ds

	MOV	AH,READ
	INT	21h

	POP	DX
	JC	Exec_bad_filej
	MOV	CX,(Exec_internal_buffer_size)/4
	LEA	DI,Exec_signature		   ; Pointer to byte location in header.

    ;
    ; Relocate a single address.
    ;
	MOV	SI,Exec_rel_fac

Exec_reloc_one:
	OR	DX,DX			; Any more entries?
	JNE	Exec_get_addr
	JMP	Exec_set_PDB

Exec_get_addr:
	LDS	BX,DWORD PTR SS:[DI]	; Get ra/sa of entry
	MOV	AX,DS			; Relocate address of item
	ADD	AX,SI
	MOV	DS,AX
	MOV	AX,WORD PTR DS:[BX]	; Relocate item
	ADD	AX,SI
	MOV	WORD PTR DS:[BX],AX
	ADD	DI,4
	DEC	DX
	LOOP	Exec_reloc_one		    ; End of internal buffer?

    ;
    ; We've exhausted a single buffer's worth.	Read in the next piece
    ; of the relocation table.
    ;

	JMP	Exec_read_reloc

    ;
    ; We have a .COM file.  First, determine if we are merely loading an overlay.
    ;
Exec_com_file:
	TEST	Exec_func, Exec_func_overlay
	JZ	Exec_alloc_com_file
   ;
   ; Load a .COM format overlay
   ;
	LDS	SI,ParameterBlock	; get arg block
	LODSW				; get load address
	MOV	Exec_dma,AX
	JMP	SHORT Exec_64k		; read it all!

;
; We must allocate the max possible size block (ick!)  and set up
; CS=DS=ES=SS=PDB pointer, IP=100, SP=max size of block.
;
Exec_alloc_com_file:
	call	AllocateMemory
	jnz	eacf1
	jmp	Exec_ext		;!! HP

eacf1:	SUB	BX,10h			; Remember header.

Exec_check_64k:
	CMP	BX,1000h		; 64k or more?
	JAE	Exec_64k		; Yes, read only 64k.
	MOV	AX,BX			; Convert size to bytes.
	MOV	CL,4
	SHL	AX,CL
	JMP	SHORT Exec_read_com

Exec_64k:
	MOV	AX,0FFFFh		; 64k-1 bytes

Exec_read_com:
	PUSH	AX			; Save number to read.
	MOV	BX,Exec_fh		; File handle of com file.
	XOR	CX,CX			; Seek to 0:0.
	MOV	DX,CX

	MOV	AX,(LSEEK SHL 8) + 0
	INT	21h

	MOV	BX,Exec_fh
	POP	CX			; Number to read.
	MOV	DS,Exec_dma
	XOR	DX,DX
	PUSH	CX

	MOV	AH,READ
	INT	21h

	POP	SI			; Get number of bytes to read.
	CMP	AX,SI			; Did we read them all?

	JNZ	Exec_skip		; Exactly the wrong number... no memory.
	JMP	Exec_no_mem

Exec_skip:
	TEST	Exec_func, Exec_func_overlay
	JNZ	Exec_set_PDB		; No start.
	MOV	AX,Exec_DMA
	SUB	AX,10h
	MOV	Exec_init_CS,AX
	MOV	Exec_init_IP,100h     ; Initial IP is 100.
	; SI is at most FFFFh
	DEC	SI			; make room for stack
	; SI is at most FFFEh, room for a 0!
	MOV	Exec_init_SP,SI       ; max value for read is also SP!
	MOV	Exec_init_SS,AX
	MOV	DS,AX
	MOV	WORD PTR DS:[SI],0	; 0 for return

Exec_set_PDB:
	MOV	BX,Exec_fh	       ; we are finished with the file.
	MOV	AH,CLOSE
	INT	21h

	TEST	Exec_func, Exec_func_overlay
	JZ	Exec_build_header
	JMP	NoErrorRet			; overlay load -> done

Exec_build_header:
    ;
    ; Build the PDB
    ;
	mov	ds,Exec_Load_Block		; get dest PDB
	mov	es,Exec_PDB
	mov	es:[PDB_NextPDB],ds		; remember child
	cCall	BuildPDB,<es,ds,ParameterBlock,Exec_size>
    ;
    ; Set up GlobalHeap pointer set from prev AllocateMemory call
    ;
	push	ax				; save default AX
	mov	ax,word ptr (Exec_GlobalHeap)
	mov	word ptr ds:[PDB_GlobalHeap],ax
	mov	ax,word ptr (Exec_GlobalHeap+2)
	mov	word ptr ds:[PDB_GlobalHeap+2],ax
    ;
    ; If COMMAND.COM use shared Windows environment, else use private copy
    ;
	call	IsCommandCom			; is it command.com?
	jz	exbh0				; yes, use global enviroment
	mov	ax,Exec_Environ
	mov	ds:[PDB_environ],ax
    ;
    ; Set PDB partition pointer
    ;
exbh0:	mov	ax,ss:[Partition]
	mov	word ptr ds:[PDB_Partition],ax
    ;
    ; Set exit address inside this code
    ;
	mov	ax,codeOffset Come_Back_Here
	mov	word ptr ds:[PDB_Exit],ax	; Process exit address.
	mov	word ptr ds:[PDB_Exit+2],cs	;
	xor	dx,dx
	mov	es,dx
	mov	es:[4*22h],ax	; Save them where we can get them later.
	mov	es:[4*22h+2],cs ; When the child exits.
    ;
    ; DMA transfer addr is PDB:80
    ;
	mov	dx,80h
	mov	ah,SET_DMA
	int	21h
    ;
    ; Copy environment into final location
    ;
	mov	es,Exec_Environ
	xor	di,di
	lds	si,ParameterBlock
	mov	ds,[Exec0_environ+si]
	xor	si,si
exbh1:	lodsb				; copy over enviroment
	stosb
	mov	ah,ds:[si]
	or	ax,ax			; until double null
	jnz	exbh1
	movsb
    ;
    ; Set argc and argv[0]
    ;
	mov	ax,1			; set argument count to 1
	stosw
	lea	si,PathBuffer.opFile
	push	ss
	pop	ds
exbh2:	lodsb
	stosb
	or	al,al
	jnz	exbh2
exbh3:	pop	ax		; restore default AX

	TEST	Exec_func, Exec_func_no_execute
	JZ	Exec_go
    ;
    ; EXEC don't start up app, set return values
    ;
	LDS	SI,DWORD PTR ( Exec_init_SP )	; get stack
	LES	DI,ParameterBlock		; and block for return
	MOV	ES:[DI].exec1_SS,DS		; return SS

	DEC	SI				; 'push' default AX
	DEC	SI
	MOV	DS:[SI],BX			; save default AX reg
	MOV	ES:[DI].exec1_SP,SI		; return 'SP'

	LDS	AX,DWORD PTR ( Exec_init_IP )
	MOV	ES:[DI].exec1_CS,DS		; initial entry stuff

	MOV	ES:[DI].exec1_IP,AX
	JMP	NoErrorRet

    ;
    ; Start the old app up
    ;
Exec_go:
	push	ss
	pop	ds
assumes ds,data
	push	ax		; save task intial AX

	xor	CX, CX		;!! HP:	We are not in the extended mode
        call    GetVars         ; set up ax with Behavior and Exec_func
        call    BeginExecution	; set up WINOLDAP execution environment

;!!  HP
;
;  Save the original HP state before the application is started
 
	push 	AX			; Save needed regs
	push	DI						 
	push	DS
	push	ES

	test	Exec_Func, 80H		; Only save if Top level
	jz 	NotSave
	xor	DI, DI
	push	SS
	pop	DS			; DS= Winoldap Data segment

;
;  Assume DS= SS= Winoldap Data segment
;
	mov	AX, [HPSeg]		; Get pointer to saved HP area
	or	AX, AX			; Do not save if it is null
	jz	NotSave
	mov	ES, AX			; ES= Original HPSystem saved area
	regptr	ESDI, ES, DI
	ccall	SaveHPSystem, <ESDI>

NotSave:
	pop	ES			; Restore regs  
	pop	DS
	pop	DI
	pop	AX
;******

	mov	bx,Exec_Load_Block
	mov	es,bx
	mov	ah,50h		; set current pdb
	int	21h
    ;
    ; The next two operations are SLEAZY!!!!! What we do is shrink
    ;  the block allocated to the app down to just the environment, then
    ;  ALLOC the space occupied by the app and its PDB. This creates the
    ;  correct environment (envir and app in DIFFERENT allocation blocks).
    ;  There are all kinds of very subtle alignment tricks going on.
    ;
	push	bx
	mov	es,Exec_Environ 	; shrink just past
	mov	bx,EnvironParas 	; the enviroment
	add	BX, MyAppCount
	dec	bx
	mov	ah,4aH
	int	21h

	mov	bx,ss:[pifMaxMem]
	cmp	bx,-1
	jnz	cn2
	mov	bx,Exec_size
cn2:	mov	ah,48H
	int	21H
	pop	bx

	push	bx
	cCall	GetCurrentTask	; get current task in AX
	mov	di,Exec_Init_SP
	mov	cx,Exec_Init_SS
	mov	si,Exec_Init_CS
	mov	dx,Exec_Init_IP
	mov	ds,ax				; point to current task
	pop	bx				; recover PDB
	pop	ax				; recover initial AX
;
;  Simulate a far frame
;
	push	cs				; simulte far return address
	push	dx
	inc	bp
	push	bp
	push	ss
	sub	sp,TASKDS
	mov	word ptr ds:[TDB_TaskSP],sp

	mov	ds,bx
	push	ax			; save inital AX
	mov	ax,3524H		; get fatal exit vector
	int	21H
	mov	word ptr ds:[PDB_Fatal_Abort+2],es
	mov	word ptr ds:[PDB_Fatal_Abort  ],bx
	pop	ax			; restore initial AX
	mov	es,ds:[PDB_Parent_PID]

	cli
	mov	word ptr es:[PDB_User_Stack+2],ss
	mov	word ptr es:[PDB_User_Stack  ],sp
	sub	word ptr es:[PDB_User_Stack  ],12*2

	mov	word ptr ds:[PDB_Entry_Stack+2],ss
	mov	word ptr ds:[PDB_Entry_Stack  ],sp
	mov	ss,cx
	mov	sp,di
	sti

	push	ds
	pop	es		; es = ds = PDB

	push	si		; Establish return address to target.
	push	dx
	xor	bx,bx		; believe or not, WORD 2.0 requires this!
xxx	proc	far
	ret			; Long return to target procedure.
xxx	endp

    ;
    ; Old apps come here when they exit
    ;
Come_back_here:
	lds	dx,cs:[Int3Fvector]	; make sure swap vector is still around
	mov	ax,253FH
	int	21H

	mov	AH, 51h			; Get what DOS thinks the current PDB
	pushf
	call	CS:[OrgInt21Proc]

	mov	es,bx		; clue DOS INT 21 listeners that the PDB is different
	mov	ah,50H
	int	21H
	mov	es,es:[PDB_NextPDB]	; get child that just exited
	cli
	mov	ss,word ptr es:[PDB_Entry_Stack+2]
	mov	sp,word ptr es:[PDB_Entry_Stack]
	sti
	add	sp,TASKDS
	pop	ds
	pop	bp
	dec	bp
	add	sp,4			; remove fake return address

assumes ds,data

;!!  HP
;
;  Only restore the original state of HP system if the exitting task is
;  the top level one.

	test	Exec_Func, 80H		; Top Level ?
	jz	NotTop

;  Save the return code of the top level app

	push	SS
	pop	DS			; DS= Winoldap Data segment
	mov	ah, 4dh			;  Get the return code of the app
	int	21h			;  DOS Wait function
	mov	wReturnCode, ax		; 

;  Restore the state of the original HP-SYSTEMS if we have a Vectra PC

	xor	DI, DI						     
	mov	AX, [HPSeg]		; Get pointer to saved HP area
	or	AX, AX
	jz	NotTop
	mov	ES, AX
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, < ESDI>

NotTop:
	test	[pifMsFlags], fCOM1 +fCOM2 ; Is it a COMM app?
	jz	exeHP			; Nop
	push	ES			; Save Exiting PDB
	mov	ES, [Partition]	
	cmp	ES:[prtfSwap], 0
	pop	ES			; Restore exiting PDB
	jnz	exeHP
	dec	CS:[COMMcount]		; One less app using COMM without
					; notification
;*****	
exeHP:
	push	ax
	mov	ax, 1
	cCall	A20Proc,<ax>		; Reenable A20
	pop	ax

	test	Exec_Func, 80H		; Top Level ?
	jz	cbNoTerminate		;  
	call	ShowTerminate		;   Yes, enter inactive mode

cbNoTerminate:
	call	FreePartition		; ES = pdb of exitting task
	mov	ax,ParentPartition
	mov	[Partition],ax		; restore to parent
	xor	CX, CX			;!! HP:	We are not in extended mode
	call	GetVars 		; set up ax with Behavior and Exec_func
	call	EndExecution
NoErrorRet:
	mov	ax,-1			; > 64 return code means no error
WinReturn:
	push	ss
	pop	ds
	push	ax
	mov	al,[ParentPif]
	mov	ah, al
	xchg	[pifBehavior],al	; restore current pif behavior
	test	Exec_func, 80H		; Top level?
	jnz	WinRet1			; Skip if yes
	test	AL, BADBITS		; Are we returning from a BAD app?
	jz	WinRet1			; Skip if not 
	test	AH, BADBITS		; Is Parent BAD?
	jnz	WinRet1			; Skip if Yes
;
; We have a GOOD Parent, so hook back interrupts for GOOD apps
;
	call	HookGood
	mov	BX, [hTTYWnd]
	mov	AX, SHOW_OPENWINDOW 
	ccall	ShowWindow,<BX,AX>
;
WinRet1:
	mov	al,ParentPif2
	mov	[pifMsFlags],al
	mov	ax,ParentPartition
	mov	[Partition],ax
	mov	bx,Exec_PDB		; restore PDB to callers PDB
	mov	ah,50H
	int	21h
	mov	ax,dataOffset pifTitle
	mov	bx,PTitleLen
	regptr	dsax,ds,ax
	cCall	GetWindowText,<hTTYwnd,dsax,bx>
	pop	ax
cEnd

;**
;
; IsCommandCom - Is the program at PathName COMMAND.COM
;
; ENTRY:
;	PathName contains name of program to check
; EXIT:
;	Zero set
;	    File at PathName is COMMAND.COM
;	Zero Re-set
;	    File at PathName is NOT COMMAND.COM
; USES:
;	ALL but DS,SS,BP
;
IsCommandCom:
	push	ds
	les	di,Pathname
	call	GetPureName
	push	cs
	pop	ds
	mov	si,codeOffset szCommand
	mov	cx,7+1+3
	rep	cmpsb
	pop	ds
	ret

;**
;
; GetExtension - Return pointer to file extension
;
; ENTRY:
;	ES:DI -> Nul terminated pathname
; EXIT:
;	DH is last character of string			LOWER CASED
;	DL is next to last character of string		LOWER CASED
;	AH is next to next to last character of string	LOWER CASED
;	AL is next to next to next to last character of string
;	DI points to char after NUL at end of string
;	DIRECTION FLAG CLEARED
;	Zero Set
;	    last 4 characters of string are .BAT
;	Zero re-set
;	    last 4 characters of string are NOT .BAT
; USES:
;	AX,CX,DX,DI,FLAGS
;
GetExtension:
	cld
	mov	cx,-1
	xor	al,al
	repnz	scasb			; scan to end of string
	mov	ax,es:[di-5]
	or	ah,20h
	mov	dx,es:[di-3]		; complete check for .COM
	or	dx,2020h		; convert to lower case

	cmp	ax,'b.'
	jnz	gex1
	cmp	dx,'ta'
gex1:	ret

;**
;
; AllocateMemory - Allocate Windows global memory for old app
;
;    This routine is called to allocate the memory needed to run
;	an old app (except for the old app DS). It allocates
;	memory consistent with the PIF memory requirements.
;
; ENTRY:
;	SS = Data of app memory is being allocated to
;	Exec_Func set
; EXIT:
;	Grabber loaded if PIF bits indicate it's needed
;	DS = Input SS
;	Zero Set
;	    Error, no memory available
;	    BX = 0
;	Zero re-set
;	    AX is address of block
;	    BX is its size in paragraphs (does not include environment)
;	    Exec_Globalheap set
;	    Exec_Environ set
;	    Exec_load_block set
;	    Exec_dma set
;	    Exec_Size set
;	    CHDIR to PIF file initial directory if indicated
;	NOTE: In the Event that this is an fResident app, or there is not
;	    enough memory to run the app under Windows, THIS ROUTINE CONTAINS
;	    THE CODE WHICH TRIES TO RUN THE APP BY EXITING WINDOWS. If this
;	    method of running the app is attempted and is successful, this
;	    routine NEVER returns.
; USES:
;	ALL but SS,BP
;
AllocateMemory:
	push	ss
	pop	ds			; Make sure DS -> old app data
assumes ds,data

	mov	al,[pifBehavior]
	test	al,BADBITS		; are we running a bad app?
	jz	gm0			; no, so no screen grabber
	test	al,fResident		; load and stay resident?
	jz	gm000			; no, continue bad app allocate
	jmp	CheckMemory		; yes, no memory/special checking

    ;
    ; Wish to run a BAD app
    ;
gm000:	call	LoadGrab		; try to load screen grabber
	jnc	gm00			; grabber in
	test	Exec_func,80H		; top level app?
	jnz	gmaa			; yes, return error
	xor	bx,bx			; indicate not enough memory
	ret				; and return

    ;
    ; Here if BAD app with work done (top level old app) and can't
    ;	 load the grabber for it
    ;
gmaa:	mov	ax,msgNeed		; Can't find WINOLDAP files error
    ;
    ; Here if error, AX is string ID of error message to print
    ;
ShowExit:
	call	ShowMessage		; AX is preserved
	jmp	DestroyExit

    ;
    ; Here for BAD app, grabber loaded
    ;
gm00:	call	GetVars 		; Set input to GetBufferSizes
	call	GetBufferSizes		; set the partition buffer sizes
    ;
    ; Here for good app or bad app main allocation
    ;
gm0:	call	SetDirectory		; Set initial directory up if indicated
	call	GetFreeMem		; Get available mem in BX
	mov	si,bx			; SI = number of paragraphs available
    ;
    ; Compute MIN allocation
    ;
	mov	cl,6
	mov	dx,[pifMinMem]
	shl	dx,cl			; set dx = min paras needed
	mov	bx,Exec_min_BSS
	or	bx,bx			; Any min in .EXE file?
	jz	gmc			; no
	add	bx,Exec_res_len_para	; BX is min para according to .EXE
    ;
    ; Resolve any difference between PIF min mem and .EXE min mem
    ;
	cmp	dx,bx
	jae	gmc			; Min in PIF >= Min in .EXE
	mov	dx,bx			; Oops, min is .EXE min
    ;
    ; Compute MAX allocation
    ;
;!! HP
;
gmc:    
	push 	DX			; Save min size required
	add	DX, EnvironParas
	add	DX, 10h			; Add on environment and PDB size
	mov	[PifMinMem1], DX	; init to be use by DOSExec in extended mode
	pop	DX
;******
    	mov	ax,[pifMaxMem]
;
;~~tqn 051587
;
; If Max desired size is -1, and we have a top level BAD app, run it in
; Extended mode.
;
	inc 	AX
	or	AX, AX			; Max = -1?
	jne	Allocm1			; No, keep on 
	test	[PifBehavior], BADBITS	; BAD app?
	jz	Allocm1			; No, go on
	test	Exec_Func, 80H		; Is it Top level?
	jz	Allocm1			; No, next
	jmp	vbax			; Ret out of mem to switch to Extended mode

Allocm1:
	dec	AX			; readjust Max
;~~
	shl	ax,cl			; set ax = max paras requested
    	mov	[pifMaxMem1], AX	; Needed for Extended mode
	mov	bx,Exec_max_BSS
	mov	[pifMaxMem],bx		; save for later realloc
	cmp	bx,-1			; Any .EXE MAX specified?
	jz	gmd			; no
	add	bx,Exec_res_len_para	; MAX from .EXE file
	mov	[pifMaxMem],bx		; save for later realloc
    ;
    ; Resolve any difference between PIF max mem and .EXE max mem
    ;
	cmp	bx,ax
	jbe	gmd			; PIF MAX >= .EXE MAX
	mov	ax,bx			; oops, .EXE MAX is bigger
    ;
    ; See if there is enough mem for app
    ;	DX is MIN paras needed FOR APP ITSELF
    ;	AX is MAX paras requested FOR APP ITSELF
    ;
gmd:	mov	bx,18H           	;!! HP: fudge factor for PDB, arena, etc.
	add	BX, [SwapArea]
	add	bx,EnvironParas 	; add in size of enviroment
	add	bx,[BufPara]		; add in Screen Grab buffer
	add	bx,[SwitchPara] 	; add in Switch buffer
;!! HP
	add	BX, [HPPara]		; add in HPSystem buffer
	add	BX, [HPPara]		; 2 buffers per application
;*****
	add	bx,[MousePara]		; add in mouse buffer
    ;
    ; BX is now paras for WINOLDAP overhead
    ;
	add	dx,bx			; DX = min required
	add	bx,ax			; BX = max desired
    ;
    ;	DX is MIN paras needed TOTAL
    ;	BX is MAX paras requested TOTAL
    ;	AX is MAX paras requested FOR APP ITSELF
    ;	SI is available paras
    ;
	or	ax,ax			; is max size specified?
	jnz	gme			; Yes
	mov	bx,si			; no, max = all of available memory
gme:	cmp	bx,dx			; make sure max >= min
	jae	gm1			; OK
	mov	bx,dx			; inconsistent, MAX = MIN
    ;
    ;	DX is MIN paras needed TOTAL
    ;	BX is MAX paras requested TOTAL
    ;	SI is available paras
    ;
gm1:	mov	[pifMinMem],dx
    ;
    ; If the desired size is < one half of available mem (excluding fudge factor)
    ;	then go ahead, otherwise try to find a partition to share.
    ;
	shr	si,1		; si = half of available memory
	add	si,(SwapArea)	;!! HP: si = half of mem, not including swaparea
	cmp	bx,si		; does maximum use more than half of memory?
	jb	gm1b		; no, just continue
    ;
    ; App desired indicates we should partition swap if possible
    ;
	push	bx		; save desired size
	mov	di,dx		; Looking for this size partition
	sub	di,(SwapArea)	;!! HP: BUT partition does NOT include fudge factor

	mov	AL, CS:[EEMSBlock].fEEMS ; EEMS active if bit 0 set
	cmp	ParentPartition, 0 ; Child?
	jz	gmCont		; Nop, go on
	or 	AL, 80h         ; Flag we have a child

gmCont:
	ccall	FindPartition, <AX> ; Try to find a partition to share
	pop	bx		; restore desired size
	jnc	gm4		; got it!!
    ;
    ; If the app is not swappable, try to alloc, else adjust desired size to
    ;	SwapSize set in WIN.INI if appropriate.
    ;
gm1b:	call	IsSwappable	; is the app swappable?
	jcxz	gm1c		; no, it isn't
	cmp	dx,[pifMinMem]	; is swapsize from WIN.INI less than des size?
	jb	gm1c		; Yes must use it
	mov	bx,dx		; desired size up to = swapsize
    ;
    ; Try to allocate desired size partition
    ;
gm1c:	call	MyAllocate 	; ask for maximum memory
	jnc	gm3		; WORKED.
    ;
    ; desired failed, try MIN
    ;
	cmp	bx,[pifMinMem]	; Any hope for min?
	jae	gm2
;!!HP
;
;  Don't kick Windows out to try to run BAD app in UGLY mode
;
	jmp	vbax
;       jmp     CheckMemory     ; see if there is enough
gm2:    call    MyAllocate
	jnc	gm3
	jmp	vbax
;       jmp     CheckMemory     ; something weird happened...
;*****
    ;
    ; Just allocated a new partition for app. If app is NOT switchable
    ;	because there are no keyboard hooks, or no switch is indicated
    ;	we're done, else reduce the size of the area by the fudge factor
    ;	which we really don't use.
    ;
gm3:	test	[pifBehavior],fKeyboard+fNoSwitch	; Switchable?
	jnz	gm4					; no
;!! HP
        mov     cx, ax			; Save handle
	push	BX
	push	DX
	mov	BX, 16		; Convert SwapArea size in bytes (DX:AX)
	mov	AX, SwapArea
	mul	BX  
        sub     si, AX
        sbb     di, DX
	mov	AX, CX
	push	AX			; Save handle
gm3a:   mov     cx,GMEM_FIXED+GMEM_ZEROINIT
        regptr  xsize,di,si
        cCall   GlobalRealloc,<ax,xsize,cx>
	pop	AX			; Restore handle
	pop	DX
	pop	BX
;*****

    ;
    ; Init the partition and heap
    ;
gm4:	mov	bx,ParentPartition
	call	InitPartition
	call	InitHeap		; AX is now addr of start of envir
	mov	word ptr (Exec_GlobalHeap),bx
	mov	word ptr (Exec_GlobalHeap+2),ds
    ;
    ; Set environment pointers
    ;
	dec	ax			; Point to arena of block
	mov	es,ax
	inc	ax
	mov	Exec_Environ,ax 	; save segment of enviroment
	add	ax,EnvironParas 	; add in size of enviroment
;
; The next 3 instruction has a long history: (bug fix for 1.04)
;  When Winoldap has two old apps sharing the same partition, they CANNOT 
;  have the same PSPs. The reason is as far as DOS is concerned, these
;  oldapps are the SAME TASK. If they have the same PSPs, The DOS Abort code
;  would blindly close all opened file handles in both apps. Closing one
;  app and going back to the other might result in LOSS of data files.
;
; * The Fix is old apps would have slightly different environment sizes.
;   This is to force an unique PSP for each oldapp. 

	mov	CX, CS:[AppCount]	; 1 more oldapp to run
	inc	CX			; Update count (always EVEN)
	inc	CX
	add	AX, CX
 	mov	es:[ga_owner],ax	; mark as owned by this PDB
	mov	Exec_load_block,ax	; PDB of app is here
	add	ax,10h			; Increment for header.
	mov	Exec_dma,ax		; Code of app is here

	mov	bx,es:[ga_size]
	sub	bx,EnvironParas 	; decrease by enviroment size
	sub	BX, CX
	mov	CS:[AppCount], CX
	mov	MyAppCount, CX		; Maintain a local copy for Load
	mov	Exec_size,bx		; This is size of block
	or	bx,bx			; Set zero correctly
	ret

    ;					  
    ; Here if not enough memory to run app under Windows, or app is fResident
    ; In either case we will attempt to run the app by exiting Windows.
    ;
CheckMemory:		
    ;
    ; Is this app a top level guy? If not, can't run it.
    ;
	test	Exec_Func,80h		; guy with work done?
	jz	vbax			; no, so no memory...
    ;
    ; Is this the very first old app? If it isn't, can't run it.
    ;
	mov	al,cs:[GoodCount]	; any good apps running?
	or	al,cs:[BadDepth]	; or bad apps running?
	or	al,cs:[IconCount]	; or Iconic apps running?
	jz	vba0			; no, this is VERY FIRST old app
vbax:	xor	bx,bx			; No memory
	ret

    ;
    ; Check to make sure WIN.COM stub is around to run very bad apps
    ;
vba0:	cCall	IsScreenGrab	; has windows been booted via WIN.COM?
	or	ax,ax
	jz	vbax		; no it hasn't, no memory
    ;
    ; ONLY apps around can be MSDOS and WINOLDAP. If more, can't run app
    ;
	cCall	GetCurrentTask
	xor	ax,ax		; zero tasks counted
vba1:	mov	es,dx		; point to HeadTDB
	inc	ax		; one more task found
	mov	dx,es:[TDB_Next]
	or	dx,dx
	jnz	vba1		; Another task
	cmp	al,2		; more than two task running?  (WINOLDAP + MSDOS)
	ja	vbax		; yes, can't run app
	cmp	[ShowParm],SW_SHOWMINIMIZED ; Is app supposed to start ICONIC?
	jz	vbax		; yes, can't run app
	cmp	[ShowParm],SW_SHOWMINNOACTIVE ; Is app supposed to start ICONIC?
	jz	vbax		; yes, can't run app

    ;
    ; Need to have the grabber to run app
    ;
	call	LoadGrab		; make sure we have a screen grabber
	jnc	vba1a			; Grabber in
	test	Exec_func,80H		; top level app?
	jz	vbax			; no, return no memory
    ;
    ; Print grabber error message
    ;
	mov	ax,msgNeed
	jmp	ShowExit

    ;
    ;  Make sure the disk with the app to run is in and store its name in OutBuf
    ;
vba1a:	mov	ax,OF_PROMPT+OF_CANCEL+OF_EXIST
	mov	si,dataOffset OutBuf
	lea	dx, PathBuffer.opFile
	regptr	ssdx,ds,dx
	cCall	OpenFile,<ssdx,sssi,ax>
	inc	ax		; Did it work?
	jnz	vba1b		; yes
	pop	ax		; get rid of return address
	jmp	NoErrorRet	; just forget it.

    ;
    ; We now stuff all of the app start info down to the
    ; WIN.COM stub
    ;
vba1b:	cCall	GetCurrentPDB
	mov	es,dx			; point at TopPDB
	mov	es,es:[PDB_Parent_PID]	; point at parent PDB (PDB of WIN.COM)
	mov	di,word ptr es:[100H]	; Address of where to stuff prog parms
    ;
    ; Set location of grabber down in WIN.COM stub
    ;
	mov	ax,cs:[GrabSeg]
	stosw
	mov	ax,cs:[GrabSize]
	stosw
    ;
    ; Store PIF info dwon in WIN.COM stub
    ;
	mov	ax,[pifSysMem]
	stosw				; stuff pif info
	mov	al,[pifMsFlags] 	; stuff the microsoft flags
	stosb
    ;
    ; Store name of program set by call to openfile in WIN.COM stub
    ;
	push	ss
	pop	ds
	mov	si,dataOffset OutBuf+8	; stuff in pathname
	mov	cx,64
	rep	movsb
    ;
    ; Store app command line down in WIN.COM stub
    ;
	lds	si,ParameterBlock	; get pointer to command line
	lds	si,[Exec0_com_line+si]
	mov	cx,128
	mov	di,cx			; blast in command line
	rep	movsb
    ;
    ; Run the old app by exiting Windows
    ;
vba2:	mov	[OldApState],SF_TERMINATED; This app is "Terminated"
	call	SetDirectory		; Set the PIF initial directory
	xor	ax,ax
	cCall	ExitWindows,<ax,ax>	; Turn off Windows and run app
	jmp	vbax			; NOTE: If all works correctly
					;   WE DO NOT GET HERE!!!!!!
					;   We execute this instruction ONLY if
					;   ExitWindows fails for some reason

;**
;
; GetEnviromentSize - Return size of app DOS environment
;
; ENTRY:
;	SS -> Data of app to set environment for
;	DS:SI -> EXEC parameters
;	PathBuffer.opFile contains program name (for argv[0])
; EXIT:
;	DS:[SI+Exec0_environ] points to environment to copy for EXEC
;	EnvironParas = Size of this environment including argc, argv and arena
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
GetEnviromentSize:
	mov	bx,ds:[si+Exec0_environ]	; Get EXECor environment pointer
	or	bx,bx				; was enviroment specified?
	jnz	ges1				; yes, continue
	mov	es,Exec_PDB			; point to parent PDB
	mov	bx,es:[PDB_environ]		; get PDB enviroment
	mov	ds:[si+Exec0_environ],bx	; and set it to this one
    ;
    ; Figure out environment size
    ;
ges1:	mov	ds,bx
	xor	si,si				; DS:SI -> envir
	xor	cx,cx
ges2:	lodsb
	mov	ah,ds:[si]
	inc	cx
	or	ax,ax				; Two bytes of 0?
	jnz	ges2				; No keep looking
    ;
    ; CX is size in bytes of environment NOT including last NUL
    ;
	push	ss
	pop	ds
	lea	si,PathBuffer.opFile
ges3:	lodsb
	inc	cx
	or	al,al
	jnz	ges3
    ;
    ; CX is size in bytes of environment, NOT including last NUL, and argv[0]
    ;	string
    ;
	add	cx,15+1+2	; para align + Double null + arg count
	shr	cx,1		; Convert to paras
	shr	cx,1
	shr	cx,1
	shr	cx,1
    ;
    ; The next two instructions make sure that the environment size is odd.
    ; The reason for this is quite sleazy. Later on in EXEC WinExec is
    ; going to "realloc" the environment via Windows alloc, at this time
    ; the old app code has already been loaded at its location after the
    ; environment. The Windows alloc code has a 32 byte granularity, NOT 16.
    ; By making sure the environment size is odd, we insure that we don't
    ; create an alignment problem where the location of the app arena and our
    ; calculated location for where to load it are off by 10H bytes.
    ;
	and	cl,11111110b
	inc	cx		; make sure its odd
	inc	cx		; one paragraph global arena header
	mov	EnvironParas,cx
	ret

;**
;
; MyAllocate - Allocate a Windows global memory object
;
; ENTRY:
;	BX is size in paras to alloc
; EXIT:
;	Zero and Carry re-set
;	    FIXED, Zeroed out block of memory allocated
;	    AX is address of block
;	Zero and Carry set
;	    Allocation failed
;	    BX is size in para of MAX size object that could be allocated
; USES:
;	ALL but DS,SS,BP
;

MyAllocate:				;!! HP

	push	bx
    ;
    ; Convert para in BX to bytes in DI:SI
    ;
	mov	cl,4
	mov	si,bx
	mov	di,bx
	rol	si,cl
	rol	di,cl
	and	si,1111111111110000b
	and	di,0000000000001111b

 	mov	cx,GMEM_FIXED+GMEM_ZEROINIT+GMEM_DDESHARE
	regptr	xsize,di,si
	cCall	GlobalAlloc,<cx,xsize>
	pop	bx
	or	ax,ax		; OK?
	jnz	ptc		; ok, Carry clear if jump
	dec	AX			; Try GlobalCompact(-1) to get the
	mov	DX, AX			; .. correct largest block
	regptr	xsize,AX,DX
	cCall	GlobalCompact,<xsize>	
	call	GetParagraphs		; bx = largest available on error
	stc
ptc:	ret

;**
;
; ParseComLine - Parse a command line built for WINOLDAP
;
;    This routine parses command lines of this form:
;
;	<parameters of command>NUL<name of program>LF
;
;    It transfers the first part up to the NUL into InBuf and NUL
;    terminates the second part and sets a pointer to it.
;
; ENTRY:
;	DS = Data of app
;	ES:SI -> Command line to parse
; EXIT:
;	Parsed parameter line at InBuf (CR terminated)
;	    1st byte is count not including terminating CR
;	pFileSpec points to NUL terminated command name part of string
; USES:
;	AX,BX,SI,DI,FLAGS
;
ParseComLine:
	mov	di,dataOffset InBuf+2		; DS:DI -> dest of parse
	mov	byte ptr [di-1],' '		; simulate space command.com puts
	xor	bx,bx
    ;
    ; Transfer parameters part
    ;
gpf1:	lods	byte ptr es:[si]
	mov	[di],al 			; Transfer to dest
	inc	di				; Next byte
	or	al,al
	jnz	gpf1
    ;
    ; NUL terminate program name and set pointer to it
    ;
	mov	bx,si
gpf1a:	lods	byte ptr es:[si]
	or	al,al
	jz	gpf1b
	cmp	al,10
	jnz	gpf1a
	mov	byte ptr es:[si-1],0		; Change LF to NUL
gpf1b:	mov	word ptr [pFileSpec],bx
	mov	word ptr [pFileSpec+2],es
    ;
    ; Set count for parameter string
    ;
SetCount:
	mov	bx,dataOffset InBuf
	mov	byte ptr [bx],0 	; start with a count of zero
	inc	bx
	cmp	word ptr [bx],' '	; just a space,null?
	jz	sc3			; Yes, NUL command line
sc1:	inc	bx
	cmp	byte ptr [bx],0 	; end of command line?
	jnz	sc1			; no, keep looking
sc3:	mov	byte ptr [bx],13	; terminate with CR
	sub	bx,dataOffset InBuf+1
	mov	[InBuf],bl		; Set count
	ret

;**
;
; GetPIFData - Set app pif info from PIF file
;
; ENTRY:
;	DS = Data of app to set PIF data for
; EXIT:
;	AX == 0
;	    No PIF, do not run app
;	AX == 1
;	    PIF variables in app DS set (may be defaults if user OKs no PIF)
; USES:
;	ALL but DS,SS,BP
;
cProc	GetPIFData,<PUBLIC,NEAR>,<si,di>

	ParmD	lpParms 			; Name of "program"
	LocalB	orgPif				; STorage for original PIF bits

cBegin
	mov	al,[pifBehavior]		; save this
	mov	orgPif,al
    ;
    ; Set default PIF parameters for case of run with defaults if no PIF
    ;  or for run from WIN.INI parametrs
    ;
	mov	[pifInts],0FF00H		; default = swap all vectors
	mov	[pifFlags],fSwap		; default = switchable
	mov	[pifSysMem],7			; default = swap text
	xor	ax,ax
	mov	[pifInitialDir],al		; default = no initial dir
	mov	[pifBehavior],al		; default = good application
	mov	[pifMsFlags],al
    ;
    ; Memory size and behavior can be set from WIN.INI for defaults
    ;
	mov	di,codeOffset szMemory
	call	GetUserProCS
	mov	[pifMinMem],52			; default = 52k
	mov	[pifMaxMem],ax			; default = Size from WIN.INI

	xor	ax,ax				; default = not compatible
	mov	di,codeOffset szComp
	call	GetUserProCS
	or	ax,ax				; compatible?
	jnz	gpf2				; yes, so leave it alone
	mov	[pifBehavior],fScreen		; mark as bad
    ;
    ; Look for PIF file
    ;
gpf2:	cCall	AnsiUpper,<lpParms>		; convert file name to upper
	les	di,lpParms			; point to name
	call	GetPureName			; don't want dir stuff
    ;
    ; Transfer the application name into pifTitle
    ;	and point at the extension
    ;
	mov	si,dataOffset pifTitle		; default = Application name
	mov	bx,si				; pointer to '.'
	mov	cx,di
gpf3:	mov	al,es:[di]			; get a char
	mov	[si],al 			; store name
	inc	di
	inc	si
	cmp	al,'.'				; To extension yet?
	jnz	gpf5				; No
	mov	bx,si				; BX -> extension in pifTitle
	mov	cx,di				; CX -> extension in input name
gpf5:	or	al,al				; end of name?
	jnz	gpf3				; no, continue
    ;
    ; If running PIF, have exact name to look for, go look with prompting
    ;
	cmp	byte ptr [bx],'P'
	jnz	gpfp				; Not .PIF
	cmp	word ptr [bx+1],'FI'
	jnz	gpfp				; Not .PIF
	mov	si,bx				; Pointer to extension in pifTitle
	mov	di,cx				; Pointer to extension in lpParm
	mov	cx,OF_PROMPT+OF_CANCEL		; Type of prompting
	jmp	short gpf8n			; Go get it

    ;
    ; EXEC name is not .PIF
    ;
gpfp:	push	bx				; Save extension pointers
	push	cx

	push	ds
	pop	es
    ;
    ; Transfer name from pifTitle to pifPathname
    ;
	mov	si,dataOffset pifTitle
	mov	di,dataOffset pifPathname	; set pathname
gpfl:	lodsb
	stosb
	or	al,al
	jnz	gpfl
    ;
    ; See if there is info in WIN.INI for this app
    ;
	mov	ax,-1
	mov	di,dataOffset pifTitle
	call	GetUserPro
	pop	di			; Pointer to extension in lpParm
	pop	si			; Pointer to extension in pifTitle
	cmp	ax,-1			; WIN.INI info?
	jz	gpf8			; No
    ;
    ; There is a WIN.INI entry for this apps behavior
    ;
	mov	[pifBehavior],fResident+fNoGrab ; Assume load and stay resident
	cmp	ax,1			; special Load and stay resident flag?
	jz	gpf7			; yes, just return

	mov	[pifMinMem],ax
	mov	[pifMaxMem],ax
gpfx:	mov	[pifBehavior],0 	; say its a good guy
	mov	byte ptr [si-1],0	; set program title to name without extension
gpf7:	jmp	gpf12			; all done, OK.

gpf8:	xor	cx,cx			; No pif file prompting
    ;
    ; Look for PIF file wherever app is
    ;
gpf8n:	lds	dx,lpParms
	mov	word ptr [di],'IP'	; Change extension to .PIF
	mov	word ptr [di+2],'F'
	mov	ax,3d00H
	int	21h			; Try
	mov	dx,ss:[si]		; restore extension from pifTitle
	mov	[di],dx
	mov	dx,ss:[si+2]
	mov	[di+2],dx
	push	ss
	pop	ds			; DS = Data
	jnc	gpf8q			; found it, read the info
    ;
    ; Look for PIF file in other interesting places
    ;
	mov	word ptr [si],'IP'	; name at pifTitle is .PIF
	mov	word ptr [si+2],'F'
gpf8r:	mov	di,dataOffset PifTitle
	call	OpenPifFile
gpf8q:	mov	bx,ax			; Handle to BX
	inc	ax			; -1 means cant open...
	jnz	gpf8p			; Have PIF file
    ;
    ; Could not find PIF file anywhere, if name is .PIF we error.
    ;  If name is .BAT we will use a good behavior default.
    ;  Else we will try to put up the No PIF dialog box.
    ;
	les	di,lpParms
        call    GetExtension            ; .BAT file ?
        jnz     gpfww                   ; NO - try for PIF
        mov     al,0                    ; Yes -
        jmp     gpf13                   ;    return fail without message box
gpfww:
	cmp	ax,'p.' 		; same for PIF
	jnz	gpfw
	cmp	dx,'fi'
	jz	gpfy		; always return failure when execing PIF
    ;
    ; No PIF file, put up NO PIF file dialog box if possible.
    ; If dialog cannot be posted, we will run with current defaults.
    ;
gpfw:	cmp	cs:[BadDepth],0 	; inside a bad app?
	jnz	gpfu			; yes, cant post warning
	mov	al,[pifBehavior]	; get current pif Behavior
	xchg	al,orgPif		; restore orginal and save current
	mov	[pifBehavior],al	; restore to orginal behavior
	call	PostWarning		; warn that we can't find file
	cmp	di,2			; did he cancel? (Default = 1 or 2)
	jbe	gpfy			; yes, return failure
	mov	al,orgPif		; get the new pif setting
	mov	[pifBehavior],al	; and set it
    ;
    ; Run with current settings, Title is name without extension
    ;	
gpfu:	mov	byte ptr [si-1],0	; set program title
	jmp	gpf12
    ;
    ; Return that program cannot be run
    ;
gpfy:	xor	ax,ax			; return failure
	jmp	gpf13

    ;
    ; Found PIF file, set up vars. First make sure it IS a PIF file.
    ;  if it is not a PIF file, run with current settings.
    ;
gpf8p:	xor	cx,cx
	xor	dx,dx
	mov	ax,4202H			; get file length
	int	21h
	or	dx,dx
	jnz	gpf8a		; Error, file way to big
	cmp	ax,2*PIFLEN	; reasonable size?
	jb	gpf8b		; yes, continue
    ;
    ; PIF file is wrong length, run with current settings
    ;
gpf8a:	jmp	gpf11

    ;
    ; Suck out PIF variables
    ; First get MSFLAGS
    ;
gpf8b:	mov	dx,MSFLAGS
	mov	di,dataOffset pifMsFlags
	mov	cx,1
	call	GetDatum
    ;
    ; Get MAX and MIN memory
    ;
	mov	dx,MAXMEMWORD
	mov	di,dataOffset pifMaxMem
	mov	cx,4
	call	GetDatum
	mov	AX, 52
	cmp	[PifMinMem], AX		; Cannot set Required below 52 Kb
	jae	gpfProgName		; in PIF
	mov	[PifMinMem], AX
	cmp	[PifMaxMem], AX		; PifMaxMem must be > PifMinMem if not 0
	jae	gpfProgName
	cmp	[PifMaxMem], 0
	jz	gpfProgName
	mov	[PifMaxMem], AX
    ;
    ; Get program name
    ;
gpfProgName:
	mov	dx,PPATHNAME
	mov	di,dataOffset pifPathname
	mov	cx,PPATHLEN
	call	GetDatum
	call	ZeroTerminate
    ;
    ; Get initial directory
    ;
	mov	dx,INITIALDIR
	mov	di,dataOffset pifInitialDir
	mov	cx,PATHMAX
	call	GetDatum
	call	ZeroTerminate
    ;
    ; Get initial command line, set prompt if '?'. Line to InBuf
    ; If there already is a command line, don't get PIF one
    ;
	mov	di,dataOffset InBuf+2		; point at command line...
	cmp	byte ptr [di-2],0		; anything there?
	jnz	gpf10				; yes, don't get default
	mov	dx,INITIALCOM
	mov	cx,COMMAX
	call	GetDatum
	cmp	byte ptr [di],'?'	; command line supplied?
	jnz	gpf9			; yes, go proccess it
	mov	byte ptr [di-2],-1
	jmp	short gpf10		; indicate we need a command line

gpf9:	mov	byte ptr [di-1],' '
	call	ZeroTerminate
	push	bx			; save file handle
	call	SetCount		; Set length count
	pop	bx
    ;
    ; Get int swap vars
    ;
gpf10:	mov	dx,INTVECWORD
	mov	di,dataOffset pifIntLow
	mov	cx,2
	call	GetDatum
    ;
    ; Get screen exchange types
    ;
	mov	dx,SYSTEMMEM
	mov	di,dataOffset pifSysMem
	mov	cx,2
	call	GetDatum
    ;
    ; Get behavior bits
    ;
	mov	dx,BEHAVBYTE
	mov	di,dataOffset pifBehavior
	mov	cx,2				; get behavior and system
	call	GetDatum
    ;
    ; Cook PIF file behavior into our internal representation
    ;
	mov	al,[PifBehavior]
	and	al,fScreen+fKeyboard+fForeground+f8087
	mov	ah,[PifMsFlags]
	and	ah,fNoGrab+fNoSwitch+fGraphics+fResident
	or	al,ah
	mov	[PifBehavior],al
    ;
    ; Get program Title
    ;
	mov	dx,PTITLE
	mov	di,dataOffset pifTitle
	mov	cx,PTITLELEN
	call	GetDatum
	call	ZeroTerminate
gpf11:
	mov	ah,3eh			; close the file
	int	21h
gpf12:
	mov	al,1			; return true
gpf13:
cEnd

;**
;
; OpenPifFile - Look for PIF file in various system places
;
;   If prompting for PIF file is selected, we look in the PIF directory
;	then along the OpenFile path.
;   If prompting for PIF file is NOT selected, we look along the OpenFile path
;	then in the PIF directory
;
; ENTRY:
;	SS = DS
;	DS:DI -> Name of PIF file
;	CX is wStyle parameter for OpenFile
; EXIT:
;	AX == -1
;	    Could not find PIF file
;	AX != -1
;	    AX is handle of PIF file
; USES:
;	ALL but DS,SS,SI,DI,CX,BP
;
OpenPifFile:
	test	cx,OF_PROMPT		; Prompting?
	jz	opf1			; No, look along OpenFile first
	push	cx
	call	LookInPifDir		; look in pif directory first
	pop	cx
	cmp	ax,-1			; Did we find it?
	jnz	opfx			; yes, done
opf1:	push	cx
	sub	sp,128			; create 128 byte buffer
	mov	bx,sp
	regptr	pFile,ds,di
	regptr	ssbx,ss,bx
	cCall	OpenFile,<pFile,ssbx,cx>
	add	sp,128			; blow away 128 byte buffer
	pop	cx
	cmp	ax,-1			; Did we find it?
	jnz	opfx			; yes, done
	test	cx,OF_PROMPT		; if we prompted for it
	jnz	opfx			; return failure
LookInPifDir:
	push	di			; Don't blast these
	push	si

	push	ds
	pop	es			; ES:DI -> name
	call	GetPureName		; Get pointer to name
	push	es			; Save pointer to name
	push	di

	push	cs
	pop	es
	mov	di,codeOffset StartUpPathname	; point to start up path
	call	GetPureName			; point past directory
	pop	si				; get pointer to pif file name
	pop	ds
    ;
    ; Tranfer name of PIF file onto end of StartUpPathname
    ;
lip1:	lodsb
	stosb
	or	al,al		; copy it over so we have \STARTUP\PIF\BOBO.PIF
	jnz	lip1
	push	cs
	pop	ds
	mov	dx,codeOffset StartupPathname
	mov	ax,3d00H	; open the file
	int	21h
	push	ss
	pop	ds
	jnc	lip2		; Worked
	mov	ax,-1		; error
lip2:	pop	si
	pop	di
opfx:	ret

;**
;
; GetUserProCS GetUserPro - Look up WIN.INI integer parms
;
; ENTRY:
;	DS:DI -> Keyword string for string in PIF section
;	For GetUserProCS string is assumed to be in CS and only DI need be set
; EXIT:
;	AX = Return from GetProfileInt
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
GetUserProCS:
	push	cs
	pop	ds
GetUserPro:
	mov	bx,codeOffset szPif
	regptr	PIFPTR,cs,bx
	regptr	KEYPTR,ds,di
	cCall	GetProfileInt,<PIFPTR,KEYPTR,ax>
	push	ss
	pop	ds
	ret

;**
;
; GetDatum - Get indicated PIF data
;
; ENTRY:
;	DX is offset in PIF file of start of section to get
;	BX is handle of PIF file
;	CX is number of bytes to get out of PIF file
;	DS:DI -> Location to store PIF data
; EXIT:
;	Carry set
;	    Error reading file
;	    AX is error code
;	Carry clear
;	    Something read
;	    AX = Count of bytes read
; USES:
;	AX,DX,FLAGS
;
GetDatum:
	push	cx			; Save byte count
	xor	cx,cx			; LSEEK to DX location in file
	mov	ax,4200h
	int	21h
	pop	cx
	mov	dx,di
	mov	ah,3fh			; Read stuff
	int	21h
	ret

;**
;
; ZeroTerminate - NUL terminate a space padded string
;
; ENTRY:
;	DS:DI -> start of string
;	CX is size of string
; EXIT:
;	Trailing spaces converted to one NUL terminator in first space of tail
;	DIRECTION FLAG CLEARED!!!!
; USES:
;	DI,CX,ES,FLAGS
;
ZeroTerminate:
	push	ds
	pop	es		; ES:DI -> string
	add	di,cx		; ES:DI -> end of string
	inc	cx
	dec	di
	mov	al,' '		; Look for first non-space
	std			; Go backwards
	repz	scasb
	mov	byte ptr [di+2],0	; zero terminated
	cld
	ret

;**
;
; GetPureName - Get pointer to file name at the end of a path
;
; ENTRY:
;	ES:DI -> NUL terminated string
; EXIT:
;	ES:DI -> file name at end of string
;	DIRECTION FLAG CLEARED
; USES:
;	AX,BX,CX,DI,FLAGS
;
GetPureName:
	cld
	xor	al,al
	mov	cx,-1
	mov	bx,di			; Save start of string
	repne	scasb			; Find NUL at end
	inc	cx
	inc	cx
	neg	cx			; CX is size of string excluding NUL
iup0:	dec	di			; Next byte
	cmp	bx,di			; back to beginning of string?
	jz	iup1			; yes, di points to name
	mov	al,es:[di-1]		; get next char
	cmp	al,'\'                  ; next char a '\'?
	jz	iup1			; yes, di points to name
	cmp	al,'/'			; next char a '/'
	jz	iup1			; yes, di points to name
	cmp	al,':'			; next char a ':'
	jz	iup1			; yes, di points to name
	jmp	iup0			; Next char
iup1:	ret

;**
;
; Allocate - Allocate a global FIXED memory object of the indicated size
;
; ENTRY:
;	DX:AX is size to allocate in bytes
; EXIT:
;	Zero set
;	    Allocation failed
;	Zero Re-Set
;	    AX is handle (address since FIXED) of allocated block
; USES:
;	ALL but BX,DS,SS,BP,SI,DI
;
Allocate:
	push	bx		; allocate the memory
	mov	bx,GMEM_FIXED+GMEM_DDESHARE
	regptr	dxax,dx,ax
	cCall	GlobalAlloc,<bx,dxax>
	pop	bx
	or	ax,ax
	ret

;**
;
; GetFreeMem - Get size in para of max amount of memory we can allocate
;
; ENTRY:
;	None
; EXIT:
;	BX is number of paras of available memory
; USES:
;	ALL but DS,SS,BP
;
GetFreeMem:
	xor	bx,bx
	cCall	GlobalCompact,<bx,bx>	; get number of free bytes
	errn$	GetParagraphs		; Fall into getparagraphs
;**
;
; GetParagraphs - Convert byte count to paragraph count
;
; ENTRY:
;	DX:AX is count of bytes
; EXIT:
;	BX is number of paras == FFFFH if byte count > FFFF0H
; USES:
;	AX,BX,CX,DX,FLAGS
;
GetParagraphs:
	add	ax,15			; Para round up
	adc	dx,0
	mov	cl,4			; DIV by 16
	ror	ax,cl
	ror	dx,cl
	and	ah,00001111b
	and	dh,11110000b
	mov	bx,ax
	or	bx,dx		; bx = number of paragraphs
	ret

;**
;
; GetBytes - Convert para count to byte count
;
; ENTRY:
;	BX is number of paras
; EXIT:
;	DI:SI is count of bytes
; USES:
;	CX,SI,DI,FLAGS
;
GetBytes:
	mov	cl,4
	mov	si,bx
	mov	di,bx
	rol	di,cl
	rol	si,cl
	and	si,1111111111110000b
	and	di,0000000000001111b
	ret

;**
;
; initheap - Initialize Windows heap for an old app
;
;   What this routine does is set up a Windows Heap
;	which occupies the old app area, the environment for the app,
;	and its PDB. This is done so that any DOS memory allocation calls
;	(48H 49H 4AH) issued by this app result in allocations
;	in this heap. This is exactly what we want to make the
;	app think it is running under DOS alone.
;
; ENTRY:
;	BX = first paragraph of partition
;	ES = arena header of partition
; EXIT:
;	DS:BX = pointer to global heap information structure
;		for private global heap.
;	AX = PDB address
; USES:
;	ALL but SS,BP
; SEE ALSO:
;	HEAP code in the KERNEL memory manager
;
initheap    PROC    NEAR
	mov	si,ES:[ga_next]
	dec	si
	and	si,GA_MASK	    ; SI = last arena entry (sentinal)
	lea	bx,[bx+1].GA_ALIGN
	and	bx,GA_MASK	    ; BX = first arena entry (sentinal)
?x = 15 + SIZE GlobalInfo
?x = ?x shr 4
?x = ?x + 1 + GA_ALIGN
	lea	ax,[bx].?x
	and	ax,GA_MASK	    ; AX = allocated arena entry

;
;~~tqn 051287
;
; Initialize lruprev, lrunext , _alt fields to meet expectations of Kernel 050587
;
	; Initialize last block
	mov	ds,si
;	mov	DS:[ga_sig],GA_ENDSIG
	mov	DS:[ga_owner],-1
	mov	DS:[ga_prev],ax
	mov	DS:[ga_next],si
;	mov	DS:[ga_handle],0
	mov	DS:[ga_size],1
	mov	DS:[ga_lruprev],ax
	mov	DS:[ga_lrunext],bx
	push	ds
	push	ds

	; Initialize middle block
	mov	ds,ax
	inc	ax
	mov	DS:[ga_sig],GA_ENDSIG
	mov	DS:[ga_prev],bx
	mov	DS:[ga_next],si
;	mov	DS:[ga_handle],0
	mov	cx,si
	sub	cx,ax
	mov	DS:[ga_size],cx
	dec	ax

	; Initialize First block
	mov	ds,bx
	mov	DS:[ga_sig],GA_SIGNATURE
	mov	DS:[ga_owner],-1
	mov	DS:[ga_prev],ds
	mov	DS:[ga_next],ax
;	mov	DS:[ga_handle],0
	mov	DS:[ga_lruprev],ds
	mov	DS:[ga_lrunext],si
	mov	cx,ax
	sub	cx,bx
	dec	cx
	mov	DS:[ga_size],cx
	push	ds
	inc	bx
	mov	ds,bx
	xor	bx,bx
; This code assumes memory zero initialized
	inc	[bx].hi_freeze
	mov	[bx].hi_count,3
	pop	[bx].hi_first
	pop	[bx].hi_last
	pop	[bx].gi_disfence

	mov	[bx].gi_alt_first,-1	; Fill in pointers to first and
	mov	[bx].gi_alt_last,-1	;  last blocks, the -1 is necessary!!
	mov	[bx].gi_alt_count,bx	; # of arena entries
	mov	[bx].gi_alt_lruchain,bx
	mov	[bx].gi_alt_lrucount,bx	; MUST be 0!
	mov	[bx].gi_alt_free_count,bx
	mov	[bx].gi_alt_reserve,bx
	mov	[bx].gi_alt_disfence,bx
	mov	[bx].gi_alt_pPhantom,-1	; MUST be -1!

	inc	ax		; point AX at new PDB location
	ret
initheap    ENDP

;**
;
; TTYquery - Can the old app be closed?
;
;    This routine handles the situation where WINDOWS tries to EXIT
;	with an old app running (in which case the EXIT is disallowed).
;	All it does is return an indicator of whether the app is terminated.
;
; ENTRY:
;	DS = Data of app to ask about
;	MUST BE IN WINDOWS ENVIRONMENT (Windows fully enabled)
; EXIT:
;	DX:AX != 0
;		App is terminated (not running)
;	DX:AX == 0
;		App is running
;		msgActive Dialog box displayed
; USES:
;	ALL but DS,SS,BP
;
TTYquery:
	mov	ax,[OldApState]		; Get terminated flag
	inc	ax			; AX > 0
	cmp	ax, SF_RUNNING+1	; Is app running?
	jne	tqu1			;  No, all done, return TRUE
    ;
    ; Post Old App active dialog
    ;
	mov	ax,msgActive
	mov	si,MB_OK+MB_ICONASTERISK+MB_SYSTEMMODAL ; select "Ok" menu
	call	ShowMessage2
	xor	ax,ax			; Return FALSE
tqu1:	cwd
	ret

;**
;
; SetDirectory - Change to the indicated initial directory and drive if
;			specified
;
; ENTRY:
;	DS = Data of app to set for
;	pifInitialDir set
; EXIT:
;	Change to pifInitialDir if given
;		Default drive changed if it includes a drive
;	If initial dir is invalid, error dialog posted if top level old app
;		and app is terminated
; USES:
;	CX,DX,SI,DI,FLAGS
;
SetDirectory:
	push	ax
	push	bx
	mov	di,dataOffset pifInitialDir	; change to start up directory
	mov	ax,[di]
	or	al,al				; Anything given?
	jz	sdx				; no, done
	cmp	ah,':'				; drive specified?
	jnz	sda				; No, just directory
    ;
    ; Drive is specified, change default drive
    ;
	or	al,20h				; Lower case
	sub	al,'a'				; numeric, A = 0
	mov	dl,al
	mov	ah,0eh				; select disk
	int	21h
	cmp	byte ptr [di+2],0		; just change drive?
	jz	sdx				; yes, all finished
    ;
    ; Change directory
    ;
sda:	mov	dx,di
	mov	ah,3bh				; change current directory
	int	21h
	jnc	sdx				; Worked, all done
    ;
    ; Something is invalid in the initial dir string. Post error
    ;	if top level old app, else ignore and proceed.
    ;
	test	Exec_func,80H			; top level guy?
	jz	sdx				; no, so no error report
	mov	ax,msgBadDir
	call	ShowExit
sdx:	pop	bx
	pop	ax
	ret

;**
;
; ShowError - Put up an error message dialog
;
; ENTRY:
;	WINDOWS ENVIRONMENT MUST BE IN EFFECT (Windows fully enabled)
;	AX is string ID of message to print - ONE of
;		msgNoMem
;		Any message ID with High bit set
;	   if the ID is not one of the above, msgNoExec is displayed
; EXIT:
;	Message printed with exclamation point Icon
; USES:
;	ALL but AX,DS,SS,BP
;
ShowError:
	push	ax
;!!HP
  	test	AL, 80H			; Bit 7 in AL set if error detected
	jz	ShowE1			; by DOSExec procedure
	and	AL, 7FH			; Turn it into Winoldap error message
	jmp	she1a

;******
ShowE1:
	cmp	ax,msgNoMem			; No Memory?
	jnz	she1				; Nope
    ;
    ; Error is pre-loaded no memory message
    ;
	mov	di,dataOffset NoMemMsg
	mov	si,MB_OK+MB_ICONEXCLAMATION	; select "Ok" menu
	call	MyMessageBox
	jmp	short she2

she1:	or	ax,ax				; Does message ID have high bit?
	js	she1a				; Yes, is message ID
	mov	ax,msgNoExec			; Others get this message
she1a:	call	ShowMessage
she2:	pop	ax
	ret

;**
;
; ShowMessage ShowMessage2 - Put up message dialogs
;
; ENTRY:
;	WINDOWS ENVIRONMENT MUST BE IN EFFECT (Windows fully enabled)
;	AX is message ID
;	SI is message box wType parameter if ShowMessage2 used
; EXIT:
;	Message printed
; USES:
;	ALL but AX,DS,SS,BP
;
ShowMessage:
	mov	si,MB_OK+MB_ICONEXCLAMATION	; select "Ok" menu
ShowMessage2:
	push	ax
    ;
    ; Load the message text
    ;
	mov	bx,msgMax
	sub	sp,bx
	mov	di,sp
	mov	ss:[di],bh		; zero terminate
	regptr	pMsg,ss,di
	cCall	LoadString,<ss,ax,pMsg,bx>
    ;
    ; Print the message
    ;
	call	MyMessageBox
	add	sp,msgMax
	pop	ax
	ret

;**
;
; MyMessageBox - Put up a message box Dialog
;
; ENTRY:
;	WINDOWS ENVIRONMENT MUST BE IN EFFECT (Windows fully enabled)
;	SS = Data of old app
;	SS:DI -> message text
;	SI = wType parameter
; EXIT:
;	Message box displayed
; USES:
;	ALL but DS,SS,BP
;
MyMessageBox:
	mov	cx,dataOffset pifTitle
	regptr	pMsg,ss,di
	regptr	lpName,ss,cx
	xor	ax,ax
	cCall	MessageBox,<ax,pMsg,lpName,si>
	ret

;**
;
; GetComSpec - Find COMSPEC string in environment (or use default)
;
; ENTRY:
;	Current PDB is task whose environment to look in
; EXIT:
;	DS:SI -> COMSPEC string
; USES:
;	AX,CX,SI,DI,DS,ES,FLAGS
;
GetComSpec:
	mov	di,codeOffset szCOMSPEC
	mov	si,8			; length of string including =
	call	FindEnviromentString
	jz	gcs2			; Found
	push	cs
	pop	ds
	mov	si,codeOffset szCommand ; Defualt
gcs2:	ret

;**
;
; FindTemp - Find TEMP string in environment
;
; ENTRY:
;	Current PDB is task whose environment to look in
; EXIT:
;	Zero Set
;	    TEMP String found
;	    DS:SI -> rest of string in environment after input string
;	Zero re-Set
;	    No TEMP string in environment
; USES:
;	AX,CX,SI,DI,DS,ES,FLAGS
;
FindTemp:
	mov	di,codeOffset szTEMP
	mov	si,5			; length of string including =
	errn$	FindEnviromentString	; Fall through
;**
;
; FindEnviromentString - Find string in environment
;
; ENTRY:
;	CS:DI -> string to look for in environment (including = if approp)
;	SI is the size of the string in bytes
;	Current PDB is task whose environment to look in
; EXIT:
;	Zero Set
;	    String found
;	    DS:SI -> rest of string in environment after input string
;	Zero re-Set
;	    String not in environment
; USES:
;	AX,CX,SI,DS,ES,FLAGS
; SEE ALSO:
;	DOS tech ref on environments
;
FindEnviromentString:
	cCall	GetCurrentPDB
	mov	ds,ax			; Point at PDB of task
	mov	ds,ds:[PDB_environ]	; get segment of enviroment
	push	cs
	pop	es			; ES:DI -> search string
	mov	cx,si			; CX is size
	xor	si,si			; Start at start of environment
    ;
    ; Look at next environment entry
    ;
fpa1:	push	cx
	push	di
	repz	cmpsb
	pop	di
	pop	cx
	jz	fpa3			; FOUND it, zero set
    ;
    ; Skip to next environment entry
    ;
fpa2:	lodsb
	or	al,al
	jnz	fpa2
	cmp	byte ptr [si],0 	; End of environment?
	jnz	fpa1			; no, look at next entry
	or	cx,cx			; reset zero
fpa3:	ret

;**
;
; GetVars - Set parameters for various routines
;
; ENTRY:
;	DS = Data of approp old app
; EXIT:
;	AX = Combo of pifBehavior and high bit of Exec_Func
; USES:
;	AX,FLAGS
;
GetVars:
	xor	ax,ax
	mov	al,[pifBehavior]	; AL is this
	test	Exec_func,80h
	jz	gv1			; High byte is 0
	inc	ah			; High bit from Exec_Func
gv1:	ret

	sEnd	code
	END
