/*++

Copyright (c) 1996    Microsoft Corporation

Module Name:

    HIDDLL.H

Abstract:

    This module contains the PUBLIC definitions for the
    code that implements the HID dll.

Environment:

    Kernel & user mode

Revision History:

    Aug-96 : created by Kenneth Ray

--*/


#ifndef _HIDSDI_H
#define _HIDSDI_H

#include "windows.h"
typedef ULONG NTSTATUS;
#include "hidpi.h"

typedef struct _HIDD_CONFIGURATION
{
   PVOID    cookie;
   ULONG    size;
   ULONG    RingBufferSize;
} HIDD_CONFIGURATION, *PHIDD_CONFIGURATION;


int HidD_Hello (char * buff, int len);

BOOLEAN __stdcall
HidD_GetGuidString (
   OUT   PCHAR *     GuidClassInputString
   );
/*++
Routine Description:
   Return the Guid String needed to enumerate HID devices through PnP.
   This routine "malloc"s the memory needed for the string.
   It is up to the caller to free this memory.

Arguments:
   GuidClassInputString a null terminated ascii string to give to PnP.

Return Value:
   TRUE if success.

   errors placed in GetLast error.
--*/

BOOLEAN __stdcall
HidD_GetPreparsedData (
   IN    HANDLE                  HidDeviceObject,
   OUT   PHIDP_PREPARSED_DATA  * PreparsedData
   );
/*++
Routine Description:
   Given a handle to a valid Hid Class Device Oject retrieve the preparsed data.
   This routine will ``malloc'' the apropriately size buffer to hold this
   preparsed data.  It is up to the caller to then free that data at the
   caller's conveniance.

Arguments:
   HidDeviceObject a handle to a HidDeviceObject.  The client can obtain this
                   handle via a create file on a string name of a Hid device.
                   This string name can be obtained using standard PnP calls.
   PreparsedData   an opaque data used by other functions in this library to
                   retreive information about a given device.

Return Value:
   TRUE if successful.

   errors returned by DeviceIoControl

--*/


BOOLEAN __stdcall
HidD_FlushQueue (
   IN    HANDLE                HidDeviceObject
   );
/*++
Routine Description:
   Flush the input queue for the given HID device.

Arguments:
   HidDeviceObject a handle to a HidDeviceObject.  The client can obtain this
                   handle via a create file on a string name of a Hid device.
                   This string name can be obtained using standard PnP calls.

Return Value:
   TRUE if successful

   errors returned by DeviceIoControl
--*/


BOOLEAN __stdcall
HidD_GetConfiguration (
   IN   HANDLE               HidDeviceObject,
   OUT  PHIDD_CONFIGURATION  Configuration,
   IN   ULONG                ConfigurationLength
   );
/*++
Routine Description:
   Get the configuration information for this hid device

Arguments:
   HidDeviceObject a handle to a HidDeviceObject.
   Configuration a configuration structure.  You MUST call HidD_GetConfiguration
                 before you can modify the configuration and use
                 HidD_SetConfiguration.
   ConfigurationLength that is ``sizeof (HIDD_CONFIGURATION)'' using this
                 parameter we can later increase the length of the configuration
                 array and maybe not break older apps.

Return Value:
same as others
--*/

BOOLEAN __stdcall
HidD_SetConfiguration (
   IN   HANDLE               HidDeviceObject,
   OUT  PHIDD_CONFIGURATION  Configuration,
   IN   ULONG                ConfigurationLength
   );
/*++
Routine Description:
   Set the configuration information for this hid device

Arguments:
   HidDeviceObject a handle to a HidDeviceObject.
   Configuration a configuration structure.  You MUST call HidD_GetConfiguration
                 before you can modify the configuration and use
                 HidD_SetConfiguration.
   ConfigurationLength that is ``sizeof (HIDD_CONFIGURATION)'' using this
                 parameter will allow us later to inclrease the size of the
                 configuration structure.


Return Value:
same as others
--*/

#if 0
HidD_GetHIDDescriptor (
   IN    HANDLE                HidDeviceHandle,
   OUT   PHIDP_HIDP_DESCRIPTOR HidDescriptor,
   OUT   PULONG                HidDescriptorLength
   );
/*++
Routine Description:
   Given a handle to a valid Hid Class Device Oject retrieve the hid desc.
   This routine will ``malloc'' the apropriately size buffer to hold this
   preparsed data.  It is up to the caller to then free that data at the
   caller's conveniance.

Arguments:
   HidDeviceHandle a handle to a HidDeviceObject.  The client can obtain this
                   handle via a create file on a string name of a Hid device.
                   This string name can be obtained using standard PnP calls.

Return Value:
·  STATUS_SUCCESS
·  STATUS_INVALID_PARAMETER
--*/

NTSTATUS
HidD_GetReportDescriptor (
   IN    HANDLE                   HidDeviceHandle,
   OUT   PHIDP_REPORT_DESCRIPTOR  ReportDesc,
   OUT   PULONG                   ReportDescLength
   );
/*++
Routine Description:
   Given a handle to a valid Hid Class Device Oject retrieve the report desc.
   This routine will ``malloc'' the apropriately size buffer to hold this
   preparsed data.  It is up to the caller to then free that data at the
   caller's conveniance.

Arguments:
   HidDeviceHandle a handle to a HidDeviceObject.  The client can obtain this
                   handle via a create file on a string name of a Hid device.
                   This string name can be obtained using standard PnP calls.

Return Value:
·  STATUS_SUCCESS
·  STATUS_INVALID_PARAMETER
--*/

#endif
#endif
