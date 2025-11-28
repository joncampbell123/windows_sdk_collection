using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.DirectSound;
using SoundDevice = Microsoft.DirectX.DirectSound.Device;
public class DlgMainForm : Form
{
    private System.ComponentModel.IContainer components;
    #region Window control declarations

    private System.Windows.Forms.Button btnOk;
    private GroupBox gbStatic;
    private System.Windows.Forms.Button btnButtonBass;
    private System.Windows.Forms.Button btnButtonSnare;
    private System.Windows.Forms.Button btnButtonHihatUp;
    private System.Windows.Forms.Button btnButtonHihatDown;
    private System.Windows.Forms.Button btnButtonCrash;
    private System.Windows.Forms.Button btnButtonUser1;
    private System.Windows.Forms.Button btnButtonUser2;
    private System.Windows.Forms.Button btnButtonUser3;
    private Label lblTextFilename1;
    private Label lblTextFilename2;
    private Label lblTextFilename3;
    private Label lblTextFilename4;
    private Label lblTextFilename5;
    private Label lblTextFilename6;
    private Label lblTextFilename7;
    private Label lblTextFilename8;
    private System.Windows.Forms.Button btnButtonDevice;
    #endregion

    //-----------------------------------------------------------------------------
    // enumeration of hypothetical control functions
    //-----------------------------------------------------------------------------
    enum GameActions
    {
        BassDrum, // base drum
        SnareDrum, // snare drum
        HiHatOpen, // open hihat
        HiHatClose, // closed hihat
        Crash, // crash
        User1, // user assigned one
        User2, // user assigned two
        User3, // user assigned three

        NumberActions // auto count for number of enumerated actions
    };
    
    // Unique application guid for the action mapping.
    private static Guid guidApp = new Guid(0x21bee2a7, 0x32d7, 0x43c8, 0x92, 0x41, 0x1d, 0x38, 0xb, 0x6, 0xd0, 0x5);
    private string pathSoundFile = DXUtil.SdkMediaPath;
    private SoundDevice applicationSoundDevice = null;
    private SecondaryBuffer[] applicationSoundBuffers = null;
    private DeviceList applicationDevices = null;
    private ActionFormat[] mappings = null;
    private Action[] actions;
    private bool[] buttonStates = new bool[ (int)GameActions.NumberActions ];
    private float[] boxColors = new float[(int)GameActions.NumberActions];
    private const int RectangleTop = 27;
    private const int RectangleLeft = 315;
    private System.Windows.Forms.Timer timer1;
    private Graphics applicationGraphics = null;

