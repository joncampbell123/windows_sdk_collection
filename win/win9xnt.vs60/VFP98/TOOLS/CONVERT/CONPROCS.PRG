*- CONPROCS.PRG
*- (c) Microsoft Corporation 1995
*
* This is a procedure file of common file and GENSCRN
* manipulation routines. Routines were taken from 
* GENSCRN and GENSCRNX programs, with additional routines added.

#INCLUDE convert.h

#DEFINE c_maxwinds        25
#DEFINE c_maxpops         25
#DEFINE c_maxscreens       5

*!*****************************************************************************
*!       Function: memofind
*!*****************************************************************************
FUNCTION memofind
* ( borrowed and modified from GENSCRNX - wordsearch )
* ( with permission from Ken Levy )
* Parameters:
*	find_str = expression to search for <expC>
*	searchfld = memo field to seach in	<expC>
*	ignoreword = use exact match?		<expL>
*	returnmline = return line number?   <expL>
*	occurance = occurance to effect 	<expN>
*   allafter = return everything		<expL>
*   lNoStrip = don't strip out leading whitespace	<expL>

* Returns:
*	returnmline (.F.) ->> char expression following directive
*	returnmline (.T.) ->> line number of expression (was _MLINE)

PARAMETERS find_str,searchfld,ignoreword,returnmline,occurance,allafter,lNoStrip

PRIVATE memodata,memline,memline2,str_data,lastmline
PRIVATE matchcount,linecount,linecount2,at_mline,at_mline2,mline2
PRIVATE lf_pos,lf_pos2,at_pos

LOCAL m.memodata2

IF TYPE('m.returnmline')=='N'
  m.returnmline=.T.
ENDIF

IF TYPE("m.allafter") # "L"
	m.allafter = .F.
ENDIF

DO CASE
  CASE TYPE('m.occurance')#'N'
    m.occurance=1
  CASE m.occurance<0
    RETURN IIF(m.returnmline,0,C_NULL)
ENDCASE

* check if memo is empty
m.memodata=EVALUATE(m.searchfld)
IF EMPTY(m.searchfld) OR EMPTY(m.memodata) OR m.memodata==C_NULL
    RETURN IIF(m.returnmline,0,C_NULL)
ENDIF

* initialize vars
m.memline2=''
m.lastmline=_MLINE
m.at_mline=0
m.at_mline2=0
m.mline2=0
m.lf_pos=0
m.lf_pos2=0
m.matchcount=0
m.linecount=0
m.linecount2=0

*- be brutal -- strip out all indents and line feeds
IF !lNoStrip
	memodata = CleanWhite(m.memodata)
ENDIF

*-SUSPEND
m.memodata=C_CR+m.memodata
_MLINE=ATC(C_CR+m.find_str,m.memodata)
IF _MLINE=0
	_MLINE=m.lastmline
	RETURN IIF(m.returnmline,0,C_NULL)
ENDIF

m.memodata2 = m.memodata			&& remember it in its pristine form

DO WHILE .T.
  DO CASE
    CASE m.occurance>0 AND _MLINE>=LEN(m.memodata)
      EXIT
    CASE _MLINE>=LEN(m.memodata)
      m.occurance=-1
    OTHERWISE
      m.at_mline=_MLINE
      m.memline=ALLTRIM(MLINE(m.memodata,1,_MLINE))
      m.lf_pos=AT(C_LF,SUBSTR(m.memodata,m.at_mline+1,LEN(m.memline)))
      IF m.lf_pos>0
        m.memline=ALLTRIM(LEFT(m.memline,m.lf_pos-1))
      ENDIF
      
      IF LEN(m.memline) < LEN(m.find_str)+1
        m.str_data = ""
      ELSE
        m.str_data=SUBSTR(m.memline,LEN(m.find_str)+1,1)
      ENDIF
      
      m.at_pos=ATC(m.find_str,m.memline)
      IF m.at_pos#1 OR (!m.ignoreword AND !EMPTY(m.str_data))
        m.at_pos=0
        m.memodata=C_LF+SUBSTR(m.memodata,_MLINE)
        _MLINE=ATC(C_LF+m.find_str,m.memodata)
        IF _MLINE>0
          LOOP
        ENDIF
        m.memodata=C_CR+SUBSTR(m.memodata,2)
        _MLINE=ATC(C_CR+m.find_str,m.memodata)
        IF _MLINE>0
          LOOP
        ENDIF
        IF m.occurance>0
          EXIT
        ENDIF
      ENDIF
      m.matchcount=m.matchcount+1
      IF m.matchcount<m.occurance OR m.occurance=0
        IF m.at_pos=1 AND (m.ignoreword OR EMPTY(m.str_data))
          m.mline2=_MLINE
          m.at_mline2=m.at_mline
          m.memline2=m.memline
          m.lf_pos2=m.lf_pos
          m.linecount2=m.linecount
        ENDIF
        IF BETWEEN(_MLINE,1,LEN(m.memodata))
          _MLINE=_MLINE-2
          m.linecount=m.linecount+_MLINE
          LOOP
        ENDIF
      ENDIF
  ENDCASE
  IF m.occurance<=0
    IF m.mline2=0
      RETURN IIF(m.returnmline,0,C_NULL)
    ENDIF
    _MLINE=m.mline2
    m.at_mline=m.at_mline2
    m.memline=m.memline2
    m.lf_pos=m.lf_pos2
    m.linecount=m.linecount2
    m.occurance=1
  ENDIF
  m.mline2=_MLINE
  _MLINE=m.lastmline
  m.at_pos=0
  m.str_data=SUBSTR(m.memline,LEN(m.find_str)+1)
  IF m.ignoreword AND !LEFT(m.str_data,1)==' ' ;
  	AND !m.allafter
    m.at_pos=AT(' ',m.str_data)
    IF m.at_pos>0
      m.str_data=SUBSTR(m.str_data,m.at_pos+1)
    ENDIF
  ENDIF
  m.str_data=ALLTRIM(m.str_data)
  IF !m.returnmline
    RETURN m.str_data
  ENDIF
  m.returnmline=m.mline2-m.at_mline+1-IIF(m.lf_pos>0,1,0)
  *-RETURN m.at_mline+m.linecount
  RETURN OCCURS(C_CR,LEFT(m.memodata2,m.at_mline+m.linecount))
ENDDO

_MLINE=m.lastmline
RETURN IIF(m.returnmline,0,C_NULL)

* END

*!*****************************************************************************
*!       Function: memostuff
*!*****************************************************************************
FUNCTION memostuff
* ( borrowed and modified from GENSCRNX - wordstuff )
* ( with permission from Ken Levy )
* Parameters:
*	stuff_str = expression to search for 	<expC>
*	searchfld = memo field to seach in		<expC>
*	replace_str = expr to replace with		<exprC>
*	insflag = directive line added/removed	<expL>
*	insbefore = insert at beginning of snippet	<expL>
*	occurance = occurance to effect 		<expN>

* Returns:
*	.T. if successful

PARAMETERS stuff_str,searchfld,replace_str,insflag,insbefore,occurance

PRIVATE var_type,memodata,memline,snptname
PRIVATE at_pos,lf_pos,str_len,remove_str,sub_str

LOCAL cTmp, nLine

IF TYPE('m.insflag')=='N'
  m.insflag=(m.insflag=1)
ENDIF

m.sub_str = IIF(TYPE('m.replace_str')='C',m.replace_str,m.stuff_str)

m.memodata=EVALUATE(m.searchfld)
m.stuff_str=ALLTRIM(m.stuff_str)

* Remove excess CRLF from top of snippet
DO WHILE LEFT(m.memodata,1)==C_CR OR LEFT(m.memodata,1)==C_LF
  m.memodata=SUBSTR(m.memodata,2)
ENDDO
REPLACE (m.searchfld) WITH m.memodata

m.remove_str=m.stuff_str
m.at_pos=AT(' ',m.remove_str)
IF m.at_pos>0
  m.remove_str=ALLTRIM(LEFT(m.remove_str,m.at_pos-1))
ENDIF
m.str_len=0

*- memofind now returns the LINE NUMBER, not the _MLINE position. So calculate
* _MLINE from the line number
nLine = memofind(m.remove_str,m.searchfld,.T.,.T.,m.occurance) - 1
cTmp = MLINE(m.memodata,m.nLine)
m.at_pos = _MLINE + IIF(SUBS(memodata,_MLINE,2) = C_CRLF,2,1)
cTmp = MLINE(m.memodata,m.nLine + 1)
str_len = _MLINE - m.at_pos + 1
*-m.at_pos=memofind(m.remove_str,m.searchfld,.T.,@m.str_len,m.occurance)
IF m.at_pos=0 OR m.str_len=0
  m.at_pos=0
ENDIF

IF m.at_pos>0
  m.memline=SUBSTR(m.memodata,m.at_pos,m.str_len)
  m.lf_pos=AT(C_LF,m.memline)
  IF m.lf_pos>0
    m.str_len=m.lf_pos
  ENDIF
  m.memodata=LEFT(m.memodata,m.at_pos-1)+SUBSTR(m.memodata,m.at_pos+m.str_len)
ENDIF

IF !m.insflag
  IF m.at_pos=0
    RETURN .F.
  ENDIF
  IF UPPER(LEFT(m.searchfld,2))=='M.'
    &searchfld=m.memodata
  ELSE
    REPLACE (m.searchfld) WITH m.memodata
  ENDIF
  RETURN .T.
ENDIF

DO CASE
  CASE m.at_pos>0
    m.stuff_str=LEFT(m.memodata,m.at_pos-1)+m.sub_str+C_CR+;
                SUBSTR(m.memodata,m.at_pos)
  CASE m.insbefore
    IF !EMPTY(m.memodata)
      m.memodata=C_CR+m.memodata
    ENDIF
    m.stuff_str=m.sub_str+m.memodata
  OTHERWISE
    IF !EMPTY(m.memodata) AND !RIGHT(m.memodata,1)==C_CRLF AND ;
       !RIGHT(m.memodata,1)==C_LF
      m.memodata=m.memodata+C_CR
    ENDIF
    DO WHILE RIGHT(m.memodata,1)==C_CR OR RIGHT(m.memodata,1)==C_LF
      m.memodata=LEFT(m.memodata,LEN(m.memodata)-1)
    ENDDO
    m.stuff_str=m.memodata+C_CR+m.sub_str+C_CR
ENDCASE
DO WHILE RIGHT(m.stuff_str,1)==C_CR OR RIGHT(m.stuff_str,1)==C_LF
  m.stuff_str=LEFT(m.stuff_str,LEN(m.stuff_str)-1)
ENDDO
m.stuff_str=m.stuff_str+C_CR
IF UPPER(LEFT(m.searchfld,2))=='M.'
  &searchfld=m.stuff_str
ELSE
  REPLACE (m.searchfld) WITH m.stuff_str
ENDIF
RETURN .T.

* END wordstuff


*!*****************************************************************************
*!       Function: GENPROC
*!*****************************************************************************
PROCEDURE genproc


*PRIVATE ALL LIKE g_*

* g_firstproc holds the line number of the first PROCEDURE or FUNCTION in
* the cleanup snippet of each screen.
DIMENSION g_firstproc[C_MAXSCREENS]
g_firstproc = 0

DIMENSION g_platlist[C_MAXPLATFORMS]
g_platlist[1] = c_dos
g_platlist[2] = c_windows
g_platlist[3] = c_mac
g_platlist[4] = c_unix

