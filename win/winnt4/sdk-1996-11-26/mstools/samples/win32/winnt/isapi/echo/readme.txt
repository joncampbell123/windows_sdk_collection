
1. Use Reskit utility instsrv to install EchoServer
2. From Control Panel change Log On As to Local System
3. Copy ISQPIEcho.DLL to IIS /Scripts directory
4. Open DEFAULT.HTM file with the browser, that supports cookies(IE 2.0, Netscape)
5. Enter data for First Name, Last Name, etc.
6. Click on "Get" to echo form parameters
7. Observe returned parameters
7. Run netstat -a to see that connection has been been established on a port
   number 22222
8. Click on Go Back
9. Click on "Get" again
10.Run netstat -a to make sure that there are NO new connection has been been established on a port
   number 22222	
11.Click on Go Back
12.Click Disconnect
13.Run netstat -a and make sure that there are no connections open on a port 
   number 22222 

Steps 6 to 12 can be repeated by starting multiple instances of browser.