VJ COM+ Wizard

What is it?
===========
The VJReg.exe has important fixes for enabling COM+ support in Java components.

The COM+ pack contains an COM+ wizard and an COM+ addin.

The COM+ wizard is a wizard which generates a skeleton project for creating an COM+ component in Visual J++.  It can be accessed through the Components node on the New Project window.

The COM+ Component AddIn is an addin that adds the components to COM+ applications after each rebuild of an COM+ component in Visual J++ automatically.  It can be accessed through Tools/Add-In Manager.

How to set it up?
=================
Assume you installed VJ at c:\Program Files\Microsoft Visual Studio.
1.  copy vjreg.exe to c:\Program Files\Microsoft Visual Studio\vj98 directory.
2.  Copy attached VJCOMPack.DLL file in c:\Program Files\Microsoft Visual Studio\vj98 and register it using regsvr32.
3.  Copy attached COM+ Wizard.vsz, COM+ Wizard.ico, op.java and COMPlus.java files in c:\Program Files\Microsoft Visual Studio\vj98\VJProjects\Components.
4.  Start VJ.  Select File/New Project.  Select Components node.  You should see the COM+ Wizard icon.  You are ready to roll.

What does the wizard do?
========================
1.  The wizard contains 2 pages.  After you select the wizard and give the project a name (using a capital letter as the first letter of your project name is a really good idea), the 1st page asks you to select the COM+ transaction type your component wants to support.
2.  The 2nd page enumerates all the applications that are installed on your machine.  You select the one in which you're going to install this component.  If COM+ is not installed, this will be blank.  You do have to manually import the component into the application through Component Services Explorer.  
3. On the second page of the wizard, there's a Create application button for you to create a new COM+ application.  If you create a new application, it will be selected in the listbox on the left.  The button is disabled if COM+ is not installed.
4.  A Project1.java file is created.  Project1 is replaced by the file name you specified.
5.  Add "import com.ms.mtx.*" in the file.
6.  @com.register is added with 2 guid.
7.  @com.transaction is added with the transaction type you specified in step 2.
8.  A 'TODO' comment is added to remind users to install the dll to the application after the component is built.
9.  A sample method is added to the Java file.
10.  In Launch tab of the project properties window, Custom launch is selected.  Program field is set to 'dllhost.exe'.  Arguments field is set to the process id of the application name you selected in step 2.  If COM+ is not installed, it will be set to default string '{Process id}'.
11.  In COM Classes tab of the project properties window, the Project1 is checked.
12.  In Output Format tab of the project properties window, 'Enable Packaging' is checked.  application Type is set to COM DLL.  File name is set to project1.dll.

What does the AddIn do?
========================
Refresh part is not necessary for COM+ any more.  Just the addition of the component to the application now.

More features are added to this recently (6/98):
1.  Move the @com tags to just above the class declaration
2.  Have the addin do an MTXSTOP prior to rebuilding the DLL
3.  Change the re-register step to either add the DLL to the COM+ application or to re-register it if its already there
4.  Menu item to allow manual addition of the DLL to the COM+ application
