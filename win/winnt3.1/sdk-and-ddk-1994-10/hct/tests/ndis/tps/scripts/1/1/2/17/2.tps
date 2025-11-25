#   -SECTION_START-( 2.0 ) -SECTION_DESC-( "Send Functional, Receive Directed,Broadcast,Functional,MacFrame,Sourcerouting & Resend Functional" ) -OPTIONALS-( MACFRAME, SOURCEROUTING )
##
## TITLE: 1.1.2.17.2 Send Functional, Receive Functional & Directed &
## TITLE:            Broadcast & Source Routing & Mac Frame & Resend
## TITLE:            Functional (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to itself on the
## same open on the test card.  The test card will have its packetfilter
## set to receive FUNCTIONAL & DIRECTED & BROADCAST & SOURCEROUTING
## & MACFRAME packets.  Each packet will contain a resend packet the
## test card will resend.  Packets of size 80 bytes, 512 bytes, and
## MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 2.01 )

# And open and setup the card.

#   -SECTION_START-( 2.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 2.02 )

#   -SECTION_START-( 2.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|DIRECTED|BROADCAST|SOURCEROUTING|MACFRAME
#   -SECTION_END-  ( 2.03 )

#   -SECTION_START-( 2.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 2.04 )

########################################################

# Now set the test card to receive packets.

#   -SECTION_START-( 2.05 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 2.05 )

########################################################

# Send 80 byte packets.

#   -SECTION_START-( 2.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 2.06 )

#   -SECTION_START-( 2.07 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.07 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 2.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 2.08 )

#   -SECTION_START-( 2.09 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.09 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 2.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 2.10 )

#   -SECTION_START-( 2.11 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.11 )

########################################################

# Then stop the receives and dump the statistics.

#   -SECTION_START-( 2.12 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 2.12 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 2.13 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 2.13 )

#   -SECTION_START-( 2.14 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 2.14 )

#   -SECTION_END-( 2.0 )
