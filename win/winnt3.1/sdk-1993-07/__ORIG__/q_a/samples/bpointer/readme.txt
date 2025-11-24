Sample: Bpointer

When several processes need to access shared data, there is no guarantee
that the shared memory is mapped to the same locations in both processes.
This can cause a problem when the data contains relative pointers because
a pointer value that is valid in one processe's context may not be valid
in the context of the other processes.

This sample demonstrates the use of based pointers to allow manipulation of
shared data from several processes using memory mapped files. This
technique is applicable to all forms of shared memory.

It contains the following modules:


readdata.exe : A console process that allows you to view the shared data;
               it dereferences pointers as it encounters them.
chgdata.exe : A console process that lets you add elements to a shared
              linked list.


Note, however, that based pointers cut down on the performance of the
application using it because the pointers need to be resolved at runtime;
that is, each access typically adds one machine instruction overhead when
dereferencing a pointer.

Once the files are compiled, execute chgdata.exe and follow the
instructions posted there.


