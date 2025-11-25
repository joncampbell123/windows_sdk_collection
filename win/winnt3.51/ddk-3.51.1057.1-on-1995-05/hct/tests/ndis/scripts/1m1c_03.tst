#######################################################
# FILE:  1m1c_03.tst
#
# These tests verifies the ability to query the statistics OIDs.
#######################################################

#------------------------------------------------------
# variation:  get the supported oids list
#------------------------------------------------------

friendly_script_name "Single Open QueryStatistics Tests"

print "TITLE:  Tests to check querying of the statistics OIDs\n"

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}

#
# set up an array containing OIDs that are required to be supported
# for the current media type.  This will be compared with the results
# of the OID_GEN_SUPPORTED_LIST stats query
#
$ExpectedList  = array long 50
$SupportedList = array long 200


$ExpectedList[0]  = OID_GEN_SUPPORTED_LIST
$ExpectedList[1]  = OID_GEN_HARDWARE_STATUS
$ExpectedList[2]  = OID_GEN_MEDIA_SUPPORTED
$ExpectedList[3]  = OID_GEN_MEDIA_IN_USE
$ExpectedList[4]  = OID_GEN_MAXIMUM_LOOKAHEAD
$ExpectedList[5]  = OID_GEN_MAXIMUM_FRAME_SIZE
$ExpectedList[6]  = OID_GEN_LINK_SPEED
$ExpectedList[7]  = OID_GEN_TRANSMIT_BUFFER_SPACE
$ExpectedList[8]  = OID_GEN_RECEIVE_BUFFER_SPACE
$ExpectedList[9]  = OID_GEN_TRANSMIT_BLOCK_SIZE
$ExpectedList[10] = OID_GEN_RECEIVE_BLOCK_SIZE
$ExpectedList[11] = OID_GEN_VENDOR_ID
$ExpectedList[12] = OID_GEN_VENDOR_DESCRIPTION
$ExpectedList[13] = OID_GEN_CURRENT_PACKET_FILTER
$ExpectedList[14] = OID_GEN_CURRENT_LOOKAHEAD
$ExpectedList[15] = OID_GEN_DRIVER_VERSION
$ExpectedList[16] = OID_GEN_MAXIMUM_TOTAL_SIZE
$ExpectedList[17] = OID_GEN_MAC_OPTIONS
$ExpectedList[18] = OID_GEN_XMIT_OK
$ExpectedList[19] = OID_GEN_RCV_OK
$ExpectedList[20] = OID_GEN_XMIT_ERROR
$ExpectedList[21] = OID_GEN_RCV_ERROR
$ExpectedList[22] = OID_GEN_RCV_NO_BUFFER