DIMENSION g_procs[1,C_MAXPLATFORMS+3]
* First column is a procedure name
* Second through n-th column is the line number in the cleanup snippet where
*    a procedure with this name starts.
* C_MAXPLATFORMS+2 column is a 1 if this procedure has been emitted.
* C_MAXPLATFORMS+3 column holds the parameter statement, if any.
* One row for each unique procedure name found in the cleanup snippet for any platform.
g_procs = -1
g_procs[1,1] = ""
g_procs[1,C_MAXPLATFORMS+3] = ""
g_procnames = 0   && the number we've found so far
g_tabchr = ""

SCAN FOR objtype = 1 AND isgenplat(platform)
    DO updprocarray
ENDSCAN

SCAN FOR objtype = 1 AND isgenplat(platform)
    IF EMPTY(proccode)
       LOOP
    ENDIF
    DO extractprocs WITH 1
ENDSCAN

RETURN

*!*****************************************************************************
*!       Function: GETFIRSTLINE
*!*****************************************************************************
FUNCTION getfirstline
*)
*) Find first line # in snippet for:
*) 1. PROCEDURE or FUNCTION statement in a cleanup
*) 2. PARAMETER statement
*) 3. SECTION1/2
PARAMETER m.snipname,m.sniptype,m.sectnum
PRIVATE proclineno, numlines, word1
_MLINE = 0
m.numlines = MEMLINES(&snipname)
FOR m.proclineno = 1 TO m.numlines
   m.line  = MLINE(&snipname, 1, _MLINE)
   DO killcr WITH m.line
   m.line  = UPPER(LTRIM(STRTRAN(m.line,C_TAB,' ')))
   m.word1 = wordnum(m.line,1)
   DO CASE
   CASE m.sniptype = "PARM"
     IF !EMPTY(m.word1) AND match(m.word1,"PARAMETERS")
       RETURN m.proclineno
     ENDIF
   CASE m.sniptype = "SECT"
     IF !EMPTY(m.word1) AND LEFT(m.line,5) = "#SECT" ; 
   		AND AT(m.sectnum,m.line) # 0
       RETURN m.proclineno
     ENDIF
   CASE m.sniptype = "PROC"
     IF !EMPTY(m.word1) AND (match(m.word1,"PROCEDURE") OR match(m.word1,"FUNCTION"))
       RETURN m.proclineno
     ENDIF
   ENDCASE  
ENDFOR
RETURN 0


*!*****************************************************************************
*!       Function: wordnum
*!*****************************************************************************
FUNCTION wordnum
*)
*) WORDNUM - Returns w_num-th word from string strg
*)
PARAMETERS m.strg,m.w_num
PRIVATE strg,s1,w_num,ret_str

m.s1 = ALLTRIM(m.strg)

* Replace tabs with spaces
m.s1 = CHRTRANC(m.s1,CHR(9)," ")

* Reduce multiple spaces to a single space
DO WHILE AT('  ',m.s1) > 0
   m.s1 = STRTRAN(m.s1,'  ',' ')
ENDDO

ret_str = ""
DO CASE
CASE m.w_num > 1
   DO CASE
   CASE AT(" ",m.s1,m.w_num-1) = 0   && No word w_num.  Past end of string.
      m.ret_str = ""
   CASE AT(" ",m.s1,m.w_num) = 0     && Word w_num is last word in string.
      m.ret_str = SUBSTR(m.s1,AT(" ",m.s1,m.w_num-1)+1,255)
   OTHERWISE                         && Word w_num is in the middle.
      m.strt_pos = AT(" ",m.s1,m.w_num-1)
      m.ret_str  = SUBSTR(m.s1,strt_pos,AT(" ",m.s1,m.w_num)+1 - strt_pos)
   ENDCASE
CASE m.w_num = 1
   IF AT(" ",m.s1) > 0               && Get first word.
      m.ret_str = SUBSTR(m.s1,1,AT(" ",m.s1)-1)
   ELSE                              && There is only one word.  Get it.
      m.ret_str = m.s1
   ENDIF
ENDCASE
RETURN ALLTRIM(m.ret_str)

*!*****************************************************************************
*!      PROCEDURE: KILLCR
*!*****************************************************************************
PROCEDURE killcr
PARAMETER m.strg
IF _MAC
   m.strg = CHRTRANC(m.strg,CHR(13)+CHR(10),"")
ENDIF
RETURN

*!*****************************************************************************
*!       Function: MATCH
*!*****************************************************************************
FUNCTION match
*)
*) MATCH - Returns TRUE if candidate is a valid 4-or-more-character abbreviation of keyword
*)
PARAMETER m.candidate, m.keyword
PRIVATE m.in_exact, m.retlog

m.in_exact = SET("EXACT")
SET EXACT OFF
DO CASE
CASE EMPTY(m.candidate)
   m.retlog = EMPTY(m.keyword)
CASE LEN(m.candidate) < 4
   m.retlog = IIF(m.candidate == m.keyword,.T.,.F.)
OTHERWISE
   m.retlog = IIF(m.keyword = m.candidate,.T.,.F.)
ENDCASE
IF m.in_exact != "OFF"
   SET EXACT ON
ENDIF

RETURN m.retlog

*!*****************************************************************************
*!
*!       Function: GETPARAM
*!
*!      Called by: CHECKPARAM()       (function  in GENSCRN.PRG)
*!
*!          Calls: ISCOMMENT()        (function  in GENSCRN.PRG)
*!               : WORDNUM()          (function  in GENSCRN.PRG)
*!               : MATCH()            (function  in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION getparam
*)
*) GETPARAM - Return the PARAMETER statement from a setup snippet, if one is there
*)
PARAMETER m.snipname
PRIVATE m.i, m.thisparam, m.numlines, m.thisline, m.word1, m.contin

m.g_dblampersand = CHR(38) + CHR(38)   && used in some tight loops.  Concatenate just once here.

* Do a quick check to see if we need to search further.
IF ATC("PARA",&snipname) = 0
   RETURN ""
ENDIF

m.numlines = MEMLINES(&snipname)
_MLINE = 0
m.i = 1
DO WHILE m.i <= m.numlines
   m.thisline = UPPER(LTRIM(MLINE(&snipname, 1, _MLINE)))
   DO killcr WITH m.thisline

   * Drop any double-ampersand comment
   IF AT(m.g_dblampersand,m.thisline) > 0
      m.thisline = LEFT(m.thisline,AT(m.g_dblampersand,m.thisline)-1)
   ENDIF

   IF !EMPTY(m.thisline) AND !iscomment(@thisline)
      * See if the first non-blank, non-comment, non-directive, non-EXTERNAL
      * line is a #SECTION 1
      DO CASE
      CASE LEFT(m.thisline,5) = "#SECT" AND AT('1',m.thisline) <> 0
         * Read until we find a #SECTION 2, the end of the snippet or a
         * PARAMETER statement.
         DO WHILE m.i <= m.numlines
            m.thisline = UPPER(LTRIM(MLINE(&snipname, 1, _MLINE)))
            DO killcr WITH m.thisline

            * Drop any double-ampersand comment
            IF AT(m.g_dblampersand,m.thisline) > 0
               m.thisline = LEFT(m.thisline,AT(m.g_dblampersand,m.thisline)-1)
            ENDIF

            m.word1 = wordnum(CHRTRANC(m.thisline,CHR(9)+';',' '),1)
            DO CASE
            CASE match(m.word1,"PARAMETERS")

               * Replace tabs with spaces
               m.thisline = LTRIM(CHRTRANC(m.thisline,CHR(9)," "))

               * Process continuation lines.  Replace tabs in incoming lines with spaces.
               DO WHILE RIGHT(RTRIM(m.thisline),1) = ';'
                  m.thisline = m.thisline + ' '+ CHR(13)+CHR(10)+CHR(9)
                  m.contin = MLINE(&snipname, 1, _MLINE)
                  DO killcr WITH m.contin
                  m.contin = CHRTRANC(LTRIM(m.contin),CHR(9)," ")
                  m.thisline = m.thisline + UPPER(m.contin)
               ENDDO

               * Clean up the parameters so that minor differences in
               * spacing don't cause the comparisons to fail.

               * Take the parameters but not the PARAMETER keyword itself
               m.thisparam = SUBSTR(m.thisline,AT(' ',m.thisline)+1)
               DO WHILE INLIST(LEFT(m.thisparam,1),CHR(10),CHR(13),CHR(9),' ')
                  m.thisparam = SUBSTR(m.thisparam,2)
               ENDDO

               * Force single spacing in the param string
               DO WHILE AT('  ',m.thisparam) > 0
                  m.thisparam = STRTRAN(m.thisparam,'  ',' ')
               ENDDO

               * Drop "m." designations so that they don't make the variables look different
               m.thisparam = STRTRAN(m.thisparam,'m.','')
               m.thisparam = STRTRAN(m.thisparam,'M.','')
               m.thisparam = STRTRAN(m.thisparam,'m->','')
               m.thisparam = STRTRAN(m.thisparam,'M->','')

               RETURN LOWER(m.thisparam)
            CASE LEFT(m.thisline,5) = "#SECT" AND AT('2',m.thisline) <> 0
               * No parameter statement, since we found #SECTION 2 first
               RETURN ""
            ENDCASE
            m.i = m.i + 1
         ENDDO
      CASE LEFT(m.thisline,1) = "#"   && some other directive
         * Do nothing.  Get next line.
      CASE match(wordnum(m.thisline,1),"EXTERNAL")
         * Ignore it.  This doesn't disqualify a later statement from being a PARAMETER
         * statement.
      OTHERWISE
         * no #SECTION 1, so no parameters
         RETURN ""
      ENDCASE
   ENDIF
   m.i = m.i + 1
ENDDO
RETURN ""


*!*****************************************************************************
*!
*!       Function: ISCOMMENT
*!
*!      Called by: WRITECODE          (procedure in GENSCRN.PRG)
*!               : WRITELINE          (procedure in GENSCRN.PRG)
*!               : ADDTOCTRL          (procedure in GENSCRN.PRG)
*!               : GETPARAM()         (function  in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION iscomment
*)
*) ISCOMMENT - Determine if textline is a comment line.
*)
PARAMETER m.textline
PRIVATE m.asterisk, m.isnote, m.ampersand, m.statement
IF EMPTY(m.textline)
   RETURN .F.
ENDIF
m.statement = UPPER(LTRIM(m.textline))

m.asterisk  = AT("*", m.statement)
m.ampersand = AT(m.g_dblampersand, m.statement)
m.isnote    = AT("NOTE", m.statement)

DO CASE
CASE (m.asterisk = 1 OR m.ampersand = 1)
   RETURN .T.
CASE (m.isnote = 1 ;
      AND (LEN(m.statement) <= 4 OR SUBSTR(m.statement,5,1) = ' '))
   * Don't be fooled by something like "notebook = 7"
   RETURN .T.
ENDCASE
RETURN .F.

*!*****************************************************************************
*!
*!       Function: ISPARAMETER
*!
*!      Called by: WRITECODE          (procedure in GENSCRN.PRG)
*!
*!          Calls: MATCH()            (function  in GENSCRN.PRG)
*!               : WORDNUM()          (function  in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION isparameter
*)
*) ISPARAMETER - Determine if strg is a PARAMETERS statement
*)
PARAMETER m.strg
PRIVATE m.ispar
m.ispar = .F.
IF !EMPTY(strg) AND match(CHRTRANC(wordnum(strg,1),';',''),"PARAMETERS")
   m.ispar = .T.
ENDIF
RETURN m.ispar


*!*****************************************************************************
*!
*!       Function: PROCSMATCH
*!
*!*****************************************************************************
FUNCTION procsmatch
*)
*) PROCSMATCH - Are the CRCs for the cleanup snippets the same for all platforms in the
*)                current screen that are being generated?
*)
PRIVATE m.crccode, m.thiscode, m.in_rec

