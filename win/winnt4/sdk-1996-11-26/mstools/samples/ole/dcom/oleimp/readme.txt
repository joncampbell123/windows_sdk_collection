DCOM Impersonation

SUMMARY
=======

The OLEIMP sample demonstrates a method for emulating auto impersonation
of clients. Normally, objects created on the server will run in the
context of the server.  An object method must explicitly impersonate the
client if the method is to run in the context of the client.  This
sample demonstrates a way to arrange for methods to "automatically" run
in the context of the client.

MORE INFORMATION
================

This program is based on the oleapt sample.  It uses a set of apartment
threads that each potentially impersonate a client. All objects created
for a user run in the same apartment.

The server registers a class-factory on the main application thread and
also spawns several worker threads. When requests arrive from clients
to create instances of the class, the server class-factory picks a
worker thread for the object in the following manner:

  - If there is a thread currently impersonating that user, use that
    thread.
  - Otherwise, if there is a free thread, set that thread up to
    impersonate the client and use that thread.
  - Otherwise fail the CreateInstance call.

The server and then goes through the process of having the object
created within the thread and properly marshalled from the worker thread
back to the class factory where it can be returned to the caller. Note
that this marshalling to the main thread is fairly transient, it lasts
for creation only: subsequent calls from the client to the object go
straight from the client's process into the worker apartment.

This sample may be compiled as either UNICODE or ANSI.

Instructions:

 To use this sample:
  * build it using the NMAKE command. NMAKE will create OLEIMP.EXE and
    CLIENT.EXE.
  * install the OLEIMP.EXE on the current machine or on a remote
    machine according to the installation instructions found in
    OLEIMP.CPP.
  * run CLIENT.EXE. use no command-line arguments to instantiate the
    object on the current machine. use a single command-line argument of
    the remote machine-name (UNC or DNS) to instantiate the object on a
    remote machine.
  * CLIENT.EXE displays some simple information about the calls it is
    making on the object.
  * OLEIMP.EXE displays information about the worker threads.
