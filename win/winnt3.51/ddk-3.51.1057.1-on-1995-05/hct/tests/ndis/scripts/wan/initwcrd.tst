#------------------------------------------
# FILE:  initwancard.tst
#
# This script gets the name of the test card driver
# (Currently it gets this from the environment)
# It then tries to open this card.  If successful, get other information
# about this card that will be needed during the tests, and
# put into global variables.  If fail, quit with an appropriate
# message.  NOTE:  global/end_global pairs are essential so those
# particular global variables get set properly!
#------------------------------------------

#
# now get the medium type in use by the card..
#
$result = queryinfo $G_ServerOpen OID_GEN_MEDIA_IN_USE $G_Media
if ($result != TEST_SUCCESSFUL)
{
   print "TEST ABORTED:  Unable to query media type from driver.\n"
   getevents $G_ServerOpen
   close $G_ServerOpen
   quit
}

#
# and get the maximum lookahead size
#
#       $result = queryinfo $G_ServerOpen OID_GEN_MAXIMUM_LOOKAHEAD $G_MaxLookahead
#       if ($result != TEST_SUCCESSFUL)
#       {
#          print "TEST ABORTED:  Unable to query maximum lookahead from driver.\n"
#          getevents $G_ServerOpen
#          close $G_ServerOpen
#          quit
#       }

#
# and the maximum total size
#
$result = queryinfo $G_ServerOpen OID_GEN_MAXIMUM_TOTAL_SIZE $G_MaxTotalSize
if ($result != TEST_SUCCESSFUL)
{
   print "TEST ABORTED:  Unable to query maximum total size from driver.\n"
   getevents $G_ServerOpen
   close $G_ServerOpen
   quit
}

#
# now get info that is dependent on medium type -- the card's address, and
# the filter types it supports
#
if ($G_Media == MEDIUM_ETHERNET)
{
   $result = queryinfo $G_ServerOpen OID_802_3_CURRENT_ADDRESS $G_TestAddress
  #if ($result != TEST_SUCCESSFUL)
  #{
  #   print "TEST ABORTED:  Unable to query current address from driver.\n"
  #   getevents $G_ServerOpen
  #   close $G_ServerOpen
  #   quit
  #}
   global
   $G_Filters = DIRECTED + MULTICAST + BROADCAST
   $G_OpenFlag = TRUE
   end_global

   $result = setpacketfilter $G_ServerOpen ALLMULTICAST
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLMULTICAST
      end_global
   }
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $result = queryinfo $G_ServerOpen OID_802_5_CURRENT_ADDRESS $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "TEST ABORTED:  Unable to query current address from driver.\n"
      getevents $G_ServerOpen
      close $G_ServerOpen
      quit
   }
   global
   $G_Filters = DIRECTED + FUNCTIONAL + GROUP + BROADCAST
   $G_OpenFlag = TRUE
   end_global

   $result = setpacketfilter $G_ServerOpen ALLFUNCTIONAL
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLFUNCTIONAL
      end_global
   }
   $result = setpacketfilter $G_ServerOpen SOURCEROUTING
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + SOURCEROUTING
      end_global
   }
   $result = setpacketfilter $G_ServerOpen MACFRAME
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + MACFRAME
      end_global
   }
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $result = queryinfo $G_ServerOpen OID_FDDI_LONG_CURRENT_ADDR $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "TEST ABORTED:  Unable to query current address from driver.\n"
      getevents $G_ServerOpen
      close $G_ServerOpen
      quit
   }
   global
   $G_Filters = DIRECTED + MULTICAST + BROADCAST
   $G_OpenFlag = TRUE
   end_global
   $result = setpacketfilter $G_ServerOpen ALLMULTICAST
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLMULTICAST
      end_global
   }
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $result = queryinfo $G_ServerOpen OID_ARCNET_CURRENT_ADDRESS $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "TEST ABORTED:  Unable to query current address from driver.\n"
      getevents $G_ServerOpen
      close $G_ServerOpen
      quit
   }
   global
   $G_Filters = DIRECTED + BROADCAST
   end_global
}
else
{
   print "TEST ABORTED:  Unrecognized media type.\n"
   getevents $G_ServerOpen
   close $G_ServerOpen
   quit
}

##      $result = setpacketfilter $G_ServerOpen PROMISCUOUS
##      if ($result == TEST_SUCCESSFUL)
##      {
##         global
##         $G_Filters = $G_Filters + PROMISCUOUS
##         end_global
##      }
$result = setpacketfilter $G_ServerOpen NONE
if ($result != TEST_SUCCESSFUL)
{
   print "TEST ABORTED:  Unable to reset filter type to NONE\n"
   getevents $G_ServerOpen
   close $G_ServerOpen
   quit
}
#
# finally, check for extraneous events, and close the card
#
$result = getevents $G_ServerOpen
if ($result != TEST_SUCCESSFUL)
{
   print "TEST ABORTED:  Unexpected netcard events detected.\n"
   close $G_ServerOpen
   quit
}


end

#-------------------------------------------
# end of file initwancard.tst
#-------------------------------------------


