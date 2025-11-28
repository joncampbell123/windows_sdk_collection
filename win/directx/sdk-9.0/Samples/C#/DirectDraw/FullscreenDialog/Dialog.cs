using System;
using System.Drawing;
using System.Windows.Forms;

public class DialogSampleForm : Form
{
    private System.ComponentModel.Container components = null;

    private Button btnOk;
    private Button btnCancel;
    private RadioButton rbRadio1;
    private RadioButton rbRadio2;
    private RadioButton rbRadio3;
    private Label lblStatic;
    private TextBox txtEdit1;
    private ComboBox cboCombo1;
    private GroupBox gbStatic;
    



    public DialogSampleForm()
    {       
        //
        // Required for Windows Form Designer support
        //      
        InitializeComponent();
    }




    #region InitializeComponent code
    private void InitializeComponent()
    {
        this.btnOk = new System.Windows.Forms.Button();
        this.btnCancel = new System.Windows.Forms.Button();
        this.rbRadio1 = new System.Windows.Forms.RadioButton();
        this.rbRadio2 = new System.Windows.Forms.RadioButton();
        this.rbRadio3 = new System.Windows.Forms.RadioButton();
        this.lblStatic = new System.Windows.Forms.Label();
        this.txtEdit1 = new System.Windows.Forms.TextBox();
        this.cboCombo1 = new System.Windows.Forms.ComboBox();
        this.gbStatic = new System.Windows.Forms.GroupBox();
        this.SuspendLayout();
        // 
        // btnOk
        // 
        this.btnOk.Location = new System.Drawing.Point(76, 198);
        this.btnOk.Name = "btnOk";
        this.btnOk.TabIndex = 0;
        this.btnOk.Text = "OK";
        this.btnOk.Click += new System.EventHandler(this.Exit);
        // 
        // btnCancel
        // 
        this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnCancel.Location = new System.Drawing.Point(162, 198);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.TabIndex = 1;
        this.btnCancel.Text = "Cancel";
        this.btnCancel.Click += new System.EventHandler(this.Exit);
        // 
        // rbRadio1
        // 
        this.rbRadio1.Location = new System.Drawing.Point(22, 80);
        this.rbRadio1.Name = "rbRadio1";
        this.rbRadio1.Size = new System.Drawing.Size(60, 16);
        this.rbRadio1.TabIndex = 2;
        this.rbRadio1.Text = "Radio 1";
        // 
        // rbRadio2
        // 
        this.rbRadio2.Location = new System.Drawing.Point(22, 104);
        this.rbRadio2.Name = "rbRadio2";
        this.rbRadio2.Size = new System.Drawing.Size(60, 16);
        this.rbRadio2.TabIndex = 3;
        this.rbRadio2.Text = "Radio 2";
        // 
        // rbRadio3
        // 
        this.rbRadio3.Location = new System.Drawing.Point(22, 128);
        this.rbRadio3.Name = "rbRadio3";
        this.rbRadio3.Size = new System.Drawing.Size(60, 16);
        this.rbRadio3.TabIndex = 4;
        this.rbRadio3.Text = "Radio 3";
        // 
        // lblStatic
        // 
        this.lblStatic.Location = new System.Drawing.Point(7, 8);
        this.lblStatic.Name = "lblStatic";
        this.lblStatic.Size = new System.Drawing.Size(240, 42);
        this.lblStatic.TabIndex = 5;
        this.lblStatic.Text = "This dialog has a few types of controls to show that everything can work on a non" +
            "-GDI display as it does with a GDI based display.";
        // 
        // txtEdit1
        // 
        this.txtEdit1.Location = new System.Drawing.Point(7, 168);
        this.txtEdit1.Name = "txtEdit1";
        this.txtEdit1.Size = new System.Drawing.Size(232, 20);
        this.txtEdit1.TabIndex = 6;
        this.txtEdit1.Text = "";
        // 
        // cboCombo1
        // 
        this.cboCombo1.Location = new System.Drawing.Point(165, 64);
        this.cboCombo1.Name = "cboCombo1";
        this.cboCombo1.Size = new System.Drawing.Size(75, 21);
        this.cboCombo1.TabIndex = 7;
        // 
        // gbStatic
        // 
        this.gbStatic.Location = new System.Drawing.Point(7, 56);
        this.gbStatic.Name = "gbStatic";
        this.gbStatic.Size = new System.Drawing.Size(150, 109);
        this.gbStatic.TabIndex = 8;
        this.gbStatic.TabStop = false;
        this.gbStatic.Text = "A Group of Radio Buttons";
        // 
        // DialogSampleForm
        // 
        this.AcceptButton = this.btnOk;
        this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
        this.ClientSize = new System.Drawing.Size(247, 245);
        this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                      this.btnOk,
                                                                      this.btnCancel,
                                                                      this.rbRadio1,
                                                                      this.rbRadio2,
                                                                      this.rbRadio3,
                                                                      this.lblStatic,
                                                                      this.txtEdit1,
                                                                      this.cboCombo1,
                                                                      this.gbStatic});
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "DialogSampleForm";
        this.ShowInTaskbar = false;
        this.Text = "Sample Dialog";
        this.TopMost = true;
        this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.DialogSampleForm_KeyUp);
        this.ResumeLayout(false);

    }
    #endregion




    protected override void Dispose( bool disposing )
    {           
        if( disposing )
        {
            if(components != null)
            {
                components.Dispose();
            }
        }
        base.Dispose( disposing );
    }




    private void Exit(object sender, System.EventArgs e)
    {       
        this.Close();
    }




    private void DialogSampleForm_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
    {
        if (Keys.Escape == e.KeyCode)
        {
            e.Handled = true;
            this.Close();
        }
    }
}