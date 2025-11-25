/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    kbd_attr.c

Abstract:

    A user mode test app demoing how to create a symbolic link
    ( a Win32 alias that an app can spcify in CreateFile() ) for
    a keyboard device, open the device, and send it an i/o request.

Environment:

    User mode only

Revision History:

    05-26-93 : created

--*/

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ntddkbd.h>



int
main(
    IN int  argc,
    IN char *argv[]
    )
/*++

Routine Description:

    Creates a symbilic link to \Device\KeyboardPort0, opens the
    device, and send it an i/o request.

Arguments:

    argc - count of command line arguments

    argv - command line arguments

Return Value:

    1 if success
    0 if an error occurs

--*/
{

    int     rc = 1;  // be optomistic, assume success
    HANDLE  hDriver;
    DWORD   cbReturned;
    char    deviceName[] = "KBD";
    char    targetPath[] = "\\Device\\KeyboardClass0";
    char    completeDeviceName[] = "\\\\.\\KBD";

    KEYBOARD_ATTRIBUTES kbdattrs;

    //
    // The kuid=0 here corresponds to the keyboard port device # 0
    // (e.g. \Device\KeyboardPort). We go through the keyboard
    // class device # 0 to get to the kbd port device # 0.
    //

    KEYBOARD_UNIT_ID_PARAMETER kuid = { 0 };


    //
    // First create a symbolic link to the NT keyboard object (number 0-
    // there may be more than 1)
    //

    if (DefineDosDevice (DDD_RAW_TARGET_PATH,
                         deviceName,
                         targetPath
                         ))
    {
        printf ("\nDefinedDosDevice (%s, %s) worked\n",
                deviceName,
                targetPath
                );
    }
    else
    {
        printf ("\nDefinedDosDevice (%s, %s) faileded\n",
                deviceName,
                targetPath
                );

        return 0;
    }



    //
    // Next, try to open the device
    //

    if ((hDriver = CreateFile (completeDeviceName,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL
                               )) != ((HANDLE)-1))

        printf ("\nRetrieved valid handle for %s\n",
                deviceName
                );


    else
    {
        printf ("Can't get a handle to %s\n",
                deviceName
                );

        rc = 0;

        goto delete_dos_device;
    }



    //
    // Send it a request
    //

    if (DeviceIoControl (hDriver,
                         (DWORD) IOCTL_KEYBOARD_QUERY_ATTRIBUTES,
                         &kuid,
                         sizeof(KEYBOARD_UNIT_ID_PARAMETER),
                         &kbdattrs,
                         sizeof(KEYBOARD_ATTRIBUTES),
                         &cbReturned,
                         0
                         ))
    {
        printf ("DeviceIoControl worked\n\n");

        printf ("\tkbd_attr.KeyboardIdentifier.Type = %d\n",
                (int) kbdattrs.KeyboardIdentifier.Type
                );

        printf ("\tkbd_attr.KeyboardIdentifier.Subtype = %d\n",
                (int) kbdattrs.KeyboardIdentifier.Subtype
                );

        printf ("\tkbd_attr.KeyboardMode = %d\n",
                (int) kbdattrs.KeyboardMode
                );

        printf ("\tkbd_attr.NumberOfFunctionKeys = %d\n",
                (int) kbdattrs.NumberOfFunctionKeys
                );

        printf ("\tkbd_attr.NumberOfIndicators = %d\n",
                (int) kbdattrs.NumberOfIndicators
                );

        printf ("\tkbd_attr.NumberOfKeysTotal = %d\n",
                (int) kbdattrs.NumberOfKeysTotal
                );

        printf ("\tkbd_attr.InputDataQueueLength = %d\n",
                (int) kbdattrs.InputDataQueueLength
                );

        printf ("\tkbd_attr.KeyRepeatMinimum.UnitId = %d\n",
                (int) kbdattrs.KeyRepeatMinimum.UnitId
                );

        printf ("\tkbd_attr.KeyRepeatMinimum.Rate = %d\n",
                (int) kbdattrs.KeyRepeatMinimum.Rate
                );

        printf ("\tkbd_attr.KeyRepeatMinimum.Delay = %d\n",
                (int) kbdattrs.KeyRepeatMinimum.Delay
                );

        printf ("\tkbd_attr.KeyRepeatMaximum.UnitId = %d\n",
                (int) kbdattrs.KeyRepeatMaximum.UnitId
                );

        printf ("\tkbd_attr.KeyRepeatMaximum.Rate = %d\n",
                (int) kbdattrs.KeyRepeatMaximum.Rate
                );

        printf ("\tkbd_attr.KeyRepeatMaximum.Delay = %d\n",
                (int) kbdattrs.KeyRepeatMaximum.Delay
                );
    }
    else
    {
        DWORD err = GetLastError();

        printf ("DeviceIoControl failed, err = %d\n(%x)\n",
                err,
                err
                );

        rc = 0;
    }



    //
    // Clean up
    //

    CloseHandle(hDriver);

delete_dos_device:

    DefineDosDevice (DDD_REMOVE_DEFINITION,
                     deviceName,
                     NULL
                     );

    return rc;
}
