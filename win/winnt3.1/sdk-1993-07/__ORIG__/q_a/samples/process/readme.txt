 Sample: Creating and Ending Processes

Summary:

The PROCESS sample provides a simple interface to the
CreateProcess and TerminateProcess functions.  In order to
create a process the user is presented with a common dialog
for selecting a file.  In this case, the file must have an
extension of .EXE.  Processes which are started are
presented in a list box.  Any of the processes may be
selected in the list box and then terminated.

Warning:  "TerminateProcess is used to cause all of the
threads within a process to terminate.  While
TerminateProcess will cause all threads within a process to
terminate, and will cause an application to exit, it does
not notify DLLs that the process is attached to that the
process is terminating.  TerminateProcess is used to
unconditionally cause a process to exit.  It should only be
used in extreme circumstances.  The state of global data
maintained by DLLs may be compromised if TerminateProcess is
used rather than ExitProcess."


