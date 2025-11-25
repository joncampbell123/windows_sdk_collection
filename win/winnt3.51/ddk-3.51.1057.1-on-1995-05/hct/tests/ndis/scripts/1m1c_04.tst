###############################################################
# File 1m1c_04.tst
#
# This script tests the ability to set and query all the legal
# packetfilter combinations on an adapter
###############################################################


friendly_script_name "Single Open SetPacketFilter Tests"

print "TITLE:  Test for ability to set the packet filter\n"

#------------------------------------------------------
# variation:   set up the array of filters to test...
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}

$Filters = array long 10

$Filters[0] = DIRECTED
$Filters[1] = BROADCAST
if (($G_Media == MEDIUM_ETHERNET) || ($G_Media == MEDIUM_FDDI))
{
   $Filters[2] = MULTICAST
   $Filters[3] = ALLMULTICAST
   $Filters[4] = PROMISCUOUS
   $FilterTests = 32
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $Filters[2] = FUNCTIONAL
   $Filters[3] = GROUP
   $Filters[4] = MACFRAME
   $Filters[5] = SOURCEROUTING
   $Filters[6] = ALLFUNCTIONAL
   $Filters[7] = PROMISCUOUS
   $FilterTests = 256
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $Filters[2] = PROMISCUOUS
   $FilterTests = 8
}

#------------------------------------------------
# variation:   check that we can set all filter combinations
#              valid for our hardware/driver
#------------------------------------------------


$Count = 1
$FilterRet = 0
$FilterMask = (~$G_Filters) & 0x0000FFFF
$ExpectNone = FALSE

while ($Count < $FilterTests)
{
   print " "
   variation

   $CurFilter = NONE
   if ($Count & 0x01)
   {
      $CurFilter = $CurFilter + $Filters[0]
   }
   if ($Count & 0x02)
   {
      $CurFilter = $CurFilter + $Filters[1]
   }
   if ($Count & 0x04)
   {
      $CurFilter = $CurFilter + $Filters[2]
   }
   if ($Count & 0x08)
   {
      $CurFilter = $CurFilter + $Filters[3]
   }
   if ($Count & 0x10)
   {
      $CurFilter = $CurFilter + $Filters[4]
   }
   if ($Count & 0x20)
   {
      $CurFilter = $CurFilter + $Filters[5]
   }
   if ($Count & 0x40)
   {
      $CurFilter = $CurFilter + $Filters[6]
   }
   if ($Count & 0x80)
   {
      $CurFilter = $CurFilter + $Filters[7]
   }

   $result = setpacketfilter $OpenInstance $CurFilter

#
# first deal with expected failures (driver can't handle this filter)
#
   if (($FilterMask & $CurFilter) != 0x00000000)
   {
      $ExpectNone = TRUE
      if ($result == TEST_SUCCESSFUL)
      {
         warn_variation
         print "WARNING:  should not have been able to set this filter!"
      }
   }

#
# deal with expected success (driver should handle this filter)
#
   else
   {
      $ExpectNone = FALSE
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  should have been able to set this filter!"
      }
   }

#
# get what filter is actually set to..
#
   $result = queryinfo $OpenInstance OID_GEN_CURRENT_PACKET_FILTER $FilterRet
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter setting!"
   }
   else
   {
      if ($ExpectNone == TRUE)
      {
         if ($FilterRet != NONE)
         {
            fail_variation
            print "FAILURE:   Filter should be NONE, it was " $FilterRet
         }
      }
      else
      {
         if ($FilterRet != $CurFilter)
         {
            fail_variation
            print "FAILURE:  Filter set to " $FilterRet ", should have been " $CurFilter
         }
      }
   }

#
# now set the filter back to none, and make sure thats what it is..
#
   print " "
   variation

   $result = setpacketfilter $OpenInstance NONE
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  failed to set filter back to NONE!"
   }

   $result = queryinfo $OpenInstance OID_GEN_CURRENT_PACKET_FILTER $FilterRet
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter setting!"
   }
   elseif ($FilterRet != NONE)
   {
      fail_variation
      print "FAILURE:   Filter should be NONE, it was " $FilterRet
   }

   $Count = $Count + 1
}

#-------------------------------------------------
# cleanup
#-------------------------------------------------

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
# end of file 1m1c_04.tst
############################################################


