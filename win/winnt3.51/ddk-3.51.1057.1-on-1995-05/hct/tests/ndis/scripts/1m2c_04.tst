#######################################################
# FILE:  1m2c_04.tst
#
# These tests verify the ability to receive "random"-addressed
# packets send from a second card on the same machine, with various
# packet filters set
#######################################################

friendly_script_name "Two Card Send/Receive Random Tests"

print "TITLE:  Test ability to receive undirected packets from another card on same machine\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$OpenOne = open $G_TrustedCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open trusted card"
   goto endvar0
}
#
# first open instance (which does sending) has packet filter set to NONE
# it should never receive any packets
#
$result = setpacketfilter $OpenOne NONE
if ($result != TEST_SUCCESSFUL)
{
   block_variation
   print "BLOCKED:  unable to set trusted card packetfilter to NONE"
   close $OpenOne
   goto endvar0
}


$OpenTwo = open $G_TestCard $G_OpenFlag
if ($OpenTwo == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   close $OpenOne
   goto endvar0
}

$MaximumPacketSize = $G_MaxTotalSize
if ($G_TrustedTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_TrustedTotalSize
}

$Filters = array long 8

$Filters[0] = DIRECTED
$Filters[1] = BROADCAST
$NumFilters = 2
$MaxTests   = 4

if ($G_Filters & PROMISCUOUS)
{
   $Filters[$NumFilters] = PROMISCUOUS
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & MULTICAST)
{
   $Filters[$NumFilters] = MULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
   if ($G_Filters & ALLMULTICAST)
   {
      $Filters[$NumFilters] = ALLMULTICAST
      $NumFilters = $NumFilters + 1
      $MaxTests = $MaxTests * 2
   }
}

elseif ($G_Filters & FUNCTIONAL)
{
   $Filters[$NumFilters] = FUNCTIONAL
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2

   $TempFilter = GROUP
   if ($G_Filters & ALLFUNCTIONAL)
   {
      $TempFilter = $TempFilter | ALLFUNCTIONAL
   }
   if ($G_Filters & MACFRAME)
   {
      $TempFilter = $TempFilter | MACFRAME
   }
   if ($G_Filters & SOURCEROUTING)
   {
      $TempFilter = $TempFilter | SOURCEROUTING
   }
   $Filters[$NumFilters] = $TempFilter
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

$SendAddress = array uchar 6
$SendAddress[0] = (uchar)0x00
$SendAddress[1] = (uchar)0x02
$SendAddress[2] = (uchar)0x04
$SendAddress[3] = (uchar)0x06
$SendAddress[4] = (uchar)0x08
$SendAddress[5] = (uchar)0x0a


$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 40

#--------------------------------------------
#  variations: send packets of various sizes to a "random"
#              address with packet filter set to each of the
#              combinations allowed for this card/driver
#--------------------------------------------

print "\n***Loop thru the supported packet filters***\n"

$Count = 0
while ($Count < $MaxTests)
{
   print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

   #
   # first, set up the packet filters..
   #
   $CurFilter = NONE
   if ($Count & 0x01)
   {
      $CurFilter = $CurFilter | $Filters[0]
   }
   if ($Count & 0x02)
   {
      $CurFilter = $CurFilter | $Filters[1]
   }
   if ($Count & 0x04)
   {
      $CurFilter = $CurFilter | $Filters[2]
   }
   if ($Count & 0x08)
   {
      $CurFilter = $CurFilter | $Filters[3]
   }
   if ($Count & 0x10)
   {
      $CurFilter = $CurFilter | $Filters[4]
   }

   $result = setpacketfilter $OpenTwo $CurFilter
   if ($result == TEST_SUCCESSFUL)
   {
      $PacketSize = $MinPacketSize
      while (TRUE)
      {
         print " "
         variation

         #
         # start receives, send packets, get send data, get receive data
         #
         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenOne $SendAddress $PacketSize $NumPackets
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  send failed to start"
         }
         else
         {
            $result = waitsend $OpenOne
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  waitsend failed"
            }
            else
            {
               $result = getsendresults $OpenOne $PacketsSent
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
         }

         print " "
         variation

         #
         # the test card will receive packets if the filter is set right
         #

         $result = stopreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $OpenTwo $Received $Resent $Transferred
            if ($result == TEST_FAILED)
            {
               fail_variation
            }
            elseif ($result == TEST_WARNED)
            {
               warn_variation
            }

            if ($Resent != 0)
            {
               fail_variation
               print "FAILURE:  no packets should have been resent"
            }
            if ( ($CurFilter & PROMISCUOUS) != 0)
            {
               if ($Received != $NumPackets)
               {
                  fail_variation
                  print "FAILURE:  should have received" $NumPackets "packets"
               }
               if ($Transferred != $NumPackets)
               {
                  fail_variation
                  print "FAILURE:  should have transferred" $NumPackets "packets"
               }
            }
            else
            {
               if ($Received != 0)
               {
                  fail_variation
                  print "FAILURE:  no packets should have been received!"
               }
               if ($Transferred != 0)
               {
                  fail_variation
                  print "FAILURE:  no packets should have been transferred!"
               }
            }
         }
         if ($PacketSize == $MaximumPacketSize)
         {
            break
         }
         elseif ($PacketSize == $MinPacketSize)
         {
            if ($CurFilter & PROMISCUOUS)
            {
               $PacketSize = ($MaximumPacketSize + $MinPacketSize) / 2
               if ( ($PacketSize & 0x0001) == 0)
               {
                  $PacketSize = $PacketSize + 1
               }
            }
            else
            {
               break
            }
         }
         else
         {
            $PacketSize = $MaximumPacketSize
         }
      }
   }
   else
   {
      block_variation
      print "BLOCKED:  unable to set packet filter"
   }

   $Count = $Count + 1
}



#---------------------------------------------
# cleanup
#---------------------------------------------

print " "
variation
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

$result = close $OpenOne
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

print " "
variation
$result = getevents $OpenTwo
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

$result = close $OpenTwo
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m2c_04.tst
############################################################

