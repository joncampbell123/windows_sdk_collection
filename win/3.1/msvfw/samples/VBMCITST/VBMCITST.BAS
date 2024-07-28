'
'   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
'   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
'   PURPOSE.
'
'   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
'
Option Explicit

Global NotifyStr(1 To 4) As String  ' as notification info

Global MoreFlag As Integer          ' Display more or less details
Global MoreSpace As Integer         ' detail properties' space
Global MDIWidth As Integer          ' original MDI form width
Global CanRecord As Integer         ' can current device record

Sub ResizeMDIHeight ()
    Dim FormHeight As Long

    frmChild.Top = 120
    frmChild.Height = frmChild.MCIWnd.Height + 24 ' 24 for border
    If (mdiSample.WindowState = 0) Then  ' normal state
        ' app window height = prop area height + control height + extra space (1000)
        FormHeight = mdiSample.picSpace.Height + frmChild.MCIWnd.Height + 1000

        ' if this height is more than screen height then reduce the height
        ' and/or reposition the app main window.
        If (FormHeight > Screen.Height) Then
            mdiSample.Height = Screen.Height
            mdiSample.Top = 0
        Else
            mdiSample.Height = FormHeight
            If (mdiSample.Top + mdiSample.Height > Screen.Height) Then
                mdiSample.Top = Screen.Height - mdiSample.Height
            End If
        End If
    End If
End Sub

Sub ResizeMDIWidth ()
    Dim FormWidth As Long

    frmChild.Left = 120
    frmChild.Width = frmChild.MCIWnd.Width + 24  ' 24 for borders
    If (mdiSample.WindowState = 0) Then  ' normal state
        ' make app main window wide enough
        If (frmChild.MCIWnd.Width + frmChild.MCIWnd.Left + 200 > MDIWidth) Then
            FormWidth = frmChild.MCIWnd.Width + frmChild.MCIWnd.Left + 200
        Else
            FormWidth = MDIWidth
        End If

        ' if this width is more than screen width then reduce the width
        ' and/or reposition the app main window.
        If (FormWidth > Screen.Width) Then
            mdiSample.Width = Screen.Width
            mdiSample.Left = 0
        Else
            mdiSample.Width = FormWidth
            If (mdiSample.Left + mdiSample.Width > Screen.Width) Then
                mdiSample.Left = Screen.Width - mdiSample.Width
            End If
        End If
    End If
End Sub

