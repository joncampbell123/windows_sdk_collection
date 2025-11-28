#define MAXDEPTH 50
#define	SPECIFYDIR_LOC 	 "You must supply the name of the Documenting Wizard target directory."
#define ACTIVATEWIN_LOC	 "You must activate an edit window first."
#define GETDIRPROMPT_LOC "Doc Wizard Output Folder?"

para m1,m2
set exact off
set conf on

PUBLIC mdir
if type("m1") = 'C'
	mdir=m.m1
ELSE
	mdir=GETDIR(sys(2003)+"out",GETDIRPROMPT_LOC)
ENDIF
IF EMPTY(m.mdir) OR !FILE(mdir+"fdxref.dbf") OR !FILE(mdir+"files.dbf")
	MESSAGEBOX(SPECIFYDIR_LOC,16)
	RETURN .f.
ENDIF
IF USED("fdxref")
	SELECT fdxref
ELSE
	USE (mdir+"fdxref") EXCLUSIVE
ENDIF
IF !ISEXCL()
	USE (DBF()) EXCLUSIVE
ENDIF
set order to symbol
IF !USED("symbols")
	SELECT upper(symbol) as symbol,count(*) as count ;
		FROM fdxref INTO CURSOR symbols order by 1 group by 1
ENDIF
SELECT symbols
LOCATE

do form jump
PROC tex
	para mm && Definition Reference Next Back Goto
	publ mwinname,mwinpos,seekmode,m.symbol
	SELECT fdxref
	set order to symbol
	seekmode=m.mm
	do setlibr
	if m.seekmode='G'
		IF EMPTY(filename)
			RETURN
		ENDIF
		IF RIGHT(UPPER(ALLTRIM(filename)),4)$".VCX.SCX.DBC"
			IF USED("snipfile")
				USE IN snipfile
			ENDIF
			USE (ALLTRIM(fdxref.filename)) AGAIN IN 0 ALIAS snipfile
			GO (fdxref.sniprecno) IN snipfile
			IF !EMPTY(fdxref.snipfld)
				MODI MEMO ("snipfile."+fdxref.snipfld) nowait noedit
				Gotorec()
			ENDIF
		ELSE
			modi comm (filename) nowait noedit
			Gotorec()
		ENDIF
		SET LIBR TO
		return
	endif
	IF type("fdstack[1]")='U'
		PUBLIC fdstack[1,1],FDSP
		fdsp=0
	ENDIF
	IF m.seekmode='B'
		IF m.fdsp>0
			mwinname=fdstack[fdsp,1]
			mwinpos=fdstack[fdsp,2]
			=CurPos("S")
			fdsp=m.fdsp-1
			IF m.fdsp>0
				DIMENSION fdstack[fdsp,2]
			ENDIF
		ELSE
		ENDIF
		set libr to
		RETURN
	ENDIF
	IF m.seekmode$"DR"
		IF TYPE("_screen.activeform.caption")#'C'
			=CurPos("G")
		ELSE
			=MessageBox(ACTIVATEWIN_LOC,16)
			RETURN
		ENDIF
	ENDIF
	*	show wind fdxref refresh
	if m.seekmode$"DR"
		=examine(seekmode)	&&see what's under cursor
	endif
	do exam	&&get cursor word into m.symbol
	set libr to
RETURN

PROC exam
	*called by examine()... m.symbol ="" if not found
	PRIVATE str
	SELECT fdxref
	if m.seekmode='T'
		set orde to
		skip
		IF eof()
			GO BOTT
		ENDIF
	else
		if empty(set("order"))
			SET ORDER TO symbol
		ENDIF
		str=PADR(UPPER(m.symbol),LEN(symbol))
		IF m.seekmode$"DR"
			SEEK str+m.seekmode
			IF m.seekmode='D' AND !FOUND()
				SEEK str+'V'
			ENDIF
			IF m.seekmode='R' AND !FOUND()
				SEEK str
			ENDIF
		ELSE
			IF !EOF()
				SKIP
			ENDIF
		ENDIF
	ENDIF
	IF m.seekmode#'T' and (EMPTY(m.symbol) OR UPPER(symbol)#UPPER(m.symbol) OR EOF())
		WAIT WINDOW NOWAIT m.seekmode+' '+m.symbol+" not found"
		m.symbol=""
	ELSE
		IF RIGHT(UPPER(ALLTRIM(filename)),4)$".VCX.SCX.DBC"
			IF USED("snipfile")
				USE IN snipfile
			ENDIF
			USE (ALLTRIM(fdxref.filename)) AGAIN IN 0 ALIAS snipfile
			GO (fdxref.sniprecno) IN snipfile
			IF !EMPTY(fdxref.snipfld)
				MODI MEMO ("snipfile."+fdxref.snipfld) nowait noedit
			ENDIF
		ELSE
			modi comm (filename) nowait noedit
		ENDIF

		IF RIGHT(TRIM(filename),3)$"PRG MPR SPR"
			SCATTER MEMVAR
			m.lineno=INT(m.lineno)
			if m.seekmode$"DR"
				fdsp=m.fdsp+1
				DIMENSION fdstack[fdsp,2]
				fdstack[fdsp,1]=mwinname
				fdstack[fdsp,2]=mwinpos
			ENDIF
		ELSE
			m.symbol=""
		ENDIF
		=Gotorec()
		WAIT WINDOW NOWAIT ALLTRIM(m.symbol)+" "+flag+" found in "+ALLTRIM(fdxref.Filename)+' '+STR(lineno,5) &&+" SP="+str(fdsp,2)  &&&&showsp
	ENDIF