    public static int Main(string[] Args)
    {
        Application.Run(new DlgMainForm());
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
        base.Dispose(disposing);	
    }
    public DlgMainForm()
    {
        //
        // Required for Windows Form Designer support
        //
        InitializeComponent();        
    }
    #region InitializeComponent code
    private void InitializeComponent()
    {
        this.components = new System.ComponentModel.Container();
        this.btnOk = new System.Windows.Forms.Button();
        this.gbStatic = new System.Windows.Forms.GroupBox();
        this.btnButtonBass = new System.Windows.Forms.Button();
        this.btnButtonSnare = new System.Windows.Forms.Button();
        this.btnButtonHihatUp = new System.Windows.Forms.Button();
        this.btnButtonHihatDown = new System.Windows.Forms.Button();
        this.btnButtonCrash = new System.Windows.Forms.Button();
        this.btnButtonUser1 = new System.Windows.Forms.Button();
        this.btnButtonUser2 = new System.Windows.Forms.Button();
        this.btnButtonUser3 = new System.Windows.Forms.Button();
        this.lblTextFilename1 = new System.Windows.Forms.Label();
        this.lblTextFilename2 = new System.Windows.Forms.Label();
        this.lblTextFilename3 = new System.Windows.Forms.Label();
        this.lblTextFilename4 = new System.Windows.Forms.Label();
        this.lblTextFilename5 = new System.Windows.Forms.Label();
        this.lblTextFilename6 = new System.Windows.Forms.Label();
        this.lblTextFilename7 = new System.Windows.Forms.Label();
        this.lblTextFilename8 = new System.Windows.Forms.Label();
        this.btnButtonDevice = new System.Windows.Forms.Button();
        this.timer1 = new System.Windows.Forms.Timer(this.components);
        this.SuspendLayout();
        // 
        // btnOk
        // 
        this.btnOk.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnOk.Location = new System.Drawing.Point(288, 320);
        this.btnOk.Name = "btnOk";
        this.btnOk.TabIndex = 0;
        this.btnOk.Text = "OK";
        this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
        // 
        // gbStatic
        // 
        this.gbStatic.Location = new System.Drawing.Point(7, 8);
        this.gbStatic.Name = "gbStatic";
        this.gbStatic.Size = new System.Drawing.Size(360, 304);
        this.gbStatic.TabIndex = 1;
        this.gbStatic.TabStop = false;
        this.gbStatic.Text = "Samples";
        this.gbStatic.Paint += new System.Windows.Forms.PaintEventHandler(this.gbStatic_Paint);
        // 
        // btnButtonBass
        // 
        this.btnButtonBass.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnButtonBass.Location = new System.Drawing.Point(22, 32);
        this.btnButtonBass.Name = "btnButtonBass";
        this.btnButtonBass.TabIndex = 2;
        this.btnButtonBass.Tag = "0";
        this.btnButtonBass.Text = "Bass Drum";
        this.btnButtonBass.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonSnare
        // 
        this.btnButtonSnare.Location = new System.Drawing.Point(22, 64);
        this.btnButtonSnare.Name = "btnButtonSnare";
        this.btnButtonSnare.TabIndex = 3;
        this.btnButtonSnare.Tag = "1";
        this.btnButtonSnare.Text = "Snare";
        this.btnButtonSnare.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonHihatUp
        // 
        this.btnButtonHihatUp.Location = new System.Drawing.Point(22, 96);
        this.btnButtonHihatUp.Name = "btnButtonHihatUp";
        this.btnButtonHihatUp.TabIndex = 4;
        this.btnButtonHihatUp.Tag = "2";
        this.btnButtonHihatUp.Text = "Hi-Hat Up";
        this.btnButtonHihatUp.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonHihatDown
        // 
        this.btnButtonHihatDown.Location = new System.Drawing.Point(22, 128);
        this.btnButtonHihatDown.Name = "btnButtonHihatDown";
        this.btnButtonHihatDown.TabIndex = 5;
        this.btnButtonHihatDown.Tag = "3";
        this.btnButtonHihatDown.Text = "Hi-Hat";
        this.btnButtonHihatDown.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonCrash
        // 
        this.btnButtonCrash.Location = new System.Drawing.Point(22, 160);
        this.btnButtonCrash.Name = "btnButtonCrash";
        this.btnButtonCrash.TabIndex = 6;
        this.btnButtonCrash.Tag = "4";
        this.btnButtonCrash.Text = "Crash";
        this.btnButtonCrash.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonUser1
        // 
        this.btnButtonUser1.Location = new System.Drawing.Point(22, 192);
        this.btnButtonUser1.Name = "btnButtonUser1";
        this.btnButtonUser1.TabIndex = 7;
        this.btnButtonUser1.Tag = "5";
        this.btnButtonUser1.Text = "User 1";
        this.btnButtonUser1.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonUser2
        // 
        this.btnButtonUser2.Location = new System.Drawing.Point(22, 224);
        this.btnButtonUser2.Name = "btnButtonUser2";
        this.btnButtonUser2.TabIndex = 8;
        this.btnButtonUser2.Tag = "6";
        this.btnButtonUser2.Text = "User 2";
        this.btnButtonUser2.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // btnButtonUser3
        // 
        this.btnButtonUser3.Location = new System.Drawing.Point(22, 256);
        this.btnButtonUser3.Name = "btnButtonUser3";
        this.btnButtonUser3.TabIndex = 9;
        this.btnButtonUser3.Tag = "7";
        this.btnButtonUser3.Text = "User 3";
        this.btnButtonUser3.Click += new System.EventHandler(this.FileChange_Click);
        // 
        // lblTextFilename1
        // 
        this.lblTextFilename1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename1.Location = new System.Drawing.Point(105, 35);
        this.lblTextFilename1.Name = "lblTextFilename1";
        this.lblTextFilename1.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename1.TabIndex = 10;
        this.lblTextFilename1.Tag = "";
        this.lblTextFilename1.Text = "No file loaded.";
        this.lblTextFilename1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename2
        // 
        this.lblTextFilename2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename2.Location = new System.Drawing.Point(105, 67);
        this.lblTextFilename2.Name = "lblTextFilename2";
        this.lblTextFilename2.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename2.TabIndex = 11;
        this.lblTextFilename2.Tag = "";
        this.lblTextFilename2.Text = "No file loaded.";
        this.lblTextFilename2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename3
        // 
        this.lblTextFilename3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename3.Location = new System.Drawing.Point(105, 99);
        this.lblTextFilename3.Name = "lblTextFilename3";
        this.lblTextFilename3.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename3.TabIndex = 12;
        this.lblTextFilename3.Tag = "";
        this.lblTextFilename3.Text = "No file loaded.";
        this.lblTextFilename3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename4
        // 
        this.lblTextFilename4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename4.Location = new System.Drawing.Point(105, 131);
        this.lblTextFilename4.Name = "lblTextFilename4";
        this.lblTextFilename4.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename4.TabIndex = 13;
        this.lblTextFilename4.Tag = "";
        this.lblTextFilename4.Text = "No file loaded.";
        this.lblTextFilename4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename5
        // 
        this.lblTextFilename5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename5.Location = new System.Drawing.Point(105, 163);
        this.lblTextFilename5.Name = "lblTextFilename5";
        this.lblTextFilename5.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename5.TabIndex = 14;
        this.lblTextFilename5.Tag = "";
        this.lblTextFilename5.Text = "No file loaded.";
        this.lblTextFilename5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename6
        // 
        this.lblTextFilename6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename6.Location = new System.Drawing.Point(105, 195);
        this.lblTextFilename6.Name = "lblTextFilename6";
        this.lblTextFilename6.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename6.TabIndex = 15;
        this.lblTextFilename6.Tag = "";
        this.lblTextFilename6.Text = "No file loaded.";
        this.lblTextFilename6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename7
        // 
        this.lblTextFilename7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename7.Location = new System.Drawing.Point(105, 227);
        this.lblTextFilename7.Name = "lblTextFilename7";
        this.lblTextFilename7.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename7.TabIndex = 16;
        this.lblTextFilename7.Tag = "";
        this.lblTextFilename7.Text = "No file loaded.";
        this.lblTextFilename7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lblTextFilename8
        // 
        this.lblTextFilename8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.lblTextFilename8.Location = new System.Drawing.Point(105, 259);
        this.lblTextFilename8.Name = "lblTextFilename8";
        this.lblTextFilename8.Size = new System.Drawing.Size(210, 16);
        this.lblTextFilename8.TabIndex = 17;
        this.lblTextFilename8.Tag = "";
        this.lblTextFilename8.Text = "No file loaded.";
        this.lblTextFilename8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // btnButtonDevice
        // 
        this.btnButtonDevice.Location = new System.Drawing.Point(7, 320);
        this.btnButtonDevice.Name = "btnButtonDevice";
        this.btnButtonDevice.Size = new System.Drawing.Size(157, 23);
        this.btnButtonDevice.TabIndex = 18;
        this.btnButtonDevice.Text = "&View Device Mappings";
        this.btnButtonDevice.Click += new System.EventHandler(this.btnButtonDevice_Click);
        // 
        // timer1
        // 
        this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
        // 
        // DlgMainForm
        // 
        this.AcceptButton = this.btnOk;
        this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
        this.ClientSize = new System.Drawing.Size(378, 354);
        this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                      this.btnOk,
                                                                      this.btnButtonBass,
                                                                      this.btnButtonSnare,
                                                                      this.btnButtonHihatUp,
                                                                      this.btnButtonHihatDown,
                                                                      this.btnButtonCrash,
                                                                      this.btnButtonUser1,
                                                                      this.btnButtonUser2,
                                                                      this.btnButtonUser3,
                                                                      this.lblTextFilename1,
                                                                      this.lblTextFilename2,
                                                                      this.lblTextFilename3,
                                                                      this.lblTextFilename4,
                                                                      this.lblTextFilename5,
                                                                      this.lblTextFilename6,
                                                                      this.lblTextFilename7,
                                                                      this.lblTextFilename8,
                                                                      this.btnButtonDevice,
                                                                      this.gbStatic});
        this.Name = "DlgMainForm";
        this.Text = "Drum Pad";
        this.Load += new System.EventHandler(this.DlgMainForm_Load);
        this.ResumeLayout(false);

    }
    #endregion

    private void DlgMainForm_Load(object sender, System.EventArgs e)
    {
        string sdkpath = DXUtil.SdkMediaPath;

        // Get the graphics object for the static groupbox control.
        applicationGraphics = gbStatic.CreateGraphics();
        mappings = new ActionFormat[1];
        mappings[0] = new ActionFormat();

        ConstructActionMap();
        // Get the devices on the system that can be mapped to the action map, and are attached.
        applicationDevices = Manager.GetDevices(mappings[0], EnumDevicesBySemanticsFlags.AttachedOnly);

        // Make sure there are sound devices available.
        if(0 == applicationDevices.Count)
        {
            MessageBox.Show("No available input devices. Sample will close.");
            Close();
            return;
        }

        try
        {
            // Create a sound device, and sound buffer array.
            applicationSoundDevice = new SoundDevice();
            applicationSoundDevice.SetCooperativeLevel(this, CooperativeLevel.Normal);
            applicationSoundBuffers = new SecondaryBuffer[8];
        }
        catch(SoundException)
        {
            MessageBox.Show("No available sound device. Sample will close.");
            Close();
            return;
        }

        foreach(SemanticsInstance instance in applicationDevices)
        {
            // Build and set the action map against each device.
            instance.Device.BuildActionMap( mappings[0], ActionMapControl.Default );
            instance.Device.SetActionMap(mappings[0], ApplyActionMap.Default );
        }

        // Load default wav files.
        LoadSoundFile(sdkpath + "drumpad-bass_drum.wav",0);
        LoadSoundFile(sdkpath + "drumpad-snare_drum.wav",1);
        LoadSoundFile(sdkpath + "drumpad-hhat_up.wav",2);
        LoadSoundFile(sdkpath + "drumpad-hhat_down.wav",3);
        LoadSoundFile(sdkpath + "drumpad-crash.wav",4);
        LoadSoundFile(sdkpath + "drumpad-voc_female_ec.wav",5);
        LoadSoundFile(sdkpath + "drumpad-speech.wav",6);

        // Turn on the timer.
        timer1.Enabled = true;
    }
    void ConstructActionMap()
    { 
        //-----------------------------------------------------------------------------
        // Name: ConstructActionMap()
        // Desc: Prepares the action map to be used
        //-----------------------------------------------------------------------------
             
        // Allocate and copy static DIACTION array
        InitializeActions();
        
        foreach(Action a in actions)
            mappings[0].Actions.Add(a);
    
        // Set the application Guid
        mappings[0].ActionMapGuid = guidApp;
        mappings[0].AxisMax = 100;
        mappings[0].AxisMin = -100;
        mappings[0].BufferSize = 16;    
        
        // Game genre
        mappings[0].Genre = (int)FightingThirdPerson.FightingThirdPerson;

        // Friendly name for this mapping
        mappings[0].ActionMap = "DeviceView - Sample Action Map";
    }    
    
    void InitializeActions()
    {
        //-----------------------------------------------------------------------------
        // actions array
        //-----------------------------------------------------------------------------
        // Be sure to delete user map files 
        // (GameActions.C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini)
        // after changing this, otherwise settings won't reset and will be read 
        // from the out of date ini files 

        actions = new Action[]
        {
            // genre defined virtual buttons
            MakeAction(GameActions.BassDrum, (int)FightingThirdPerson.ButtonAction, "Bass Drum"),
            MakeAction(GameActions.SnareDrum,(int)FightingThirdPerson.ButtonJump, "Snare Drum"),
            MakeAction(GameActions.HiHatOpen, (int)FightingThirdPerson.ButtonUse, "Open Hi-Hat"),
            MakeAction(GameActions.HiHatClose, (int)FightingThirdPerson.ButtonRun, "Closed Hi-Hat"),
            MakeAction(GameActions.Crash, (int)FightingThirdPerson.ButtonMenu, "Crash"),
            MakeAction(GameActions.User1, (int)FightingThirdPerson.ButtonDodge, "User 1"),
            MakeAction(GameActions.User2, (int)FightingThirdPerson.ButtonSelect, "User 2"),
            MakeAction(GameActions.User3, (int)FightingThirdPerson.ButtonView, "User 3"),

            // keyboard mapping
            MakeAction(GameActions.BassDrum, (int)Keyboard.B, "Bass Drum"),
            MakeAction(GameActions.SnareDrum, (int)Keyboard.N, "Snare Drum"),
            MakeAction(GameActions.HiHatOpen, (int)Keyboard.A, "Open Hi-Hat"),
            MakeAction(GameActions.HiHatClose, (int)Keyboard.Z, "Closed Hi-Hat"),
            MakeAction(GameActions.Crash, (int)Keyboard.C, "Crash"),
            MakeAction(GameActions.User1, (int)Keyboard.K, "User 1"),
            MakeAction(GameActions.User2, (int)Keyboard.L, "User 2"),
            MakeAction(GameActions.User3, (int)Keyboard.SemiColon, "User 3"),

            // mouse mapping
            MakeAction(GameActions.BassDrum, (int)Mouse.Button2, "Bass Drum"),
            MakeAction(GameActions.SnareDrum, (int)Mouse.Button1, "Snare Drum"),
            MakeAction(GameActions.HiHatOpen, (int)Mouse.Button3, "Open Hi-Hat"),
            MakeAction(GameActions.HiHatClose, (int)Mouse.Button4, "Closed Hi-Hat"),
            MakeAction(GameActions.Crash, (int)Mouse.Wheel, "Crash"),
            MakeAction(GameActions.User1, (int)Mouse.Button5, "User 1"),
            MakeAction(GameActions.User2, (int)Mouse.Button6, "User 2"),
            MakeAction(GameActions.User3, (int)Mouse.Button7, "User 3")
        };
    }
    Action MakeAction(GameActions a, int semantic, string name)
    {
        // Fills in an action struct with data.

        Action ret = new Action();
        ret.ActionName = name;
        ret.Semantic = semantic;
        ret.ApplicationData = a;

        return ret;
    }
    void ProcessInput()
    {        
        //-----------------------------------------------------------------------------
        // Name: ProcessInput()
        // Desc: Gathers user input, plays audio, and draws output. Input is gathered 
        //       from the DInput devices, and output is displayed in the app's window.
        //-----------------------------------------------------------------------------

        // Loop through all devices and check game input
        applicationDevices.Reset();
        foreach(SemanticsInstance instance in applicationDevices)
        {
            BufferedDataCollection data = null;

            // Need to ensure that the devices are acquired, and pollable devices
            // are polled.
            try
            {                
                instance.Device.Acquire();
                instance.Device.Poll();
                data = instance.Device.GetBufferedData();
            }
            catch(InputException){ continue; }
                              
            if (null == data)
                continue;

            // Get the sematics codes. 
            // Each event has a type stored in "ApplicationData", and actual data is stored in
            // "Data".
            foreach(BufferedData d in data) 
            {
                // Non-axis data is recieved as "button pressed" or "button
                // released". Parse input as such.
                bool state = (0x80 == d.Data) ? true : false;
                int index  = (int)d.ApplicationData;

                if( buttonStates[index] == false && state)
                {
                    PlaySound(index);
                    boxColors[index] = 255.0f;
                }
                buttonStates[index] = state;
            }
        }
    
        DrawRects(false);
    }

    void DrawRects(bool drawAll)
    {
        Color c = new Color();

        for( int i = 0; i < (int)GameActions.NumberActions; i++ )
        {
            Rectangle r = new Rectangle( RectangleLeft, 0, 35, 16);

            if( boxColors[i] >= 0.0f || drawAll )
            {
                // make sure color is not negative
                if( boxColors[i] < 0.0f )
                    boxColors[i] = 0.0f;

                // figure out what color to use
                c = Color.FromArgb( (int)boxColors[i] / 2,  (int)boxColors[i], 0);
                
                r.Offset(0, RectangleTop + (int)(i* 32) );

                // draw the rectangle
                applicationGraphics.FillRectangle(new SolidBrush(c), r);
                
                // fade the color to black
                boxColors[i] -= 3.5f;
            }
        }
    }

    private void timer1_Tick(object sender, System.EventArgs e)
    {
        ProcessInput();
    }
    private void btnOk_Click(object sender, System.EventArgs e)
    {
        Close();
    }

    void PlaySound(int index)
    {
        if(null != applicationSoundBuffers[index] )
            applicationSoundBuffers[index].Play(0, BufferPlayFlags.Default);    
    }

    private void FileChange_Click(object sender, System.EventArgs e)
    {
        // Get the index from the tag object of the sender.
        string s = (string)((System.Windows.Forms.Button)sender).Tag;
        int i = (int)s[0] - 48;

        OpenFileDialog ofd = new OpenFileDialog();
        ofd.InitialDirectory = pathSoundFile;
        ofd.Filter=  "Wave files(*.wav)|*.wav";

        if( DialogResult.Cancel == ofd.ShowDialog() )
            return;
     
        if(LoadSoundFile(ofd.FileName, i))
            pathSoundFile = Path.GetDirectoryName(ofd.FileName);
    }

    private bool LoadSoundFile(string filename, int index)
    {
        BufferDescription desc = new BufferDescription();
        Label target = null;

        // Iterate the collection to find the label control that corresponds to this index.
        foreach(Control temp in Controls)
        {
            if( typeof(Label) ==  temp.GetType() )
            {
                if(temp.Name == "lblTextFilename" + (index + 1))
                {
                    target = (Label)temp;
                    break;
                }
            }
        }

        try
        {
            // Attempt to create a sound buffer out of this file.
            applicationSoundBuffers[index] = new SecondaryBuffer(filename, desc, applicationSoundDevice);
        }
        catch(SoundException)
        {
            target.Text = "No file loaded.";
            return false;
        }
        
        target.Text = Path.GetFileName(filename);
        return true;
    }

    private void gbStatic_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
    {
        DrawRects(true);
    }

    private void btnButtonDevice_Click(object sender, System.EventArgs e)
    {
        ConfigureDevicesParameters cdp = new ConfigureDevicesParameters();
        cdp.SetActionFormats(mappings);
        
        Manager.ConfigureDevices(cdp, ConfigureDevicesFlags.Default);
    }
}
