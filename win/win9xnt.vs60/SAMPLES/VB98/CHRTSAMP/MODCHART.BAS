Attribute VB_Name = "modChart"
' ------------------------------------------------------------------------
'      Copyright © 1997 Microsoft Corporation.  All rights reserved.
'
' You have a royalty-free right to use, modify, reproduce and distribute
' the Sample Application Files (and/or any modified version) in any way
' you find useful, provided that you agree that Microsoft has no warranty,
' obligations or liability for any Sample Application Files.
' ------------------------------------------------------------------------

Option Explicit
' IMPORTANT NOTE: If you are running Windows NT 4.0, be sure
' to install Service Pack 3.

' Public shtGas As WorkSheet ' Use this if you are using Excel 95
Public appGas As Excel.Application
Public shtGas As Workbook
Public rngGas As Range

Public ExcelWasNotRunning As Boolean
Public intRows              ' Number of rows. Use this to set how many rows in a chart.
Public arrMPG()             ' Miles per gallon
Public arrGall()            ' Gallons per tank
Public arrMiles()           ' Miles per tank
Public arrPrices()          ' Prices of tanks
Public arrPerTank()         ' Price per tank
Public arrMPGandTank()      ' MPG and gallons per tank
Public arrMPGandMiles()     ' MPG and Miles per tank

Public Sub SetupChart()
Attribute SetupChart.VB_Description = "Begins to configure chart by first invoking the GetObject method on an Excel worksheet. "
    ' IMPORTANT: If your machine does not have Excel 97 installed,
    ' you must change the reference to the Excel 95 Object Library.
    ' Then, in the Declarations section above, change the variable
    ' declaration "shtGas as Workbook" to "shtGas As Worksheet"
    
    On Error Resume Next 'Ignore errors
    
    Set appGas = GetObject(, "Excel.Application") 'look for a running copy of Excel
    If Err.Number <> 0 Then 'If Excel is not running then
        Set appGas = CreateObject("Excel.Application") 'run it
        ExcelWasNotRunning = True
    End If
    Err.Clear   ' Clear Err object in case error occurred.
    
    On Error GoTo 0 'Resume normal error processing
    
    Set shtGas = appGas.Workbooks.Open(App.Path & "\gas.xls")

    ' Set the range variable to the CurrentRegion of column A.
    Set rngGas = shtGas.Worksheets(1).Range("A1").CurrentRegion
    
    ' Using the range object, you can now get the number of rows in the
    ' spreadsheet. Subtract 1 because the first row is a header, and not
    ' valid data.
    intRows = rngGas.Rows.Count - 1
        
    ' Configure the chart.
    With frmChart.chtSample
        .Title = shtGas.Name
        .RowCount = intRows ' Set the number of rows. This must be done before
                            '   setting chart data.
        .ColumnCount = 2    ' Two columns. The first shows the miles per
                            '   gallon, the second the gallons used.
        .ColumnLabelCount = 2
        .AllowDynamicRotation = True
        .AllowDithering = True  ' Set this to False if your color monitor
                                '   uses only 8 bits.
                                
        ' Set the legend for the map in the top right corner. Then
        '   coordinates for the legend.
        .Legend.Location.LocationType = VtChLocationTypeTop
        '.Legend.Location.LocationType = VtChLocationTypeTopRight
        .Legend.VtFont.Style = VtFontStyleBold
        .Legend.Location.Rect.Max.Set 7560, 5132
        .Legend.Location.Rect.Min.Set 3004, 4864
    End With
End Sub

Public Sub PopOneArray(thisarray As Variant, col As String)
    ' This procedure just populates arrays.
    Dim i As Integer
    ReDim thisarray(1 To intRows, 1 To 2)
    For i = 1 To intRows
        ' Get the Date values.
        thisarray(i, 1) = CStr(rngGas.Range("A" & i + 1).Value)
        ' Get values.
        thisarray(i, 2) = Format(rngGas.Range(col & i + 1).Value, "##.##")
        frmChart.prgArrays.Value = i
    Next i

End Sub

Public Sub PopTwoArray(ByRef thisarray, col1 As String, col2 As String)
    ' Populates any array with two series. The third element is used to get
    '   date values.
    Dim i As Integer
    ReDim thisarray(1 To intRows, 1 To 3)
    For i = 1 To intRows
        ' Get the Date values.
        thisarray(i, 1) = CStr(rngGas.Range("A" & i + 1).Value)
        ' Get values.
        thisarray(i, 2) = Format(rngGas.Range(col1 & i + 1).Value, "##.##")
        thisarray(i, 3) = Format(rngGas.Range(col2 & i + 1).Value, "##.##")
        frmChart.prgArrays.Value = i
    Next i
End Sub

Public Sub PopThreeArray(ByRef thisarray, col1 As String, col2 As String, col3 As String)
    Dim i As Integer
    ReDim thisarray(1 To intRows, 1 To 4)
    For i = 1 To intRows
        ' Get the Date values.
        thisarray(i, 1) = CStr(rngGas.Range("A" & i + 1).Value)
        ' Get values.
        thisarray(i, 2) = Format(rngGas.Range(col1 & i + 1).Value, "##.##")
        thisarray(i, 3) = Format(rngGas.Range(col2 & i + 1).Value, "##.##")
        thisarray(i, 4) = Format(rngGas.Range(col3 & i + 1).Value, "##.##")
        frmChart.prgArrays.Value = i
    Next i
End Sub

Public Sub Chart(ByRef arrayName(), chtTitle As String, colLabel As String)
Attribute Chart.VB_Description = "This procedure takes an array as an argument, and sets the ChartData property to the array, which creates a chart."
    ' This procedure takes an array as an argument, and sets the
    '   ChartData property to the array, which creates a chart.
    With frmChart.chtSample
        .ChartData = arrayName()
        .Title = chtTitle
        .ColumnCount = 1
        .ColumnLabelCount = 1
        .Column = 1
        .ColumnLabel = colLabel
        .Refresh
    End With
End Sub

Public Sub twoColChart()

    ' Use the arrMPGandTank array to create a two column chart
    With frmChart.chtSample
        .ChartData = arrMPGandTank
        .Title = "Miles per Gallon"
        .ColumnLabelCount = 2
        .ColumnCount = 2
        .Column = 1
        .ColumnLabel = "Gallons"
        .Column = 2
        .ColumnLabel = "MPG"
        .Refresh
    End With
End Sub

Public Sub threeColChart()
    ' Use the array MPGandMiles to create a three column chart. Set the
    '   ChartData property to the array, then set the title and column
    '   labels.
    With frmChart.chtSample
        .ChartData = arrMPGandMiles
        .Title = "Costs"
        .ColumnCount = 3
        .ColumnLabelCount = 3
        .Column = 1
        .ColumnLabel = "Price per Gallon"
        .Column = 2
        .ColumnLabel = "Gallons"
        .Column = 3
        .ColumnLabel = "Price * Gallons"
        .Refresh
    End With
End Sub

Public Sub Cleanup()
    ' Invoke this sub before the app terminates.
    ' Set all global variables to nothing.

    
    shtGas.Close 'close worksheet
    
    Set shtGas = Nothing
    Set rngGas = Nothing
    
    ' If this copy of Microsoft Excel was not running when you
    ' started, close it using the Application property's Quit method.
    ' Note that when you try to quit Microsoft Excel, the
    ' title bar blinks and a message is displayed asking if you
    ' want to save any loaded files.
    If ExcelWasNotRunning = True Then
        appGas.Quit
    End If
    Set appGas = Nothing
End Sub

