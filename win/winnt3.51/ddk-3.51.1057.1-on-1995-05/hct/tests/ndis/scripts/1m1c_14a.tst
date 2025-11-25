###############################################################
# File 1m1c_14a.tst
#
# This script tests the ability to send, receive, and resend multicast
# packets with various receive filters set..
###############################################################


friendly_script_name "Single Open Send/Receive/Resend Multicast Tests"

print "TITLE:  Test send, receive, and resend with multicast packets\n"

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

if ($G_Filters & ALLMULTICAST)
{
   $Filters[$NumFilters] = ALLMULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}



$SendAddress = array uchar 6
$SendAddress[0] = (uchar)0x01
$SendAddress[1] = (uchar)0x02
$SendAddress[2] = (uchar)0x03
$SendAddress[3] = (uchar)0x04
$SendAddress[4] = (uchar)0x05
$SendAddress[5] = (uchar)0x06



$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$MinPacketSize = 64
$NumPackets = $G_PacketsToSend
$NumReceives = $NumPackets * 2

$result = addmulticast $OpenInstance $SendAddress
if ($result != TEST_SUCCESSFUL)
{
   block_variation
   print "BLOCKED:  failed to set multicast address"
   close $OpenInstance
   goto endvar0
}

#------------------------------------------------
# variation:   send multicast packets of various sizes with
#              the packet filter set to each of the legal
#              settings for this card/driver
#------------------------------------------------

$Count = 0
print "\n***Loop thru supported packet filters***\n"

while ($Count < $MaxTests)
{
   print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

   #
   # first set the packet filter
   #
   $CurFilter = MULTICAST
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

   $result = setpacketfilter $OpenInstance $CurFilter
   if ($result == TEST_SUCCESSFUL)
   {
      $PacketSize = $MinPacketSize
      while (TRUE)
      {
         print " "
         variation

         #
         # start receive, send packets, check send results, then check
         # receive results
         #
         $result = startreceive $OpenInstance
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         $result = send $OpenInstance $SendAddress $PacketSize $NumPackets $SendAddress
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

            if ($Resent != $NumPackets)
            {
               fail_variation
               print "FAILURE:  should have resent" $NumPackets "packets"
            }
            if ($Received != $NumReceives)
            {
               fail_variation
               print "FAILURE:  should have received" $NumReceives "packets"
            }
            if ($Transferred != $NumReceives)
            {
               fail_variation
               print "FAILURE:  should have transferred" $NumReceives "packets"
            }
         }

         if ($PacketSize == $G_MaxTotalSize)
         {
            break
         }
         elseif (($PacketSize == $MinPacketSize) && ($CurFilter & $Filters[0]))
         {
            $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
            if ( ($PacketSize & 0x0001) == 0)
            {
               $PacketSize = $PacketSize + 1
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


$result = deletemulticast $OpenInstance $SendAddress
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  failed to set multicast address"
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
# end of file 1m1c_14a.tst
############################################################


