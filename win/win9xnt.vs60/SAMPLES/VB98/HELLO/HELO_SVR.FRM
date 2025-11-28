VERSION 5.00
Begin VB.Form frmHello 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "HelloWorld Server"
   ClientHeight    =   1110
   ClientLeft      =   3150
   ClientTop       =   2325
   ClientWidth     =   2820
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1110
   ScaleWidth      =   2820
End
Attribute VB_Name = "frmHello"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Load()
'Note1: The following two segments of code demonstrate two options
'for instantiating an object by a client... the first one uses early binding
'and requires "HelloProj" to be referenced from the Tools.References
'dialog.  the second option uses late binding and requires an
'additional statement to create the object.  Early binding is faster
'than late binding... but late binding gives the designer greater flexibility
'in deciding at runtime what objects are to be used.

'Note2:  This client code is present in this project to facilitate debugging.
'The code can be used in this (server) project to verify syntax and
'basic functionality.  It can then be pasted into a separate client
'project and 'used *without* changes to access the server from an
'external executable.  The fact that object creation is identical across
'internal and external project boundaries can greatly facilitate testing
'and code sharing.

'Early binding option
'Dim objNew As New HelloProj.HelloClass
'MsgBox objNew.SayHello
'Set objNew = Nothing

'Late binding option
'Dim objNew As Object
'Set objNew = CreateObject("HelloProj.HelloClass")
'MsgBox objNew.SayHello
'Set objNew = Nothing
End Sub