m.in_rec = IIF(!EOF(),RECNO(),1)
m.crccode = "0"
* Get the headers for all the platforms we are generating
SCAN FOR objtype = 1 AND isgenplat(platform)
   m.thiscode = ALLTRIM(SYS(2007,proccode))
   DO CASE
   CASE m.crccode = "0"
      m.crccode = m.thiscode
   CASE m.thiscode <> m.crccode AND m.crccode <> "0"
      RETURN .F.
   ENDCASE
ENDSCAN
GOTO m.in_rec
RETURN .T.

*!*****************************************************************************
*!
*!       Function: ISGENPLAT
*!
*!      Called by: GENPROCEDURES      (procedure in GENSCRN.PRG)
*!               : PROCSMATCH()       (function  in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION isgenplat
*)
*) ISGENPLAT - Is this platform one of the ones being generated?
*)
PARAMETER m.platname
RETURN IIF(ASCAN(g_platforms,ALLTRIM(UPPER(m.platname))) > 0, .T. , .F. )



*!*****************************************************************************
*!
*!       Function: CLEANPARAM
*!
*!*****************************************************************************
FUNCTION cleanparam
*)
*) CLEANPARAM - Clean up a parameter string so that it may be compared with another one.
*)              This function replaces tabs with spaces, capitalizes the string, merges
*)              forces single spacing, and strips out CR/LF characters.
*)
PARAMETER m.p, m.cp
m.cp = UPPER(ALLTRIM(CHRTRANC(m.p,";"+CHR(13)+CHR(10),"")))   && drop CR/LF and continuation chars
m.cp = CHRTRANC(m.cp,CHR(9),' ')   && tabs to spaces
DO WHILE AT('  ',m.cp) > 0         && reduce multiple spaces to a single space
   m.cp = STRTRAN(m.cp,'  ',' ')
ENDDO
DO WHILE AT(', ',m.cp) > 0         && drop spaces after commas
   m.cp = STRTRAN(m.cp,', ',',')
ENDDO
RETURN m.cp

*!*****************************************************************************
*!
*!      Procedure: ADDPROCNAME
*!
*!      Called by: UPDPROCARRAY       (procedure in GENSCRN.PRG)
*!
*!          Calls: GETPLATNUM()       (function  in GENSCRN.PRG)
*!
*!*****************************************************************************
PROCEDURE addprocname
*)
*) ADDPROCNAME - Update g_procs with pname data
*)
PARAMETER m.pname, m.platname, m.linenum, m.lastmline
PRIVATE m.rnum, m.platformcol, m.i, m.j
IF EMPTY(m.pname)
   RETURN
ENDIF

* Look up this name in the procedures array
m.rnum = 0
FOR m.i = 1 TO m.g_procnames
   IF g_procs[m.i,1] == m.pname
      m.rnum = m.i
      EXIT
   ENDIF
ENDFOR

IF m.rnum = 0
   * New name
   g_procnames = m.g_procnames + 1
   DIMENSION g_procs[m.g_procnames,C_MAXPLATFORMS+3]
   g_procs[m.g_procnames,1] = UPPER(ALLTRIM(m.pname))
   FOR m.j = 1 TO c_maxplatforms
      g_procs[m.g_procnames,m.j + 1] = -1
   ENDFOR
   g_procs[m.g_procnames,C_MAXPLATFORMS+2] = .F.   && not emitted yet
   g_procs[m.g_procnames,C_MAXPLATFORMS+3] = ""    && parameter statement
   m.rnum = m.g_procnames
ENDIF

m.platformcol = getplatnum(m.platname) + 1
IF m.platformcol > 1
   g_procs[m.rnum, m.platformcol] = m.lastmline
ENDIF
RETURN

*!*****************************************************************************
*!
*!       Function: GETPLATNUM
*!
*!      Called by: PREPWNAMES         (procedure in GENSCRN.PRG)
*!               : ADDPROCNAME        (procedure in GENSCRN.PRG)
*!               : WRITECODE          (procedure in GENSCRN.PRG)
*!               : WRITELINE          (procedure in GENSCRN.PRG)
*!               : ADDTOCTRL          (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION getplatnum
*)
*) GETPLATNUM - Return the g_platlist array index given a platform name
*)
PARAMETER m.platname
PRIVATE m.i
FOR m.i = 1 TO c_maxplatforms
   IF g_platlist[m.i] == UPPER(ALLTRIM(m.platname))
      RETURN m.i
   ENDIF
ENDFOR
RETURN 0



*!*****************************************************************************
*!
*!      Procedure: EXTRACTPROCS
*!
*!*****************************************************************************
PROCEDURE extractprocs
*)
*) EXTRACTPROCS - Output the procedures for the current platform in the current screen
*)
* We only get here if we are emitting for multiple platforms and the cleanup snippets
* for all platforms are not identical.  We are positioned on a screen header record for
* the g_genvers platform.

*- NOTE: Also called if multiple procs in a VALID or other snippet
*- If passed, snipname is memo field to go through. Otherwise, set to "proccode"

PARAMETER m.scrnno, m.snipname

PRIVATE m.hascontin, m.iscontin, m.sniplen, m.i, m.thisline, m.pnum, m.word1, m.word2

IF PARAMETERS() = 1
	m.snipname = "proccode"
ENDIF

_MLINE = 0
m.sniplen   = LEN(&snipname)
m.numlines  = MEMLINES(&snipname)
m.hascontin = .F.
DO WHILE _MLINE < m.sniplen
   m.thisline  = UPPER(ALLTRIM(MLINE(&snipname,1, _MLINE)))
   DO killcr WITH m.thisline
   m.iscontin  = m.hascontin
   m.hascontin = RIGHT(m.thisline,1) = ';'
   IF LEFT(m.thisline,1) $ "PF" AND !m.iscontin
      m.word1 = wordnum(m.thisline, 1)
      IF match(m.word1,"PROCEDURE") OR match(m.word1,"FUNCTION")
         m.word2 = wordnum(m.thisline,2)
         * Does this procedure have a name conflict?
         IF PARAMETERS() = 1
			m.pnum = getprocnum(m.word2)
	        IF pnum > 0 
	            DO CASE
		            CASE g_procs[m.pnum,C_MAXPLATFORMS+2]
		               * This one has already been generated.  Skip past it now.
		               DO emitproc WITH .F., m.thisline, m.sniplen, m.scrnno, m.snipname
		               LOOP
		            CASE hasconflict(m.pnum)
		               * Name collision detected.  Output bracketed code for all platforms
		               DO emitbracket WITH m.pnum, m.scrnno
		            OTHERWISE
		               * This procedure has no name collision and has not been emitted yet.
		               DO emitproc WITH .T., m.thisline, m.sniplen, m.scrnno, m.snipname
	            ENDCASE
	            g_procs[pnum,C_MAXPLATFORMS+2] = .T.
	        ENDIF &&  pnum > 0
	     ELSE
	       *- special case, called for VALID or WHEN
	       *- Welcome to Kludge City
           g_tabchr = ""
			DIMENSION g_platforms[1]
			STORE "" TO g_platforms
			DIMENSION g_platlist[C_MAXPLATFORMS]
			g_platlist[1] = c_dos
			g_platlist[2] = c_windows
			g_platlist[3] = c_mac
			g_platlist[4] = c_unix
		   DO emitproc WITH .T., m.thisline, m.sniplen, m.scrnno, m.snipname
         ENDIF && PARAMETERS() = 1
      ENDIF
   ENDIF
ENDDO
RETURN

*!*****************************************************************************
*!
*!      Procedure: EMITPROC
*!
*!      Called by: EXTRACTPROCS       (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
PROCEDURE emitproc
*)
*) EMITPROC - Scan through the next procedure/function in the current cleanup snippet.
*)            If dowrite is TRUE, emit the code as we go.  Otherwise, just skip over it
*)            and advance _MLINE.
*)
* We are positioned on the PROCEDURE or FUNCTION line now and there isn't a name
* conflict.
PARAMETER m.dowrite, m.thisline, m.sniplen, m.scrnno, m.snipname
PRIVATE m.word1, m.word2, m.line, m.upline, m.done, m.lastmline, ;
   m.iscontin, m.hascontin, m.platnum

m.hascontin = .F.
m.done = .F.

* Write the PROCEDURE/FUNCTION statement
m.upline = UPPER(ALLTRIM(CHRTRANC(m.thisline,chr(9),' ')))

m.g_genvers = g_platforms[1]

m.platnum = getplatnum(m.g_genvers)

IF m.dowrite    && actually emit the procedure?
   DO writeline WITH m.thisline, m.g_genvers, m.platnum, m.upline, m.scrnno
ENDIF

* Write the body of the procedure
DO WHILE !m.done AND _MLINE < m.sniplen
   m.lastmline = _MLINE          && note where this line started

   m.line = MLINE(&snipname,1, _MLINE)
   DO killcr WITH m.line
   m.upline = UPPER(ALLTRIM(CHRTRANC(m.line,chr(9),' ')))

   m.iscontin = m.hascontin
   m.hascontin = RIGHT(m.upline,1) = ';'
   IF LEFT(m.upline,1) $ "PF" AND !m.iscontin
      m.word1 = wordnum(m.upline, 1)
      IF match(m.word1,"PROCEDURE") OR match(m.word1,"FUNCTION")
         done = .T.
         _MLINE = m.lastmline    && drop back one line and stop writing
         LOOP
      ENDIF
   ENDIF

   IF m.dowrite    && actually emit the procedure?
      DO writeline WITH m.line, m.g_genvers, m.platnum, m.upline, m.scrnno
   ENDIF

ENDDO
RETURN && emitproc

*!*****************************************************************************
*!
*!      Procedure: EMITBRACKET
*!
*!      Called by: EXTRACTPROCS       (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
PROCEDURE emitbracket
*)
*) EMITBRACKET - Emit DO CASE/CASE _DOS brackets and call putproc to emit code for this procedure
*)
PARAMETER m.pnum, m.scrnno
PRIVATE m.word1, m.word2, m.line, m.upline, m.done, m.lastmline, ;
   m.iscontin, m.hascontin, m.i
m.hascontin = .F.
m.done = .F.

REPLACE _FOX3SPR.SPRMEMO WITH C_CRLF +;
  "PROCEDURE "+g_procs[m.pnum,1]+ C_CRLF ADDITIVE

IF !EMPTY(g_procs[m.pnum,C_MAXPLATFORMS+3])

 REPLACE _FOX3SPR.SPRMEMO WITH ;
   "PARAMETERS "+g_procs[m.pnum,C_MAXPLATFORMS+3]+ C_CRLF ADDITIVE

ENDIF

REPLACE _FOX3SPR.SPRMEMO WITH "DO CASE" + C_CRLF ADDITIVE

* Peek ahead and get the parameter statement
FOR m.platnum = 1 TO c_maxplatforms
   IF g_procs[m.pnum,m.platnum+1] < 0
      * There was no procedure for this platform
      LOOP
   ENDIF
   
   REPLACE _FOX3SPR.SPRMEMO WITH "CASE "+"_"+g_platlist[m.platnum]+ C_CRLF ADDITIVE
   m.g_tabchr=C_TAB
   DO putproc WITH m.platnum, m.pnum, m.scrnno
   m.g_tabchr=""
ENDFOR

REPLACE _FOX3SPR.SPRMEMO WITH "ENDCASE" + C_CRLF ADDITIVE

RETURN

*!*****************************************************************************
*!
*!      Procedure: PUTPROC
*!
*!      Called by: EMITBRACKET        (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
PROCEDURE putproc
*)
*) PUTPROC - Write actual code for procedure procnum in platform platnum
*)
PARAMETER m.platnum, m.procnum, m.scrnno
PRIVATE m.in_rec, m.oldmine, m.done, m.line, m.upline, m.iscontin, m.hascontin, ;
   m.word1, m.word2, m.platnum

