#------------------------------------------
# File:  perf_mlt.tst
#
# performance tests with multicast-addressed packets
#
#-------------------------------------------

friendly_script_name "Multicast Performance Tests"

variation

#---------------------------
# first, open up the test card...
#---------------------------


$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   goto perfend
}

#---------------------------
# set up some local variables we need
#---------------------------


$McastAddr = array uchar 6
$Speed    = 0

$McastAddr[0] = (uchar)0x01
$McastAddr[1] = (uchar)0x02
$McastAddr[2] = (uchar)0x03
$McastAddr[3] = (uchar)0x04
$McastAddr[4] = (uchar)0x05
$McastAddr[5] = (uchar)0x06

$result = queryinfo $OpenInstance OID_GEN_LINK_SPEED $Speed
if ($result != TEST_SUCCESSFUL)
{
   block_variation
   print "BLOCKED:  unable to get link speed"
   close $OpenInstance
   goto perfend
}

#
# set the lookahead to a standard value
#
$result = setlookahead $OpenInstance 32
if ($result != TEST_SUCCESSFUL)
{
   print "WARNING:  unable to set lookahead"
}

setpacketfilter $OpenInstance NONE

#
# set number of packets to send in each test
#


if ($G_Media == MEDIUM_ETHERNET)
{
   if ($Speed == 1000000)     # 100MBit ethernet
   {
      $NumMin = 2000000
      $NumMed =  500000
      $NumMax =  250000
   }
   elseif ($Speed == 100000)  # normal 10MBit ethernet
   {
      $NumMin = 200000
      $NumMed =  50000
      $NumMax =  25000
   }
   else                       # must be encapsulated ethernet
   {
      $NumMin =  40000
      $NumMed =  10000
      $NumMax =   5000
   }
}

elseif ($G_Media == MEDIUM_FDDI) # 100MBit
{
   $NumMin = 2000000
   $NumMed =  500000
   $NumMax =  250000
}

else
{
   print "TEST ABORTED:  Unrecognized media type.\n"
   close $OpenInstance
   goto perfend
}


#
# set size of packets to use
#
$SizeMin  = 64
$SizeMed  = 512
$SizeMax  = $G_MaxTotalSize
if ($SizeMed > $SizeMax)
{
   $SizeMed = ($SizeMin + $SizeMax) / 2
}

print "\nSend multicast packets\n\n"

#
# send only performance test with multicast packets
#
variation
performance $OpenInstance $McastAddr $SizeMin $NumMin 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $McastAddr $SizeMed $NumMed 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $McastAddr $SizeMax $NumMax 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

#
# now, do the multicast receive tests
#

addmulticast $OpenInstance $McastAddr
setpacketfilter $OpenInstance MULTICAST

#
# test 4 different speeds with tiny packets
#
$Delay = 0
while ($Delay < 14)
{
   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $McastAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $McastAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Delay = $Delay + 4
}

#
# just top speed with medium and big packets
#
$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $McastAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $McastAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $McastAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $McastAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

deletemulticast $OpenInstance $McastAddr

close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perf_mlt.tst
#-------------------------------------------------------------------



