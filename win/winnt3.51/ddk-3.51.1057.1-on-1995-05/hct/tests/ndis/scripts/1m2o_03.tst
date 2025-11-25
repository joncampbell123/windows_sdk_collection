#######################################################
# FILE:  1m2o_03.tst
#
# These tests verify the ability to independently set the
# packet filter from 3 separate open instances.  Note that
# only supported packet filters are tested
#######################################################

friendly_script_name "Multiple Open SetPacketFilter Tests"

print "TITLE:  Test ability to independently set packet filter from different open instances\n"

#------------------------------------------------------
# variation:  set up the filters list
#------------------------------------------------------

variation

$OpenOne = open $G_TestCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open card"
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

$OpenThree = open $G_TestCard $G_OpenFlag
if ($OpenThree == 0)
{
   block_variation
   print "BLOCKED:  unable to open card a third time"
   close $OpenOne
   close $OpenTwo
   goto endvar0
}


$Filters = array long 8

$Filters[0] = DIRECTED
$Filters[1] = BROADCAST
$NumFilters = 2
$MaxTests   = 4

if ($G_Filters & MULTICAST)
{
   $Filters[$NumFilters] = MULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & ALLMULTICAST)
{
   $Filters[$NumFilters] = ALLMULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & PROMISCUOUS)
{
   $Filters[$NumFilters] = PROMISCUOUS
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & FUNCTIONAL)
{
   $Filters[$NumFilters] = FUNCTIONAL
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & ALLFUNCTIONAL)
{
   $Filters[$NumFilters] = ALLFUNCTIONAL
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & GROUP)
{
   $Filters[$NumFilters] = GROUP
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & MACFRAME)
{
   $Filters[$NumFilters] = MACFRAME
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

if ($G_Filters & SOURCEROUTING)
{
   $Filters[$NumFilters] = SOURCEROUTING
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
}

#-------------------------------------------------
# variation:  set filter for two instances to different
#             possibly overlapping values.  make sure that
#             they are set correctly
#-------------------------------------------------

$Count = 0
$Curfilter = 0
while ($Count < $MaxTests)
{
   print " "
   variation

   #
   # first, set up the packet filters
   #
   $Filter1 = NONE
   $Filter2 = NONE
   $Filter3 = $G_Filters
   if ($Count & 0x01)
   {
      $Filter1 = $Filter1 + $Filters[0]
      $Filter2 = $Filter2 + $Filters[($NumFilters-1)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-1)]
   }
   if ($Count & 0x02)
   {
      $Filter1 = $Filter1 + $Filters[1]
      $Filter2 = $Filter2 + $Filters[($NumFilters-2)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-2)]
   }
   if ($Count & 0x04)
   {
      $Filter1 = $Filter1 + $Filters[2]
      $Filter2 = $Filter2 + $Filters[($NumFilters-3)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-3)]
   }
   if ($Count & 0x08)
   {
      $Filter1 = $Filter1 + $Filters[3]
      $Filter2 = $Filter2 + $Filters[($NumFilters-4)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-4)]
   }
   if ($Count & 0x10)
   {
      $Filter1 = $Filter1 + $Filters[4]
      $Filter2 = $Filter2 + $Filters[($NumFilters-5)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-5)]
   }
   if ($Count & 0x20)
   {
      $Filter1 = $Filter1 + $Filters[5]
      $Filter2 = $Filter2 + $Filters[($NumFilters-6)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-6)]
   }
   if ($Count & 0x40)
   {
      $Filter1 = $Filter1 + $Filters[6]
      $Filter2 = $Filter2 + $Filters[($NumFilters-7)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-7)]
   }
   if ($Count & 0x80)
   {
      $Filter1 = $Filter1 + $Filters[7]
      $Filter2 = $Filter2 + $Filters[($NumFilters-8)]
      $Filter3 = $Filter3 - $Filters[($NumFilters-8)]
   }

   $result = setpacketfilter $OpenOne $Filter1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set packet filter for open instance 1"
   }

   $result = setpacketfilter $OpenTwo $Filter2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set packet filter for open instance 2"
   }

   $result = setpacketfilter $OpenThree $Filter3
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set packet filter for open instance 3"
   }

   print " "
   variation
   #
   # now read them to make sure they are correct...
   #
   $result = queryinfo $OpenOne OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 1!"
   }
   elseif ($Filter1 != $Curfilter)
   {
      fail_variation
      print "FAILURE:  filter set to" $Curfilter ", should have been" $Filter1
   }

   $result = queryinfo $OpenTwo OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 2!"
   }
   elseif ($Filter2 != $Curfilter)
   {
      fail_variation
      print "FAILURE:  filter set to" $Curfilter ", should have been" $Filter2
   }

   $result = queryinfo $OpenThree OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 3!"
   }
   elseif ($Filter3 != $Curfilter)
   {
      fail_variation
      print "FAILURE:  filter set to" $Curfilter ", should have been" $Filter3
   }

   #
   # now set them all to NONE and check again
   #

   print " "
   variation

   $result = setpacketfilter $OpenOne NONE
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear packet filter for open instance 1"
   }

   $result = setpacketfilter $OpenTwo NONE
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear packet filter for open instance 2"
   }

   $result = setpacketfilter $OpenThree NONE
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear packet filter for open instance 3"
   }

   $result = queryinfo $OpenOne OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 1!"
   }
   elseif ($Curfilter != NONE)
   {
      fail_variation
      print "FAILURE:  Filter set to" $Curfilter ", should have been NONE"
   }

   $result = queryinfo $OpenTwo OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 2!"
   }
   elseif ($Curfilter != NONE)
   {
      fail_variation
      print "FAILURE:  filter set to" $Curfilter ", should have been NONE"
   }

   $result = queryinfo $OpenThree OID_GEN_CURRENT_PACKET_FILTER $Curfilter
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to read filter for open instance 3!"
   }
   elseif ($Curfilter != NONE)
   {
      fail_variation
      print "FAILURE:  filter set to" $Curfilter ", should have been NONE"
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
   print "WARNNIG:  unexpected events occurred."
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

print " "
variation
$result = getevents $OpenThree
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

$result = close $OpenThree
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m2o_03.tst
############################################################

