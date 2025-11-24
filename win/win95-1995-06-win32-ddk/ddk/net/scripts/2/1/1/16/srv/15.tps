#	-SECTION_START-( 12.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Functional & Broadcast & Resend Functional-server-side" )
##
## TITLE: 2.1.1.16.15 Send Functional, Receive Directed & Functional &
## TITLE              Broadcast & Resend Functional (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & FUNCTIONAL & BROADCAST
## packets.  Each packet will contain a resend packet the test card will
## resend.  Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## The server side of the test is run on the machine with the test
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 12.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.01 )

# And open and setup the card.

#	-SECTION_START-( 12.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 12.02 )

#	-SECTION_START-( 12.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL
#	-SECTION_END-( 12.03 )

#	-SECTION_START-( 12.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.04 )

# Now set the trusted card to receive packets.

#	-SECTION_START-( 12.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 12.05 )

########################################################

# Now wait for the client side to start the test

#	-SECTION_START-( 12.06 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21116151
#	-SECTION_END-( 12.06 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 12.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.07 )

#	-SECTION_START-( 12.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 12.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.09 )

#	-SECTION_START-( 12.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 12.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.11 )

#	-SECTION_START-( 12.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.12 )

########################################################

# Now tell the client side to clean up.

#	-SECTION_START-( 12.13 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21116152
#	-SECTION_END-( 12.13 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 12.14 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 12.14 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 12.15 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 12.15 )

#	-SECTION_START-( 12.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.16 )

#	-SECTION_END-( 12.00 )
