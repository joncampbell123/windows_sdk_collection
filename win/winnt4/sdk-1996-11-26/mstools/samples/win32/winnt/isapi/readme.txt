12345678901234567890123456789012345678901234567890123456789012345678901234567890
ISAPI Samples
=============

This directory contains a few ISAPI samples to help you to write
extensions to Microsoft Internet Information Server, or other HTTP
servers that support ISAPI.

Async     - Sample ISAPI app doing a simple file transfer
            using the Async TransmitFile support with callback.
AuthFilt  - Sample authentication filter
CGIWrap   - Wrapper for ISAPI Apps that allows them to be run as CGI apps.
CvtDoc    - Sample ISAPI app that converts documents from their native 
            formats to HTML.
FormDump  - Simple ISAPI app to demonstrate form handling techniques.
HomeFilt  - Simple ISAPI app that redirects requests for the server's
            home page.
IS2WCGI   - Sample to help run WinCGI apps.
Echo      - Simple ISAPI sample
ISrvMon   - Simple ISAPI application that returns server load information 
            to clients.
Lottery   - Simple ISAPI sample
NewLog    - Simple ISAPI sample
Redir     - Simple sample that redirects clients to a new site 
            (for example for an ad) and logs the transaction.
UpCase    - Simple filter that converts all cate in a HTML file to upper case.
WebRun    - ISAPI app and control panel applet that allows administrators 
            to run commands remotely via a web page.

Several samples will be copied to your inetsrv directory if you 
set two environment variables: WWWROOT to your inetsrv wwwroot and
WWWSCRIPTS to your inetsrv scripts directory. Be sure to create an SDK
directory below your wwwroot and script directory for these file to be copied 
too (for example, md %WWWROOT%\sdk and md %WWWSCRIPTS%\sdk. These can be 
accessed from ISAPIsmp.htm which is copied to your WWWROOT directory.
  
The samples that  are copied with thier html files are: 
async, authfilt, cgiwrap, formdump, homefilt, is2wcgi, isrvmon, lottery, 
newlog, redir, and upcase. 

These samples will not be automatically copied because they require
additional setup: webrun, echo, cvtdoc.

Debugging ISAPI Apps and Filters
=================================

Here is a list of things to check when debugging an ISAPI application or 
filter since the browser does not return much useful information if there 
is a server configuration problem.

1. Run IIS (W3SVC.exe) as a process
2. Set debug user rights
3. Set debug session parameters
4. Set registry parameters
5. Check DLL dependencies 
6. Check directory access
7. Call the app/filter plus parameters from the browser

1. Run IIS as a Application
IIS by default runs as a service.  It is easier to debug apps than services.  
Do this by stopping the IIS service with IS Server Manager-> Stop Running.
Then disable the Automatic restart of IIS in Settings, Control-Panel, 
Services, set WWW publishing to "Manual".

2. Set "debug" user rights
ISAPI Apps and filters are run in the context of a user. To debug the, set 
user rights for debug session.  Either use an existing user or create a new 
user with the User Manager.  It is simplest to add the user to Administrator 
and then in User Manger, Policies, User Rights, Advanced Functions. 
Add the following rights:
   - Act  as part of operating system
   - Generate security audits
   - Replace a process level token

3. Set debug session parameters
To start Web server from VC++ debug environment use VC++'s Build menu option, 
select Settings, Debug, General  and enter path of inetinfo.exe. Set to: 
<full path to inetsrv>\inetinfo.exe with parameters -e W3SVC
and then set VC++, Build, Settings, Debug, Additional DLLs to the path of your 
app (DLL) we want to debug.  If you use a debugger other than the one in VC, 
do whatever is necessary in that environment to start the debugger, 
with the application to debug being 
"<full path to inetsrv>\inetinfo.exe -e W3SVC", with the current 
directory including your ISAPI App/Filter and related DLLs/files.

4. Set registry parameters
Caching does not allow the app/filter to be overwritten during subsequent 
builds.  Disabling extension caching is useful for debugging but can have 
a serious negative impact on the performance of IIS.

For ISAPI Apps: Set the key to release DLL.
  HKEY_LOCAL_MACHINES/SYSTEM/CurrentControlSet/
                       Services/W3SVC/Parameters/CacheExtensions = 0

Furthermore, ISAPI filters are automatically loaded at IIS startup time.
So add or remove the filter name from:
  HKEY_LOCAL_MACHINES/SYSTEM/CurrentControlSet/
                       Services/W3SVC/Parameters/Filter DLLs
Be sure to add a comma between entries and then the path to debug filter.


5. Check App /Filter dependencies
The filter or app will not run if the resources it is using are not found 
in the same directory or in the search path.  Be sure to check these 
dependencies. Verify what your code uses: Data files, DLLs, etc.  Also check 
for export of correct functions. Your filter or app will run only if the 
correct functions are exported for ISAPI.  You can check what DLLs are linked 
to your ISAPI program by running 'link -dump -imports <filename>.dll'.  You can
check to make sure that your DLL has the proper exports by running 
'link -dump -exports <filename>.dll' and looking for HttpExtensionProc 
& GetExtensionVersion (ISAPI Apps) or GetFilterVersion & HttpFilterProc 
(ISAPI Filters). You can also use QuickView on your DLL to view its exports. 
In Explorer, right mouse click your DLL and chose Quick View.  Examine 
the Export Table. 

6. Check Directory Access
Make sure that we are pointing to correct URLs with correct permission. 
Use Internet Server Manager, Service properties, Directories section. Add a 
directory, set Execute permission, and allow Browsing.  

7. Invoking ISAPI app from Browser
First browse the directory to verify you have access. Call DLL from the browser 
with No parameters to verify that you have execute permissions. Invoking ISAPI 
app from the browser by calling DLL+parameters from browser.  Set a breakpoint 
at HttpExtensionProc() to verify that the debugger is setup correctly and that 
you are calling into the filter or app from the browser.  
  