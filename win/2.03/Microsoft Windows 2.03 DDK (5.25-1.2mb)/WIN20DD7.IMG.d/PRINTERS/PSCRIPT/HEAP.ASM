	title	GDI Initialization Code
	%out	HeapInit
	page	,132


	.xlist
	include cmacros.inc
	.list
	public	HeapInit

;	Imported Kernel Routines
	ExternFP LocalInit		;Initialize local heap




createSeg _INIT,nres,byte,public,CODE
sBegin	nres
assumes cs,nres
assumes ds,data

;	To ease the pains of implementing additional initialization
;	code, all initialization code will be written as subroutines,
;	and those subroutines called from this here routine.
;
;	The data segment will be locked for the duration of the
;	initialization procedure.  All procedures must return
;	with the 'Z' flag set to indicater an error occured and
;	that an abort should occur.
;
;	Since any initialization code is truncated from the GDI module,
;	this code can be as verbose as desired.
;
;	Entry:	CX = size of heap
;		DI = module handle
;		DS = automatic data segment
;		ES:SI = address of command line (not used)
;
;	Exit:	ax = 0 if error
;
;	Uses:	ax,bx,cx,dx,es,flags


cProc	HeapInit,<FAR,PUBLIC>,<si,di>

cBegin
	xor	ax,ax
	cCall	LocalInit,<ax,ax,cx>
	xor	ax,ax
	not	ax
cEnd
sEnd	nres
end	HeapInit