m.in_rec    = RECNO()
* Store the _MLINE position in the original snippet
m.oldmline  = _MLINE
m.hascontin = .F.       && the previous line was not a continuation line.
LOCATE FOR platform = g_platlist[m.platnum] AND objtype = 1
IF FOUND()
   * go to the PROCEDURE/FUNCTION statement
   _MLINE = g_procs[m.procnum,m.platnum+1]
   * Skip the PROCEDURE line, since we've already output one.
   m.line = MLINE(proccode,1, _MLINE)
   DO killcr WITH m.line

   * We are now positioned at the line following the procedure statement.
   * Write until the end of the snippet or the next procedure.
   m.done = .F.
   DO WHILE !m.done
      m.line = MLINE(proccode,1, _MLINE)
      DO killcr WITH m.line
      m.upline = UPPER(ALLTRIM(CHRTRANC(m.line,chr(9),' ')))
      m.iscontin = m.hascontin
      m.hascontin = RIGHT(m.upline,1) = ';'
      IF LEFT(m.upline,1) $ "PF" AND !m.iscontin
         m.word1 = wordnum(m.upline, 1)
         IF RIGHT(m.word1,1) = ';'
            m.word1 = LEFT(m.word1,LEN(m.word1)-1)
         ENDIF

         DO CASE
         CASE match(m.word1,"PROCEDURE") OR match(m.word1,"FUNCTION")
            * Stop when we encounter the next snippet
            m.done = .T.
            LOOP
         CASE match(m.word1,"PARAMETERS")
            * Don't output it, but keep scanning for other code
            DO WHILE m.hascontin
               m.line = MLINE(proccode,1, _MLINE)
               DO killcr WITH m.line
               m.upline = UPPER(ALLTRIM(CHRTRANC(m.line,chr(9),' ')))
               m.hascontin = RIGHT(m.upline,1) = ';'
            ENDDO
            LOOP
         ENDCASE
      ENDIF

      DO writeline WITH m.line, g_platlist[m.platnum], m.platnum, m.upline, m.scrnno

      * Stop if we've run out of snippet
      IF _MLINE >= LEN(proccode)
         m.done = .T.
      ENDIF
   ENDDO
ENDIF

GOTO m.in_rec
* Restore the _MLINE position in the main snippet we are outputing
_MLINE = m.oldmline
RETURN



*!*****************************************************************************
*!
*!      Procedure: UPDPROCARRAY
*!
*!*****************************************************************************
PROCEDURE updprocarray
*)
*) UPDPROCARRAY - Pick out the procedures names in the current cleanup snippet and call
*)                  AddProcName to update the g_procs array.
*)
PRIVATE m.i, m.numlines, m.line, m.upline, m.word1, m.word2, m.iscontin, m.hascontin, ;
   m.lastmline, m.thisproc

_MLINE = 0
m.numlines = MEMLINES(proccode)
m.hascontin = .F.
FOR m.i = 1 TO m.numlines
   m.lastmline = _MLINE                && note starting position of this line
   m.line      = MLINE(proccode,1, _MLINE)
   DO killcr WITH m.line
   m.upline    = UPPER(ALLTRIM(m.line))
   m.iscontin  = m.hascontin
   m.hascontin = RIGHT(m.upline,1) = ';'
   IF LEFT(m.upline,1) $ "PF" AND !m.iscontin
      m.word1 = CHRTRANC(wordnum(m.upline, 1),';','')
      DO CASE
      CASE match(m.word1,"PROCEDURE") OR match(m.word1,"FUNCTION")
         m.word2 = wordnum(m.upline,2)
         DO addprocname WITH m.word2, platform, m.i, m.lastmline
         m.lastproc = m.word2
      CASE match(m.word1,"PARAMETERS")
         * Associate this parameter statement with the last procedure or function
         m.thisproc = getprocnum(m.lastproc)
         IF m.thisproc > 0
            m.thisparam = ALLTRIM(SUBSTR(m.upline,AT(' ',m.upline)+1))
            * Deal with continued PARAMETER lines
            DO WHILE m.hascontin AND m.i <= m.numlines
               m.lastmline = _MLINE                && note the starting position of this line
               m.line   = MLINE(proccode,1, _MLINE)
               DO killcr WITH m.line
               m.upline = UPPER(ALLTRIM(CHRTRANC(m.line,chr(9),' ')))
               m.thisparam = ;
                  m.thisparam + CHR(13)+CHR(10) + m.line
               m.hascontin = RIGHT(m.upline,1) = ';'
               m.i = m.i + 1
            ENDDO
            * Make sure that this parameter matches any others we've seen for this function
            DO CASE
            CASE EMPTY(g_procs[m.thisproc,C_MAXPLATFORMS+3])
               * First occurrence, or one platform has a parameter statement and another doesn't
               g_procs[m.thisproc,C_MAXPLATFORMS+3] = m.thisparam
            CASE cleanparam(m.thisparam) = cleanparam(g_procs[m.thisproc,C_MAXPLATFORMS+3])
               * The new one is a superset of the existing one.  Use the longer one.
               g_procs[m.thisproc,C_MAXPLATFORMS+3] = m.thisparam
            ENDCASE
         ENDIF
      ENDCASE
   ENDIF
ENDFOR
RETURN



*!*****************************************************************************
*!
*!       Function: GETPROCNUM
*!
*!      Called by: EXTRACTPROCS       (procedure in GENSCRN.PRG)
*!               : UPDPROCARRAY       (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION getprocnum
*)
*) GETPROCNUM - Return the g_procs array position of the procedure named pname
*)
PARAMETER m.pname
PRIVATE m.i
FOR m.i = 1 TO g_procnames
   IF g_procs[m.i,1] == m.pname
      RETURN m.i
   ENDIF
ENDFOR
RETURN  0


*!*****************************************************************************
*!
*!       Function: CLEANPARAM
*!
*!*****************************************************************************
FUNCTION cleanparam
*)
*) CLEANPARAM - Clean up a parameter string so that it may be compared with another one.
*)              This function replaces tabs with spaces, capitalizes the string, merges
*)              forces single spacing, and strips out CR/LF characters.
*)
PARAMETER m.p, m.cp
m.cp = UPPER(ALLTRIM(CHRTRANC(m.p,";"+CHR(13)+CHR(10),"")))   && drop CR/LF and continuation chars
m.cp = CHRTRANC(m.cp,CHR(9),' ')   && tabs to spaces
DO WHILE AT('  ',m.cp) > 0         && reduce multiple spaces to a single space
   m.cp = STRTRAN(m.cp,'  ',' ')
ENDDO
DO WHILE AT(', ',m.cp) > 0         && drop spaces after commas
   m.cp = STRTRAN(m.cp,', ',',')
ENDDO
RETURN m.cp

*!*****************************************************************************
*!
*!      Procedure: WRITELINE
*!
*!      Called by: EMITPROC           (procedure in GENSCRN.PRG)
*!               : PUTPROC            (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
PROCEDURE writeline
*)
*) WRITELINE - Emit a single line
*)
PARAMETER m.line, m.platname, m.platnum, m.upline, m.scrnno
PRIVATE m.at, m.expr

   * This code relies upon partial matching (e.g., "*! Comment" will equal "*")
  DO CASE
	CASE m.upline = "*"
	  IF !(m.upline = "*!" OR m.upline = "*:")
		REPLACE _FOX3SPR.SPRMEMO WITH m.g_tabchr + m.line+ C_CRLF ADDITIVE
	  ENDIF
	CASE m.upline = "#"
	   * don't output a generator directive, but #DEFINES are OK
	   IF LEFT(m.upline,5) = "#DEFI" ;
			OR LEFT(m.upline,3) = "#IF" ;
			OR LEFT(m.upline,5) = "#ELSE" ;
			OR LEFT(m.upline,6) = "#ENDIF"
		REPLACE _FOX3SPR.SPRMEMO WITH m.g_tabchr + m.line+ C_CRLF ADDITIVE
	   ENDIF
	OTHERWISE
	   REPLACE _FOX3SPR.SPRMEMO WITH m.g_tabchr + m.line+ C_CRLF ADDITIVE
  ENDCASE
RETURN


*!*****************************************************************************
*!
*!       Function: HASCONFLICT
*!
*!      Called by: EXTRACTPROCS       (procedure in GENSCRN.PRG)
*!
*!*****************************************************************************
FUNCTION hasconflict
*)
*) HASCONFLICT - Is there a name collision for procedure number num?
*)
PARAMETER m.num
PRIVATE m.i, m.cnt
m.cnt = 0
FOR m.i = 1 TO c_maxplatforms
   IF g_procs[m.num,m.i+1] > 0
      m.cnt = m.cnt +1
   ENDIF
ENDFOR
RETURN IIF(m.cnt > 1,.T.,.F.)

*!*****************************************************************************
*!
*!      Procedure: GETARRANGE
*!
*!*****************************************************************************
PROCEDURE getarrange
PARAMETER m.astring,m.curplat,m.arrange_flag, m.center_flag, m.row, m.col, m.lscxcenter
PRIVATE m.j, m.pname, m.entries
IF !EMPTY(m.astring)
	m.entries = INT(LEN(m.astring)/26)
	m.center_flag = m.lscxcenter
	FOR m.j = 1 TO m.entries
		m.pname = ALLTRIM(UPPER(SUBSTR(m.astring,(m.j-1)*26+1,8)))
		m.pname = ALLTRIM(CHRTRANC(m.pname,CHR(0)," "))
		IF m.pname == m.curplat	&& found the right one-platform
			IF INLIST(UPPER(SUBSTR(m.astring,(m.j-1)*26 + 9,1)),'Y','T')		&& is it arranged?
				m.arrange_flag = .T.
				IF INLIST(UPPER(SUBSTR(m.astring,(m.j-1)*26 + 10,1)),'Y','T')	&& is it centered?
					m.center_flag = .T.
				ELSE
					m.center_flag = .F.
					m.row = VAL(SUBSTR(m.astring,(m.j-1)*26 + 11,8))
					m.col = VAL(SUBSTR(m.astring,(m.j-1)*26 + 19,8))
				ENDIF
			ENDIF
			EXIT
		ENDIF
	NEXT
ENDIF

RETURN

******************************************************************************
******************************************************************************
* Misc Generic File Utility Routines 
******************************************************************************
******************************************************************************

*!*****************************************************************************
*!       Function: STRIPEXT
*!*****************************************************************************
FUNCTION stripext
*)
*) STRIPEXT - Strip the extension from a file name.
*)
*) Description:
*) Use the algorithm employed by FoxPRO itself to strip a
*) file of an extension (if any): Find the rightmost dot in
*) the filename.  If this dot occurs to the right of a "\"
*) or ":", then treat everything from the dot rightward
*) as an extension.  Of course, if we found no dot,
*) we just hand back the filename unchanged.
*)
*) Parameters:
*) filename - character string representing a file name
*)
*) Return value:
*) The string "filename" with any extension removed
*)
PARAMETER m.filename
PRIVATE m.dotpos, m.terminator
m.dotpos = RAT(".", m.filename)
m.terminator = MAX(RAT("\", m.filename), RAT(":", m.filename))
IF m.dotpos > m.terminator
   m.filename = LEFT(m.filename, m.dotpos-1)
ENDIF
RETURN m.filename

*!*****************************************************************************
*!       Function: STRIPPATH
*!*****************************************************************************
FUNCTION strippath
*)
*) STRIPPATH - Strip the path from a file name.
*)
*) Description:
*) Find positions of backslash in the name of the file.  If there is one
*) take everything to the right of its position and make it the new file
*) name.  If there is no slash look for colon.  Again if found, take
*) everything to the right of it as the new name.  If neither slash
*) nor colon are found then return the name unchanged.
*)
*) Parameters:
*) filename - character string representing a file name
*)
*) Return value:
*) The string "filename" with any path removed
*)
PARAMETER m.filename
PRIVATE m.slashpos, m.namelen, m.colonpos
m.slashpos = RAT("\", m.filename)
IF m.slashpos > 0
   m.namelen  = LEN(m.filename) - m.slashpos
   m.filename = RIGHT(m.filename, m.namelen)
ELSE
   m.colonpos = RAT(":", m.filename)
   IF m.colonpos > 0
      m.namelen  = LEN(m.filename) - m.colonpos
      m.filename = RIGHT(m.filename, m.namelen)
   ENDIF
ENDIF
RETURN m.filename

*!*****************************************************************************
*!       Function: STRIPCR
*!*****************************************************************************
FUNCTION stripcr
*)
*) STRIPCR - Strip off terminating carriage returns and line feeds
*)
PARAMETER m.strg
* Don't use a CHRTRANC since it's remotely possible that the CR or LF might
* be in a user's quoted string.
strg = ALLTRIM(strg)
i = LEN(strg)
DO WHILE i >= 0 AND INLIST(SUBSTR(strg,i,1),CHR(13),CHR(10))
   i = i - 1
