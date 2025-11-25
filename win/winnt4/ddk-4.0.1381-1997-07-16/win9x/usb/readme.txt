
The files and samples found in this subdirectory are for creating USB
and prototype HID device drivers.  Drivers created using these
materials are ONLY for use with the WDM enabled Windows 95 OSR 2.1
release (aka Detroit) included with the MSDN Jan '97 release.


What is WDM?
------------
The Windows Driver Model (WDM) is based on the Windows NT driver
model, with extensions for PnP and power management.  See WDM.DOC in
the .\DOC directory for information about the Windows Driver Model,
plug and play, and a list of Windows NT kernel services supported
under the OSR 2.1 version of Windows 95.

Important Note: The interfaces described in this subdirectory are
specific to the OSR 2.1 release of Windows 95.  Drivers created from
these materials will not run on the retail releases of Windows 95,
Windows 95 OSR 2 nor Windows NT 4.0.  These interfaces are also
expected to undergo modification and/or enhancement in future releases
of Windows 95 and Windows NT.


Using the Windows NT 4.0 DDK
----------------------------
You must first install the Windows NT 4.0 DDK to build the sample
drivers in this subdirectory and to get additional Windows NT device
driver documentation.  After successfully installing the Windows NT
4.0 DDK, xcopy the contents of the current .\DDK directory to a USB
subdirectory under the .\SRC directory created by the Windows NT DDK.

For example, the command should look something similar to this:

xcopy /s /e .\ddk c:\NTDDK\SRC\USB

Since the sample drivers in this subdirectory are plug and play, they
will not build with all the normal files (NTDDK.H, NTDEF.H, for
example) provided with the Windows NT 4.0 DDK.  This means that you
must also use the additional/updated include files in the .\NTDDK\INC
subdirectory (WDM.H and NTPOAPI.H) along with the library NTPNP.LIB in
.\NTDDK\LIB subdirectory.

To use these files, copy the contents of the .\NTDDK\INC and
.\NTDDK\LIB subdirectories into the appropriate \INC and \LIB\I386\*
directories of the installed Windows NT DDK.


Additional Release Notes
------------------------
WDM.H supercedes/replaces NTPNP.H and any modified NTDDK.H header
files included in any earlier releases you may have received.

You MUST be using the Windows NT 4.0 DDK, due to use of
IoAttachDeviceToStackDevice() which is a new API added in Windows NT
4.0.

IoAttachDeviceByPointer() no longer exists in WDH.H.  You should use
IoAttachDeviceToStackDevice().

PDRIVER_ADD_DEVICE has changed.  The PhysicalDeviceObject was a **
(ptr to ptr).  Now it is just a * (ptr).

old:

//
// Define driver AddDevice routine type.
//

typedef
NTSTATUS
(*PDRIVER_ADD_DEVICE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN OUT struct _DEVICE_OBJECT **PhysicalDeviceObject
    );

new:

//
// Define driver AddDevice routine type.
//

typedef
NTSTATUS
(*PDRIVER_ADD_DEVICE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN struct _DEVICE_OBJECT *PhysicalDeviceObject
    );
