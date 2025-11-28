using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectInput;
using System.Threading;
using System.Diagnostics;
using System.IO;

namespace Scrawl
{
	/// <summary>
	/// Summary description for frmMain.
	/// </summary>
	public class frmMain : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
        
        private AutoResetEvent DataArrivalEvent = null;
        private Graphics ApplicationGraphics = null;
        private ContextMenu ApplicationMenu = null;
        private const int SampleBufferSize = 16;
        private Device ApplicationDevice = null;
        private Point CurrentPoint = new Point(); // Virtual coordinates
        private Point OldPoint = new Point();
        private Thread DeviceThread = null;
        private Cursor CursorBlank = null;
        private bool Drawing = false;
        private int Sensitivity; // Mouse sensitivity 
        private int dxFuzz; // Leftover x-fuzz from scaling 
        private int dyFuzz; // Leftover y-fuzz from scaling 
        private const int ScawlCXBitmap = 512;
        private const int ScrawlCYBitmap = 300;

		public frmMain()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		
            DataArrivalEvent.Set();
        }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            // 
            // frmMain
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(512, 294);
            this.Cursor = System.Windows.Forms.Cursors.Cross;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Scrawl";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.frmMain_KeyUp);
            this.Activated += new System.EventHandler(this.frmMain_Activated);
        }
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmMain());
		}

        private void DataArrivalHandler()
        {
            while (Created)
            {                
                DataArrivalEvent.WaitOne();
             
                if(!Created)
                    break;

                BufferedDataCollection col = null;

                try
                {
                   col = ApplicationDevice.GetBufferedData();
                }
                catch(InputException e)
                {
                    if(e is InputLostException || e is AcquiredException)
                    {
                        SetAcquire(true);
                        continue;
                    }
                }
                
                if(null == col)
                    continue;

                foreach(BufferedData data in col)
                {
                    switch(data.Offset)
                    {
                        case (int)MouseOffset.X:
                            UpdateCursorPosition(data.Data, 0);
                            break;
                        case (int)MouseOffset.Y:
                            UpdateCursorPosition(0, data.Data);
                            break;
                        case (int)MouseOffset.Button0:
                            if( 0 != (data.Data & 0x80) )
                            {
                                Cursor = CursorBlank;
                                OldPoint = CurrentPoint = PointToClient(Cursor.Position);
                                Drawing = true;
                            }
                            else
                            {
                                Cursor = Cursors.Cross;
                                Cursor.Position = PointToScreen(CurrentPoint);
                                Drawing = false;
                            }
                            break;
                        case (int)MouseOffset.Button1:
                            SetAcquire(false);
                            ApplicationMenu.Show(this, new Point(10,10));
                            break;
                    }
                }
                if(Drawing)
                {                    
                    ApplicationGraphics.DrawLine(new Pen(Color.Black), new Point(CurrentPoint.X, CurrentPoint.Y), OldPoint);
                    OldPoint = CurrentPoint;
                }
            }
        }

        private void UpdateCursorPosition(int dx, int dy)
        {   

            //-----------------------------------------------------------------------------
            // Name: UpdateCursorPosition()
            // Desc: Move our private cursor in the requested direction, subject
            //       to clipping, scaling, and all that other stuff.
            //
            //       This does not redraw the cursor.  You need to do that yourself.
            //-----------------------------------------------------------------------------

            // Pick up any leftover fuzz from last time.  This is important
            // when scaling down mouse motions.  Otherwise, the user can
            // drag to the right extremely slow for the length of the table
            // and not get anywhere.
            
            dx += dxFuzz;     
            dxFuzz = 0;

            dy += dyFuzz;
            dyFuzz = 0;            

            switch(Sensitivity) 
            {
                case 1:     // High sensitivity: Magnify! 
                    dx *= 2;
                    dy *= 2;
                    break;

                case -1:    // Low sensitivity: Scale down 
                    dxFuzz = dx % 2;  // remember the fuzz for next time 
                    dyFuzz = dy % 2;
                    dx /= 2;
                    dy /= 2;
                    break;

                case 0:     // normal sensitivity 
                    // No adjustments needed 
                    break;
            }

            CurrentPoint.X += dx;
            CurrentPoint.Y += dy;

            // clip the cursor to our client area
            if( CurrentPoint.X < 0 ) 
                CurrentPoint.X = 0;

            if( CurrentPoint.X >= ScawlCXBitmap )
                CurrentPoint.X = ScawlCXBitmap - 1;

            if( CurrentPoint.Y < 0 )  
                CurrentPoint.Y = 0;

            if( CurrentPoint.Y >= ScrawlCYBitmap ) 
                CurrentPoint.Y = ScrawlCYBitmap - 1;
        }

        private void frmMain_Load(object sender, System.EventArgs e)
        {
            ApplicationDevice = new Device(SystemGuid.Mouse);
            ApplicationDevice.SetCooperativeLevel(this, CooperativeLevelFlags.Foreground | CooperativeLevelFlags.NonExclusive);

            DataArrivalEvent = new AutoResetEvent(false);
            DeviceThread = new Thread(new ThreadStart(DataArrivalHandler));
            DeviceThread.Start();

            ApplicationDevice.SetEventNotification(DataArrivalEvent);
            ApplicationDevice.Properties.BufferSize = SampleBufferSize;
            SetAcquire(true);
            ApplicationGraphics = this.CreateGraphics();
            
            MenuItem[] PopoutItem = new MenuItem[3];
            PopoutItem[0] = new MenuItem("&Low\t1", new EventHandler(ContextSubMenuEvent));
            PopoutItem[1] = new MenuItem("&Normal\t2", new EventHandler(ContextSubMenuEvent));
            PopoutItem[2] = new MenuItem("&High\t3", new EventHandler(ContextSubMenuEvent));
            PopoutItem[1].Checked = true;
            
            MenuItem[] item = new MenuItem[3];
            item[0] = new MenuItem("&Exit", new EventHandler(MenuEventExit));
            item[1] = new MenuItem("&Clear", new EventHandler(MenuEventClear));
            item[2] = new MenuItem("&Sensitivity");
            item[2].MenuItems.AddRange(PopoutItem);

            ApplicationMenu = new ContextMenu(item);
            ApplicationMenu.Popup += new EventHandler(MenuCreated);
            this.ContextMenu = ApplicationMenu;
            
            CursorBlank = new Cursor(DXUtil.SdkMediaPath + "blank.cur");
            Point p = new Point(this.ClientRectangle.Bottom / 2, this.ClientRectangle.Right / 2);
            Cursor.Position = PointToScreen(p);
            OldPoint = CurrentPoint = p;
        }
        private void MenuCreated(object sender, System.EventArgs e)
        {
            SetAcquire(false);
        }
        private void MenuEventExit(object sender, System.EventArgs e)
        {
            SetAcquire(false);
            Close();
        }
        private void MenuEventClear(object sender, System.EventArgs e)
        {
            OnClear();
            SetAcquire(true);
        }
        private void ContextSubMenuEvent(object sender, System.EventArgs e)
        {
            MenuItem item = (MenuItem)sender;
            int i = -1;

            foreach(MenuItem m in ApplicationMenu.MenuItems[2].MenuItems)
            {
                if ( m.Equals(item) )
                {
                    m.Checked = true;
                    Sensitivity = i;
                }
                else
                    m.Checked = false;
                i++;
            }
            item.Checked = true;

            SetAcquire(true);
        }
        private void frmMain_Activated(object sender, System.EventArgs e)
        {
            SetAcquire(true);
        }
        private void SetAcquire(bool state)
        {
            if (null != ApplicationDevice)
            {
                if(state)
                {
                    try{ApplicationDevice.Acquire();}
                    catch(InputException){}
                }
                else
                {
                    try{ApplicationDevice.Unacquire();}
                    catch(InputException){}
                }
            }
        }
        private void OnClear()
        {
            //-----------------------------------------------------------------------------
            // Name: OnClear()
            // Desc: Makes the form white
            //-----------------------------------------------------------------------------            
            ApplicationGraphics.Clear(Color.White);            
        }

        private void frmMain_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Escape)
                Close();
        }

    }
}