ENDDO
RETURN LEFT(strg,i)

*!*****************************************************************************
*!       Function: ADDBS
*!*****************************************************************************
FUNCTION addbs
*)
*) ADDBS - Add a backslash unless there is one already there.
*)
PARAMETER m.pathname
PRIVATE m.separator
m.separator = IIF(_MAC,":","\")
m.pathname = ALLTRIM(UPPER(m.pathname))
IF !(RIGHT(m.pathname,1) $ '\:') AND !EMPTY(m.pathname)
   m.pathname = m.pathname + m.separator
ENDIF
RETURN m.pathname

*!*****************************************************************************
*!       Function: JUSTFNAME
*!*****************************************************************************
FUNCTION justfname
*)
*) JUSTFNAME - Return just the filename (i.e., no path) from "filname"
*)
PARAMETERS m.filname

*- use platform specific path (10/28/95 jd)
LOCAL clocalfname, cdirsep

cdirsep = IIF(_mac,':','\')
IF !_mac AND ':' $ filname
	*- maybe we have a funny filename with extra ":", because of futzing with Mac paths (jd 7/17/96)
	clocalfname = SUBSTR(m.filname,AT(":",m.filname,OCCURS(":",m.filname)) + 1)
ELSE
	clocalfname = m.filname
ENDIF

clocalfname = SYS(2027,m.clocalfname )
IF RAT(m.cdirsep ,m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,RAT(m.cdirsep,m.clocalfname)+1,255)
ENDIF
IF AT(':',m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,AT(':',m.clocalfname)+1,255)
ENDIF
RETURN ALLTRIM(m.clocalfname)

*!*****************************************************************************
*!       Function: JUSTSTEM
*!*****************************************************************************
FUNCTION juststem
* Return just the stem name from "filname"
PARAMETERS m.filname
*- use platform specific path (10/28/95 jd)
LOCAL clocalfname, cdirsep
clocalfname = SYS(2027,m.filname)
cdirsep = IIF(_mac,':','\')
IF RAT(m.cdirsep ,m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,RAT(m.cdirsep,m.clocalfname)+1,255)
ENDIF
IF AT(':',m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,AT(':',m.clocalfname)+1,255)
ENDIF
IF AT('.',m.clocalfname) > 0
   m.clocalfname = SUBSTR(m.clocalfname,1,AT('.',m.clocalfname)-1)
ENDIF
RETURN ALLTRIM(UPPER(m.clocalfname))

*!*****************************************************************************
*!       Function: JUSTPATH
*!*****************************************************************************
FUNCTION justpath
*)
*) JUSTPATH - Returns just the pathname.
*)
PARAMETERS m.filname
m.filname = ALLTRIM(UPPER(m.filname))
*- use platform specific path (10/28/95 jd)
LOCAL clocalfname, cdirsep
clocalfname = SYS(2027,m.filname)
cdirsep = IIF(_mac,':','\')
IF m.cdirsep $ m.clocalfname 
   m.clocalfname = SUBSTR(m.clocalfname,1,RAT(m.cdirsep,m.clocalfname ))
   IF RIGHT(m.filname,1) = m.cdirsep AND LEN(m.filname) > 1 ;
            AND SUBSTR(m.clocalfname,LEN(m.clocalfname)-1,1) <> ':'
         clocalfname= SUBSTR(m.clocalfname,1,LEN(m.clocalfname)-1)
   ENDIF
   RETURN m.clocalfname
ELSE
   RETURN ''
ENDIF

*!*****************************************************************************
*!       Function: JUSTEXT
*!*****************************************************************************
FUNCTION justext
* Return just the extension from "filname"
PARAMETERS m.filname
PRIVATE m.ext
filname = justfname(m.filname)   && prevents problems with ..\ paths
m.ext = ""
IF AT('.',m.filname) > 0
   m.ext = SUBSTR(m.filname,AT('.',m.filname)+1,3)
ENDIF
RETURN UPPER(m.ext)

*!*****************************************************************************
*!       Function: JustDrive
*!*****************************************************************************
FUNCTION JustDrive
*- Return just the drive from "filname"
PARAMETERS m.filname
RETURN LEFT(m.filname,IIF(":" $ m.filname,AT(":",m.filname) - 1,""))

*!*****************************************************************************
*!      Procedure: PARTIALFNAME
*!*****************************************************************************
FUNCTION partialfname
PARAMETER m.filname, m.fillen
* Return a filname no longer than m.fillen characters.  Take some chars
* out of the middle if necessary.  No matter what m.fillen is, this function
* always returns at least the file stem and extension.
PRIVATE m.bname, m.elipse, m.remain,m.g_pathsep
IF _MAC
	m.g_pathsep = ":"
ELSE
    m.g_pathsep = "\"
ENDIF

m.elipse = "..." + m.g_pathsep
IF _MAC
    m.bname = SUBSTR(m.filname, RAT(":",m.filname)+1)
ELSE
	m.bname = justfname(m.filname)
ENDIF
DO CASE
CASE LEN(m.filname) <= m.fillen
   m.retstr = m.filname
CASE LEN(m.bname) + LEN(m.elipse) >= m.fillen
   m.retstr = m.bname
OTHERWISE
   m.remain = MAX(m.fillen - LEN(m.bname) - LEN(m.elipse), 0)
   IF _MAC
	   m.retstr = LEFT(SUBSTR(m.filname,1,RAT(":",m.filname)-1),m.remain) ;
		    +m.elipse+m.bname
   ELSE
  	   m.retstr = LEFT(justpath(m.filname),m.remain)+m.elipse+m.bname
   ENDIF
ENDCASE
RETURN m.retstr

*!*****************************************************************************
*!       Function: FORCEEXT
*!*****************************************************************************
FUNCTION forceext
*)
*) FORCEEXT - Force filename to have a particular extension.
*)
PARAMETERS m.filname,m.ext
PRIVATE m.ext
IF SUBSTR(m.ext,1,1) = "."
   m.ext = SUBSTR(m.ext,2,3)
ENDIF

m.pname = justpath(m.filname)
m.filname = justfname(UPPER(ALLTRIM(m.filname)))
IF AT('.',m.filname) > 0
   m.filname = SUBSTR(m.filname,1,AT('.',m.filname)-1) + '.' + m.ext
ELSE
   m.filname = m.filname + '.' + m.ext
ENDIF
RETURN addbs(m.pname) + m.filname

********************
procedure CPCodes
********************
	* This procedure initializes a two-column array containing code pages
	* and their corresponding DBF Byte identifier.

	parameters m.wzaCPCodes

	dimension wzaCPCodes[20,2]

	wzaCPCodes[ 1,1] = 437		&& US MS-DOS
	wzaCPCodes[ 1,2] = 1
	wzaCPCodes[ 2,1] = 850		&& International MS-DOS
	wzaCPCodes[ 2,2] = 2
	wzaCPCodes[ 3,1] = 1252		&& Windows ANSI
	wzaCPCodes[ 3,2] = 3
	wzaCPCodes[ 4,1] = 10000	&& Standard Macintosh
	wzaCPCodes[ 4,2] = 4
	wzaCPCodes[ 5,1] = 852		&& Eastern European MS-DOS
	wzaCPCodes[ 5,2] = 100
	wzaCPCodes[ 6,1] = 866		&& Russian MS-DOS
	wzaCPCodes[ 6,2] = 101
	wzaCPCodes[ 7,1] = 865		&& Nordic MS-DOS
	wzaCPCodes[ 7,2] = 102
	wzaCPCodes[ 8,1] = 861		&& Icelandic MS-DOS
	wzaCPCodes[ 8,2] = 103
	wzaCPCodes[ 9,1] = 895		&& Kamenicky (Czech) MS-DOS
	wzaCPCodes[ 9,2] = 104
	wzaCPCodes[10,1] = 620		&& Mazovia (Polish) MS-DOS
	wzaCPCodes[10,2] = 105
	wzaCPCodes[11,1] = 737		&& Greek MS-DOS
	wzaCPCodes[11,2] = 106
	wzaCPCodes[12,1] = 857		&& Turkish MS-DOS
	wzaCPCodes[12,2] = 107
	wzaCPCodes[13,1] = 10007	&& Russian Macintosh
	wzaCPCodes[13,2] = 150
	wzaCPCodes[14,1] = 10029	&& Eastern European Macintosh
	wzaCPCodes[14,2] = 151
	wzaCPCodes[15,1] = 10006	&& Greek Macintosh
	wzaCPCodes[15,2] = 152
	wzaCPCodes[16,1] = 1250		&& Eastern European Windows
	wzaCPCodes[16,2] = 200
	wzaCPCodes[17,1] = 1251		&& Russian Windows
	wzaCPCodes[17,2] = 201
	wzaCPCodes[18,1] = 1253		&& Greek Windows
	wzaCPCodes[18,2] = 203
	wzaCPCodes[19,1] = 1254		&& Turkish Windows
	wzaCPCodes[19,2] = 202
	wzaCPCodes[20,1] = 0		&& Not tagged
	wzaCPCodes[20,2] = 0

ENDPROC

********************
procedure CPTag
********************
	* This procedure tags the specified table with the specified code page.
	* This procedure is designed to be called after having opened a table and
	* checked the CPDBF() value. It does not verify that the file is a DBF.

	parameters m.wzsFName, m.wziCodePage

	#DEFINE C_NOFILE	20	&& Invalid code page
	#DEFINE C_BADCODEPG	21	&& Invalid code page
	#DEFINE C_NOOPEN	22	&& File could not be opened
	private m.wziHandle, m.wzaCPCodes, m.wziDBFByte, m.wzi

	dimension wzaCPCodes[1]
	do CPCodes with wzaCPCodes

	m.wziDBFByte=-1
	for m.wzi=1 to alen(wzaCPCodes,1)
		if m.wziCodePage=wzaCPCodes[m.wzi,1]
			m.wziDBFByte=wzaCPCodes[m.wzi,2]
			exit
		endif
	endfor
	if m.wziDBFByte=-1 && Invalid code page
		=MESSAGEBOX(STRTRAN(E_BADCODEPAGE_LOC,"@1",ALLT(STR(m.wziCodePage)),1))
		return .f.
	else
		if !file(m.wzsFName) && File does not exist
			=MESSAGEBOX(STRTRAN(E_FILENOEXIST_LOC,"@1",m.wzsFName,1))
			return .f.
		else
			m.wziHandle=FOPEN(m.wzsFName,2)
			if !m.wziHandle=-1
				* Poke the codepage id into byte 29
				=fseek(m.wziHandle,29)
				=fwrite(m.wziHandle,chr(m.wziDBFByte))
				=fclose(m.wziHandle)
			else && File could not be opened
				=MESSAGEBOX(E_NOOPEN_LOC + m.wzsFName)
				return .f.
			endif
		endif
	endif

