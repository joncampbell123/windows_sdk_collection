                  CtlCfg.vbp and CtlView.vbp

The two projects in this folder demonstrate dynamic control addition (Form1.Controls.Add), or the ability to add unreferenced controls at run time to a form, ActiveX document, or user control.

The CtlCfg (control configure) sample allows you to add information about any control--either an intrinsic control or user control--to the Controls.mdb database. Most importantly, the database stores the control's license key if the license key is required to load and add the control. The database can also store various properties that can be set as the control is dynamically added. 

Once the information about a control is added to the database, you can open the ctlView (control viewer) sample. This sample simply reads the database and displays a list of controls you can dynamically add. You can add any control the application by clicking on it. 

	NOTE: Be sure the Controls.mdb file is writeable.

For more information about dynamic control addition, see the following help topics:

Add Method (Controls Collection)
Add Method (Licenses Collection)
EventInfo Object
Licenses Collection
ObjectEvent Event
Parameter Object (Visual Basic)
