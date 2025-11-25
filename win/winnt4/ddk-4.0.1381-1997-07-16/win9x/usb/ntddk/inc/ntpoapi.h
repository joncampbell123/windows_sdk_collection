/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1995  Microsoft Corporation

Module Name:

    ntpoapi.h

Abstract:

    This module contains the user APIs for the NT Power Management.

Author:

Revision History:

--*/

#ifndef _NTPOAPI_
#define _NTPOAPI_

//
// Power Management user APIs
//

// begin_ntddk begin_ntifs begin_nthal begin_ntminiport

typedef enum _SYSTEM_POWER_STATE {
    PowerSystemUnspecified = 0,
    PowerSystemWorking,
    PowerSystemSleeping1,
    PowerSystemSleeping2,
    PowerSystemSleeping3,
    PowerSystemHibernate,
    PowerSystemMaximum
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

typedef enum _DEVICE_POWER_STATE {
    PowerDeviceUnspecified = 0,
    PowerDeviceD0,
    PowerDeviceD1,
    PowerDeviceD2,
    PowerDeviceD3,
    PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

typedef union _POWER_STATE {
    SYSTEM_POWER_STATE SystemState;
    DEVICE_POWER_STATE DeviceState;
    ULONG DeviceSpecificState;
} POWER_STATE, *PPOWER_STATE;

typedef enum _POWER_STATE_TYPE {
    SystemPowerState,
    DevicePowerState,
    DeviceSpecificPowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;


// end_ntddk end_nthal end_ntifs end_ntminiport

NTSYSAPI
NTSTATUS
NTAPI
NtSetSystemPowerState(
    IN SYSTEM_POWER_STATE SystemPowerState,
    IN BOOLEAN NoResumeAlarm,
    IN BOOLEAN ForcePowerDown
    );

// begin_ntddk begin_nthal begin_ntminiport

typedef enum {
    BatteryInformation,
    BatteryTemperature,
    BatteryExtimatedTime,
    BatteryDeviceName,
    BatteryManufactureDate,
    BatteryManufactureName,
    BatteryModelNumber,
    BatteryUniqueID
} BATTERY_QUERY_INFORMATION_LEVEL;

typedef struct _BATTERY_QUERY_INFORMATION {
    ULONG                           BatteryTag;
    BATTERY_QUERY_INFORMATION_LEVEL InformationLevel;
} BATTERY_QUERY_INFORMATION, *PBATTERY_QUERY_INFORMATION;

typedef struct _BATTERY_INFORMATION {
    ULONG       Capabilities;
    UCHAR       Technology;
    UCHAR       Reserved[2];
    UCHAR       Chemistry[4];
    ULONG       DesignedCapacity;
    ULONG       FillChargedCapacity;
    ULONG       CycleCount;
} BATTERY_INFORMATION, *PBATTERY_INFORMATION;

typedef enum {
    BatteryCharge,
    BatteryDischarge
} BATTERY_SET_INFORMATION_LEVEL;


typedef struct _BATTERY_SET_INFORMATION {
    ULONG                         BatteryTag;
    BATTERY_SET_INFORMATION_LEVEL InformationLevel;
    UCHAR                         Buffer[1];
} BATTERY_SET_INFORMATION, *PBATTERY_SET_INFORMATION;

typedef struct _BATTERY_WAIT_STATUS {
    ULONG       BatteryTag;
    ULONG       Timeout;
    ULONG       PowerState;
    ULONG       LowCapacity;
    ULONG       HighCapacity;
} BATTERY_WAIT_STATUS, *PBATTERY_WAIT_STATUS;

typedef struct _BATTERY_STATUS {
    ULONG       PowerState;
    ULONG       Capacity;
    ULONG       Voltage;
    LONG        Current;
} BATTERY_STATUS, *PBATTERY_STATUS;

#define BATTERY_POWER_ON_LINE   0x00000001
#define BATTERY_DISCHARING      0x00000002
#define BATTERY_CHARGING        0x00000004
#define BATTERY_CRITICAL        0x00000008

//
// Power management IOCTLs
//

#define IOCTL_BATTERY_QUERY_TAG         \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x10, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_BATTERY_QUERY_INFORMATION \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x11, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_BATTERY_SET_INFORMATION   \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x12, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_BATTERY_QUERY_STATUS      \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x13, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_BATTERY_SET_RESUME        \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x14, METHOD_BUFFERED, FILE_READ_ACCESS)

// end_ntddk end_nthal end_ntminiport

#endif // _NTPOAPI_