ENDFUNC

	
*------------------------------------
PROCEDURE EscHand
*------------------------------------
RETURN

*------------------------------------
PROCEDURE Readable
*------------------------------------
*- Check to see if the file is readable

	PARAMETER cFile

	LOCAL m.nFileReady

	m.nFileReady = FOPEN(cFile)
	IF m.nFileReady = -1
		RETURN .F.
	ELSE
		=FCLOSE(m.nFileReady)
		RETURN .T.
	ENDIF

ENDPROC


*------------------------------------
PROCEDURE TmpAlias
*------------------------------------
* Returns generated name for use as an alias.
PRIVATE m.wzsAlias
m.wzsAlias=SYS(2015)
DO WHILE USED(m.wzsAlias)
	m.wzsAlias=SYS(2015)
ENDDO
RETURN m.wzsAlias


*------------------------------------
procedure UpdPaths
*------------------------------------
PARAMETERS m.wzsOldName

PRIVATE m.wzsJustPath, m.wzsPath, m.wzsTName
m.wzsPath=SET('path')
SET PATH TO

m.wzsJustPath=addbs(upper(justpath(SYS(2027,m.wzsOldName))))

SCAN FOR !UPPER(type)=='FPC'
	IF m.wzsJustPath$SYS(2027,upper(path))
		m.wzsTName=STRTRAN(SYS(2027,upper(path)),m.wzsJustPath,'',1,1)
		IF file(addbs(justpath(SYS(2027,dbf())))+m.wzsTName)
			REPLACE path WITH upper(addbs(justpath(SYS(2027,dbf())))+m.wzsTName)
			LOOP
		ENDIF
		IF JUSTDRIVE(SYS(2027,upper(path)))==JUSTDRIVE(m.wzsJustPath)
			m.wzsTName=STRTRAN(SYS(2027,UPPER(path)),JUSTDRIVE(m.wzsJustPath),'',1,1)
			IF FILE(JUSTDRIVE(SYS(2027,dbf()))+m.wzsTName)
				REPLACE path WITH upper(justdrive(SYS(2027,dbf()))+m.wzsTName)
			ENDIF
		ENDIF
	ENDIF
ENDSCAN

LOCATE FOR UPPER(type)=='FPC'
REPLACE path WITH DBF()

SET PATH TO (m.wzsPath)
RETURN

*------------------------------------
procedure DBTable
*------------------------------------
* This procedure returns .t. if the specified file has a dBASE III or IV
* memo field that will need to be converted.

parameters m.wzsFName

if !file(m.wzsFName)
	return .f.
endif

wzsFName = SYS(2027,wzsFName)

private m.wziTypeByte, m.wziHandle
m.wziHandle=FOPEN(m.wzsFName)
if m.wziHandle=-1
	=MESSAGEBOX(E_NOOPEN_LOC + m.wzsFName)
	return .f.
endif
m.wziTypeByte=asc(fread(m.wziHandle,1))
if !fclose(m.wziHandle)
	=MESSAGEBOX(E_NOCLOSE_LOC + m.wzsFName)
endif

* 0x83 (131) FoxBASE+/dBASE III PLUS, with memo
* 0x8B (139) dBASE IV, with memo
if m.wziTypeByte=131 .or. m.wziTypeByte=139
	return .t.
else
	return .f.
endif

*------------------------------------
procedure FileExt
*------------------------------------
* This procedure returns the FoxPro extension based on the
* type of file.
parameters m.wzsType, m.wzsMethod
do case
case m.wzsType='TABLE'
	return 'dbf'
case m.wzsType='QUERY'
	if m.wzsMethod='DESIGN'
		return 'qpr'
	else
		return 'fpq'
	endif
case m.wzsType='FORM'
	return 'scx'
case m.wzsType='REPORT'
	return 'frx'
case m.wzsType='LABEL'
	return 'lbx'
case m.wzsType='PROGRAM'
	return 'prg'
case m.wzsType='CATALOG'
	return 'fpc'
endcase

*----------------------------------
FUNCTION CvtLine
*----------------------------------
	*-      Convert DOS char-type lines to line objects
	*-
	*-		Assume report or screen is open exclusive as DBF, with alias newfile
	*-		and is positioned on record with text field
	PARAMETER avline

	#DEFINE K_LOWLINE		179
	#DEFINE K_HILINE		218
	#DEFINE K_HORZLN1		CHR(196)
	#DEFINE K_VERTLN1		CHR(179)

	#DEFINE K_ULCORN1		CHR(218)
	#DEFINE K_URCORN1		CHR(191)
	#DEFINE K_LLCORN1		CHR(192)
	#DEFINE K_LRCORN1		CHR(217)

	#DEFINE K_CROSS1		CHR(197)
	#DEFINE K_LCROSS1		CHR(195)
	#DEFINE K_RCROSS1		CHR(180)
	#DEFINE K_TCROSS1		CHR(194)
	#DEFINE K_BCROSS1		CHR(193)

	#DEFINE K_RV1H2			CHR(181)	&& Right side, single vertical, double horizontal
	#DEFINE K_LV1H2			CHR(198)	&& Left side, single vertical, double horizontal
	#DEFINE K_CV1H2			CHR(216)	&& Cross, single vertical, double horizontal

	*- mixed single/double
	#DEFINE K_V1H2			CHR(213) + CHR(209) + CHR(184) + CHR(198) + CHR(216) + CHR(181) + CHR(212) + CHR(207) + CHR(190)
	#DEFINE K_V2H1			CHR(214) + CHR(210) + CHR(183) + CHR(199) + CHR(215) + CHR(182) + CHR(211) + CHR(208) + CHR(189)

	#DEFINE K_HORZSET1		K_HORZLN1 + K_ULCORN1 + K_URCORN1 + K_LLCORN1 + K_LRCORN1 + K_CROSS1 + K_LCROSS1 + K_RCROSS1 + K_TCROSS1 + K_BCROSS1 + K_V2H1
	#DEFINE K_VERTSET1		K_VERTLN1 + K_ULCORN1 + K_URCORN1 + K_LLCORN1 + K_LRCORN1 + K_CROSS1 + K_LCROSS1 + K_RCROSS1 + K_TCROSS1 + K_BCROSS1 + K_V1H2
	*----------------------------------
	#DEFINE K_HORZLN2		CHR(205)
	#DEFINE K_VERTLN2		CHR(186)

	#DEFINE K_ULCORN2		CHR(201)
	#DEFINE K_URCORN2		CHR(187)
	#DEFINE K_LLCORN2		CHR(200)
	#DEFINE K_LRCORN2		CHR(188)

	#DEFINE K_CROSS2		CHR(206)
	#DEFINE K_LCROSS2		CHR(204)
	#DEFINE K_RCROSS2		CHR(185)
	#DEFINE K_TCROSS2		CHR(203)
	#DEFINE K_BCROSS2		CHR(202)

	#DEFINE K_HORZSET2		K_HORZLN2 + K_ULCORN2 + K_URCORN2 + K_LLCORN2 + K_LRCORN2 + K_CROSS2 + K_LCROSS2 + K_RCROSS2 + K_TCROSS2 + K_BCROSS2 + K_V1H2
	#DEFINE K_VERTSET2		K_VERTLN2 + K_ULCORN2 + K_URCORN2 + K_LLCORN2 + K_LRCORN2 + K_CROSS2 + K_LCROSS2 + K_RCROSS2 + K_TCROSS2 + K_BCROSS2 + K_V2H1

	LOCAL lhsingle, lhdouble, lvsingle, lvdouble, nlen, nstartpos, nvpos, nhpos, nctr,;
		ncurrec, ccolorpr
		
	*- see if special chars are in the text field
	m.nlen = LEN(newfile.expr)
	m.nstartpos = -1
	STORE .F. TO m.lhsingle, m.lhdouble
	FOR m.nctr = 2 TO m.nlen
		m.cchar = SUBS(newfile.expr,m.nctr,1)
		m.nascval = ASC(m.cchar)
		IF m.cchar $ K_HORZSET1 OR m.cchar $ K_HORZSET2
			*- horizontal line
			*- see if run of the little buggers
			IF m.nstartpos = -1
				*- remember line characteristics
				IF m.cchar $ K_HORZSET1
					m.lhsingle = .T.
					m.lhdouble = .F.
				ENDIF
				IF m.cchar $ K_HORZSET2
					m.lhsingle = .F.
					m.lhdouble = .T.
				ENDIF
				m.nstartpos = m.nctr
			ENDIF
		ELSE
			IF m.nstartpos <> -1
				*- started a run, and it has ended
				*- remember current coords
				m.nvpos = newfile.vpos
				m.nhpos = newfile.hpos
				IF m.filetype = dbiv_scr_type
					m.ccolorpr = newfile.colorpair
				ENDIF
				m.ncurrec = RECNO()
				*- remove line chars from text field
				IF m.nctr - m.nstartpos - 1 = m.nlen
					*- entire text field is a line, so use this record
				ELSE
					REPLACE newfile.expr WITH LEFT(newfile.expr,m.nstartpos - 1) + ;
							SPACE(m.nctr - m.nstartpos) + ;
							SUBS(newfile.expr,m.nctr)
					*- add record to report/screen
					APPEND BLANK
				ENDIF
				REPLACE newfile.objtype WITH 7,;
					newfile.objcode WITH IIF(m.lhsingle,4,5),;
					newfile.vpos WITH m.nvpos,;
					newfile.hpos WITH m.nhpos + m.nstartpos - 2,;
					newfile.height WITH 1,;
					newfile.width WITH m.nctr - m.nstartpos
				IF m.filetype = dbiv_scr_type
					REPLACE newfile.colorpair WITH m.ccolorpr
				ENDIF
				*- reset start pos
				m.nstartpos = -1
				*- return to the record in question
				GO m.ncurrec
			ENDIF
		ENDIF
		STORE .F. TO m.lvsingle, m.lvdouble
		IF m.cchar $ K_VERTSET1
			m.lvsingle = .T.
		ENDIF
		IF m.cchar $ K_VERTSET2
			m.lvdouble = .T.
		ENDIF
		IF m.lvsingle OR m.lvdouble
			*- vertical line
			IF m.filetype = dbiv_scr_type
				m.ccolorpr = newfile.colorpair
			ENDIF
			m.nhpos = newfile.hpos + m.nctr - 1
			IF avline[m.nhpos,1] = -1
				*- new vertical line
				avline[m.nhpos,1] = newfile.vpos
				avline[m.nhpos,2] = 1
				avline[m.nhpos,4] = m.lvsingle
				IF m.filetype = dbiv_scr_type
					avline[m.nhpos,3] = m.ccolorpr
				ENDIF
			ELSE
				avline[m.nhpos,2] = avline[m.nhpos,2] + 1
			ENDIF
			IF m.nlen = 3
				*- entire text field is a line, so delete this record
				DELETE
			ELSE
				*- replace vertical line with space
				REPLACE newfile.expr WITH LEFT(newfile.expr,m.nctr - 1) + ;
					SPACE(1) + ;
					SUBS(newfile.expr,m.nctr + 1)
			ENDIF
		ELSE
			*- check if vertical line ended
			m.nhpos = newfile.hpos + m.nctr - 1
			IF avline[m.nhpos,1] <> -1
				*- remember current coords
				m.ncurrec = RECNO()
				*- add record
				APPEND BLANK
				REPLACE newfile.objtype WITH 7,;
					newfile.objcode WITH IIF(avline[m.nhpos,4],4,5),;
					newfile.vpos WITH avline[m.nhpos,1],;
					newfile.hpos WITH m.nhpos - 1,;
					newfile.height WITH avline[m.nhpos,2],;
					newfile.width WITH 1 
				IF m.filetype = dbiv_scr_type
					REPLACE newfile.colorpair WITH avline[m.nhpos,3]
				ENDIF
				*- return to the record in question
				GO m.ncurrec
				*- reset array
				avline[m.nhpos,1] = -1
				avline[m.nhpos,2] = -1
				avline[m.nhpos,3] = -1
				avline[m.nhpos,4] = -1
			ENDIF  && end of vertical line
		ENDIF
	NEXT
	*- trim off unnecessary spaces?
	m.nleadspac = LEN(SUBS(newfile.expr,2)) - LEN(LTRIM(SUBS(newfile.expr,2)))
	IF m.nleadspac > 0
		REPLACE newfile.expr WITH '"' + SUBS(newfile.expr,m.nleadspac + 2),;
			newfile.hpos WITH newfile.hpos + m.nleadspac,;
			newfile.width WITH MAX(LEN(newfile.expr) - 2,0)
	ENDIF
	m.ntrailspac = LEN(newfile.expr) - 1 - LEN(TRIM(LEFT(newfile.expr,LEN(newfile.expr) - 1)))
	IF m.ntrailspac > 0
		REPLACE newfile.expr WITH LEFT(newfile.expr,LEN(newfile.expr) - 1 - m.ntrailspac) + '"',;
			newfile.width WITH MAX(LEN(newfile.expr) - 2,0)
	ENDIF
	*- remove invalid records
	DELETE ALL FOR newfile.objtype = 5 AND newfile.expr = '""'
	RETURN

