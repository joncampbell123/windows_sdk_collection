#######################################################
# FILE:  2m2c_02.tst
#
# These tests verify the ability to receive broadcast packets sent
# from a card on another machine, with various
# packet filters set
#######################################################

friendly_script_name "Two Machine Send/Receive Broadcast Tests"

print "TITLE:  Test ability to receive broadcast packets sent by another card\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$ClientOpen = open $G_TestCard $G_OpenFlag
if ($ClientOpen == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   goto endvar0
}


$MaximumPacketSize = 0
queryinfo $G_ServerOpen OID_GEN_MAXIMUM_TOTAL_SIZE $MaximumPacketSize
if ($G_MaxTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_MaxTotalSize
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
   $Filters[$NumFilters] = GROUP
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 4

   $TempFilter = NONE
   if ($G_Filters & ALLFUNCTIONAL)
   {
      $TempFilter = ALLFUNCTIONAL
   }
   if ($G_Filters & MACFRAME)
   {
      $TempFilter = $TempFilter | MACFRAME
   }
   if ($G_Filters & SOURCEROUTING)
   {
      $TempFilter = $TempFilter | SOURCEROUTING
   }
   if ($TempFilter != NONE)
   {
      $Filters[$NumFilters] = $TempFilter
      $NumFilters = $NumFilters + 1
      $MaxTests = $MaxTests * 2
   }
}

$SendAddress = array uchar 6
if ($G_Media == MEDIUM_TOKENRING)
{
   $SendAddress[0] = (uchar)0xc0
   $SendAddress[1] = (uchar)0x00
   $SendAddress[2] = (uchar)0xff
   $SendAddress[3] = (uchar)0xff
   $SendAddress[4] = (uchar)0xff
   $SendAddress[5] = (uchar)0xff
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $SendAddress[0] = (uchar)0x00
   $SendAddress[1] = (uchar)0x00
   $SendAddress[2] = (uchar)0x00
   $SendAddress[3] = (uchar)0x00
   $SendAddress[4] = (uchar)0x00
   $SendAddress[5] = (uchar)0x00
}
else              # MEDIUM_ETHERNET or MEDIUM_FDDI
{
   $SendAddress[0] = (uchar)0xff
   $SendAddress[1] = (uchar)0xff
   $SendAddress[2] = (uchar)0xff
   $SendAddress[3] = (uchar)0xff
   $SendAddress[4] = (uchar)0xff
   $SendAddress[5] = (uchar)0xff
}

$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 40

#-------------------------------------------------
# variations:  send multiple sizes of broadcast packets with the
#              receive filter set to each of the combinations
#              legal for this card/driver
#-------------------------------------------------

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

   $result = setpacketfilter $ClientOpen $CurFilter
   if ($result == TEST_SUCCESSFUL)
   {
      $PacketSize = $MinPacketSize
      while (TRUE)
      {
         print " "
         variation

         #
         # start receives, do send, get send results, get receive results
         #
         $result = startreceive $ClientOpen
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $G_ServerOpen $SendAddress $PacketSize $NumPackets
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  send failed to start"
         }
         else
         {
            $result = waitsend $G_ServerOpen
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  waitsend failed"
            }
            else
            {
               $result = getsendresults $G_ServerOpen $PacketsSent
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

         $result = stopreceive $ClientOpen
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $ClientOpen $Received $Resent $Transferred
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
            if ( ($CurFilter & (BROADCAST + PROMISCUOUS)) != 0)
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
         # set up packet size for next test
         #
         if ($PacketSize == $MaximumPacketSize)
         {
            break
         }
         elseif ($PacketSize == $MinPacketSize)
         {
            if (($CurFilter & BROADCAST) || ($CurFilter == PROMISCUOUS))
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
$result = getevents $G_ServerOpen
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

print " "
variation
$result = getevents $ClientOpen
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

$result = close $ClientOpen
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 2m2c_02.tst
############################################################

