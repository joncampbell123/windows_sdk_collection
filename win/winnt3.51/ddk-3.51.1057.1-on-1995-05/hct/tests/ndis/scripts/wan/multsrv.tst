#----------------------------------------------
# File:  multsrv.tst
#
# This scripts drives the server side of tests for multiple lines (channels).
# It is assumed that the maximum channels possible
# on the miniport is 2. If more channels are supported,
# change the variable $G_NumChannels to the actual
# number. For example, if 23 channels are supported, change
# this variable to 23. Also, change the dimensions of
# the arrays: $G_ClientOpen and $G_ServerOpen
#
# Make sure that the wancard# environ vars are set ( as in mwsrv.bat)
#
# Script globals are defined here.  Whenever they are set
# as the result of a command, they must be enclosed in the
# "global" "end_global" pair, or else the script might create
# a local variable with the same name.
#
# Adapted in most part from ndiswan.tst
#-------------------------------------------

debuglevel 1

global

$G_OpenFlag = FALSE
#--------------------------------------
# card names
#--------------------------------------
$G_MesgCard = "bogus"
$G_TestCard = "bogus"

#--------------------------------------
# this global variable is the array of open instances used for sending
# commands to the server
#--------------------------------------
$G_NumChannels = 2
$G_ClientOpen = array ulong 2

end_global

#
# enable wan connectivity
#
enablewan multserver


#
# open all channels and start servers on all
#

$Count = 0
while ( $Count < $G_NumChannels)
{
    global
    $G_TestCard = getwancardname
    $G_ClientOpen[$Count] = open $G_TestCard $G_OpenFlag
    end_global

    if ($G_ClientOpen[$Count] == 0)
    {
      block_variation
      print "BLOCKED:  unable to open test card"
      goto endvar0
    }
    $Count = $Count + 1
}

sleep 30
#
# now enable the receives on all the opens
#
$Count = 0
while ($Count < $G_NumChannels)
{

   $CurFilter = DIRECTED
   $result = setpacketfilter $G_ClientOpen[$Count] $CurFilter

   $result = startreceive $G_ClientOpen[$Count]
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  receive failed to start"
   }
    $Count = $Count + 1
}

#
# sleep for a while - 80 secs
#
sleep 80
$NumPackets = 20

#
# stop receives and see if decent results
#

$Received = 0
$Resent = 0
$Transferred = 0
$Count = 0
while ($Count < $G_NumChannels)
{
   $result = stopreceive $G_ClientOpen[$Count]
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  stopreceive failure"
   }
   else
   {
      $result = getreceiveresults $G_ClientOpen[$Count] $Received $Resent $Transferred
      if ($result == TEST_FAILED)
      {
         fail_variation
      }
      elseif ($result == TEST_WARNED)
      {
         warn_variation
      }
      elseif ($Received != $NumPackets)
      {
        fail_variation
        print "FAILURE:  should have received" $NumPackets "packets"
      }
   }
    $Count = $Count + 1
}

#
# close all opens
#
$Count = 0
while ( $Count < $G_NumChannels )
{
    close $G_ClientOpen[$Count]
    $Count = $Count + 1
}

:endvar

end

#--------------------------------------
# end of file server.tst
#--------------------------------------


