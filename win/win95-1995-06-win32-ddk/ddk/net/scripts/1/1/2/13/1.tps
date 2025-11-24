#   -SECTION_START-( 1.0 )  -SECTION_DESC-( "Send Functional,Receive Functional,Resend Functional" )
##
## TITLE: 1.1.2.13.1 Send Functional, Receive Functional & Resend
## TITLE:            Functional (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to itself on the
## same open on the test card.  The test card will have its packetfilter
## set to receive FUNCTIONAL packets.  Each packet will contain
## a resend packet the test card will resend.  Packets of size
## 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.01 )

# And open and setup the card.

#   -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 1.02 )

#   -SECTION_START-( 1.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL
#   -SECTION_END-  ( 1.03 )

#   -SECTION_START-( 1.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 1.04 )

########################################################

# Now set the test card to receive packets.

#   -SECTION_START-( 1.05 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.05 )

########################################################

# Send 80 byte packets.

#   -SECTION_START-( 1.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 1.06 )

#   -SECTION_START-( 1.07 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.07 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 1.08 )

#   -SECTION_START-( 1.09 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.09 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 1.10 )

#   -SECTION_START-( 1.11 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.11 )

########################################################

# Then stop the receives and dump the statistics.

#   -SECTION_START-( 1.12 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.12 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 1.13 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.13 )

#   -SECTION_START-( 1.14 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.14 )

#   -SECTION_END-  ( 1.0 )
