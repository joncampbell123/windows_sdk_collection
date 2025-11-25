/*++

Copyright (c) 1996  Microsoft Corporation

Module Name: 

    input.h

Abstract

    include file for inputport drivers


Author:

    Daniel Dean.

Environment:

    Kernel mode

Revision History:


--*/

#ifndef __HID_H__
#define __HID_H__


#ifdef OUTPUT
#undef OUTPUT
#endif



//
// The CHANNEL is the fundamental element of an input device,
// it is the input or output axis.
//

typedef unsigned long    CHANNEL;
typedef unsigned long  * PCHANNEL;


typedef struct _USAGE
{
    //
    // Specific usage number on page. See HID Specification Section 5.4.5.1
    //

    USHORT index;

    //
    // Usage page number. See HID Specification Section 5.4.4.1
    //
    
    USHORT page;

} USAGE, * PUSAGE;

//
// The EXTENT structure is derived from the HID Specification,
// it describs the type of data in a channel.
//

typedef struct _EXTENT
{
    //
    // Flag, if TRUE the value is an integer.
    //
    
    ULONG integer; 

    //
    // Logical minimum value. See HID Specification Section 5.4.4.2
    //
    
    ULONG logicalmin;

    //
    // Logical maximum value. See HID Specification Section 5.4.4.2
    //
    
    ULONG logicalmax;

    //
    // Physical minimum value. See HID Specification Section 5.4.4.3
    //
    
    ULONG physicalmin;

    //
    // Physical maximum value. See HID Specification Section 5.4.4.3
    //
    
    ULONG physicalmax;

} EXTENT, * PEXTENT;

//
// The UNIT structure is derived from the HID Specification,
// it describs the type of data in a channel.
//

typedef struct _UNIT
{
    //
    // TIME, MASS, LENGTH, SYSTEM, attribute of channel. See HID Specification Section 5.4.4.5
    //
    
    ULONG   unit;

    //
    // The base 10 exponent of channel. See HID Specification Section 5.4.4.4
    //
    
    ULONG   exponent;

} UNIT, * PUNIT;

//
// that provides information about the specific part or parts of the
// human body that are inputting information.
//

typedef struct _DESIGNATOR
{
    //
    //
    //

    ULONG   designator;

    //
    //
    //

    ULONG   desFlags;

} DESIGNATOR, * PDESIGNATOR;

//
//The INPUT structure idescribs the type of data in a input channel.
//

typedef struct _INPUT
{
    //
    // Flag defined from the HID Specification, see section 5.4.3.1.
    //
    
    ULONG description;
    
    //
    // A UNIT structure.
    //
    
    UNIT unit;
    
    //
    // An EXTENT structure.
    //
    
    EXTENT extent;

} INPUT, * PINPUT;

//
// The OUTPUT structure idescribs the type of data in a output channel.
//

typedef struct _OUTPUT
{
    //
    // Flag defined from the HID Specification, see section 5.4.3.2.
    //
    
    ULONG description;
    
    //
    // A UNIT structure.
    //
    
    UNIT unit;
    
    //
    // A EXTENT structure.
    //
    
    EXTENT extent;

} OUTPUT, * POUTPUT;

//
// The FEATURE structure describs.
//

typedef struct _FEATURE
{
    //
    // Flag defined from the HID Specification, see section 5.4.3.2.
    //
    
    ULONG description;
    
    //
    // A UNIT structure.
    //
    
    UNIT unit;
    
    //
    // A EXTENT structure.
    //
    
    EXTENT extent;

} FEATURE, * PFEATURE;

typedef struct _COLLECTION
{
    //
    // Flag defined from the HID Specification, see section 5.4.3.1.
    //
    
    ULONG description;
    
} COLLECTION, * PCOLLECTION;

//
// The CHANNEL structure describes a fundamental element of an input device
//

typedef struct _CHANNELDESC
{
    struct _CHANNELDESC * pNext;

    //
    // Size of the structure.
    //
    
    ULONG size;
    
    //
    // Type of descriptor ie INPUT, OUTPUT, FEATURE, COLLECTION.
    //
    
    ULONG type;
    
    //
    // Flag defined from the HID Specification, see section 5.4.3.1.
    //
    
    ULONG typeValue;

    //
    // Usage struct describing the usage. 
    //
    
    USAGE Usage;
    
    //
    // If the type is INPUT and typeValue is ARRAY this is the size 
    // of the usage array following this structure.
    //
    
    ULONG UsageArraySize;

    //
    // Information describing the specific part or parts of the human body
    // that are inputting information to the channel.
    //

    DESIGNATOR Designator;
    
    //
    // String describing entity.
    //

#ifdef _NTDDK_
    UNICODE_STRING String;
#else
    ULONG StringLength;
    PVOID String;
#endif   
 
    //
    // A UNIT structure.
    //
    
    UNIT Unit;
    
    //
    // An EXTENT structure.
    //
    
    EXTENT Extent;

} CHANNELDESC, *PCHANNELDESC;

