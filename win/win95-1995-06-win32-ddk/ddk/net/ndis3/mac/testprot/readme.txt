This file describes the NDIS 3.1 MAC test tool, TESTPROT.


    Testprot consists of a dos application and a VXD.  It's primary purpose
    is to test a MAC in a similar fashion to the NDIS 2.0 test tool Pancake.

    The ndistdsn.doc file is a Word For Windows 2.0 version of the spec
    used to create testprot.  ndistdsn.txt is a text version of the spec.

    Testprot is not fully implemented to the specification.  The stress
    functionality has been modified in an effort to exercise the MAC
    under extreme network traffic.

    This version of Testprot has been modified to meet the NDIS 3.1
    addendum to the NDIS 3.0 spec, specifically to support the dynamic
    loading and unloading of MAC drivers.  From the MAC developers
    perspective, nothing has changed.

    To use Testprot, copy nettpdrv.inf to your \windows\inf directory,
    and copy the appropriate tpdrvr.386 and tpdrvr.sym to the \windows\system
    directory.  Start Windows, and from the Network Control Panel, add
    the Microsoft NDIS Netcard Tester to your protocol list, binding it
    to the netcard under test.  Restart Windows, and Testprot will be
    ready to run your existing tests.

    After starting windows, run the dos application tpctl.exe to run
    Testprot.

    Two machines are required for the stress tests.  Set up the second
    identically to the first.


General Notes:

    The packets per second number is an approximate value.  An external timer
    should be used for accurate statistics.

    Testprot is most useful if used on two high speed machines with 8mb
    memory.  When the packettype CYCLICAL test is used with packet makeup
    ONES, FULL_RESPONSE, DATA CHECKING ENABLED, and PACKETS FROM POOL, the
    MAC driver will be exercised under extreme conditions.  When running
    this test the client machine should expect to process approximately %25
    of the packets returned by the server.

    The CYCLICAL option implementation differs slightly in that instead
    of sending out a factorial number of packets, it sends out one packet
    of each size with 8 buffers.

    Not in the documentation is the one buffer packet (packet makeup type
    one).  This is most when a pool is used and generates extreme stress
    on the mac.

