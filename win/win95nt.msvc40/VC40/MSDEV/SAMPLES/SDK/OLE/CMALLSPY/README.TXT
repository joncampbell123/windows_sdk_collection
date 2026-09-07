                         CMallSpy Sample Notes
                         ---------------------

When the capability for replaceable memory allocators was removed from 32-bit
OLE, the ability for applications to provide their own debug allocators for
OLE was also removed.  OLE disallows caller provided allocators for
multi-thread safety and general robustness.  In order to provide a way for
applications to monitor memory allocation, detect memory leaks and simulate
memory failure, OLE now provides support for a simple wrapper for the IMalloc
interface: the IMallocSpy interface.

This sample code demonstrates a simple implementation of the IMallocSpy
interface.

See the OLE programmer's documentation for a complete description of the
IMallocSpy interface and the CoRegisterMallocSpy and CoRevokeMallocSpy
functions.

You can also find a more "real-world" implementation of IMallocSpy in the
OLESTD sample library in the MALSPY.C and MALSPY.H files.
