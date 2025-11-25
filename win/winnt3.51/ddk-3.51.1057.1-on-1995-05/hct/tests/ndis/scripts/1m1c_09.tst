###############################################################
# File 1m1c_09.tst
#
# This script tests the ability to receive(and send) broadcast
# packets with various receive filters set..
###############################################################


friendly_script_name "Single Open Send/Receive Broadcast Tests"

print "TITLE:  Test ability to receive broadcast packets with various filters set\n"

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
$MinPacketSize = 40
$NumPackets = $G_PacketsToSend

#-------------------------------------------------
# variations:  send multiple sizes of broadcast packets with the
#              receive filter set to each of the combinations
#              legal for this card/driver
#-------------------------------------------------

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
         print " "
         variation

         #
         # start receive, do send, get send results, get receive results
         #
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
         if ($PacketSize == $G_MaxTotalSize)
         {
            break
         }
         elseif ($PacketSize == $MinPacketSize)
         {
            if ($CurFilter & BROADCAST)
            {
               $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
               if ( ($PacketSize & 0x0001) == 0)
               {
                  $PacketSize = $PacketSize + 1
               }
            }
            elseif ($CurFilter & PROMISCUOUS)
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
# end of file 1m1c_09.tst
############################################################


