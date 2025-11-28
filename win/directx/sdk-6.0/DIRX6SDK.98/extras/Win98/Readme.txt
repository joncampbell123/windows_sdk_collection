
Win98 Debug/Retail DLLs for DSound and DInput
=============================================

THIS INFORMATION WILL INTEREST ONLY DEVELOPERS USING Win98.

Because Win98 ships with updated versions of DSound.dll and DInput.dll 
(i.e. more recent than DX6), DXSetup will not update these DLLs.  The 
problem with this is that debug DLLs for these components cannot be
installed from the SDK or DXSetup.  Therefore, we offer the debug and
retail DLLs here, along with an Install.inf to install and uninstall
debug DLL for these components.

\Win98Dbg (Win98 Debug)

   Contains the debug DLLs for DSound and DInput.  To install these
   components, right click on the Install.inf and select "Install".
   You will want to restart your system to ensure you have the 
   components properly installed.

\Win98Rtl (Win98 Retail)

   If you would like to reinstall the retail components, this folder
   contains the retail DLLs for DSound and DInput.  To install these
   components, right click on the Install.inf and select "Install".
   You will want to restart your system to ensure you have the 
   components properly installed.

 