/*++

Copyright (c) 1996      Microsoft Corporation

Module Name:

        HIDPI.H

Abstract:

   Public Interface to the HID parsing library.

Environment:

    Kernel & user mode

Revision History:

    09-29-95 : created

--*/

#ifndef   __HIDPI_H__
#define   __HIDPI_H__


// Please include "hidsdi.h" to use the user space (dll / parser)
// Please include "hidpddi.h" to use the kernel space parser


typedef enum _HIDP_REPORT_TYPE
{
   HidP_Input,
   HidP_Output,
   HidP_Feature
} HIDP_REPORT_TYPE;


typedef struct _HIDP_BUTTON_CAPS
{
   UCHAR    UsagePage;
   UCHAR    LinkCollection;   // A unique internal index pointer
   UCHAR    LinkUsagePage;
   UCHAR    LinkUsage;

   BOOLEAN  IsRange;
   BOOLEAN  IsStringRange;
   BOOLEAN  IsDesignatorRange;
   UCHAR    ReportID;

   UCHAR    Reserved[10];
   USHORT   BitField;
   union
   {
      struct
      {
         UCHAR    UsageMin,         UsageMax;
         UCHAR    StringMin, 	      StringMax;
         UCHAR    DesignatorMin,    DesignatorMax;
         UCHAR    Reserved[2];
      } Range;
      struct
      {
         UCHAR    Usage,            Reserved1;
         UCHAR    StringIndex,      Reserved2;
         UCHAR    DesignatorIndex,  Reserved3;
         UCHAR    Reserved[2];
      } NotRange;
   };
} HIDP_BUTTON_CAPS, *PHIDP_BUTTON_CAPS;


typedef struct _HIDP_VALUE_CAPS
{
   UCHAR    UsagePage;
   UCHAR    LinkCollection;   // A unique internal index pointer
   UCHAR    LinkUsagePage;
   UCHAR    LinkUsage;

   BOOLEAN  IsRange;
   BOOLEAN  IsStringRange;
   BOOLEAN  IsDesignatorRange;
   BOOLEAN  HasNull;          // Does this channel have a null report

   UCHAR    ReportID;
   UCHAR    Reserved[7];

   USHORT   BitSize;          // How many bits are devoted to this value?
   USHORT   BitField;
   union
   {
      struct
      {
         UCHAR    UsageMin,         UsageMax;
         UCHAR    StringMin, 	      StringMax;
         UCHAR    DesignatorMin,    DesignatorMax;
         UCHAR    Reserved[2];
      } Range;
      struct
      {
         UCHAR    Usage,            Reserved1;
         UCHAR    StringIndex,      Reserved2;
         UCHAR    DesignatorIndex,  Reserved3;
         UCHAR    Reserved[2];
      } NotRange;
   };

   LONG     Null;  // The value that says this channel has no valid data
   LONG     LogicalMin,       LogicalMax;
   LONG     PhysicalMin,      PhysicalMax;
} HIDP_VALUE_CAPS, *PHIDP_VALUE_CAPS;


typedef PCHAR  PHIDP_REPORT_DESCRIPTOR;
typedef PCHAR  PHIDP_HID_DESCRIPTOR;
typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;

typedef struct _HIDP_CAPS
{
   UCHAR    UsagePage;
   UCHAR    Usage;
   USHORT   InputReportByteLength;
   USHORT   OutputReportByteLength;
   USHORT   FeatureReportByteLength;
   USHORT   NumberInputButtonCaps;
   USHORT   NumberInputValueCaps;
   USHORT   NumberOutputButtonCaps;
   USHORT   NumberOutputValueCaps;
   USHORT   NumberFeatureButtonCaps;
   USHORT   NumberFeatureValueCaps;
   USHORT   Reserved[14];
} HIDP_CAPS, *PHIDP_CAPS;

NTSTATUS __stdcall
HidP_GetCaps (
   IN      PHIDP_PREPARSED_DATA      PreparsedData,
   OUT     PHIDP_CAPS                Capabilities
   );
/*++
Routine Description:
   Returns a list of capabilities of a given hid device as described by its
   preparsed data.

Arguments:
   PreparsedData    The preparsed data returned from Hidclass.
   Capabilities     a HIDP_CAPS structure

Return Value:
·  STATUS_SUCCESS
·  STATUS_INVALID_PARAMETER
--*/

