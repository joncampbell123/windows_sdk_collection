#######################################################
# FILE:  1m2o_01.tst
#
# These tests verify the ability to query the information OIDs
# from two different open instances..
#######################################################

friendly_script_name "Two Open QueryInformation Tests"

print "TITLE:  Test querying of information OIDs from different open instances\n"

#------------------------------------------------------
# variation:  get the supported oids list
#------------------------------------------------------

variation

$OpenOne = open $G_TestCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}
$OpenTwo = open $G_TestCard $G_OpenFlag
if ($OpenTwo == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card a second time"
   close $OpenOne
   goto endvar0
}



$ListOne = array long 200
$ListTwo = array long 200

#
# get the supported list for both open instances.  The lists should be identical
# (same items in same order)
#

print "\n***Get supported OID lists and compare***\n"
variation

$ListOneLength = 0
$ListTwoLength = 0
$result = queryinfo $OpenOne OID_GEN_SUPPORTED_LIST $ListOne $ListOneLength
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to get list of supported OIDs for open instance 1."
   close $OpenOne
   close $OpenTwo
   goto endvar0
}

print " "
variation
$result = queryinfo $OpenTwo OID_GEN_SUPPORTED_LIST $ListTwo $ListTwoLength
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to get list of supported OIDs for open instance 2."
   close $OpenOne
   close $OpenTwo
   goto endvar0
}

#
# now compare the two lists..
#

print " "
variation
if ($ListOneLength != $ListTwoLength)
{
   fail_variation
   print "FAILURE:  OID lists are not the same length"
}
else
{
   $Count = 0
   while ($Count < $ListOneLength)
   {
      if ($ListOne[$Count] != $ListTwo[$Count])
      {
         fail_variation
         print "FAILURE:  OID lists are not identical"
         break
      }
      $Count = $Count + 1
   }
}

#--------------------------------------------------------------
# variation:  check to see that both open instances actually support all
#             of the OIDs that they claim to...
#--------------------------------------------------------------

print "\n***Check that listed OIDs really are supported***\n"
$Count = 0
while ($Count < $ListOneLength)
{
   if ($ListOne[$Count] != OID_GEN_PROTOCOL_OPTIONS)
   {
      print " "
      variation

      $result = queryinfo $OpenOne $ListOne[$Count]
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to query this OID"
      }

      $result = queryinfo $OpenTwo $ListTwo[$Count]
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to query this OID"
      }
   }
   $Count = $Count + 1
}


#----------------------------------------------------
# variation:   check of subset of the general required list to
#              verify that we get the same result with both queries
#----------------------------------------------------

#
# set an array containing all OIDs that are required to be supported by each media type
# AND have (U)LONG values.  These will later be checked to make sure queries from both
# open instances return the same value
#
$ListOfUlongs  = array long 20

$ListOfUlongs[0]  = OID_GEN_HARDWARE_STATUS
$ListOfUlongs[1]  = OID_GEN_MAXIMUM_LOOKAHEAD
$ListOfUlongs[2]  = OID_GEN_MAXIMUM_FRAME_SIZE
$ListOfUlongs[3]  = OID_GEN_LINK_SPEED
$ListOfUlongs[4]  = OID_GEN_TRANSMIT_BUFFER_SPACE
$ListOfUlongs[5]  = OID_GEN_RECEIVE_BUFFER_SPACE
$ListOfUlongs[6]  = OID_GEN_TRANSMIT_BLOCK_SIZE
$ListOfUlongs[7]  = OID_GEN_RECEIVE_BLOCK_SIZE
$ListOfUlongs[8]  = OID_GEN_VENDOR_ID
$ListOfUlongs[9]  = OID_GEN_CURRENT_PACKET_FILTER
$ListOfUlongs[10] = OID_GEN_CURRENT_LOOKAHEAD
$ListOfUlongs[11] = OID_GEN_MAXIMUM_TOTAL_SIZE
$ListOfUlongs[12] = OID_GEN_MAC_OPTIONS

if ($G_Media == MEDIUM_ETHERNET)
{
   $ListOfUlongs[13] = OID_802_3_MAXIMUM_LIST_SIZE
   $ListLength = 14
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $ListLength = 13
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $ListOfUlongs[13] = OID_FDDI_LONG_MAX_LIST_SIZE
   $ListOfUlongs[14] = OID_FDDI_SHORT_MAX_LIST_SIZE
   $ListLength = 15
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $ListLength = 13
}

print "\n***Check that both open instances return the same value for OIDs***\n"

$Count = 0
$ReturnOne = 0
$ReturnTwo = 0
while ($Count < $ListLength)
{
   print " "
   variation

   $result = queryinfo $OpenOne $ListOfUlongs[$Count] $ReturnOne
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query this OID"
   }

   $result = queryinfo $OpenTwo $ListOfUlongs[$Count] $ReturnTwo
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query this OID"
   }

   if ($ReturnOne != $ReturnTwo)
   {
      fail_variation
      print "FAILURE:  queryinfo returned different results from two open instances"
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
# end of file 1m2o_01.tst
############################################################

