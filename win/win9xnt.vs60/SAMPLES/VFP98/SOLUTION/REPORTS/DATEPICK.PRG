
*!* First we create an instance of the form class defined below
*!* (This is the form that allows the user to pick a start and end date)
oFrmDate = CREATEOBJECT("frmdatepick")

*!* Now, we show it.  Since the form is modal, execution suspends
*!* until the form is released (and the user has selected a valid
*!* start and end date)
oFrmDate.Show

*!* Note that there is no error checking here because the form is
*!* designed so that the user cannot exit with invalid data returned.

*!* The form has put the user's selection into two public variables
*!* (dStart_date and dEnd_date), so now we can just release the form
*!* and return back to the report.
RELEASE oFrmDate
RETURN


PUBLIC ofrmdatepick

ofrmdatepick=CREATEOBJECT("frmdatepick")
ofrmdatepick.Show()
RETURN



**************************************************
*-- Form:         frmdatepick 
*-- ParentClass:  form
*-- BaseClass:    form
*
DEFINE CLASS frmdatepick AS form


	Height = 101
	Width = 335
	DoCreate = .T.
	AutoCenter = .T.
	BorderStyle = 2
	Caption = "Date Range to Print"
	Closable = .F.
	MaxButton = .F.
	MinButton = .F.
	WindowType = 1
	Name = "frmdatepick"


	ADD OBJECT label1 AS label WITH ;
		AutoSize = .T., ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		Caption = "Select dates to print a range of invoices.", ;
		Height = 15, ;
		Left = 10, ;
		Top = 10, ;
		Width = 196, ;
		TabIndex = 1, ;
		Name = "Label1"


	ADD OBJECT label2 AS label WITH ;
		AutoSize = .T., ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		Caption = "\<From:", ;
		Height = 15, ;
		Left = 10, ;
		Top = 39, ;
		Width = 28, ;
		TabIndex = 2, ;
		Name = "Label2"


	ADD OBJECT cbofrommonth AS combobox WITH ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		BoundColumn = 2, ;
		ColumnCount = 2, ;
		ColumnWidths = "72,0", ;
		RowSourceType = 1, ;
		RowSource = "Jan,1,Feb,2,Mar,3,Apr,4,May,5,Jun,6,Jul,7,Aug,8,Sep,9,Oct,10,Nov,11,Dec,12", ;
		Value = (STR(1)), ;
		Height = 23, ;
		Left = 49, ;
		Style = 2, ;
		TabIndex = 3, ;
		Top = 35, ;
		Width = 72, ;
		Name = "cboFromMonth"


	ADD OBJECT label3 AS label WITH ;
		AutoSize = .T., ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		Caption = "\<To:", ;
		Height = 15, ;
		Left = 10, ;
		Top = 70, ;
		Width = 18, ;
		TabIndex = 5, ;
		Name = "Label3"


	ADD OBJECT cbotomonth AS combobox WITH ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		BoundColumn = 2, ;
		ColumnCount = 2, ;
		ColumnWidths = "72,0", ;
		RowSourceType = 1, ;
		RowSource = "Jan,1,Feb,2,Mar,3,Apr,4,May,5,Jun,6,Jul,7,Aug,8,Sep,9,Oct,10,Nov,11,Dec,12", ;
		Value = (STR(2)), ;
		Height = 23, ;
		Left = 49, ;
		Style = 2, ;
		TabIndex = 6, ;
		Top = 66, ;
		Width = 72, ;
		Name = "cboToMonth"


	ADD OBJECT cmdok AS commandbutton WITH ;
		Top = 10, ;
		Left = 254, ;
		Height = 23, ;
		Width = 72, ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		Caption = "OK", ;
		Default = .F., ;
		TabIndex = 8, ;
		Name = "cmdOK"


	ADD OBJECT cbofromyear AS combobox WITH ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		RowSourceType = 1, ;
		RowSource = "1992,1993,1994,1995", ;
		Value = (STR(1994)), ;
		Height = 23, ;
		Left = 128, ;
		TabIndex = 4, ;
		Top = 35, ;
		Width = 100, ;
		Name = "cboFromYear"


	ADD OBJECT cbotoyear AS combobox WITH ;
		FontName = "MS Sans Serif", ;
		FontSize = 8, ;
		RowSourceType = 1, ;
		RowSource = "1992,1993,1994,1995", ;
		Value = (STR(1994)), ;
		Height = 23, ;
		Left = 128, ;
		TabIndex = 7, ;
		Top = 66, ;
		Width = 100, ;
		Name = "cboToYear"

	PROCEDURE cmdok.Click
		#DEFINE FROM_GREATER_TO_LOC "The from date must be less than or equal to the to date."
		#DEFINE FROM_MONTH_LOC "You must select a month to start."
		#DEFINE FROM_YEAR_LOC "You must select or enter a year to start."
		#DEFINE TO_MONTH_LOC "You must select a month to end."
		#DEFINE TO_YEAR_LOC "You must select or enter a year to end."

		PUBLIC dStart_Date,dEnd_Date

		*!* First, get the values the user entered into the combo boxes.
		nFromMonth = VAL(THISFORM.cboFromMonth.value)
		nFromYear = VAL(THISFORM.cboFromYear.value)
		nToMonth = VAL(THISFORM.cboToMonth.value)
		nToYear = VAL(THISFORM.cboToYear.value)

		*!* We need to check to make sure that the user entered a value
		*!* into all four combo boxes; If not, send them back to try again.
		DO CASE
			CASE EMPTY(nFromMonth)
				MESSAGEBOX(FROM_MONTH_LOC)
				THISFORM.cboFromMonth.SetFocus
				RETURN
			CASE EMPTY(nFromYear)
				MESSAGEBOX(FROM_YEAR_LOC)
				THISFORM.cboFromYear.SetFocus
				RETURN
			CASE EMPTY(nToMonth)
				MESSAGEBOX(TO_MONTH_LOC)
				THISFORM.cboToMonth.SetFocus
				RETURN
			CASE EMPTY(nToYear)
				MESSAGEBOX(TO_YEAR_LOC)
				THISFORM.cboToYear.SetFocus
				RETURN
		ENDCASE

		*!* We need to check to make sure the end date selected is 
		*!* after the start date.
		IF (nFromYear > nToYear) OR (nFromYear = nToYear AND ;
			nFromMonth > nToMonth) THEN
			MESSAGEBOX(FROM_GREATER_TO_LOC)
			THISFORM.cboFromYear.SetFocus
			RETURN
		ENDIF

		*!* For the end date, we need to figure out the last day of the month
		*!* that was selected.
		*!* (Note that the one flaw in this CASE statement is that it doesn't
		*!* take into account leap year.)
		DO CASE
			CASE nToMonth = 2
				nEndDay = 28
			CASE INLIST(nToMonth,4,6,9,11)
				nEndDay = 30
			OTHERWISE
				nEndDay = 31
		ENDCASE

		*!* Finally, we can set up our start and end date public variables
		
		dStart_Date = DATE(nFromYear,nFromMonth,1)
		dEnd_Date = DATE(nToYear,nToMonth,nEndDay)

		THISFORM.Release
	ENDPROC


ENDDEFINE
*
*-- EndDefine: frmdatepick
**************************************************
