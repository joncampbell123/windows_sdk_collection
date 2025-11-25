#------------------------------------------
# FILE:  getwantest.tst
#
# This script gets the name of the test card driver
# (Currently it gets this from the environment)
# It then tries to open this card.  If successful, get other information
# about this card that will be needed during the tests, and
# put into global variables.  If fail, quit with an appropriate
# message.  NOTE:  global/end_global pairs are essential so those
# particular global variables get set properly!
#------------------------------------------

variation
#
# first, get the name of the test card instance
#
global
$G_MesgCard = getwancardname
$G_TestCard = getwancardname
end_global

if ($G_MesgCard == "nocard")
{
   print "TEST ABORTED:  Unable to get card name.\n"
   print "Make sure the environment variable \"wancard1\"\n"
   print "is set properly, then try again.\n"
   quit
}

if ($G_TestCard == "nocard")
{
   print "TEST ABORTED:  Unable to get card name.\n"
   print "Make sure the environment variable \"wancard2\"\n"
   print "is set properly, then try again.\n"
   quit
}

end

#-------------------------------------------
# end of file getwantest.tst
#-------------------------------------------