ENDFUNC

*----------------------------------
PROCEDURE FixVert
*----------------------------------
*- Check for vertical lines that haven't been added to form
	PARAMETER avline

	LOCAL m.nctr

	FOR m.nctr = 1 TO ALEN(avline,1)
		IF avline[m.nctr,1] <> -1
			*- add record
			APPEND BLANK
			REPLACE newfile.objtype WITH 7,;
				newfile.objcode WITH IIF(avline[m.nctr,4],4,5),;
				newfile.vpos WITH avline[m.nctr,1],;
				newfile.hpos WITH m.nctr,;
				newfile.height WITH avline[m.nctr,2],;
				newfile.width WITH 1,;
				newfile.uniqueid WITH SYS(2015),;
				newfile.platform WITH "DOS" 
			IF m.filetype = dbiv_scr_type
				REPLACE newfile.colorpair WITH avline[m.nctr,3]
			ENDIF
		ENDIF
	NEXT
	*- pack to get rid of deleted vertical line records
	PACK
	RETURN
	
ENDPROC		&& FixVert

*----------------------------------
PROCEDURE GoodName
*----------------------------------
	*- Make a legal alias out of parm.
	PARAMETERS m.wzsAlias
	PRIVATE m.i,m.c,m.retval
	IF '\' $ m.wzsAlias
		m.wzsAlias = JustStem(m.wzsAlias)
	ENDIF
	IF m.wzsAlias # '_'  AND !ISALPHA(m.wzsAlias)
		m.wzsAlias = '_' + m.wzsAlias
	ENDIF
	*- reworked code to prevent err if name is longer than 10 (jd 5/6/94)
	m.retval=""
	FOR m.i=1 TO MIN(LEN(m.wzsAlias),10)	&&max len of alias
		m.c = SUBSTR(m.wzsAlias,m.i,1)
		IF !ISALPHA(m.c) AND m.c # '_' AND !ISDIGIT(m.c)
			m.retval = m.retval + "_"
		ELSE
			m.retval = m.retval + m.c
		ENDIF
	ENDFOR
	RETURN m.retval
	
ENDPROC		&& GoodName

*----------------------------------
FUNCTION EvalData
* function no longer used
*----------------------------------
	PARAMETER m.cData,m.cDataType
	DO CASE
		CASE m.cDataType= 'C'
			RETURN m.cData
		CASE m.cDataType= 'N'
			RETURN VAL(m.cData)
		CASE m.cDataType= 'D'
			RETURN CTOD(m.cData)
		CASE m.cDataType= 'L'
			m.ctempexpr = m.cData
			RETURN EVALUATE(ctempexpr)
		CASE  m.cDataType= 'A'
			*- handle arrays differently
			RETURN ''
		OTHERWISE
			*- ???? unknown? undefined??
			RETURN ''
	ENDCASE
	RETURN ''
ENDFUNC		&& EvalData

*----------------------------------
FUNCTION StripQuote
*----------------------------------
	*- strip off quotes of string
	PARAMETER cString

	LOCAL  cQuote

	cQuote = LEFT(cString,1)
	IF m.cQuote $ ["'] + "]"
		cString = STRTRAN(cString,cQuote,"")
		IF cQuote = "["
			cString = STRTRAN(cString,"]","")
		ENDIF
	ENDIF
	RETURN cString

ENDFUNC		&& StripQuote

*----------------------------------
FUNCTION StripParen
*----------------------------------
	*- strip out any text within parens
	PARAMETER cText, cLParen, cRParen

	IF cLParen $ cText AND !(cRParen $ cText)
		*- no matching rparen
		cText = LEFT(cText,AT(cLParen,cText) - 1)
	ENDIF

	DO WHILE cLParen $ cText
		cText = LEFT(cText,AT(cLParen,cText) - 1) + ;
			IIF(AT(cRParen,cText) = LEN(cText),"",SUBS(cText,RAT(cRParen,cText) + 1))
	ENDDO
	*- strip out lingering right parens
	m.cText = STRTRAN(m.cText,cRParen,"")
	RETURN m.cText
ENDFUNC		&& StripParen

*----------------------------------
FUNCTION GoodName
*----------------------------------
	*- convert a string to a valid VFP object name
	PARAMETER cText

	LOCAL j

	IF !(ISALPHA(SUBS(m.cText,1,1)) OR SUBS(m.cText,1,1) == "_")
		m.cText = STUFF(m.cText,1,1,"_")
	ENDIF
	FOR m.j = 2 TO LEN(m.cText)
		IF !(ISALPHA(SUBS(m.cText,j,1)) OR ;
			ISDIGIT(SUBS(m.cText,j,1)) OR ;
			SUBS(m.cText,j,1) == "_")
			m.cText = STUFF(m.cText,j,1,"_")
		ENDIF
	NEXT
	RETURN m.cText
ENDFUNC

*----------------------------------
FUNCTION IsDir
*----------------------------------
	*- test if a directory exists
	PARAMETER cDir

	LOCAL aDirArry, iDirCt

	DIMENSION aDirArry[1]

	iDirCt = ADIR(aDirArry,AddBS(cDir) + "*.*", "D")

	RETURN (m.iDirCt > 0)

ENDFUNC

*----------------------------------
PROCEDURE EscHandler
*----------------------------------

	IF MESSAGEBOX(C_ESCAPE_LOC,MB_YESNO + 256) = IDYES
		IF TYPE("gOPJX") = 'O'
			*- it's an object
			gOPJX.Error(0)
		ELSE
			*- problem -- escape has been set, but no object (should be impossible)
			CLOSE ALL
			RETURN TO MASTER
		ENDIF
	ENDIF

ENDPROC

*----------------------------------
PROCEDURE FatalErr
*----------------------------------
	*-  ON ERROR is set to this in the Error Method
	=MESSAGEBOX(E_FATAL_LOC)
	gError = .T.
	RETURN TO MASTER

ENDPROC

********************
 PROCEDURE autoname
********************
	* This procedure generates an automatic name based on a filename.
	
	PARAMETERS m.wzsbasename, m.wzsextension, m.wzlwithmemo
	PRIVATE m.wzi, m.wzspath, m.wzsstem, m.wziwidth
	
	m.wzspath=addbs(justpath(SYS(2027,m.wzsbasename)))
	m.wzsstem=juststem(m.wzsbasename)
	m.wzi=1
	DO WHILE .T.
		DO CASE
			CASE _DOS .OR. _WINDOWS
				m.wziwidth=8-(LEN(ALLTRIM(STR(m.wzi)))+1)
				m.wzsautoname=UPPER(m.wzspath+LEFT(m.wzsstem,m.wziwidth)+'_'+ ;
					ALLTRIM(STR(m.wzi))+'.'+m.wzsextension)
			CASE _MAC
				m.wziwidth=27-(LEN(ALLTRIM(STR(m.wzi)))+1)  && max stem for Mac is 27 (27 + .xxx = 31)
				m.wzsautoname=UPPER(m.wzspath+LEFT(m.wzsstem,m.wziwidth)+'_'+ ;
					ALLTRIM(STR(m.wzi))+'.'+m.wzsextension)
			OTHERWISE
				&& work needed here for _unix
				RETURN ""
				*-DO errhand WITH LINENO(), 0, wzatext[175], ''
		ENDCASE
		IF FILE(m.wzsautoname)
			m.wzi=m.wzi+1
		ELSE
			DO CASE
				CASE UPPER(m.wzsextension)='SCX'
					IF FILE(forceext(m.wzsautoname,'SCT'))
						m.wzi=m.wzi+1
					ELSE
						EXIT
					ENDIF
				CASE UPPER(m.wzsextension)='FRX'
					IF FILE(forceext(m.wzsautoname,'FRT'))
						m.wzi=m.wzi+1
					ELSE
						EXIT
					ENDIF
				CASE m.wzlwithmemo
					IF FILE(forceext(m.wzsautoname,'FPT'))
						m.wzi=m.wzi+1
					ELSE
						EXIT
					ENDIF
				OTHERWISE
					EXIT
			ENDCASE
		ENDIF
	ENDDO
	RETURN m.wzsautoname
*: EOP: AUTONAME

********************
procedure PutName
********************
parameters m.wzsFType, m.wzsMethod, m.wzlDelFile
	private m.wzsSafety, m.wzsFname, m.wzsPrompt, m.wzsString

	m.wzsMethod=iif(empty(m.wzsMethod),'',m.wzsMethod)

	m.wzsSafety=set('safety')
	set safety on

	do while .t.
		*- don't supply default of "*.ext" if _mac (4/22/94 jd)
		IF _mac
			m.wzsFName=putfile(STRTRAN(C_SAVETO_LOC,"@1",proper(locword(m.wzsFType))), ;
				"", FileExt(m.wzsFType,m.wzsMethod))
		ELSE
			m.wzsFName=putfile(STRTRAN(C_SAVETO_LOC,"@1",proper(locword(m.wzsFType))), ;
				'*.'+FileExt(m.wzsFType,m.wzsMethod), FileExt(m.wzsFType,m.wzsMethod))
		ENDIF
		if empty(m.wzsFName)
			exit && user cancelled putfile()
		endif
		if m.wzlDelFile
			do case
			case m.wzsFType='CATALOG'
				erase (m.wzsFName)
				erase (forceext(m.wzsFName,'FCT'))
			case m.wzsFType='TABLE'
				erase (m.wzsFName)
				erase (forceext(m.wzsFName,'FPT'))
				erase (forceext(m.wzsFName,'CDX'))
			case m.wzsFType='QUERY'
				erase (m.wzsFName)
			case m.wzsFType='FORM'
				erase (m.wzsFName)
				erase (forceext(m.wzsFName,'SCT'))
			case m.wzsFType='REPORT'
				erase (m.wzsFName)
				erase (forceext(m.wzsFName,'FRT'))
			case m.wzsFType='LABEL'
				erase (m.wzsFName)
				erase (forceext(m.wzsFName,'LBT'))
			case m.wzsFType='PROGRAM'
				erase (m.wzsFName)
			endcase
			exit
		endif
		do case
		*- I believe this should be FCT, not FCX as was the case (jd 5/6/94)
		*- also, changed code to "YESNO" code since question is asked (jd 5/12/94)
		case m.wzsFType='CATALOG' .and. file(forceext(m.wzsFName,'FCT'))
			IF (MESSAGEBOX(STRTRAN(C_OVERWRITE_LOC,"@1",forceext(m.wzsFName,'FCT')),4) = K_NO)
				exit
			endif
		case m.wzsFType='TABLE' .and. file(forceext(m.wzsFName,'FPT'))
			IF (MESSAGEBOX(STRTRAN(C_OVERWRITE_LOC,"@1",forceext(m.wzsFName,'FPT')),4) = K_NO)
				exit
			endif
		case m.wzsFType='FORM' .and. file(forceext(m.wzsFName,'SCT'))
			IF (MESSAGEBOX(STRTRAN(C_OVERWRITE_LOC,"@1",forceext(m.wzsFName,'SCT')),4) = K_NO)
				exit
			endif
		case m.wzsFType='REPORT' .and. file(forceext(m.wzsFName,'FRT'))
			IF (MESSAGEBOX(STRTRAN(C_OVERWRITE_LOC,"@1",forceext(m.wzsFName,'FRT')),4) = K_NO)
				exit
			endif
		case m.wzsFType='LABEL' .and. file(forceext(m.wzsFName,'LBT'))
			IF (MESSAGEBOX(STRTRAN(C_OVERWRITE_LOC,"@1",forceext(m.wzsFName,'LBT')),4) = K_NO)
				exit
			endif
		otherwise
			exit
		endcase
	enddo

	set safety &wzsSafety
	return m.wzsFName
