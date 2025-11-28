Imports System
Imports System.Drawing
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw


Namespace EnumDevices
    Public Class EnumDevices
        Inherits System.Windows.Forms.Form
        Private components As System.ComponentModel.Container = Nothing
        Private tvDevices As System.Windows.Forms.TreeView
        Private Modes As TreeNode = Nothing


        Public Sub New()
            InitializeComponent()
            GatherDeviceInformation() ' Call the function that enumerates the devices on the system.
        End Sub 'New

        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose

        Private Sub InitializeComponent()

            Me.tvDevices = New System.Windows.Forms.TreeView()
            Me.SuspendLayout()
            ' 
            ' tvDevices
            ' 
            Me.tvDevices.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.tvDevices.ImageIndex = -1
            Me.tvDevices.Location = New System.Drawing.Point(16, 16)
            Me.tvDevices.Name = "tvDevices"
            Me.tvDevices.Nodes.AddRange(New System.Windows.Forms.TreeNode() {New System.Windows.Forms.TreeNode("Devices")})
            Me.tvDevices.SelectedImageIndex = -1
            Me.tvDevices.Size = New System.Drawing.Size(232, 296)
            Me.tvDevices.TabIndex = 0
            ' 
            ' EnumDevices
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(264, 326)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.tvDevices})
            Me.Name = "EnumDevices"
            Me.Text = "EnumDevices"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        Public Shared Sub Main()
            Application.Run(New EnumDevices())
        End Sub 'Main

        Private Sub GatherDeviceInformation()
            ' Get a DevicesCollection that contains all the installed devices.
            Dim d As New DevicesCollection(GetDevicesFlags.All)

            Dim info As DeviceInformation
            For Each info In d
                GetInformation(info)
            Next info
        End Sub 'GatherDeviceInformation

        Private Sub GetInformation(ByVal d As DeviceInformation)
            '
            ' This function is where the app finds out info about the particular device passed in.
            '
            ' Create a few tree nodes that will be used to display device info.
            Modes = New TreeNode()
            Dim DeviceNode As New TreeNode()

            DeviceNode.Text = d.Description
            Modes.Text = "Modes"

            Dim draw As Device = Nothing
            Try
                ' Create a new DirectDraw.Device object using the passed in Guid object.
                draw = New Device(d.Driver)
            Catch
                ' This call may fail if the device isn't attached, if so, just ignore it and exit
                Return
            End Try
            ' Now get all of the display modes this device supports.
            Dim m As New DisplayModesCollection(draw)

            Dim desc As SurfaceDescription
            For Each desc In m
                ' Create a new tree node that will be used to store the
                ' display mode information.
                Dim node As New TreeNode()

                ' Add the info to the node.
                node.Text = desc.Width.ToString() + "x" + desc.Height.ToString() + "@" + desc.PixelFormatStructure.RgbBitCount.ToString() + " bit"

                ' Add this node to the Modes node.
                Modes.Nodes.Add(node)
            Next desc

            ' After the foreach is complete, the Modes node will be filled
            ' with child nodes describing the width, height, and bit depth of
            ' each mode the display adapter supports. Add this info to the
            ' root of the DeviceNode. 
            DeviceNode.Nodes.Add(Modes)
            ' Now add this node to the root node.
            tvDevices.Nodes(0).Nodes.Add(DeviceNode)
        End Sub 'GetInformation
    End Class 'EnumDevices
End Namespace 'EnumDevices