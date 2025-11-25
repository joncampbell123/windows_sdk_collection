#----------------------------------------------
# FILE:  serverwan.tst
#
# This script starts up the server which will participate in the
# WAN tests.......
#
# Adapted in most part from server.tst
#----------------------------------------------

#-------------------------------------------------
# variation:  open up card, prepare for tests...
#-------------------------------------------------

$Media    = 0

variation

debuglevel 0
$MesgCard = "bogus"

$MesgCard = getwancardname
if ($MesgCard == "nocard")
{
   print "Server ABORTED:  Unable to get trusted card name"
   print "Make sure the environment variable \"trustedcard\""
   print "is set properly, then try again."
   fail_variation
   goto endvar
}

#
# enable wan connectivity
#
enablewan server

#
# open the instance of card used to communicate with the client
#
:restart

$MessageOpen = open $MesgCard FALSE
if ($MessageOpen == 0)
{
   print "Server ABORTED:  Unable to open \"" $MesgCard "\""
   print "Check to make sure name is correct, and that the driver is loaded"
   print "You may need to load the driver via control panel/devices"
   fail_variation
   goto endvar
}

#
# now get the medium type in use by the card..
#
$result = queryinfo $MessageOpen OID_GEN_MEDIA_IN_USE $Media
if ($result != TEST_SUCCESSFUL)
{
   print "TEST ABORTED:  Unable to query media type from driver.\n"
   close $MessageOpen
   quit
}

debuglevel  0

#
# open the instance of card used to do the actual work
#

$WorkCard = getwancardname
if ($WorkCard == "nocard")
{
   print "Server ABORTED:  Unable to get trusted card name"
   print "Make sure the environment variable \"trustedcard\""
   print "is set properly, then try again."
   fail_variation
   goto endvar
}

#
$WorkOpen = open $WorkCard FALSE
if ($WorkOpen == 0)
{
   print "Server ABORTED:  Unable to do second open of \"" $WorkCard "\""
   close $MessageOpen
   fail_variation
   goto endvar
}
$result = runserver $MessageOpen $WorkOpen
if ($result != TEST_SUCCESSFUL)
{
   print "Server ABORTED:  Unable to start server"
   fail_variation
}

close $MessageOpen
close $WorkOpen

if ($Media == MEDIUM_ARCNET)
{
   $MessageOpen = open $WorkCard TRUE
   if ($MessageOpen == 0)
   {
      print "Server ABORTED:  Unable to open \"" $WorkCard "\""
      print "Check to make sure name is correct, and that the driver is loaded"
      print "You may need to load the driver via control panel/devices"
      fail_variation
      goto endvar
   }

   #
   # open the instance of card used to do the actual work
   #
   $WorkOpen = open $WorkCard TRUE
   if ($WorkOpen == 0)
   {
      print "Server ABORTED:  Unable to do second open of \"" $WorkCard "\""
      close $MessageOpen
      fail_variation
      goto endvar
   }
   $result = runserver $MessageOpen $WorkOpen
   if ($result != TEST_SUCCESSFUL)
   {
      print "Server ABORTED:  Unable to start server"
      fail_variation
   }

   close $MessageOpen
   close $WorkOpen
}

goto restart


:endvar

end

#--------------------------------------
# end of file server.tst
#--------------------------------------


