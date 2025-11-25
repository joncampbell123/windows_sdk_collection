#------------------------------------------
# File:  multcli.tst
#
# This scripts drives the client side of tests for multiple lines (channels).
# It is assumed that the maximum channels possible
# on the miniport is 2. If more channels are supported,
# change the variable $G_NumChannels to the actual
# number. For example, if 23 channels are supported, change
# this variable to 23. Also, change the dimensions of
# the arrays: $G_ClientOpen and $G_ServerOpen
#
# Make sure that the wancard# environ vars are set ( as in mwcli.bat)
#
# Script globals are defined here.  Whenever they are set
# as the result of a command, they must be enclosed in the
# "global" "end_global" pair, or else the script might create
# a local variable with the same name.
#
# Adapted in most part from ndiswan.tst
#-------------------------------------------

global

#--------------------------------------
# the following global variables describe the test card
#--------------------------------------
$G_TestCard = "bogus"
$G_MesgCard = "bogus"
$G_Media = 0
$G_Filters = 0
$G_MaxLookahead = 0
$G_MaxTotalSize = 0
$G_TestAddress = array uchar 6
$G_TestAddress[0] = (uchar) 0x00
$G_TestAddress[1] = (uchar) 0x00
$G_TestAddress[2] = (uchar) 0x00
$G_TestAddress[3] = (uchar) 0x00
$G_TestAddress[4] = (uchar) 0x00
$G_TestAddress[5] = (uchar) 0x00

#--------------------------------------
# this global variable is only used for arcnet cards.
# it determines whether it is opened in Arcnet878_2 mode (if FALSE)
# or encapsulated ethernet mode (if TRUE)
#--------------------------------------
$G_OpenFlag = TRUE

#--------------------------------------
# the following global variables describe the trusted card
#--------------------------------------
$G_TrustedCard = "bogus"
$G_TrustedTotalSize = 0
$G_TrustedAddress = array uchar 6
$G_TrustedAddress[0] = (uchar) 0x00
$G_TrustedAddress[1] = (uchar) 0x00
$G_TrustedAddress[2] = (uchar) 0x00
$G_TrustedAddress[3] = (uchar) 0x00
$G_TrustedAddress[4] = (uchar) 0x00
$G_TrustedAddress[5] = (uchar) 0x00

#--------------------------------------
# this global variable is the array of open instances used for sending
# commands to the server
#--------------------------------------
$G_NumChannels = 2
$G_ConnId = array ulong 2
$G_ClientOpen = array ulong 2

#--------------------------------------
# this global variable determine the number of packets that are
# sent in all the send tests
#--------------------------------------
$G_PacketsToSend = 10

end_global

debuglevel  1
#------------------------------------------
# run 2 machine, 2 card tests -- note:  uses single server
#------------------------------------------

variation

#
# enable wan connectivity
#
enablewan client

#
# initialize the connection params
#
$G_NumWanPorts = conninit "isdn" "9,5561393" "64000" "0"

if ($G_NumWanPorts != $G_NumChannels)
{
    print "WARNING: number of ports does not equal the number of channels"
}

#
# open all channels
#

$Count = 0
while ( $Count < $G_NumChannels)
{
    global
    $G_MesgCard = getwancardname
    $G_ClientOpen[$Count] = open $G_MesgCard $G_OpenFlag
    end_global

    if ($G_ClientOpen[$Count] == 0)
    {
      block_variation
      print "BLOCKED:  unable to open test card"
      goto endvar0
    }

    $G_ConnId[$Count] = connup "sync" $Count

    $Count = $Count + 1
}

sleep 30

#
# set packet filter to DIRECTED and send across some packets
# on all channels
#

$Count = 0
$PacketSize = 40
$NumPackets = 20
while($Count < $G_NumChannels)
{
    $CurFilter = DIRECTED
    $result = setpacketfilter $G_ClientOpen[$Count] $CurFilter
    if ($result == TEST_SUCCESSFUL)
    {
         $result = queryinfo $G_ClientOpen[$Count] OID_802_3_CURRENT_ADDRESS $G_TestAddress

         #
         # start receives, send packets, check send results, then check
         # receive results
         #
         $result = send $G_ClientOpen[$Count] $G_TestAddress $PacketSize $NumPackets
    }
    $Count = $Count + 1
}

$PacketsSent = 0
$Count = 0
while ($Count < $G_NumChannels)
{
    $result = waitsend $G_ClientOpen[$Count]

    if ($result != TEST_SUCCESSFUL)
    {
       fail_variation
       print "FAILURE:  waitsend failed"
    }
    else
    {
        $result = getsendresults $G_ClientOpen[$Count] $PacketsSent
        if ($result != TEST_SUCCESSFUL)
        {
            fail_variation
            print "FAILURE:  unable to get send results"
        }
        elseif ($PacketsSent != $NumPackets)
        {
            fail_variation
            print "FAILURE:  should have sent" $NumPackets "packets"
        }
    }

    $Count = $Count + 1
}


#
# Now close all opens
#

$Count = 0
while ( $Count < $G_NumChannels )
{
    conndown $G_ConnId[$Count]

    close $G_ClientOpen[$Count] 
    $Count = $Count + 1
}

#-------------------------------------------------------------------
# end of file ndiswan.tst
#-------------------------------------------------------------------


