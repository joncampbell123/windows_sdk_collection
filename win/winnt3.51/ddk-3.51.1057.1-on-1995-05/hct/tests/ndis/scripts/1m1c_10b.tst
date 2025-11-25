###############################################################
# File 1m1c_10b.tst
#
# This script tests the ability to receive(and send) functional
# packets with various receive filters set..
###############################################################


friendly_script_name "Single Open Send/Receive Functional Tests"

print "TITLE:  Test ability to receive functional packets with various filters set\n"

#------------------------------------------------------
# variation:   set up for the tests
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}

$Filters = array long 8

$Filters[0] = DIRECTED
$Filters[1] = BROADCAST
$Filters[2] = FUNCTIONAL
$NumFilters = 3
$MaxTests   = 8


if ($G_Filters & PROMISCUOUS)
{
   $Filters[$NumFilters] = PROMISCUOUS
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & ALLFUNCTIONAL)
{
   $Filters[$NumFilters] = ALLFUNCTIONAL
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}


# merging group/sourcerouting/macframe filters for this test (for speed purposes)

$TempFilter = GROUP
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


$SendAddress    = array uchar 6
$GoodFunctional = array uchar 4
$BadFunctional  = array uchar 4
$NullFunctional = array uchar 4

$SendAddress[0] = (uchar)0xc0
$SendAddress[1] = (uchar)0x00
$SendAddress[2] = (uchar)0x01
$SendAddress[3] = (uchar)0x02
$SendAddress[4] = (uchar)0x03
$SendAddress[5] = (uchar)0x04

$GoodFunctional[0] = (uchar)0x01
$GoodFunctional[1] = (uchar)0x02
$GoodFunctional[2] = (uchar)0x03
$GoodFunctional[3] = (uchar)0x04

$BadFunctional[0] = (uchar)0x10
$BadFunctional[1] = (uchar)0x20
$BadFunctional[2] = (uchar)0x30
$BadFunctional[3] = (uchar)0x40

$NullFunctional[0] = (uchar)0x00
$NullFunctional[1] = (uchar)0x00
$NullFunctional[2] = (uchar)0x00
$NullFunctional[3] = (uchar)0x00

$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$MinPacketSize = 40
$NumPackets = $G_PacketsToSend

#--------------------------------------
#  variations: send functional addressed packets with
#              the receive filter set to each of the
#              combinations allowed for this card/driver
#              This test is done with no functional address set,
#              with the functional address set == sent address, and
#              with the functional address set != sent address
#--------------------------------------

print "\n***Loop thru all supported packet filters***\n"

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
   if ($Count & 0x20)
   {
      $CurFilter = $CurFilter | $Filters[5]
   }

   $result = setpacketfilter $OpenInstance $CurFilter
   if ($result == TEST_SUCCESSFUL)
   {
      $PacketSize = $MinPacketSize
      while (TRUE)
      {
         #-----------------------------------
         # no functional address set--PROMISCUOUS, ALLFUNCTIONAL should receive
         #-----------------------------------

         print "\n***No functional address set***"
         variation

         $result = setfunctional $OpenInstance $NullFunctional
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear functional address"
         }

         $result = startreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenInstance $SendAddress $PacketSize $NumPackets
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  send failed to start"
         }
         else
         {
            $result = waitsend $OpenInstance
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  waitsend failed"
            }
            else
            {
               $result = getsendresults $OpenInstance $PacketsSent
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

            $result = stopreceive $OpenInstance
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  stopreceive failure"
            }
            else
            {
               $result = getreceiveresults $OpenInstance $Received $Resent $Transferred
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
               if ( ($CurFilter & (ALLFUNCTIONAL + PROMISCUOUS)) != 0)
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
         # correct functional address set--receive FUNCTIONAL, PROMISCUOUS, ALLFUNCTIONAL
         #--------------------------------------------

         print "\n***Correct functional address set***"
         variation

         $result = setfunctional $OpenInstance $GoodFunctional
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set functional address"
         }

         $result = startreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenInstance $SendAddress $PacketSize $NumPackets
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  send failed to start"
         }
         else
         {
            $result = waitsend $OpenInstance
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  waitsend failed"
            }
            else
            {
               $result = getsendresults $OpenInstance $PacketsSent
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

         $result = stopreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $OpenInstance $Received $Resent $Transferred
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

            if ( ($CurFilter & (FUNCTIONAL + ALLFUNCTIONAL + PROMISCUOUS)) != 0)
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
         # wrong functional address -- receive PROMISCUOUS and ALLFUNCTIONAL
         #--------------------------------------------------

         print "\n***Incorrect functional address set***"
         variation

         $result = setfunctional $OpenInstance $BadFunctional
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set functional address"
         }

         $result = startreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenInstance $SendAddress $PacketSize $NumPackets
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  send failed to start"
         }
         else
         {
            $result = waitsend $OpenInstance
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  waitsend failed"
            }
            else
            {
               $result = getsendresults $OpenInstance $PacketsSent
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

         $result = stopreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $OpenInstance $Received $Resent $Transferred
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

            if ( ($CurFilter & (ALLFUNCTIONAL + PROMISCUOUS)) != 0)
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
            if ($CurFilter & FUNCTIONAL)
            {
               $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
               if ( ($PacketSize & 0x0001) == 0)
               {
                  $PacketSize = $PacketSize + 1
               }
            }
            elseif (($CurFilter == PROMISCUOUS) || ($CurFilter == ALLFUNCTIONAL))
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



#----------------------------------------------------
# cleanup
#----------------------------------------------------

print " "
variation
$result = getevents $OpenInstance
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

$result = close $OpenInstance
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m1c_10b.tst
############################################################


