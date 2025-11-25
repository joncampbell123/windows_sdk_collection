#######################################################
# FILE:  str_2c3.tst
#
# One machine, two card stress tests
#######################################################

friendly_script_name "Two Card Stress Tests with Test Card as Client/Server"

print "TITLE:  2 card stress tests: test card is client/server, trusted card is server\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

#
# Open "client" card
#
$OpenOne = open $G_TestCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   goto endvar0
}

#
# Open "server" card
#
$OpenTwo = open $G_TrustedCard $G_OpenFlag
if ($OpenTwo == 0)
{
   close $OpenOne
   block_variation
   print "BLOCKED:  unable to open trusted card"
   goto endvar0
}


$MaximumPacketSize = $G_MaxTotalSize
if ($G_TrustedTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_TrustedTotalSize
}

#
# set up variables for all the tests..
#
$PacketCount = array long 10
$Iterations  = array long 10
$PacketSize  = array long 10
$OptionMask  = array long 10

$PacketCount[0] = 10000
$Iterations[0]  = -1
$PacketSize[0]  = $MaximumPacketSize
$OptionMask[0]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_ZEROS | STRESS_ACK | STRESS_FIXEDDELAY
$OptionMask[0]  = $OptionMask[0] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[1] = 10000
$Iterations[1]  = -1
$PacketSize[1]  = $MaximumPacketSize
$OptionMask[1]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_ONES | STRESS_ACK | STRESS_FIXEDDELAY
$OptionMask[1]  = $OptionMask[1] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[2] = 10000
$Iterations[2]  = -1
$PacketSize[2]  = 256
$OptionMask[2]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_SMALL | STRESS_ACK | STRESS_FIXEDDELAY
$OptionMask[2]  = $OptionMask[2] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[3] = -1
$Iterations[3]  = 1
$PacketSize[3]  = $MaximumPacketSize  # should it be 512?
$OptionMask[3]  = STRESS_BOTH | STRESS_CYCLICAL | STRESS_KNOWN | STRESS_ACK | STRESS_FIXEDDELAY
$OptionMask[3]  = $OptionMask[3] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[4] = -1
$Iterations[4]  = 10
$PacketSize[4]  = $MaximumPacketSize #should it be 512?
$OptionMask[4]  = STRESS_BOTH | STRESS_CYCLICAL | STRESS_RAND | STRESS_FULLRESP | STRESS_FIXEDDELAY
$OptionMask[4]  = $OptionMask[4] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[5] = 10000
$Iterations[5]  = -1
$PacketSize[5]  = 100
$OptionMask[5]  = STRESS_BOTH | STRESS_FIXEDSIZE | STRESS_SMALL | STRESS_ACK | STRESS_FIXEDDELAY
$OptionMask[5]  = $OptionMask[5] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_ON

$PacketCount[6] = 10000
$Iterations[6]  = -1
$PacketSize[6]  = 256
$OptionMask[6]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_SMALL | STRESS_ACK10 | STRESS_FIXEDDELAY
$OptionMask[6]  = $OptionMask[6] | STRESS_WINDOWING_ON | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_OFF

$PacketCount[7] = 10000
$Iterations[7]  = -1
$PacketSize[7]  = $MaximumPacketSize
$OptionMask[7]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_RAND | STRESS_NORESP | STRESS_FIXEDDELAY
$OptionMask[7]  = $OptionMask[7] | STRESS_WINDOWING_OFF | STRESS_DATACHECKING_OFF | STRESS_PACKETSFROMPOOL_ON

$PacketCount[8] = 10000
$Iterations[8]  = -1
$PacketSize[8]  = $MaximumPacketSize
$OptionMask[8]  = STRESS_BOTH | STRESS_RANDOMSIZE | STRESS_RAND | STRESS_FULLRESP | STRESS_FIXEDDELAY
$OptionMask[8]  = $OptionMask[8] | STRESS_WINDOWING_OFF | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_ON

$PacketCount[9] = 10000
$Iterations[9]  = -1
$PacketSize[9]  = 60
$OptionMask[9]  = STRESS_BOTH | STRESS_FIXEDSIZE | STRESS_RAND | STRESS_FULLRESP | STRESS_FIXEDDELAY
$OptionMask[9]  = $OptionMask[9] | STRESS_WINDOWING_OFF | STRESS_DATACHECKING_ON | STRESS_PACKETSFROMPOOL_ON


#
# Now run the tests..
#
$Count = 0
while ($Count < 10)
{
   print " "
   variation

   $result = startstressserver $OpenTwo
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to start the stress server"
   }

   $result = startstress $OpenOne $PacketCount[$Count] $Iterations[$Count] $PacketSize[$Count] 0 $OptionMask[$Count]
   if ($result == TEST_SUCCESSFUL)
   {
      $result = waitstress $OpenOne
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  failure in waitstress"
      }
      $result = getstressresults $OpenOne
      if ($result == TEST_BLOCKED)
      {
         block_variation
      }
      elseif ($result == TEST_WARNED)
      {
         warn_variation
      }
      elseif ($result == TEST_FAILED)
      {
         fail_variation
      }
   }
   else
   {
      fail_variation
      print "FAILURE:  unable to start stress test"
   }

   $result = stopstress $OpenTwo
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to stop stress server"
   }

   $result = getevents $OpenOne
   if ($result == TEST_FAILED)
   {
      fail_variation
      print "FAILURE:  unexpected events occurred."
   }
   elseif ($result == TEST_WARNED)
   {
      warn_variation
      print "WARNING:  unexpected events occurred."
   }

   $result = getevents $OpenOne
   if ($result == TEST_FAILED)
   {
      fail_variation
      print "FAILURE:  unexpected events occurred."
   }
   elseif ($result == TEST_WARNED)
   {
      warn_variation
      print "WARNING:  unexpected events occurred."
   }

   $Count = $Count + 1
}

#---------------------------------------------
# cleanup
#---------------------------------------------

print " "
variation

$result = close $OpenOne
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

$result = close $OpenTwo
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file str_2c3.tst
############################################################