#if 0
NTSTATUS __stdcall
HidP_GetChannels (
   IN  HIDP_REPORT_TYPE      ReportType,
   IN  PHIDP_PREPARSED_DATA  PreparsedData,
   OUT PHIDP_CHANNEL_DESC    ChannelArray,
   IN  ULONG                 ChannelElements
   );
/*++
THIS FUNCTION is undocumented!!!
Please use GetButtons and GetValues instead.

Routine Description:
   Returns a list of capabilities of a given hid device as described by its
   preparsed data.

Arguments:
   ReportType  One of HidP_Input, HidP_Output, or HidP_Feature.
   PreparsedData     The preparsed data returned from Hidclass.
   ChannelArray      Pointer an array of channel descriptors.
                     Use HidP_GetCaps to determine the needed number of
                     elements in ChannelArray.
   ChannelElements   the number of elements of said array.

Return Value:
·  STATUS_SUCCESS
·  STATUS_BUFFER_TOO_SMALL The given ChannelArray is too small to hold the
                           channels of the device.  Use HidP_GetCaps to
                           determine the required length.
·  STATUS_INVALID_PARAMETER
--*/
#endif

NTSTATUS __stdcall
HidP_GetButtonCaps (
   IN       HIDP_REPORT_TYPE     ReportType,
   OUT      PHIDP_BUTTON_CAPS    ButtonCaps,
   IN OUT   PULONG               ButtonCapsLength,
   IN       PHIDP_PREPARSED_DATA PreparsedData
);

/*++
Description:
   HidP_GetButtonCaps returns all the buttons (binary values) that are a part
   of the given report type for the Hid device represented by the given
   preparsed data.

Parameters:
   ReportType  One of HidP_Input, HidP_Output, or HidP_Feature.

   ButtonCaps A _HIDP_BUTTON_CAPS array contain information about all the
               binary values in the given report.  This buffer is provided by
               the caller.

   ButtonLength   Starts off as the length of the caller provided buffer, and
                  ends up the length of the button values.  Both are in units
                  array elemenst, not byte length.  The number returned is
                  always less than the corresponding Number*Channels field in
                  struct _HIDP_CAPS obtained from the HidP_GetCaps function.
                  (A channel is an internally defined entity describing all
                  buttons and values in a report.)

   PreparsedData  The preparsed data returned from Hidclass.


Return Value
HidP_GetButtonCaps returns the following error codes:
· STATUS_SUCCESS.
· STATUS_BUFFER_TOO_SMALL

--*/

NTSTATUS __stdcall
HidP_GetValueCaps (
   IN       HIDP_REPORT_TYPE     ReportType,
   OUT      PHIDP_VALUE_CAPS     ValueCaps,
   IN OUT   PULONG               ValueCapsLength,
   IN       PHIDP_PREPARSED_DATA PreparsedData
);

/*++
Description:
   HidP_GetValueCaps returns all the values (non-binary) that are a part
   of the given report type for the Hid device represented by the given
   preparsed data.

Parameters:
   ReportType  One of HidP_Input, HidP_Output, or HidP_Feature.

   ValueCaps   A _HIDP_Value_CAPS array contain information about all the
               binary values in the given report.  This buffer is provided by
               the caller.

   ValueLength    Starts off as the length of the caller provided buffer, and
                  ends up the length of the button values.  Both are in units
                  array elemenst, not byte length.  The number returned is
                  always less than the corresponding Number*Channels field in
                  struct _HIDP_CAPS obtained from the HidP_GetCaps function.
                  (A channel is an internally defined entity describing all
                  buttons and values in a report.)

   PreparsedData  The preparsed data returned from Hidclass.


Return Value
HidP_GetValueCaps returns the following error codes:
· STATUS_SUCCESS.
· STATUS_BUFFER_TOO_SMALL

--*/

NTSTATUS __stdcall
HidP_SetUsages (
   IN       HIDP_REPORT_TYPE      ReportType,
   IN       UCHAR                 UsagePage,
   IN       UCHAR                 LinkCollection, // Optional
   IN       PUCHAR                UsageList,
   IN OUT   PULONG                UsageLength,
   IN       PHIDP_PREPARSED_DATA  PreparsedData,
   IN OUT	PCHAR                 Report,
   IN       ULONG                 ReportLength
   );
