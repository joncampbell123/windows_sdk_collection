#######################################################
# FILE:  1m2o_09c.tst
#
# These tests verify the ability to send group-addressed packets from
# one open instance and receive them on the other, with various
# packet filters set
#######################################################

friendly_script_name "Two Open Send/Receive Group Tests"

print "TITLE:  Test ability to receive group packets sent from other open instance\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$OpenOne = open $G_TestCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open card"
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
   print "BLOCKED:  unable to set packetfilter to NONE"
   close $OpenOne
   goto endvar0
}

$OpenTwo = open $G_TestCard $G_OpenFlag
if ($OpenTwo == 0)
{
   block_variation
   print "BLOCKED:  unable to open card a second time"
   close $OpenOne
   goto endvar0
}


$Filters = array long 8

$Filters[0] = DIRECTED
$Filters[1] = BROADCAST
$Filters[2] = GROUP
$NumFilters = 3
$MaxTests   = 8

if ($G_Filters & PROMISCUOUS)
{
   $Filters[$NumFilters] = PROMISCUOUS
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

# put all of these together (for speed)

$TempFilter = FUNCTIONAL
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
$MaxTests = $MaxTests * 2
$NumFilters = $NumFilters + 1


$SendAddress = array uchar 6
$GoodGroup   = array uchar 4
$BadGroup    = array uchar 4
$NullGroup   = array uchar 4

$SendAddress[0] = (uchar)0xc0
$SendAddress[1] = (uchar)0x00
$SendAddress[2] = (uchar)0x81
$SendAddress[3] = (uchar)0x02
$SendAddress[4] = (uchar)0x03
$SendAddress[5] = (uchar)0x04

$GoodGroup[0] = (uchar)0x81
$GoodGroup[1] = (uchar)0x02
$GoodGroup[2] = (uchar)0x03
$GoodGroup[3] = (uchar)0x04

$BadGroup[0] = (uchar)0x90
$BadGroup[1] = (uchar)0x20
$BadGroup[2] = (uchar)0x30
$BadGroup[3] = (uchar)0x40

$NullGroup[0] = (uchar)0x00
$NullGroup[1] = (uchar)0x00
$NullGroup[2] = (uchar)0x00
$NullGroup[3] = (uchar)0x00

$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 40

#--------------------------------------
#  variations: send group addressed packets with
#              the receive filter set to each of the
#              combinations allowed for this card/driver
#              This test is done with no group address set,
#              with the group address set == sent address, and
#              with the group address set != sent address
#--------------------------------------

print "\n***Loop thru the supported packet filters***\n"
$Count = 0
while ($Count < $MaxTests)
{
   print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"
   #
   # first, set up the packet filter
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
         #-----------------------------------
         # no group address set--open 2 should receive when PROMISCUOUS
         #-----------------------------------

         print "\n***No group address set***"
         variation

         $result = setgroup $OpenTwo $NullGroup
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear group address"
         }

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

            print " "
            variation

            #
            # the second open instance will receive packets if the filter is set right
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
         }

         #--------------------------------------------
         # correct group address set--open 2 should receive when GROUP, PROMISCUOUS
         #--------------------------------------------

         print "\n***Correct group address set***"
         variation

         $result = setgroup $OpenTwo $GoodGroup
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set group address"
         }

         $result = startreceive $OpenOne
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }
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
         # the first open instance should never receive anything
         #
         $result = stopreceive $OpenOne
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $OpenOne $Received $Resent $Transferred
            if ($result == TEST_FAILED)
            {
               fail_variation
            }
            elseif ($result == TEST_WARNED)
            {
               warn_variation
            }

            if ( ($Resent != 0) || ($Received != 0) || ($Transferred != 0) )
            {
               fail_variation
               print "FAILURE:  this open instance should not receive any packets"
            }
         }

         print " "
         variation

         #
         # the second open instance will receive packets if the filter is set right
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

            if ( ($CurFilter & (GROUP + PROMISCUOUS)) != 0)
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

         #--------------------------------------------------
         # wrong group address -- open 2 should receive when PROMISCUOUS
         #--------------------------------------------------

         print "\n***Incorrect group address set***"
         variation

         $result = setgroup $OpenTwo $BadGroup
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set group address"
         }

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
         # the second open instance will receive packets if the filter is set right
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

         if ($PacketSize == $G_MaxTotalSize)
         {
            break
         }
         elseif ($PacketSize == $MinPacketSize)
         {
            if ($CurFilter & GROUP)
            {
               $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
               if ( ($PacketSize & 0x0001) == 0)
               {
                  $PacketSize = $PacketSize + 1
               }
            }
            elseif ($CurFilter == PROMISCUOUS)
            {
               $PacketSize = $G_MaxTotalSize
            }
            else
            {
               break
            }
         }
         else
         {
            $PacketSize = $G_MaxTotalSize
         }
      }
   }
   else
   {
      block_variation
      print "BLOCKED:  failed to set packet filter"
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
# end of file 1m2o_09c.tst
############################################################

