Attribute VB_Name = "VCRModule"
'**********************************************
' Purpose: General module for the VCR sample
' application. Contains shared procedures
'**********************************************
Option Explicit

' Instantiate the recorder class
Public Recorder As New clsRecorder

'**********************************************
' Purpose:  Enables or disables buttons on the
'           VCR form based on the current mode.
' Inputs:   Button: the Command button calling
'           the procedure.
'**********************************************
Sub ButtonManager(Button As Control)
    Dim vntControl As Variant   ' Control value
    
    ' determine which function button was pushed
    ' and update all buttons and Recorder class
    Select Case Button
        Case frmVCR.cmdPlay, frmVCR.cmdForward, frmVCR.cmdRewind
            frmVCR.cmdPause.Enabled = True
            frmVCR.cmdStop.Enabled = True
            frmVCR.cmdRec.Enabled = False
            frmVCR.cmdDown.Enabled = False
            frmVCR.cmdUp.Enabled = False
            Recorder.Enabled = False
        Case frmVCR.cmdRec
            frmVCR.cmdPause.Enabled = True
            frmVCR.cmdStop.Enabled = True
            frmVCR.cmdPlay.Enabled = False
            frmVCR.cmdForward.Enabled = False
            frmVCR.cmdRewind.Enabled = False
            frmVCR.cmdDown.Enabled = False
            frmVCR.cmdUp.Enabled = False
            Recorder.Enabled = False
        Case frmVCR.cmdPause
            frmVCR.cmdPause.Enabled = False
        Case frmVCR.cmdStop
            frmVCR.cmdStop.Enabled = False
            frmVCR.cmdPause.Enabled = False
            frmVCR.cmdRec.Enabled = True
            frmVCR.cmdPlay.Enabled = True
            frmVCR.cmdForward.Enabled = True
            frmVCR.cmdRewind.Enabled = True
            frmVCR.cmdDown.Enabled = True
            frmVCR.cmdUp.Enabled = True
            Recorder.Enabled = True
    End Select
    ' assign the button to it's corresponding
    ' shape control
    vntControl = Button.Name
    vntControl = Right$(vntControl, Len(vntControl) - 3)
    vntControl = "shp" & vntControl
    ' call routine to update indicators
    HighlightButton vntControl
End Sub
'**********************************************
' Purpose:  Enables or disables shape controls
'           on the VCR form based on the current
'           mode.
' Inputs:   MyControl: the Shape control passed
'           to the procedure.
'**********************************************
Sub HighlightButton(MyControl As Variant)
    Dim i As Integer    ' Counter variable
    
    ' Step through the Controls collection
    For i = 0 To frmVCR.Controls.Count - 1
        ' Test for a Shape control
        If TypeOf frmVCR.Controls(i) Is Shape Then
            ' If it's the passed control, make it
            ' visible, otherwise make it invisible
            If frmVCR.Controls(i).Name = MyControl Then
                frmVCR.Controls(i).Visible = True
            Else
                frmVCR.Controls(i).Visible = False
            End If
        End If
    Next
End Sub
'**********************************************
' Purpose:  Stores the last channel number so
'           we can restore it after recording.
' Inputs:   Channel: the channel number passed
'           to the procedure.
' Returns:  The channel stored in the Static variable.
'**********************************************
Function SaveChannel(Channel As Variant) As Integer
    Static intChannel As Integer 'Stores channel
    
    ' assign the value to the Static variable
    If Channel <> 0 Then
        intChannel = Channel
    End If
    
    ' Return the channel number
    SaveChannel = intChannel
End Function
