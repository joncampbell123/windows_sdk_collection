#------------------------------------------
# FILE: gettrust.tst
#
# This script gets the name of the trusted card driver.
# (Currently it gets this from the environment.
# It then tries to open this card.  If successful, it then gets
# other information about this card that will be needed during
# the tests, and puts it into global variables.
# If it fails, it sets $G_TrustedTotalSize to 0
# and exit with an appropriate message.  The 1 machine 2 card tests
# will then be skipped.
# NOTE:  global/end_global pairs are essential so those
# particular global variables get set properly!
#------------------------------------------

friendly_script_name "Trying to Find Trusted Card"

variation

$Media = 0
$TotalSize = 0

#
# first, get the name of the trusted card instance
#
global
$G_TrustedCard = getcardname FALSE
end_global

if ($G_TrustedCard == "nocard")
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Unable to get trusted card name."
   print "Make sure the environment variable \"trustedcard\""
   print "is set properly, then try again.\n"
   warn_variation
   goto endvar
}

#
# next, try to open the card
#
$OpenInstance = open $G_TrustedCard $G_OpenFlag
if ($OpenInstance == 0)
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Unable to open \"" $G_TrustedCard "\""
   print "Check to make sure name is correct, and that the driver is loaded."
   print "You may need to load the driver via control panel/devices or net start.\n"
   warn_variation
   goto endvar
}

#
# now get the medium type in use by the card..
#
$result = queryinfo $OpenInstance OID_GEN_MEDIA_IN_USE $Media
if ($result != TEST_SUCCESSFUL)
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Unable to query media type from driver.\n"
   close $OpenInstance
   warn_variation
   goto endvar
}

if ($Media != $G_Media)
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Media types differ\n"
   close $OpenInstance
   warn_variation
   goto endvar
}

#
# get the maximum total size
#
$result = queryinfo $OpenInstance OID_GEN_MAXIMUM_TOTAL_SIZE $TotalSize
if ($result != TEST_SUCCESSFUL)
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Unable to query maximum total size from driver.\n"
   close $OpenInstance
   warn_variation
   goto endvar
}

#
# now get info that is dependent on medium type -- the card's address
#
if ($G_Media == MEDIUM_ETHERNET)
{
   $result = queryinfo $OpenInstance OID_802_3_CURRENT_ADDRESS $G_TrustedAddress
}
elseif ($G_Media == MEDIUM_TOKENRING)
{
   $result = queryinfo $OpenInstance OID_802_5_CURRENT_ADDRESS $G_TrustedAddress
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $result = queryinfo $OpenInstance OID_FDDI_LONG_CURRENT_ADDR $G_TrustedAddress
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $result = queryinfo $OpenInstance OID_ARCNET_CURRENT_ADDRESS $G_TrustedAddress
}
if ($result != TEST_SUCCESSFUL)
{
   print "\n\n1 Machine 2 Card Tests ABORTED:  Unable to query current address from driver.\n"
   close $OpenInstance
   warn_variation
   goto endvar
}

#
# at this point, check to see if the trusted card is able to talk to the
# test card. If this fails then either (1) the test card/driver is really bad
# (2) the trusted card/driver is really bad, or (3) the test card and the trusted card
# are not connected to each other (ie, not on the same network).  We are going to assume
# case 3, and thus will abort out if the test card is unable to receive packets sent by the
# trusted card...

$TestOpen = open $G_TestCard $G_OpenFlag
if ($TestOpen != 0)
{
   $result = setpacketfilter $TestOpen DIRECTED
   if ($result == TEST_SUCCESSFUL)
   {
      $result = startreceive $TestOpen
      if ($result == TEST_SUCCESSFUL)
      {
         $result = send $OpenInstance $G_TestAddress 64 10
         if ($result == TEST_SUCCESSFUL)
         {
            $result = waitsend $OpenInstance
            if ($result == TEST_SUCCESSFUL)
            {
               $PacketsSent = 0
               $result = getsendresults $OpenInstance $PacketsSent
               if (($result == TEST_SUCCESSFUL) && ($PacketsSent == 10))
               {
                  $result = stopreceive $TestOpen
                  if ($result == TEST_SUCCESSFUL)
                  {
                     $Received = 0
                     $Resent = 0
                     $Transferred = 0
                     $result = getreceiveresults $TestOpen $Received $Resent $Transferred
                     if (($result == TEST_SUCCESSFUL) && ($Received == 10))
                     {
#
# NOTE:  this is only successful completion!
#
                        close $TestOpen
                        close $OpenInstance

                        global
                        $G_TrustedTotalSize = $TotalSize
                        end_global
                        goto endvar
                     }
                  }
               }
            }
         }
      }
   }
   close $TestOpen
}
close $OpenInstance
print "\n\nTEST ABORTED:  \"" $G_TestCard "\" and \"" $G_TrustedCard "\" are unable to"
print "communicate.  Please check to make sure your cabling is correct--"
print "both cards must be connected to the same network.  Then retry the test."
print "With some cards, you may need to reboot the system first if cables were"
print "not connected properly.\n"
quit


:endvar

end

#-----------------------------------------
# end of file gettrust.tst
#-----------------------------------------