//
// This strucutre discribes the position of an
// input channel in a raw data packet.
//

typedef struct _RAWDESC
{
    //
    // Byte offset into raw packet
    //

    ULONG ByteOffset;

    //
    // Bit offset added to Byte offset into raw packet
    //

    ULONG BitOffset;

    //
    // Bit size of raw data
    //

    ULONG BitSize;

} RAWDESC, * PRAWDESC;

typedef struct _PORTDESC
{
    //
    // Usage struct describing the usage of the device. 
    //
    
    USAGE Usage;

    //
    // Flags describing device type ie. POLLED, INTERUPT ... . 
    //
    
    ULONG systemtype; // POLLED | INTERRUPT | POINTER_DEVICE
    
    //
    // size of string name of device ie "MOUSE".
    //
    
    ULONG devicenamesize;
    
    //
    // size of string name of OEM ie "MICROSOFT".
    //
    
    ULONG oemnamesize;
    
    //
    // Vendor specific info.
    //
    
    ULONG extrainfo;
    
    //
    // Version Number of the device.
    //
    
    ULONG versionnumber;
    
    //
    // Vendor id number.
    //
    
    ULONG vendorid;

    //
    // Vendors device id.
    //
    
    ULONG deviceid;

    //
    // NULL terminated strings.
    //

    //WCHAR DeviceString[1];
    //WCHAR OEMString[1];

} PORTDESC, * PPORTDESC;

//
// The DEVICEDESC is a structure describing the input device.
//

typedef struct _DEVICEDESC
{
    //
    // Size of the structure.
    //
    
    ULONG size; 
    
    //
    // Size of packet in CHANNEL's.
    //
    
    ULONG packetsize; 
    
    //
    // Size of queue in PacketSize. 
    //
    
    ULONG queuesize;
    
    //
    // Port device information.
    //

    PORTDESC PortDesc;

} DEVICEDESC, *PDEVICEDESC;

//
// DeviceType defines.
//

#define POLLED              0x00000001      // Device can be polled
#define INTERRUPT           0x00000002      // Device can be interrupt driven
#define SYS_KEYBOARD        0x00000004      // Can be used as a system keyboard
#define SYS_MOUSE           0x00000008      // Can be used as a system mouse
#define SYS_JOYSTICK        0x00000010      // Can be used as a system joystick


#ifdef _NTDDK_

//
// Input call this function in the port driver to query for strings, designators, ect.
//

#define HID_QUERY_PORTDESC    0
#define HID_QUERY_STRING      1
#define HID_QUERY_DESIGNATOR  2

typedef struct _QUERYID
{
   ULONG QueryType;
   ULONG Index;
   ULONG LanguageID;

} QUERYID, * PQUERYID;


//
// Kernal mode class api's
//
// Port driver call this function in the class driver to add data to the queue.
//

VOID 
HIDQueueData(
    IN PDEVICE_OBJECT ClassObject, 
    OUT PULONG pBuffer,
    IN ULONG Number
    );

//
// Port drivers call this function in the class driver to register a new device.
//

NTSTATUS
HIDCreateClassObject(
    IN PDEVICE_OBJECT * ClassObject,
    PUNICODE_STRING FullPortName,
    PUCHAR Entity,
    ULONG EntitySize
   );

//
// Port drivers call this function in the class driver when they need to unload
//

