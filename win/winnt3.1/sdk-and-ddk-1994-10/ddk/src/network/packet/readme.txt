
PACKET.SYS

The PACKET.SYS driver demonstrates the use of the upper edge NDIS 3.0
services to communicate with NDIS 3.0 MAC drivers.

The driver creates a device object which may be opened by Win32 apps
via a symbolic link.  A Packet may be sent by writing data to the
driver.  Receives may be pended to the driver by reading from the
driver.

The driver expects that it will be bound an Ethernet card as it makes
assumptions about frame size.

The testapp that accompanies the driver is there only to illustrate
that the kernel mode driver functions as advertised.  It is not
intended as a full featured reliable test application.



In order to install the driver there is an .INI which is run using
REGINI.EXE.

To use it type

REGINI.EXE PACKET.INI

The PACKET.INI file will need to be edited for the particular netcard
in use in a given machine.

The "Bind =" line in must be changed for proper netcard driver.

For example \Device\Elnk301, \Device\NE200001


                    Parameters
-->                     Bind = \Device\pcnet02
                        Export = \Device\Packet