RETURN


proc setlibr
		set libr to (IIF(file("fd3fll\fd3.fll"),;
				"fd3fll\fd3.fll",;
				LOCFILE(sys(2004)+"wizards\fd3.fll")))
		IF "fd3"$SET("LIBR")
			RETURN .T.
		ENDIF
return .f.

proc tre
	PARAMETER nmode,ol
	PRIVATE lvl,cnt,err,i
	ol.style=5
	ol.clear
	IF !USED("files")
		use (mdir+"files") EXCL in 0
	ENDIF
	select files
	IF !ISEXCL()
		USE (DBF()) EXCL ALIAS files
	ENDIF
	go 1
	mtop=JustStem(file)
	select fdxref
	lvl=0
	m.cnt=0
	m.err=.f.
	mvar1="procname"
	mvar2="symbol"
	m.allowdup=.t.
	set talk off
	ol.visible=.f.	&&debug
	
	DO CASE
	CASE nMode=1	&&calling tree
		do treediag
	CASE nMode=3	&&Class Hierarchy
		ON ERROR m.err=.t.
		SET ORDER TO classes
		IF m.err
			index on upper(procname) for flag$"BC" tag classes
		ENDIF
		ON ERROR
		SELECT DISTINCT procname FROM fdxref;
			WHERE flag$"BC";
			ORDER BY 1;
			INTO CURSOR obj
		SCAN
			myrec=recno()
			MTOP=UPPER(ALLTRIM(Procname))
			SELECT fdxref
			DO showit WITH mtop
			SELECT obj
			go myrec
		ENDSCAN
		USE IN obj
		SELECT fdxref
	CASE nMode=2 && Derived class hierarchy
		do classdiag
	ENDCASE
	
	FOR i=0 TO ol.listcount-1
		IF ol.hassubitems[i]
			ol.picturetype[i]=0
		ELSE
			ol.picturetype[i]=2
		ENDIF
	ENDFOR
	ol.visible=.t.
RETURN


