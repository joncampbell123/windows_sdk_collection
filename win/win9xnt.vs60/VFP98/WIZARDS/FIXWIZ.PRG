* This program file can be used to convert existing forms 
* created using the Visual FoxPro 5.0 Form Wizard. Forms
* created with the VFP5 Wizard used a very large classlib
* called WIZSTYLE.VCX. This file has since been broken up 
* into a number of smaller more reusable class libraries.
* WIZSTYLE.VCX is still included for backward compatibility.
* 
* This utility simply redirects the old class WIZSTYLE class
* references to the new files.
* 
* A backup copy with _OLD at the end of name is made for each
* file updated.

LOCAL lMakeBackup,cFileName,cSaveArea,lCopyName 
lMakeBackup = .t.
cFileName = GETFILE("SCX|VCX")
cSaveArea = SELECT()
IF EMPTY(cFileName) OR !INLIST(UPPER(JUSTEXT(m.cFileName)),"SCX","VCX")
	RETURN
ENDIF
SELECT 0
USE (m.cFileName) EXCL
IF EMPTY(ALIAS())
	SELECT (m.cSaveArea)
	RETURN
ENDIF
IF m.lMakeBackUp
	lCopyName = JUSTSTEM(m.cFileName) +"_OLD.SCX"
	COPY TO (m.lCopyName)
ENDIF

* Standard Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizstnd") ;
	FOR ATC("Standard",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizstnd") ;
	FOR ATC("Standard",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizstnd") FOR ;
	ATC("Standard",class)#0
	
* Chiseled Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizchsl") ;
	FOR ATC("Chisel",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizchsl") ;
	FOR ATC("Chisel",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizchsl") FOR ;
	ATC("Chisel",class)#0
	
* Boxed Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbox") ;
	FOR ATC("Box",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizbox") ;
	FOR ATC("Box",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizbox") FOR ;
	ATC("Box",class)#0
	
* Shadow Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizshdow") ;
	FOR ATC("Shadow",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizshdow") ;
	FOR ATC("Shadow",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizshdow") FOR ;
	ATC("Shadow",class)#0
	
* Embossed Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizembss") ;
	FOR ATC("emboss",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizembss") ;
	FOR ATC("emboss",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizembss") FOR ;
	ATC("emboss",class)#0
	
* Fancy Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizfancy") ;
	FOR ATC("Fancy",class)#0
REPLACE ALL parent WITH STRTRAN(parent,"wizstyle","wizfancy") ;
	FOR ATC("Fancy",class)#0
REPLACE ALL class WITH STRTRAN(class,"wizstyle","wizfancy") FOR ;
	ATC("Fancy",class)#0
	
* Buttons Style
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbtns") ;
	FOR ATC("txtbtns",class)#0
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbtns") ;
	FOR ATC("picbtns",class)#0
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbtns") ;
	FOR ATC("scrollgrid",class)#0
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbtns") ;
	FOR ATC("grid",class)#0
REPLACE ALL classloc WITH STRTRAN(classloc,"wizstyle","wizbtns") ;
	FOR ATC("search",class)#0
	

USE	
SELECT (m.cSaveArea)
