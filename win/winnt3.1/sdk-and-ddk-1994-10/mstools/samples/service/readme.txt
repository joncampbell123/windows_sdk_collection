Sample: Simple Service

Summary:

The Simple Service sample demonstrates how to create and
install a service.

In this particular case, the service merely opens a named
pipe of the name \\.\pipe\simple, and waits for traffic.
If it receives anything, it surrounds the input with:

        Hello! [<input goes here>]

and sends it back down the pipe to the client.

The service can be Started, Stopped, Paused, and Continued.

To install the service, first compile everything, and then
user INSTSRV to install SimpleService as follows:

        instsrv SimpleService <location of simple.exe>

Now all you have to do is start it, either using the
"net start" method or via the control panel Services applet.

Once the service has been started, you can use the CLIENT
program to verify that it really is working, using the syntax:

        client \\.\pipe\simple Hello

which should return the response:

        Hello! [Hello]

If, after playing with the sample you wish to remove the service,
simple say:

        instsrv SimpleService remove

Note that INSTSRV can be a little dangerous -- it'll install
and remove any service you tell it to, so be careful.


Additional reference words:

CloseServiceHandle, InitializeSecurityDescriptor,
SetSecurityDescriptorDacl, SetServiceStatus, OpenSCManager,
StartServiceCtrlDispatcher, RegisterEventSource,
DeregisterEventSource, RegisterServiceCtrlHandler
