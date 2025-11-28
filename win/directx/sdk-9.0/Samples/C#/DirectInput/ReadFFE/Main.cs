using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using System.Collections;

public class MainForm : Form
{
    private System.ComponentModel.Container components = null;
    private Label lblStatic;
    private Label lblStatic1;
    private Label lblStatic2;
    
    string strPath = string.Empty;
    private Device applicationDevice = null;
    private System.Windows.Forms.Button btnReadFile;
    private System.Windows.Forms.Button btnPlayEffects;
    private System.Windows.Forms.Button btnExit;
    private ArrayList applicationEffects = new ArrayList();

    public static int Main(string[] Args)
    {
        Application.Run(new MainForm());
        return 0;
    }
    
    protected override void Dispose( bool disposing )
    {
        if(disposing)
        {
            if (null != components)
            {
                components.Dispose();
            }
        }
        base.Dispose(disposing);	}
    public MainForm()
    {
        //
        // Req  uired for Windows Form Designer support
        //
        InitializeComponent();
    }
    #region InitializeComponent code
    private void InitializeComponent()
    {
        this.lblStatic = new System.Windows.Forms.Label();
        this.lblStatic1 = new System.Windows.Forms.Label();
        this.lblStatic2 = new System.Windows.Forms.Label();
        this.btnReadFile = new System.Windows.Forms.Button();
        this.btnPlayEffects = new System.Windows.Forms.Button();
        this.btnExit = new System.Windows.Forms.Button();
        this.SuspendLayout();
        // 
        // lblStatic
        // 
        this.lblStatic.Location = new System.Drawing.Point(10, 9);
        this.lblStatic.Name = "lblStatic";
        this.lblStatic.Size = new System.Drawing.Size(312, 13);
        this.lblStatic.TabIndex = 3;
        this.lblStatic.Text = "This sample reads a DirectInput force feedback effects file.  After";
        // 
        // lblStatic1
        // 
        this.lblStatic1.Location = new System.Drawing.Point(10, 24);
        this.lblStatic1.Name = "lblStatic1";
        this.lblStatic1.Size = new System.Drawing.Size(283, 13);
        this.lblStatic1.TabIndex = 4;
        this.lblStatic1.Text = "reading the file the effects can be played back on the force ";
        // 
        // lblStatic2
        // 
        this.lblStatic2.Location = new System.Drawing.Point(10, 38);
        this.lblStatic2.Name = "lblStatic2";
        this.lblStatic2.Size = new System.Drawing.Size(84, 13);
        this.lblStatic2.TabIndex = 5;
        this.lblStatic2.Text = "feedback device.";
        // 
        // btnReadFile
        // 
        this.btnReadFile.Location = new System.Drawing.Point(8, 72);
        this.btnReadFile.Name = "btnReadFile";
        this.btnReadFile.Size = new System.Drawing.Size(80, 24);
        this.btnReadFile.TabIndex = 6;
        this.btnReadFile.Text = "&Read File";
        this.btnReadFile.Click += new System.EventHandler(this.btnReadFile_Click);
        // 
        // btnPlayEffects
        // 
        this.btnPlayEffects.Enabled = false;
        this.btnPlayEffects.Location = new System.Drawing.Point(96, 72);
        this.btnPlayEffects.Name = "btnPlayEffects";
        this.btnPlayEffects.Size = new System.Drawing.Size(80, 24);
        this.btnPlayEffects.TabIndex = 7;
        this.btnPlayEffects.Text = "&Play Effects";
        this.btnPlayEffects.Click += new System.EventHandler(this.btnPlayEffects_Click);
        // 
        // btnExit
        // 
        this.btnExit.Location = new System.Drawing.Point(256, 72);
        this.btnExit.Name = "btnExit";
        this.btnExit.Size = new System.Drawing.Size(80, 24);
        this.btnExit.TabIndex = 8;
        this.btnExit.Text = "&Exit";
        this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
        // 
        // MainForm
        // 
        this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
        this.ClientSize = new System.Drawing.Size(344, 106);
        this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                      this.btnExit,
                                                                      this.btnPlayEffects,
                                                                      this.btnReadFile,
                                                                      this.lblStatic,
                                                                      this.lblStatic1,
                                                                      this.lblStatic2});
        this.Name = "MainForm";
        this.Text = "DirectInput FFE File Reader";
        this.Load += new System.EventHandler(this.MainForm_Load);
        this.Activated += new System.EventHandler(this.MainForm_Activated);
        this.ResumeLayout(false);

    }
    #endregion

    private void btnReadFile_Click(object sender, System.EventArgs e)
    {
        //-----------------------------------------------------------------------------
        // Name: OnReadFile()
        // Desc: Reads a file contain a collection of DirectInput force feedback 
        //       effects.  It creates each of effect read in and stores it 
        //       in the linked list, g_EffectsList.
        //-----------------------------------------------------------------------------
        
        OpenFileDialog ofd = new OpenFileDialog();
        EffectList effects = null;        
    
        if(strPath == string.Empty)
            strPath = DXUtil.SdkMediaPath;

        // Setup the OpenFileDialog structure.
        ofd.Filter = "FEdit Files|*.ffe|All Files|*.*";
        ofd.InitialDirectory = strPath;
        ofd.Title = "Open FEdit File";
        ofd.ShowReadOnly = false;
        
        // Display the OpenFileName dialog. Then, try to load the specified file.
        if( DialogResult.Cancel == ofd.ShowDialog() )
            return;

        // Store the path.
        strPath = Path.GetDirectoryName(ofd.FileName);

        // Get the effects in the file selected.
        effects = applicationDevice.GetEffects(ofd.FileName, FileEffectsFlags.ModifyIfNeeded);

        EmptyEffectList();

        foreach(FileEffect f in effects)
        {
            EffectObject eo = new EffectObject(f.EffectGuid, f.EffectStruct, applicationDevice);
            applicationEffects.Add(eo);
        }

        // If list of effects is empty, then there are no effects created.
        if( 0 == applicationEffects.Count )
        {
            // Pop up a box informing the user.
            MessageBox.Show("Unable to create any effects.");
            btnPlayEffects.Enabled = false;
        }   
        else
        {
            // There are effects, so enable the 'play effects' button.
            btnPlayEffects.Enabled = true;
        }   
    }

    private void MainForm_Load(object sender, System.EventArgs e)
    {
        DeviceList list = Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.AttachedOnly | EnumDevicesFlags.ForceFeeback);
        if(0 == list.Count)
        {
            MessageBox.Show("No force feedback devices attached to the system. Sample will now exit.");
            Close();
        }

        list.MoveNext();
        applicationDevice = new Device(((DeviceInstance)list.Current).InstanceGuid);
        applicationDevice.SetDataFormat(DeviceDataFormat.Joystick);
        applicationDevice.SetCooperativeLevel(this, CooperativeLevelFlags.Exclusive | CooperativeLevelFlags.Background);
        
        try{applicationDevice.Acquire();}
        catch(InputException){}

    }
    private void OnPlayEffects()
    {
        //-----------------------------------------------------------------------------
        // Name: OnPlayEffects()
        // Desc: Plays all of the effects enumerated in the file 
        //-----------------------------------------------------------------------------

        // Stop all previous forces.
        applicationDevice.SendForceFeedbackCommand(ForceFeedbackCommand.StopAll);

        foreach (EffectObject eo in  applicationEffects)
        {
            // Play all of the effects enumerated in the file .
            eo.Start( 1, EffectStartFlags.NoDownload);
        }
    }

    private void btnPlayEffects_Click(object sender, System.EventArgs e)
    {
        OnPlayEffects();
    }

    private void MainForm_Activated(object sender, System.EventArgs e)
    {
        if(null!= applicationDevice)
        {
            try{applicationDevice.Acquire();}
            catch(InputException){}
        }
    }

    private void btnExit_Click(object sender, System.EventArgs e)
    {
        Close();
    }

    private void EmptyEffectList()
    {
        applicationEffects.Clear();
    }
}
