#----------------------------------------------
# FILE:  server.tst
#
# This script starts up the server which will participate in the
# "two machine two card" tests.......
#----------------------------------------------

#-------------------------------------------------
# variation:  open up card, prepare for tests...
#-------------------------------------------------

friendly_script_name "NdisTest Server"

$Media    = 0

variation

debuglevel 0
$ServerCard = "bogus"

$ServerCard = getcardname FALSE
if ($ServerCard == "nocard")
{
   print "Server ABORTED:  Unable to get trusted card name"
   print "Make sure the environment variable \"trustedcard\""
   print "is set properly, then try again."
   fail_variation
   goto endvar
}

#
# open the instance of card used to communicate with the client
#
:restart

$MessageOpen = open $ServerCard FALSE
if ($MessageOpen == 0)
{
   print "Server ABORTED:  Unable to open \"" $ServerCard "\""
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


#
# open the instance of card used to do the actual work
#
$WorkOpen = open $ServerCard FALSE
if ($WorkOpen == 0)
{
   print "Server ABORTED:  Unable to do second open of \"" $ServerCard "\""
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
   $MessageOpen = open $ServerCard TRUE
   if ($MessageOpen == 0)
   {
      print "Server ABORTED:  Unable to open \"" $ServerCard "\""
      print "Check to make sure name is correct, and that the driver is loaded"
      print "You may need to load the driver via control panel/devices"
      fail_variation
      goto endvar
   }

   #
   # open the instance of card used to do the actual work
   #
   $WorkOpen = open $ServerCard TRUE
   if ($WorkOpen == 0)
   {
      print "Server ABORTED:  Unable to do second open of \"" $ServerCard "\""
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


