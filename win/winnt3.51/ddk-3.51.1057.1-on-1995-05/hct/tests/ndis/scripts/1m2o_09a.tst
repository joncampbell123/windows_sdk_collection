#######################################################
# FILE:  1m2o_09a.tst
#
# These tests verify the ability to send multicast-addressed packets from
# one open instance and receive them on the other, with various
# packet filters set
#######################################################

friendly_script_name "Two Open Send/Receive Multicast Tests"

print "TITLE:  Test ability to receive multicast packets sent from other open instance\n"

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
$NumFilters = 2
$MaxTests   = 4

if ($G_Filters & PROMISCUOUS)
{
   $Filters[$NumFilters] = PROMISCUOUS
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

$Filters[$NumFilters] = MULTICAST
$NumFilters = $NumFilters + 1
$MaxTests = $MaxTests * 2

if ($G_Filters & ALLMULTICAST)
{
   $Filters[$NumFilters] = ALLMULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}



$GoodMulticast = array uchar 6
$BadMulticast  = array uchar 6
$GoodMulticast[0] = (uchar)0x01
$GoodMulticast[1] = (uchar)0x02
$GoodMulticast[2] = (uchar)0x03
$GoodMulticast[3] = (uchar)0x04
$GoodMulticast[4] = (uchar)0x05
$GoodMulticast[5] = (uchar)0x06

$BadMulticast[0] = (uchar)0x01
$BadMulticast[1] = (uchar)0x02
$BadMulticast[2] = (uchar)0x03
$BadMulticast[3] = (uchar)0x04
$BadMulticast[4] = (uchar)0x05
$BadMulticast[5] = (uchar)0x00

$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 40

#--------------------------------------
#  variations: send multicast addressed packets with
#              the receive filter set to each of the
#              combinations allowed for this card/driver
#              This test is done with no multicast address set,
#              with the multicast address set == sent address, and
#              with the multicast address set != sent address
#--------------------------------------

print "\n***Loop thru supported packet filters***\n"

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
         # no multicast address set--open 2 should receive when PROMISCUOUS, ALLMULTICAST
         #-----------------------------------

         print "\n***No multicast address set***"
         variation

         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenOne $GoodMulticast $PacketSize $NumPackets
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
               if ( ($CurFilter & (ALLMULTICAST + PROMISCUOUS)) != 0)
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
         # correct multicast address set--open 2 should receive MULTICAST, PROMISCUOUS, ALLMULTICAST
         #--------------------------------------------

         print "\n***Correct multicast address set***"
         variation

         $result = addmulticast $OpenTwo $GoodMulticast
         if ($result != TEST_SUCCESSFUL)
         {
            block_variation
            print "BLOCKED:  unable to set multicast address"
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

         $result = send $OpenOne $GoodMulticast $PacketSize $NumPackets
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

            if ( ($CurFilter & (MULTICAST + ALLMULTICAST + PROMISCUOUS)) != 0)
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
         # wrong multicast address -- open 2 should receive PROMISCUOUS and ALLMULTICAST
         #--------------------------------------------------

         print "\n***Incorrect multicast address set***"
         variation

         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenOne $BadMulticast $PacketSize $NumPackets
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

            if ( ($CurFilter & (ALLMULTICAST + PROMISCUOUS)) != 0)
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

         #
         # prepare for next test--remove the multicast address and
         # reset the packet size (and filter if necessary)
         #
         $result = deletemulticast $OpenTwo $GoodMulticast
         if ($result != TEST_SUCCESSFUL)
         {
            block_variation
            print "BLOCKED:  unable to delete multicast address"
         }

         if ($PacketSize == $G_MaxTotalSize)
         {
            break
         }

         elseif ($PacketSize == $MinPacketSize)
         {
            if ($CurFilter & MULTICAST)
            {
               $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
               if ( ($PacketSize & 0x0001) == 0)
               {
                  $PacketSize = $PacketSize + 1
               }
            }
            elseif (($CurFilter == PROMISCUOUS) || ($CurFilter == ALLMULTICAST))
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
# end of file 1m2o_09a.tst
############################################################