PROCEDURE JustStem
	PARAMETERS mfile
	IF AT('\',m.mfile)>0
		mfile=SUBSTR(m.mfile,RAT('\',m.mfile)+1)
	ENDIF && AT('/',m.mfile)>0
	IF AT(".",m.mfile)>0
		mfile=LEFT(m.mfile,AT(".",m.mfile)-1)
	ENDIF && AT(".",m.mfile)>0
RETURN m.mfile
*EOP JustStem

PROC ClassDiag
	LOCAL mr
	PRIVATE lvl,cCollate
	cCollate=SET("collate")
	SET COLLATE TO "machine"
	SELECT symbol,procname,flag,filename,' ' AS done;
		FROM  fdxref ;
		WHERE flag$"CB" AND;
			UPPER(symbol) # UPPER(procname);
		INTO CURSOR classd1
	USE DBF("classd1") EXCL AGAIN IN 0 ALIAS classd
	SELECT classd
	USE IN classd1
	INDEX ON done+flag+UPPER(procname) TAG dprocname
	INDEX ON UPPER(procname) TAG procname
	INDEX ON UPPER(symbol)  TAG symbol
	m.lvl=0
	m.cnt=0
	DO WHILE SEEK(' ',"classd","dprocname")
		mr=RECNO()
		DO WHILE SEEK(UPPER(procname)),"classd","symbol")
			mr=RECNO()
		ENDDO
		GO mr
		m.lvl=1
		ol.additem(ALLTRIM(procname))
		ol.indent[m.cnt]=m.lvl
		m.cnt=m.cnt+1
		DO showclas WITH UPPER(ALLTRIM(procname))
		SET ORDER TO symbol
	ENDDO
	USE IN classd
	SET COLLATE TO (m.cCollate)
RETURN
	
PROC showclas
	PARA m.procname
	LOCAL mr
	m.lvl=m.lvl+1
	IF SEEK(' C'+m.procname+' ',"classd","dprocname")
		SET ORDER TO procname
		SCAN WHILE UPPER(ALLTRIM(procname))+' ' = m.procname+' '
			REPLACE done WITH 'Y'
			IF m.lvl>1
				mr=recno()
				mparent=UPPER(procname)
				SKIP
				GO m.mr
			ENDIF
			ol.additem(ALLTRIM(symbol))
			ol.indent[m.cnt]=m.lvl
			m.cnt=m.cnt+1

			mr=recno()
			DO showclas WITH UPPER(ALLTRIM(symbol))
			GO m.mr
			SET ORDER TO procname
		ENDSCAN
	ENDIF
	m.lvl=m.lvl-1
RETURN


proc treediag
	PRIVATE lvl,cnt,err
	PRIVATE aLev
	PRIVATE mindent,mparent
	PRIVATE cActionChars
	PRIVATE track
	PRIVATE mtop
	local msetexact,mr
	DIMENSION track[MAXDEPTH]
	track=""
	msetexact=set("exact")
	set exact on
	CREATE CURSOR did (proc c(len(fdxref.symbol)))
	INDEX ON upper(proc) TAG proc
	select files
	LOCA
	IF EOF()
		RETURN .f.
	ENDIF
	m.cnt=0
	go 1
	mtop=PADR(JustStem(file),LEN(fdxref.procname))	&&bugbug
	select fdxref
	lvl=1
	m.cnt=1
	m.err=.t.
	DO WHILE !EMPTY(TAG(m.cnt))
		IF tag(m.cnt)="PROCEDURE"
			m.err=.f.
			EXIT
		ENDIF
		m.cnt=m.cnt+1
	ENDDO
	IF m.err
		index on upper(procname) for flag$'DF' tag procedure
	ELSE
		SET ORDER TO procedure
	ENDIF
	m.cnt=0
	track=""
	ol.additem(ALLTRIM(m.mtop))
	ol.indent[m.cnt]=m.lvl
	m.cnt=m.cnt+1
	DO showit WITH mtop
	*now find all missing subtrees
	SELECT fdxref
	SCAN for flag='D'
		MR=recno()
		*find top of subtree
		m.mtop=fdxref.symbol
		DO WHILE SEEK(UPPER(m.mtop)+'F',"fdxref","symbol") AND !"."$fdxref.procname AND ;
				UPPER(ALLTRIM(fdxref.symbol)) # UPPER(ALLTRIM(fdxref.procname))
			m.mtop=PADR(fdxref.procname,LEN(fdxref.symbol))
		ENDDO
		m.mtop=PADR(m.mtop,len(fdxref.procname))
		IF !SEEK(UPPER(m.mtop),"did")
			m.lvl=1	
			ol.additem(ALLTRIM(m.mtop))
			ol.indent[m.cnt]=m.lvl
			m.cnt=m.cnt+1
			DO showit WITH PADR(fdxref.symbol,LEN(fdxref.procname))
		ENDIF
		GO m.MR
	ENDSCAN
	USE IN did
	SET ORDER TO
	set exact &msetexact
RETURN



PROC showit
	Para prg
	priv mr,i
	INSERT INTO did VALUES (UPPER(m.prg))
	seek UPPER(m.prg)
	IF !FOUND() OR m.lvl>=MAXDEPTH
		RETURN
	ENDIF
	lvl=m.lvl+1
	scan while upper(procname) = UPPER(m.prg)
		if flag #'D'

			IF m.lvl>1
				mr=recno()
				mparent=UPPER(procname)
				SKIP
				GO m.mr
			ENDIF
			ol.additem(ALLTRIM(symbol))
			ol.indent[m.cnt]=m.lvl
			m.cnt=m.cnt+1
			I = ASCAN(track,UPPER(TRIM(symbol)))
			IF m.i>0
				ol.additem("Recursion")
				ol.indent[m.cnt]=m.lvl
				m.cnt=m.cnt+1
			ELSE
				mr=recno()
				track[m.lvl]=UPPER(trim(symbol))
				do showit with PADR(symbol,LEN(fdxref.procname))
				track[m.lvl]=""
				go mr
			ENDIF
		endif
	ENDsc
	lvl=m.lvl-1
RETURN


proc gotorec
proc curpos
proc examine
