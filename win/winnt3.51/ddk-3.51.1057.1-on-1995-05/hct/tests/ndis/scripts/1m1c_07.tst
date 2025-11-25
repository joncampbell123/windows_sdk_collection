###############################################################
# File 1m1c_07.tst
#
# This script tests the ability to send packets
###############################################################

friendly_script_name "Single Open Simple Send Tests"

print "TITLE:  Test ability to send packets\n"

#------------------------------------------------------
# variation:   set up things for the test...
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}

#---------------------------------------------------
# variation -- (pass 0) send to a random ("directed") address
#              (pass 1) send to the broadcast address
#              (pass 2) send to functional or multicast address
#              (pass 3) send to group address
#---------------------------------------------------

$DestAddr = array uchar 6
$DestAddr[0] = (uchar)0x00
$DestAddr[1] = (uchar)0x02
$DestAddr[2] = (uchar)0x04
$DestAddr[3] = (uchar)0x06
$DestAddr[4] = (uchar)0x08
$DestAddr[5] = (uchar)0x0a

$MinPacketSize = 40
$PacketsSent = 0
$NumPackets  = $G_PacketsToSend

$Pass = 0
print "\n***Sending to a (random) directed address***"

$PacketSize  = $MinPacketSize

while (TRUE)
{
   while (TRUE)
   {
      print " "
      variation

      $result = send $OpenInstance $DestAddr $PacketSize $NumPackets
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
               print "FAILURE:  should have sent " $NumPackets "packets"
            }
         }
      }

      if ($PacketSize == $G_MaxTotalSize)
      {
         break
      }
      elseif ($PacketSize == $MinPacketSize)
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

   #
   # set up for next test..
   #
   $PacketSize = $MinPacketSize
   if ($Pass == 0)         # just finished random directed address
   {                       # set up for sending to broadcast address
      $Pass = 1
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0xff
         $DestAddr[3] = (uchar)0xff
         $DestAddr[4] = (uchar)0xff
         $DestAddr[5] = (uchar)0xff
      }
      elseif ($G_Media == MEDIUM_ARCNET)
      {
         $DestAddr[0] = (uchar)0x00
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x00
         $DestAddr[3] = (uchar)0x00
         $DestAddr[4] = (uchar)0x00
         $DestAddr[5] = (uchar)0x00
      }
      else                 # MEDIUM_ETHERNET or MEDIUM_FDDI
      {
         $DestAddr[0] = (uchar)0xff
         $DestAddr[1] = (uchar)0xff
         $DestAddr[2] = (uchar)0xff
         $DestAddr[3] = (uchar)0xff
         $DestAddr[4] = (uchar)0xff
         $DestAddr[5] = (uchar)0xff
      }
      print "\n***Sending to a broadcast address***"
   }

   elseif ($Pass == 1)     # just finished broadcast address
   {                       # set up for sending to functional or multicast address
      $Pass = 2
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x01
         $DestAddr[3] = (uchar)0x02
         $DestAddr[4] = (uchar)0x03
         $DestAddr[5] = (uchar)0x04
         print "\n***Sending to a functional address***"
      }
      elseif ($G_Media == MEDIUM_ARCNET)
      {
         break
      }
      else                 # MEDIUM_ETHERNET or MEDIUM_FDDI
      {
         $DestAddr[0] = (uchar)0x01
         $DestAddr[1] = (uchar)0x02
         $DestAddr[2] = (uchar)0x03
         $DestAddr[3] = (uchar)0x04
         $DestAddr[4] = (uchar)0x05
         $DestAddr[5] = (uchar)0x06
         print "\n***Sending to a multicast address***"
      }

   }

   elseif ($Pass == 2)     # just finished multicast or functional address
   {                       # set up for sending to group address
      $Pass = 3
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x81
         $DestAddr[3] = (uchar)0x02
         $DestAddr[4] = (uchar)0x03
         $DestAddr[5] = (uchar)0x04
         print "\n***Sending to a group address***"
      }
      else
      {
         break
      }
   }

   else                    # just finished group address
   {                       # we are all done!
      break
   }
}


#---------------------------------------------------
# cleanup
#---------------------------------------------------

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
# end of file 1m1c_07.tst
############################################################