/*++

Routine Description:
   This function sets binary values (buttons) in the report.  Given an
   initialized packet of correct length, it modifies the report packet so that
   each element in the given list of usages has been set in the report packet.
   For example, in an output report with 5 LED’s, each with a given usage,
   an application could turn on any subset of these lights by placing their
   usages in any order into the byte array (usageList).  HidP_SetUsage would,
   in turn, set the appropriate bit or add the corresponding byte into the
   HID Main Array Item.

   A properly initialized Report packet is one of the correct byte length,
   and all zeros.

Parameters:
   ReportType One of HidP_Output or HidP_Feature.

   UsagePage  All of the usages in the usage array, which HidP_SetUsage will
              set in the report, refer to this same usage page.
              If the client wishes to set usages in a packet for multiple
              usage pages then that client needs to make subsequent SetUsages
              calls.

   UsageList  A byte array containing the usages that HidP_SetUsage will set in
              the report packet.

   UsageLength The length of the given byte array.
               The parser sets this value to position in the usage array at
               which it stoped processing.  In the successful case UsageList
               will be the same as the value passed in.  In any error condition
               this field reflect how many of the usages in the usage list have
               actually been set by the parser.  This is useful for finding
               the usage in the list which caused the error.  However, in
               the event of an error condition, the report packet itself is in
               an unknown state.

   PreparsedData the preparsed data recevied from the HidClass device object.

   Report      The report packet.

   ReportLength   Length of the given report packet.


Return Value
HidP_SetUsage returns the following error codes.  Upon an error the report
packet is in an unknown state.

· STATUS_SUCCESS upon successful insertion of usages into the report packet.
· STATUS_INVALID_PARAMETER_1 if reportType is not valid.
· STATUS_COULD_NOT_INTERPRET if there exists a byte in the usage list for which
                             there is no corresponding control.
· STATUS_INVALID_PARAMETER_7 if the length of the report packet is not the
                             size expected.
· STATUS_BUFFER_TOO_SMALL if there are not enough entries in a given Main Array
                          Item to list all of the given usages.  The user needs
                          to split his request to set usages up.
--*/

NTSTATUS __stdcall
HidP_GetUsages (
   IN       HIDP_REPORT_TYPE     ReportType,
   IN       UCHAR                UsagePage,
   IN       UCHAR                LinkCollection, // Optional
   OUT      UCHAR *              UsageList,
   IN OUT   ULONG *              UsageLength,
   IN       PHIDP_PREPARSED_DATA PreparsedData,
   IN       PCHAR                Report,
   IN       ULONG                ReportLength
   );
		
/*++

Routine Description:
This function returns the binary values (buttons) in a HID report.
Given a report packet of correct length, it searches the report packet
for each usage for the given usage page and returns them in the usage list.

Parameters:

   ReportType One of HidP_Output or HidP_Feature.

   UsagePage  All of the usages in the usage array, which HidP_SetUsage will
              retrieve in the report, refer to this same usage page.
              If the client wishes to get usages in a packet for multiple
              usage pages then that client needs to make subsequent getUsages
              calls.

   UsageList  A byte array containing the usages that HidP_GetUsage found in
              the report packet.

   UsageLength The length of the given byte array.
               This value initially describes the length of the usage list,
               but HidP_GetUsage sets this value to the length of found usages.
               Use HidP_MaxUsageListLength to determine the maximum length list
               of usages that a given report packet may contain.

   PreparsedData the preparsed data recevied from the HidClass device object.

   Report      The report packet.

   ReportLength   Length of the given report packet.


Return Value
HidpGetUsage returns the following error codes:
· STATUS_SUCCESS.
· STATUS_INVALID_PARAMETER_1 if reportType is not valid.
· STATUS_INVALID_PARAMETER_2 if no control for this device matches the given
                             usagePage.
· STATUS_BUFFER_TOO_SMALL if the given usageList is not long enough to hold the
                          usages found in the given report packet.
                          HidP_MaxUsageListLength should be used to prevent
                          this error.
· STATUS_INVALID_PARAMETER_5 if the given preparsed data is invalid
· STATUS_INVALID_PARAMETER_7 if the length of the report packet is not the size
                             expected.
--*/

ULONG __stdcall
HidP_MaxUsageListLength (
   IN HIDP_REPORT_TYPE      ReportType,
   IN	UCHAR                 UsagePage,
	IN PHIDP_PREPARSED_DATA  PreparsedData
   );
