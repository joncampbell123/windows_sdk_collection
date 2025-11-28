//-----------------------------------------------------------------------------
// File: EnumDevices.cs
//
// Desc: This sample shows how to enumerate the current DirectDraw devices.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace EnumDevices
{   
    public class EnumDevices : System.Windows.Forms.Form
    {
        private System.ComponentModel.Container components = null;
        private System.Windows.Forms.TreeView tvDevices;
        private TreeNode Modes = null;

        public EnumDevices()
        {
            InitializeComponent();
            GatherDeviceInformation(); // Call the function that enumerates the devices on the system.
        }




        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }




        #region Windows Form Designer generated code
        private void InitializeComponent()
        {
            this.tvDevices = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // tvDevices
            // 
            this.tvDevices.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right);
            this.tvDevices.ImageIndex = -1;
            this.tvDevices.Location = new System.Drawing.Point(16, 16);
            this.tvDevices.Name = "tvDevices";
            this.tvDevices.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
                                                                                  new System.Windows.Forms.TreeNode("Devices")});
            this.tvDevices.SelectedImageIndex = -1;
            this.tvDevices.Size = new System.Drawing.Size(232, 296);
            this.tvDevices.TabIndex = 0;
            // 
            // EnumDevices
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(264, 326);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.tvDevices});
            this.Name = "EnumDevices";
            this.Text = "EnumDevices";
            this.ResumeLayout(false);

        }
        #endregion




        static void Main() 
        {
            Application.Run(new EnumDevices());
        }




        private void GatherDeviceInformation()
        {
            // Get a DevicesCollection that contains all the installed devices.
            DevicesCollection d = new DevicesCollection(GetDevicesFlags.All);

            foreach (DeviceInformation info in d)
                GetInformation(info);
        }




        private void GetInformation(DeviceInformation d)
        {
            //
            // This function is where the app finds out info about the particular device passed in.
            //

            // Create a few tree nodes that will be used to display device info.
            Modes = new TreeNode();
            TreeNode DeviceNode = new TreeNode();

            DeviceNode.Text = d.Description;
            Modes.Text = "Modes";

            Device draw = null;
            try
            {
                // Create a new DirectDraw.Device object using the passed in Guid object.
                draw = new Device(d.Driver);
            }
            catch
            {
                // This call may fail if the device isn't attached, if so, just ignore it and exit
                return;
            }
            // Now get all of the display modes this device supports.
            DisplayModesCollection modes = new DisplayModesCollection(draw);
            
            foreach(SurfaceDescription desc in modes)
            {
                // Create a new tree node that will be used to store the
                // display mode information.
                TreeNode node = new TreeNode();

                // Add the info to the node.
                node.Text = desc.Width + "x" + desc.Height + "@" + desc.PixelFormatStructure.RgbBitCount + " bit";

                // Add this node to the Modes node.
                Modes.Nodes.Add(node);
            }

            // After the foreach is complete, the Modes node will be filled
            // with child nodes describing the width, height, and bit depth of
            // each mode the display adapter supports. Add this info to the
            // root of the DeviceNode. 
            DeviceNode.Nodes.Add(Modes);
            // Now add this node to the root node.
            tvDevices.Nodes[0].Nodes.Add(DeviceNode);
        }
    }
}
