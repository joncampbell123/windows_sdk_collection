* MyTrnsfm.prg
* Example of subclassing the Transformer.
* To run the subclassed Transformer, execute:
*      DO Transfrm WITH "MyTrnsfm.prg,MyTransformer","MyTrnsfm.prg,MyTransformFiles"
*
	
#DEFINE CAPTION_LOC	"My Transformer"
#DEFINE SUBCLASS_LOC	"  The transformation process has been subclassed."
#DEFINE MENU_LOC	"Menu: "
#DEFINE REPORT_LOC	"Report: "


DEFINE CLASS MyTransformer AS Transformer

	Caption=CAPTION_LOC

ENDDEFINE



DEFINE CLASS MyTransformFiles AS TransformFiles


	cGetFileExt='pjx|scx|vcx|mnx|frx'


	FUNCTION Do(tlAddFontRules)
		MESSAGEBOX(SUBCLASS_LOC,48, ;
			this.parent.Caption)
	RETURN oTransformFiles::Do(tlAddFontRules)
	ENDFUNC


	PROTECTED FUNCTION ProcessMenuRules
	ACTIVATE SCREEN
	? MENU_LOC+DBF()
	GO BOTTOM
	ENDFUNC


	PROTECTED FUNCTION ProcessReportRules
	ACTIVATE SCREEN
	? REPORT_LOC+DBF()
	GO BOTTOM
	ENDFUNC


ENDDEFINE
