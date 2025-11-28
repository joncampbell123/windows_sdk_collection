Attribute VB_Name = "modMT"
Option Explicit

' This module demonstrates that Visual Basic
'   creates a separate instance of global data
'   for each thread.  Thus, the glngGlobalData
'   variable will have a separate value for
'   each thread Visual Basic starts.
'
' This fact is used to keep a count of Coffee
'   objects on each thread:  In the Coffee
'   object's Initialize event, it adds one to
'   glngGlobalData; in its Terminate event,
'   it subtracts one.
'
' Any Coffee object can then find out how
'   many Coffee objects are on its thread by
'   testing glngGlobalData.  Clients can also
'   find out by calling the NumberOnThread
'   method, which returns glngGlobalData.
'
' When MTCoffee is run in the development
'   environment, where there's only a single
'   thread, NumberOnThread is the total number
'   of Coffee objects.  When MTCoffee is
'   compiled with Thread Per Object selected,
'   the count will be one (1) for each Coffee
'   object, unless you call GetCoffeeOnSameThread
'   to create a second Coffee on the thread.
'
' When MTCoffee is compiled with thread pooling,
'   NumberOnThread will be greater than one
'   on some thread whenever the number of
'   active Coffee objects is greater than
'   the number of threads in the pool.
'
' This subject is covered in "Scalability and
'   Multithreading," in Books Online.

Public glngGlobalData As Long

