<HTML>
<HEAD>
<TITLE>RDS 1.1 Query Page</TITLE>
</HEAD>
<BODY BACKGROUND="Addressbook/Arcadia.gif">
<CENTER>
<H1>Remote Data Service 1.1 Query Page</H1>
<BR>

<OBJECT ID="GRID" WIDTH=700 HEIGHT=200
	CLASSID="CLSID:BC496AE0-9B4E-11CE-A6D5-0000C0BE9395"
	CODEBASE="HTTP://<%=Request.ServerVariables("SERVER_NAME")%>/MSADC/Samples/ssdatb32.cab">
	<PARAM NAME="BackColor"     VALUE="-2147483643">
	<PARAM NAME="BackColorOdd"  VALUE="-2147483643">
	<PARAM NAME="ForeColorEven" VALUE="0">	
</OBJECT>
<BR>
<BR>

<table>
	<tr><td>ADC Server:<td><INPUT NAME=Server SIZE=70>
	<tr><td>Connection:<td><INPUT NAME=Connect SIZE=70>
	<tr><td>Query:<td><INPUT NAME=SQL SIZE=70>
	</table>

<BR>
<INPUT TYPE=BUTTON NAME="Run" VALUE="Run!">
<INPUT TYPE=BUTTON NAME="First" VALUE="First">
<INPUT TYPE=BUTTON NAME="Prev" VALUE="Prev">
<INPUT TYPE=BUTTON NAME="Next" VALUE="Next">
<INPUT TYPE=BUTTON NAME="Last" VALUE="Last"> <BR><BR>
<INPUT TYPE=BUTTON NAME="Update" VALUE="Save Changes">
<INPUT TYPE=BUTTON NAME="Cancel" VALUE="Cancel Changes">
</CENTER>


<OBJECT CLASSID="clsid:9381D8F2-0288-11d0-9501-00AA00B911A5"
    	ID=ADC HEIGHT=1 WIDTH = 1
	CODEBASE="HTTP://<%=Request.ServerVariables("SERVER_NAME")%>/MSADC/mdac11.cab#version=1,1,2,0">
    	<PARAM NAME="BINDINGS" VALUE="Grid;">
	</OBJECT>


<SCRIPT LANGUAGE= "VBScript">

SUB Window_OnLoad
	Server.Value = "http://<%=Request.ServerVariables("SERVER_NAME")%>"
   	Connect.Value = "DSN=AdvWorks"
   	SQL.Value = "Select * from Products"
END SUB

SUB First_onClick
	ADC.MoveFirst
END SUB

SUB Next_onClick
	On Error Resume Next
	ADC.MoveNext
	IF ERR.Number <> 0 THEN
		ADC.MoveLast  'If already at end of recordset stay at end.
	END IF
END SUB

SUB Prev_onClick
	On Error Resume Next
	ADC.MovePrevious
	IF ERR.Number <> 0 THEN
		ADC.MoveFirst  'If already at start of recordset stay at top.
	END IF
END SUB

SUB Last_onClick
	ADC.MoveLast
END SUB

'Submits edits made and pull a clean copy of the new data.
SUB Update_onClick
  	ADC.SubmitChanges
	ADC.Refresh

	'Optional command specific to the Sheridan Grid to ensure display of the correct data
	Grid.Rebind
END SUB

'Cancel edits and restores original values.
SUB Cancel_onClick
  	ADC.CancelUpdate
	ADC.Refresh

	'Optional command specific to the Sheridan Grid to ensure display of the correct data
	Grid.Rebind
END SUB

SUB Run_onClick
	ADC.Server = Server.Value
    	ADC.Connect = Connect.Value
    	ADC.SQL = SQL.Value

	ADC.Refresh

	'Optional command specific to the Sheridan Grid to ensure display of the correct data
	Grid.Rebind
END SUB

</SCRIPT>
</BODY>
</HTML>