/*++		
Routine Description:

   This function returns the maximum length of usages that a HidpGetUsage
   could return for the given HID Report and Usage Page.

Parameters:

   ReportType  One of HidP_Input or HidP_Feature.

   UsagePage   All of the usages in the usage array, for which HidP_GetUsage will
               search in the report, refer to this same usage page.

   PreparsedData the preparsed data recevied from the HidClass device object.

Return Value:

   The length of the usage list array required for the HidpGetUsage
   function call.
--*/


NTSTATUS __stdcall
HidP_SetUsageValue (
   IN       HIDP_REPORT_TYPE     ReportType,
   IN       UCHAR                UsagePage,
   IN       UCHAR                LinkCollection, // Optional
   IN       UCHAR                Usage,
   IN       ULONG                UsageValue,
   IN       PHIDP_PREPARSED_DATA PreparsedData,
   IN OUT   PCHAR                Report,
   IN       ULONG                ReportLength
   );

/*++
Description:
   HidpSetUsageValue inserts the given value into the given HID Report Packet,
   in the field corresponding to the given usage page and usage.
   HidP_SetUsageValue casts this value to the appropriate bit length.  If there
   are two channel in the report packet with the same usage and UsagePage, then
   they can be destinguished with the optional LinkCollection Field.

Parameters:

   ReportType  One of HidP_Output or HidP_Feature.

   UsagePage   The usage page to which the given usage refers.

   LinkCollection  (Optional)  If there are more than one channel with the
               given usage and usage page, then the client may used this field
               to distinguish them.  A LinkValue of zero is ingnored.  The
               first channel that matches the given usage page, usage page, and
               Link number is the one affected.

   Usage       The usage whose value HidP_SetUsageValue will set.

   UsageValue  The value.  This value must be within the logical range or
               null value specified by the Report Descriptor.

   PreparsedData The data retreived from the HID device

   Report      The report packet.

   ReportLength   Length of the given report packet.


Return Value:
   HidpSetUsageValue returns the following error codes:

· STATUS_SUCCESS.
· STATUS_INVALID_PARAMETER_1 if reportType is not valid.
· STATUS_INVALID_PARAMETER_4 if the given usage does not correspond to a
                             control on the device, or if it refers to a button
                             style control.
· STATUS_INVALID_PARAMETER_5 if the given value is not logically valid for this
                             control.
· STATUS_INVALID_PARAMETER_8 if the length of the report packet is not the size
                             expected given the HIDP_CHANNELS structure.
--*/

NTSTATUS __stdcall
HidP_GetUsageValue (
   IN    HIDP_REPORT_TYPE     ReportType,
   IN    UCHAR                UsagePage,
   IN    UCHAR                LinkCollection, // Optional
   IN    UCHAR                Usage,
   OUT   PULONG               UsageValue,
   IN    PHIDP_PREPARSED_DATA PreparsedData,
   IN    PCHAR                Report,
   IN    ULONG                ReportLength
   );

/*
Description
   HidP_GetUsageValue retrieves the given value from the given HID Report
   Packet, for the specified usage.

Parameters:

   ReportType  One of HidP_Output or HidP_Feature.

   UsagePage   The usage page to which the given usage refers.

   LinkCollection  (Optional)  If there are more than one channel with the
               given usage and usage page, then the client may used this field
               to distinguish them.  A LinkValue of zero is ingnored.  The
               first channel that matches the given usage page, usage page, and
               Link number is the one affected.

   Usage       The usage whose value HidP_GetUsageValue will retreive.

   UsageValue  The value.  This value must be within the logical range or
               null value specified by the Report Descriptor.

   PreparsedData The data retreived from the HID device

   Report      The report packet.

   ReportLength   Length of the given report packet.


Return Value:
   HidpSetUsageValue returns the following error codes:

· STATUS_SUCCESS.
· STATUS_INVALID_PARAMETER_1 if reportType is not valid.
· STATUS_INVALID_PARAMETER_4 if the given usage does not correspond to a
                             control on the device, or if it refers to a button
                             style control.
· STATUS_INVALID_PARAMETER_5 if the given value is not logically valid for this
                             control.
· STATUS_INVALID_PARAMETER_8 if the length of the report packet is not the size
                             expected given the HIDP_CHANNELS structure.
--*/



#endif