*- eop PutName

********************
procedure LocWord
********************
* This procedure returns the localized word for the type
* of file.
parameters m.wzsType, m.wziVariation

	m.wzsType=upper(m.wzsType)

	do case
	case empty(m.wziVariation)
		do case
		case m.wzsType='TABLE'
			return C_TABLE_LOC		&& Table
		case m.wzsType='QUERY'
			return C_QUERY_LOC		&& Query
		case m.wzsType='FORM'
			return C_FORM_LOC		&& Screen
		case m.wzsType='REPORT'
			return C_REPORT_LOC		&& Report
		case m.wzsType='LABEL'
			return C_LABEL_LOC		&& Label
		case m.wzsType='PROGRAM'
			return C_PROGRAM_LOC	&& Program
		case m.wzsType='CATALOG'
			return C_CATALOG_LOC	&& Catalog
		endcase
	case m.wziVariation=1 && "your screen", "your report", etc.
		do case
		case m.wzsType='TABLE'
			return C_TABLE1_LOC		&& your table
		case m.wzsType='QUERY'
			return C_QUERY1_LOC		&& your query
		case m.wzsType='FORM'
			return C_FORM1_LOC		&& your screen
		case m.wzsType='REPORT'
			return C_REPORT1_LOC	&& your report
		case m.wzsType='LABEL'
			return C_LABEL1_LOC		&& your label
		case m.wzsType='PROGRAM'
			return C_PROGRAM1_LOC	&& your program
		endcase
	case m.wziVariation=2 && "The new screen", "The new report", etc.
		do case
		case m.wzsType='FORM'
			return C_FORM2_LOC		&& The new screen
		case m.wzsType='REPORT'
			return C_REPORT2_LOC	&& The new report
		case m.wzsType='LABEL'
			return C_LABEL2_LOC		&& The new label
		endcase
	case m.wziVariation=3 && "Screen Wizard", "Report Wizard", etc.
		do case
		case m.wzsType='TABLE'
			return C_TABLE3_LOC		&& Table Wizard
		case m.wzsType='QUERY'
			return C_QUERY3_LOC		&& Query Wizard
		case m.wzsType='FORM'
			return C_FORM3_LOC		&& Screen Wizard
		case m.wzsType='REPORT'
			return C_REPORT3_LOC	&& Report Wizard
		case m.wzsType='LABEL'
			return C_LABEL3_LOC		&& Label Wizard
		case m.wzsType='PROGRAM'
			return C_PROGRAM3_LOC	&& Application Wizard
		endcase
	endcase
*- eop locword.prg

*!*****************************************************************************
*!       Function: FORCEDEC
*!*****************************************************************************
FUNCTION ForceDec
*)
*) FORECDEC - Force a string to a certain number of decimal places
PARAMETER cString, nDecimals

RETURN STR(VAL(cString),LEN(cString) + nDecimals + 1,nDecimals)

*------------------------------------
FUNCTION CleanWhite
*------------------------------------
	*- strip out leading white space
	PARAMETER cText

	LOCAL cTmp

	cTmp = STRTRAN(TRIM(cText),C_CRLF,C_CR)
	cTmp = STRTRAN(cTmp,C_TAB,' ')
	DO WHILE C_CR + ' ' $ cTmp
		cTmp = STRTRAN(cTmp,C_CR+' ',C_CR)
	ENDDO
	DO WHILE LEFT(cTmp,1) $ C_LF + ' '
		cTmp = SUBS(cTmp,2)
	ENDDO
	RETURN cTmp

ENDFUNC

FUNCTION CHRTRANC(d1,d2,d3)
RETURN CHRTRAN(m.d1,m.d2,m.d3)

*----------------------------------
FUNCTION GetArray
*----------------------------------
PARAMETER cText, aList
LOCAL m.iTextLen, m.cchar

m.iTextLen = LEN(m.cText)
FOR i = 1 TO m.iTextLen

	m.cchar = SUBS(m.cText,i,1)
	
	IF !ISALPHA(m.cchar) AND !ISDIGIT(m.cchar)
		LOOP
	ENDIF
	
	nextItem = GetItem(SUBS(m.cText,i))
	IF EMPTY(aList[1])
		aList[1] = nextItem
	ELSE
		DIMENSION aList[ALEN(aList) + 1]
		aList[ALEN(aList)] = nextItem
	ENDIF
	
	i = i + AT(nextItem,SUBS(cText,i)) + LEN(nextItem) - 1
NEXT
RETURN

*----------------------------------
FUNCTION GetItem
*----------------------------------
	LPARAMETER m.cText

	#DEFINE     k_quote             ['"] + '['
	#DEFINE		k_lbracket			'['
	#DEFINE		k_rbracket			']'
	#DEFINE     k_lparen            "("
	#DEFINE     k_rparen            ")"
	#DEFINE     k_space             ' '
	#DEFINE     k_comma             ','
	#DEFINE     k_tab               CHR(9)
	#DEFINE     k_semicol            ";"
	#DEFINE     k_cr                CHR(13)

	LOCAL m.iLineLoc, m.quote, m.word, m.n1, m.lparenct, m.iTextLen

	*- get everything up to next unenclosed rparen or rbracket
	m.iTextLen = LEN(m.cText)
	FOR m.iLineLoc = 1 TO m.iTextLen

		m.cchar = SUBS(m.cText,m.iLineLoc,1)
		
		IF ISALPHA(m.cchar) OR ISDIGIT(m.cchar)
			LOOP
		ENDIF

		*- treat any sequence of spaces or tabs as 1 word
		IF m.cchar $ k_space + k_tab + k_cr
		    FOR m.iLineLoc = m.iLineLoc + 1 TO m.iTextLen
				m.cchar = SUBS(m.cText,m.iLineLoc,1)
				IF !m.cchar $ k_space + k_tab + k_cr
					EXIT
				ENDIF
		    NEXT
		ENDIF
		
		*- if lparen, move ahead to matching rparen
		IF m.cchar $ k_lparen + k_lbracket
			=GetRParen(m.cText,@iLineLoc,m.iTextLen)
			EXIT
		ENDIF

	NEXT

	RETURN LEFT(m.cText,m.iLineLoc)

ENDFUNC

*----------------------------------
FUNCTION GetRParen
*----------------------------------
	LPARAMETERS cText,iLineLoc,m.iTextLen
	LOCAL cchar, m.rparen, m.lparenct
	m.cchar = SUBS(cText,iLineLoc,1)
	m.rparen = IIF(m.cchar = k_lparen, k_rparen, k_rbracket)
	m.lparenct = 1
	FOR m.iLineLoc = m.iLineLoc + 1 TO m.iTextLen
		*- treat enquoted stuff as 1 word
		m.cchar = SUBS(m.cText,m.iLineLoc,1)
		IF m.cchar $ k_quote
			m.endquote = IIF(m.cchar = "[","]",m.cchar)
			m.iLineLoc = m.iLineLoc + AT(m.endquote, SUBS(m.cText,iLineLoc + 1)) + 1
			m.cchar = SUBS(m.cText,m.iLineLoc,1)
		ENDIF
		IF m.cchar = m.rparen
			EXIT
		ENDIF
		IF m.cchar $ k_lparen + k_lbracket
			*- found a nested lparen
			=GetRParen(m.cText, @iLineLoc, m.iTextLen)		&& recursive call!
		ENDIF
	NEXT
	RETURN
ENDFUNC


*----------------------------------
FUNCTION pReadOnly
*----------------------------------
	LPARAMETER cFile

	LOCAL ARRAY aDirInfo[1]

	IF ADIR(aDirInfo,cFile) == 0
		*- file isn;t there, so fail
		RETURN .T.
	ENDIF

	RETURN ('R' $ aDirInfo[1,5])

ENDFUNC

*----------------------------------
FUNCTION UpdateSCX
*----------------------------------
	PARAMETER cFile, lRecurse

	LOCAL ARRAY aFiles[1,5]
	LOCAL i, iALen, cTarget
	
	cTarget = cFile + IIF(RIGHT(cFile,1) == IIF(_mac,':','\'),"*.*","")
	iALen = ADIR(aFiles, cTarget, 'D')

	FOR i = 1 TO iALen
	
		IF !(m.lVCX AND JustExt(aFiles[i,1]) == 'VCX') AND ;
			!(m.lSCX AND JustExt(aFiles[i,1]) == 'SCX') AND ;
			!('D' $ aFiles[i,5])
			*- neither an SCX or a VCX, and not a directory
			LOOP
		ENDIF
		
		DO CASE
			CASE 'D' $ aFiles[i,5]
				IF aFiles[i,1] == "." OR aFiles[i,1] == ".."
					LOOP
				ENDIF
				IF m.lRecurse
					*- directory -- recursive call!
					UpdateSCX(AddBS(AddBS(cFile) + aFiles[i,1]), lRecurse)
				ENDIF
				LOOP
		
			CASE  'R' $ aFiles[i,5]
				*- file is read-only
				=MESSAGEBOX(TRIM(aFiles[i,5]) + E_NOCONVERT3_LOC)
				
			CASE  'H' $ aFiles[i,5] OR 'S' $ aFiles[i,5]
				*- file is hidden, or a system file
				=MESSAGEBOX(TRIM(aFiles[i,5]) + E_NOCONVERT4_LOC)
				
			OTHERWISE
				goMaster.aConvParms[4] = AddBS(JustPath(cFile)) + aFiles[i,1]
							
				=ACOPY(goMaster.aConvParms,aParms)
				oConvObject = CREATE(goMaster.scx30ConverterClass, @aParms, .T.)

				IF TYPE("oConvObject") # 'O'
					*- object was not created
					goMaster.lHadError = .T.
					gReturnVal = -1
					RETURN
				ENDIF

				IF oConvObject.lHadError
					*- error creating converter object: 
					*- assume error has already been presented to user
					goMaster.lHadError = .T.
					RELEASE oConvObject
					gReturnVal = -1
					RETURN
				ENDIF

				gReturnVal = oConvObject.Converter()

				RELEASE oConvObject
		ENDCASE
	NEXT	&& going through array of files to convert
			
ENDFUNC		&& UpdateSCX


*- eof