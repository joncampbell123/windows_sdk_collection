#------------------------------------------
# FILE:  gettest.tst
#
# This script gets the name of the test card driver
# (Currently it gets this from the environment)
# It then tries to open this card.  If successful, get other information
# about this card that will be needed during the tests, and
# put into global variables.  If fail, quit with an appropriate
# message.  NOTE:  global/end_global pairs are essential so those
# particular global variables get set properly!
#------------------------------------------

friendly_script_name "Trying to Find Test Card"

variation
#
# first, get the name of the test card instance
#
global
$G_TestCard = getcardname TRUE
end_global

if ($G_TestCard == "nocard")
{
   print "\n\nTEST ABORTED:  Unable to get card name."
   print "Make sure the environment variable \"testcard\""
   print "is set properly, then try again.\n"
   quit
}

#
# next, try to open the card
#
$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   print "\n\nTEST ABORTED:  Unable to open \"" $G_TestCard "\""
   print "Check to make sure name is correct, and that the driver is loaded."
   print "You may need to load the driver via control panel/devices"
   print "or net start.\n"
   quit
}

#
# now get the medium type in use by the card..
#
$result = queryinfo $OpenInstance OID_GEN_MEDIA_IN_USE $G_Media
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unable to query media type from driver.\n"
   getevents $OpenInstance
   close $OpenInstance
   quit
}

#
# and get the maximum lookahead size
#
$result = queryinfo $OpenInstance OID_GEN_MAXIMUM_LOOKAHEAD $G_MaxLookahead
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unable to query maximum lookahead from driver.\n"
   getevents $OpenInstance
   close $OpenInstance
   quit
}

#
# and the maximum total size
#
$result = queryinfo $OpenInstance OID_GEN_MAXIMUM_TOTAL_SIZE $G_MaxTotalSize
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unable to query maximum total size from driver.\n"
   getevents $OpenInstance
   close $OpenInstance
   quit
}

#
# now get info that is dependent on medium type -- the card's address, and
# the filter types it supports
#
if ($G_Media == MEDIUM_ETHERNET)
{
   $result = queryinfo $OpenInstance OID_802_3_CURRENT_ADDRESS $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "\n\nTEST ABORTED:  Unable to query current address from driver.\n"
      getevents $OpenInstance
      close $OpenInstance
      quit
   }
   global
   $G_Filters = DIRECTED + MULTICAST + BROADCAST
   $G_OpenFlag = TRUE
   end_global

   $result = setpacketfilter $OpenInstance ALLMULTICAST
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLMULTICAST
      end_global
   }
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $result = queryinfo $OpenInstance OID_802_5_CURRENT_ADDRESS $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "\n\nTEST ABORTED:  Unable to query current address from driver.\n"
      getevents $OpenInstance
      close $OpenInstance
      quit
   }
   global
   $G_Filters = DIRECTED + FUNCTIONAL + GROUP + BROADCAST
   $G_OpenFlag = TRUE
   end_global

   $result = setpacketfilter $OpenInstance ALLFUNCTIONAL
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLFUNCTIONAL
      end_global
   }
   $result = setpacketfilter $OpenInstance SOURCEROUTING
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + SOURCEROUTING
      end_global
   }
   $result = setpacketfilter $OpenInstance MACFRAME
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + MACFRAME
      end_global
   }
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $result = queryinfo $OpenInstance OID_FDDI_LONG_CURRENT_ADDR $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "\n\nTEST ABORTED:  Unable to query current address from driver.\n"
      getevents $OpenInstance
      close $OpenInstance
      quit
   }
   global
   $G_Filters = DIRECTED + MULTICAST + BROADCAST
   $G_OpenFlag = TRUE
   end_global
   $result = setpacketfilter $OpenInstance ALLMULTICAST
   if ($result == TEST_SUCCESSFUL)
   {
      global
      $G_Filters = $G_Filters + ALLMULTICAST
      end_global
   }
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $result = queryinfo $OpenInstance OID_ARCNET_CURRENT_ADDRESS $G_TestAddress
   if ($result != TEST_SUCCESSFUL)
   {
      print "\n\nTEST ABORTED:  Unable to query current address from driver.\n"
      getevents $OpenInstance
      close $OpenInstance
      quit
   }
   global
   $G_Filters = DIRECTED + BROADCAST
   end_global
}
else
{
   print "\n\nTEST ABORTED:  Unrecognized media type.\n"
   getevents $OpenInstance
   close $OpenInstance
   quit
}

$result = setpacketfilter $OpenInstance PROMISCUOUS
if ($result == TEST_SUCCESSFUL)
{
   global
   $G_Filters = $G_Filters + PROMISCUOUS
   end_global
}
$result = setpacketfilter $OpenInstance NONE
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unable to reset filter type to NONE\n"
   getevents $OpenInstance
   close $OpenInstance
   quit
}
#
# finally, check for extraneous events, and close the card
#
$result = getevents $OpenInstance
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unexpected netcard events detected."
   print "This can occur if the test card is connected to a public"
   print "network, or if the test card, trusted card, or server card"
   print "has protocols bound to it.  If none of these is the case,"
   print "then comment out the close and quit commands as indicated in"
   print "scripts\\gettest.tst, and retry.\n"
#
# if necessary, comment out the "close" and "quit" commands
# on the following two lines
#
   close $OpenInstance
   quit
}

$result = close $OpenInstance
if ($result != TEST_SUCCESSFUL)
{
   print "\n\nTEST ABORTED:  Unable to close open instance\n"
   quit
}


end

#-------------------------------------------
# end of file gettest.tst
#-------------------------------------------


