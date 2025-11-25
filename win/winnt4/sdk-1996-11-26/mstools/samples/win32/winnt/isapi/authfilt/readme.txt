Microsoft Internet Information Server
AuthFilt - A Filter for Advanced Authentication 


AuthFilt demonstrates how to write an authentication filter based on an
external datasource. Authentication is the process of accepting or
denying a request from a client, so AuthFilt will be notified each time
an authentication request comes in. This sample uses a file
(c:\inetsrv\userdb.txt) to keep track of authorized users, but you might
modify this sample to access a database which holds user info. 

For each authentication request, AuthFilt first looks in a cache of
recently authenticated users, and when that fails, AuthFilt looks in the
userdb.txt file. This shows an efficient way to authorize connections: 
a cache allows the filter to quickly authenticate users, and because 
each request comes in through the filter, speed is critical. 

Steps to build the sample: 

1. Set the environment variables WWWROOT and WWWSCRIPTS to point to your 
   c:\inetsrv\wwwroot and c:\inetsrv\scripts directory, respectively. 

2. Make sure your INCLUDE variable points to both the Win32 SDK and the 
   ISAPI header files, and make sure your LIB variable points to the Win32 
   SDK. 

3. With your PATH variable set to your compiler, run nmake from the 
   command line inside the AuthFilt directory.

4. Run REGEDT32.EXE and modify the server's registry as follows. Choose
   HKEY_LOCAL_MACHINE, CurrentControlSet, Services, W3SVC, Parameters, 
   then Filter DLLs. Select the Edit menu, then String. This will bring
   up a dialog to modify the value of the key. Append a comma (,) and a
   local path to AuthFilt.Dll, usually 
   C:\INetSrv\Scripts\SDK\AuthFilt.Dll.

5. Restart the WWW service. 

The build results will be copied to your WWW structure: AuthFilt.Dll 
will be copied to the SDK sub-directory off the path specified in 
WWWSCRIPTS, and Default.Htm will be copied to the AuthFilt sub-
directory off the path specified in WWWROOT. The registry entry Filter
DLLs is used by IIS to determine what filters to load when the WWW 
service is starting.