NTSTATUS
HIDDestroyClassObject(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// HID ERROR CODES
//

#define STATUS_HID_NOT_INITIALIZED STATUS_APP_INIT_FAILURE


#endif _NTDDK_


//
// 0x8000 - 0xFFFF are reserved for use by customers.
//

#define FILE_DEVICE_HID         FILE_DEVICE_MOUSE

//
// 0x800 - 0xFFF are reserved for customers.
//

#define PORT_IOCTL_INDEX        0x830
#define CLASS_IOCTL_INDEX       0x930

//
// The HID Port device driver IOCTLs.
//

#define IOCTL_INTERNAL_HID_QUERY          CTL_CODE(FILE_DEVICE_HID, PORT_IOCTL_INDEX+1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_HID_DISCONNECT     CTL_CODE(FILE_DEVICE_HID, PORT_IOCTL_INDEX+2, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_HID_ENABLE         CTL_CODE(FILE_DEVICE_HID, PORT_IOCTL_INDEX+3, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_HID_DISABLE        CTL_CODE(FILE_DEVICE_HID, PORT_IOCTL_INDEX+4, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_HID_POLL           CTL_CODE(FILE_DEVICE_HID, PORT_IOCTL_INDEX+5, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// The HID Class device IOCTLs.
//

#define IOCTL_CLASS_HID_DEVICE            CTL_CODE(FILE_DEVICE_HID, CLASS_IOCTL_INDEX+0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CLASS_HID_CHANNEL           CTL_CODE(FILE_DEVICE_HID, CLASS_IOCTL_INDEX+1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CLASS_HID_FLUSH             CTL_CODE(FILE_DEVICE_HID, CLASS_IOCTL_INDEX+2, METHOD_BUFFERED, FILE_ANY_ACCESS)
        
//
// Misc.
//

#define MAX_HID_PORTS               100
#define MAX_HID_PORTS_LSTR_SIZE     5
#define MAX_SUFFIX_SIZE             8
#define HID_CLASS_PORT_NAME         L"HIDClass"
#define HID_DEVICE_PORT_NAME        L"HIDPort"

//
// Defines from the HID 1.0 spec
//

#define ITEM_SIZE                   0x3
#define ITEM_TYPE                   0x0C
#define TYPE_ENTITY                 0xFC
#define TYPE_ATTRIBUTE              1
#define TYPE_CONTROL                2
#define TYPE_RESERVED               3
#define ITEM_TAG                    0xF0

//
// Entity item tags
//

#define INPUT                       0x80
#define OUTPUT                      0x90
#define COLLECTION                  0xA0
#define FEATURE                     0xB0
#define END_COLLECTION              0xC0

//
// Entity Attribute item tags
//

#define USAGE_PAGE                  0x04
#define LOGICAL_EXTENT_MIN          0x14
#define LOGICAL_EXTENT_MAX          0x24
#define PHYSICAL_EXTENT_MIN         0x34
#define PHYSICAL_EXTENT_MAX         0x44
#define UNIT_EXPONENT               0x54
#define UNIT                        0x64
#define REPORT_SIZE                 0x74
#define REPORT_ID                   0x84
#define REPORT_COUNT                0x94
#define PUSH                        0xA4
#define POP                         0xB4

//
// Control attribute item tags
//

#define USAGE                       0x08
#define USAGE_MIN                   0x18
#define USAGE_MAX                   0x28
#define DESIGNATOR_INDEX            0x38
#define DESIGNATOR_MIN              0x48
#define DESIGNATOR_MAX              0x58
#define STRING_INDEX                0x68
#define STRING_MIN                  0x78
#define STRING_MAX                  0x88

//
// Usage Pages
//

#define USEPAGE_DESKTOP         0x01

//
// Desktop Page Usages
//

#define USAGE_X             0x30
#define USAGE_Y             0x31
#define USAGE_Z             0x32
#define USAGE_SLIDER            0x36
#define USAGE_DIAL          0x37
#define USAGE_BUTTON            0x38
#define USAGE_HATSWITCH         0x39

//
// INPUT Parameter Bits
//

#define INPUT_BIT0          0x01
#define INPUT_DATA          0x00
#define INPUT_CONSTANT          INPUT_BIT0

#define INPUT_BIT1          0x02
#define INPUT_ARRAY         0x00
#define INPUT_VARIABLE          INPUT_BIT1

#define INPUT_BIT2          0x04
#define INPUT_ABSOLUTE          0x00
#define INPUT_RELATIVE          INPUT_BIT2

#define INPUT_BIT3          0x08
#define INPUT_NOWRAP            0x00
#define INPUT_WRAP          INPUT_BIT3

#define INPUT_BIT4          0x10
#define INPUT_LINEAR            0x00
#define INPUT_NONLINEAR         INPUT_BIT4

#define INPUT_BIT5          0x20
#define INPUT_PREFERRED         0x00
#define INPUT_NOPREFERRED       INPUT_BIT5

#define INPUT_BIT6          0x40
#define INPUT_NULL          0x00
#define INPUT_NONULL            INPUT_BIT6

#define INPUT_TESTPARAM(x, mask, val) (((x) & (mask)) == (val))

#endif // __HID_H__

/*++
      (C) C O P Y R I G H T   M I C R O S O F T   C O R P   1 9 9 6.
--*/
