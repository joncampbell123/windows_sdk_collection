Microsoft Internet Information Server
HomeFilt - A Filter for Dynamic URL Redirection

HomeFilt demonstrates a way to detect a request for the virtual root of the
server and redirect it to another URL.  Once the filter is registered in
the filter chain, it will be called for each request.  This sample examines
the URL requested by the client, and if it is just a slash (/), the URL is
changed to /sdk/homefilt/home.htm.  The concept behind redirection is quite
powerful: you might redirect your entire site to a single HTML file, or you
might dynamically redirect requests to other HTML files chosen at random.  
This sample shows that redirection is not complex.

Steps to build the sample:

1. Set the environment variables WWWROOT and WWWSCRIPTS to point to your 
   c:\inetsrv\wwwroot and c:\inetsrv\scripts directory, respectively.

2. Make sure your INCLUDE variable points to both the Win32 SDK and the 
   ISAPI header files, and make sure your LIB variable points to the Win32 
   SDK.

3. With your PATH variable set to your compiler, run nmake from the command 
   line inside the HomeFilt directory.

4. Run REGEDT32.EXE and modify the server's registry as follows.  Choose 
   HKEY_LOCAL_MACHINE, CurrentControlSet, Services, W3SVC, Parameters, then 
   Filter DLLs.  Select the Edit menu, then String.  This will bring up a 
   dialog to modify the value of the key.  Append a comma (,) and a local 
   path to HomeFilt.Dll, usually C:\INetSrv\Scripts\SDK\HomeFilt.Dll.

5. Restart the WWW service.

The build results will be copied to your WWW structure: HomeFilt.Dll will 
be copied to the SDK sub-directory off the path specified in WWWSCRIPTS, 
and Default.Htm/Home.Htm will be copied to the HomeFilt sub-directory off 
the path specified in WWWROOT.  The registry entry Filter DLLs is used by 
IIS to determine what filters to load when the WWW service is starting.

The makefile that comes with the Internet SDK uses the DLL version of the
C Runtime Library. You must have this DLL on your server in order for the
samples to work.  For example, if you are using Visual C++ 4.0 to build 
the DLL, it will need the debug version of the C Runtime Library DLL, 
named MSVCR40D.DLL.  Verify that this file is in your server's SYSTEM32 
directory.