if ($G_Media == MEDIUM_ETHERNET)
{
   $ExpectedList[23] = OID_802_3_PERMANENT_ADDRESS
   $ExpectedList[24] = OID_802_3_CURRENT_ADDRESS
   $ExpectedList[25] = OID_802_3_MULTICAST_LIST
   $ExpectedList[26] = OID_802_3_MAXIMUM_LIST_SIZE
   $ExpectedList[27] = OID_802_3_RCV_ERROR_ALIGNMENT
   $ExpectedList[28] = OID_802_3_XMIT_ONE_COLLISION
   $ExpectedList[29] = OID_802_3_XMIT_MORE_COLLISIONS
   $ExpectedListLength = 30
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $ExpectedList[23] = OID_802_5_PERMANENT_ADDRESS
   $ExpectedList[24] = OID_802_5_CURRENT_ADDRESS
   $ExpectedList[25] = OID_802_5_CURRENT_FUNCTIONAL
   $ExpectedList[26] = OID_802_5_CURRENT_GROUP
   $ExpectedList[27] = OID_802_5_LAST_OPEN_STATUS
   $ExpectedList[28] = OID_802_5_CURRENT_RING_STATUS
   $ExpectedList[29] = OID_802_5_CURRENT_RING_STATE
   $ExpectedList[30] = OID_802_5_LINE_ERRORS
   $ExpectedList[31] = OID_802_5_LOST_FRAMES
   $ExpectedListLength = 32
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $ExpectedList[23] = OID_FDDI_LONG_PERMANENT_ADDR
   $ExpectedList[24] = OID_FDDI_LONG_CURRENT_ADDR
   $ExpectedList[25] = OID_FDDI_LONG_MULTICAST_LIST
   $ExpectedList[26] = OID_FDDI_LONG_MAX_LIST_SIZE
   $ExpectedList[27] = OID_FDDI_SHORT_PERMANENT_ADDR
   $ExpectedList[28] = OID_FDDI_SHORT_CURRENT_ADDR
   $ExpectedList[29] = OID_FDDI_SHORT_MULTICAST_LIST
   $ExpectedList[30] = OID_FDDI_SHORT_MAX_LIST_SIZE
   $ExpectedList[31] = OID_FDDI_ATTACHMENT_TYPE
   $ExpectedList[32] = OID_FDDI_UPSTREAM_NODE_LONG
   $ExpectedList[33] = OID_FDDI_DOWNSTREAM_NODE_LONG
   $ExpectedList[34] = OID_FDDI_FRAME_ERRORS
   $ExpectedList[35] = OID_FDDI_FRAMES_LOST
   $ExpectedList[36] = OID_FDDI_RING_MGT_STATE
   $ExpectedList[37] = OID_FDDI_LCT_FAILURES
   $ExpectedList[38] = OID_FDDI_LEM_REJECTS
   $ExpectedList[39] = OID_FDDI_LCONNECTION_STATE
   $ExpectedListLength = 40
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $ExpectedList[23] = OID_ARCNET_PERMANENT_ADDRESS
   $ExpectedList[24] = OID_ARCNET_CURRENT_ADDRESS
   $ExpectedList[25] = OID_ARCNET_RECONFIGURATIONS
   $ExpectedListLength = 26
}

print "\n***Get the supported list and verify all required OIDs are supported***\n"
#
# now actually get the supported list
#
$SupportedListLength = 0
$result = querystats $OpenInstance OID_GEN_SUPPORTED_LIST $SupportedList $SupportedListLength
if ($result != 0)
{
   fail_variation
   print "FAILURE:  Unable to get list of supported OIDs."
   getevents $OpenInstance
   close $OpenInstance
   goto endvar0
}

#
# and compare with our "expected" list, to make sure all the required ones are there..
#
$ExpCount = 0
while ($ExpCount < $ExpectedListLength)
{
   $SupCount = 0
   $Found = FALSE
   while ($SupCount < $SupportedListLength)
   {
      if ($ExpectedList[$ExpCount] == $SupportedList[$SupCount])
      {
         $Found = TRUE
         break
      }
      $SupCount = $SupCount + 1
   }
   if ($Found == FALSE)
   {
      fail_variation
      print "FAILURE:  required oid " $ExpectedList[$ExpCount] " not found in list."
   }
   $ExpCount = $ExpCount + 1
}

#--------------------------------------------------------------
# variation:   check to see that we actually support all of the OIDs
#              that we claim we do...
#--------------------------------------------------------------

print "\n***Test that OIDs listed as supported really are supported***\n"

$SupCount = 0
while ($SupCount < $SupportedListLength)
{
   print " "
   variation

   $result = querystats $OpenInstance $SupportedList[$SupCount]
   if ($result != TEST_SUCCESSFUL)
   {
      if ($SupportedList[$SupCount] == OID_GEN_PROTOCOL_OPTIONS)
      {
         print "INFO:  OID_GEN_PROTOCOL_OPTIONS is a set-only OID."
      }
      else
      {
         fail_variation
         print "FAILURE:  unable to query this OID"
      }
   }

   $SupCount = $SupCount + 1
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
# end of file 1m1c_03.tst
############################################################

