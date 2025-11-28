Attribute VB_Name = "modLoadRes"
' This procedure will load resource strings associated with controls on a
' form based on the Resource ID stored in the Tag property of  a control.

' The resource string will be loaded into a control's property as follows:
' Object      Property
' Form        Caption
' Menu        Caption
' TabStrip    Caption, ToolTipText
' Toolbar     ToolTipText
' ListView    ColumnHeader.Text

Sub LoadResStrings(frm As Form)
  On Error Resume Next
  
  Dim ctl As Control
  Dim obj As Object
  
  'set the form's caption
  If IsNumeric(frm.Tag) Then
    frm.Caption = LoadResString(CInt(frm.Tag))
  End If
  
  'set the controls' captions using the caption
  'property for menu items and the Tag property
  'for all other controls
  For Each ctl In frm.Controls
    Err.Clear
    If TypeName(ctl) = "Menu" Then
      If IsNumeric(ctl.Caption) Then
        If Err = 0 Then
          ctl.Caption = LoadResString(CInt(ctl.Caption))
        End If
      End If
    ElseIf TypeName(ctl) = "TabStrip" Then
      For Each obj In ctl.Tabs
        Err.Clear
        If IsNumeric(obj.Tag) Then
          obj.Caption = LoadResString(CInt(obj.Tag))
        End If
        'check for a tooltip
        If IsNumeric(obj.ToolTipText) Then
          If Err = 0 Then
            obj.ToolTipText = LoadResString(CInt(obj.ToolTipText))
          End If
        End If
      Next
    ElseIf TypeName(ctl) = "Toolbar" Then
      For Each obj In ctl.Buttons
        Err.Clear
        If IsNumeric(obj.Tag) Then
          obj.ToolTipText = LoadResString(CInt(obj.Tag))
        End If
      Next
    ElseIf TypeName(ctl) = "ListView" Then
      For Each obj In ctl.ColumnHeaders
        Err.Clear
        If IsNumeric(obj.Tag) Then
          obj.Text = LoadResString(CInt(obj.Tag))
        End If
      Next
    Else
      If IsNumeric(ctl.Tag) Then
        If Err = 0 Then
          ctl.Caption = LoadResString(CInt(ctl.Tag))
        End If
      End If
      'check for a tooltip
      If IsNumeric(ctl.ToolTipText) Then
        If Err = 0 Then
          ctl.ToolTipText = LoadResString(CInt(ctl.ToolTipText))
        End If
      End If
    End If
  Next

End Sub
