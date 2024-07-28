PAGE 58,132
;******************************************************************************
TITLE I13DEBUG.ASM -- Debugging code for Int13.386
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	I13DEBUG.ASM -- Debugging code for Int13.386
;
;   Version:	1.00
;
;   Date:	11-Oct-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   11-Oct-1990 RAL
;
;==============================================================================


	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE BlockDev.Inc
	.LIST


;******************************************************************************
;
;******************************************************************************


Start_Trace_Line	EQU	2
Last_Trace_Line 	EQU	17


IFDEF DEBUG

VxD_DATA_SEG

I13_IO_Start_Time	dd	?
I13_Total_Time		dd	0
I13_Last_Sector_Pos	dd	0

Debug_Last_DOS_AX	dw	0
Debug_Last_DOS_BX	dw	0
Debug_DOS_Count 	db	0

Cur_Trace_Line		db	Last_Trace_Line
I13_Debug_Display	db	0

VxD_DATA_ENDS

;------------------------------------------------------------------------------

VxD_ICODE_SEG

;******************************************************************************
;
;   Int13_Debug_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Debug_Init

	mov	eax, 21h
	mov	esi, OFFSET32 Int13_Debug_Int21_Hook
	VMMcall Hook_V86_Int_Chain
	clc
	ret

EndProc Int13_Debug_Init


VxD_ICODE_ENDS




VxD_CODE_SEG

;******************************************************************************
;
;   Int13_Debug_Int21_Hook
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Debug_Int21_Hook

	mov	eax, [ebp.Client_EAX]
	mov	[Debug_Last_DOS_AX], ax
	mov	eax, [ebp.Client_EBX]
	mov	[Debug_Last_DOS_BX], ax

	inc	[Debug_DOS_Count]

	stc
	ret

EndProc Int13_Debug_Int21_Hook


;******************************************************************************
;
;   Int13_Debug_Query
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Debug_Query

	Trace_Out "===== INT13 DEBUGGING SERVICES ====="
	Trace_Out " "
	Trace_Out "[1] Turn on/off mono status display"
	Trace_Out "[2] Display Int 13h statistics to debug terminal"
	Trace_Out "[3] Reset Int 13h statistics"
	Trace_Out " "
	Trace_Out "Enter selection or [Esc] to quit: ", No_EOL

I13_DQ_In_Loop:
	VMMcall In_Debug_Chr
	jz	I13_DQ_Exit
	sub	al, "1"
	je	SHORT I13_DQ_Toggle_Display
	jb	I13_DQ_In_Loop
	cmp	al, 2
	ja	I13_DQ_In_Loop
	je	SHORT I13_DQ_Trace_Out_Stats


;
;   Reset the Int 13h statistics
;

I13_DQ_Trace_Out_Stats:
	Trace_Out " "
	Trace_Out " "
	Trace_Out "This dosen't work yet"
	jmp	I13_DQ_Exit


;
;   Turn mono display on/off
;
I13_DQ_Toggle_Display:
	VMMcall Clear_Mono_Screen

	xor	[I13_Debug_Display], -1
	jz	I13_DQ_Exit

	Mono_Out_At 0, 0, "CUR     CMD   DRV    SECTOR    COUNT     SEC DST  CONTIG  DOS AH,BX,Inc  TIME(d)"
		    ;;;;   -->	  write   C    12345678    1234    12345678    YES   12, 1234, ##    1234
	Mono_Out_At 1, 0, "--------------------------------------------------------------------------------"
	Mono_Out_At 18,0, "================================================================================"
	Mono_Out_At 19,0, "           80-READ  80-WRITE   81-READ  81-WRITE    READ      WRITE      GRAND"
	Mono_Out_At 20,0, "SECTORS   ........  ........  ........  ........  ........  ........   ........"
	Mono_Out_At 21,0, "BLOCKS    ........  ........  ........  ........  ........  ........   ........"
	Mono_Out_At 22,0, "TIME (d)  ........  ........  ........  ........  ........  ........   ........"
	Mono_Out_At 23,0, "CYL DST   ........  ........  ........  ........  ........  ........   ........", NO_EOL
	Mono_Out_At 24,0, "SEC DST   ........  ........  ........  ........  ........  ........   ........", NO_EOL


I13_DQ_Exit:
	Trace_Out " "
	clc
	ret

EndProc Int13_Debug_Query


;******************************************************************************
;
;   Int13_Debug_Start_IO
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Debug_Start_IO

	cmp	[I13_Debug_Display], 0
	je	I13_DSIO_Exit

	pushad

	%OUT Check for mono display on!

	mov	dh, [Cur_Trace_Line]
	xor	dl, dl
	VMMcall Set_Mono_Cur_Pos
	Mono_Out "   "
	inc	dh
	cmp	dh, Last_Trace_Line
	jbe	SHORT I13_DSIO_Set_New_Line
	mov	dh, Start_Trace_Line
I13_DSIO_Set_New_Line:
	mov	[Cur_Trace_Line], dh
	VMMcall Set_Mono_Cur_Pos

	cmp	[esi.BD_CB_Command], BDC_Read
	jne	SHORT I13_DSIO_Write
	Mono_Out "-->    Read ", NO_EOL
	jmp	SHORT I13_DSIO_Rest_O_Stats

I13_DSIO_Write:
	Mono_Out "-->    Write", NO_EOL

I13_DSIO_Rest_O_Stats:
	mov	eax, DWORD PTR [esi.BD_CB_Sector]
	mov	ebx, [esi.BD_CB_Count]
	movzx	ecx, [edi.BDD_Int_13h_Number]
	lea	edx, [eax+ebx]
	xchg	edx, [I13_Last_Sector_Pos]
	neg	edx
	add	edx, eax
	Mono_Out "   #CL  #EAX    #BX    #EDX    ", NO_EOL

	or	edx, edx
	jz	SHORT I13_DSIO_Is_Contig
	Mono_Out "no     ", No_EOL
	jmp	SHORT I13_DSIO_More_Stuff

I13_DSIO_Is_Contig:
	Mono_Out "YES    ", No_EOL

I13_DSIO_More_Stuff:
	mov	ax, [Debug_Last_DOS_AX]
	mov	bx, [Debug_Last_DOS_BX]
	mov	cl, [Debug_DOS_Count]
	mov	al, ah
	Mono_Out "#AL, #BX, #CL    ", No_EOL

	VMMcall Get_System_Time
	mov	[I13_IO_Start_Time], eax

	popad
I13_DSIO_Exit:
	ret

EndProc Int13_Debug_Start_IO


;******************************************************************************
;
;   Int13_Debug_End_IO
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Int13_Debug_End_IO

	cmp	[I13_Debug_Display], 0
	je	SHORT I13_DEIO_Exit

	pushad

	VMMcall Get_System_Time
	sub	eax, [I13_IO_Start_Time]
	jz	SHORT I13_DEIO_No_Convert

	add	[I13_Total_Time], eax

	VMMcall Debug_Convert_Hex_Decimal
I13_DEIO_No_Convert:
	Mono_Out "#AX"

	mov	eax, [I13_Total_Time]
	VMMcall Debug_Convert_Hex_Decimal
	Mono_Out_At 22, 71, "#EAX"

	popad

I13_DEIO_Exit:
	ret


EndProc Int13_Debug_End_IO



VxD_CODE_ENDS


ENDIF

	END
