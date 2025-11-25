#------------------------------------------
# File:  perf_fnc.tst
#
# performance tests with functionally addressed packets
#
#-------------------------------------------

friendly_script_name "Functional Performance Tests"

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


$SendAddr  = array uchar 6
$FuncAddr  = array uchar 4
$NullAddr  = array uchar 4
$Speed    = 0

$FuncAddr[0] = (uchar)0x01
$FuncAddr[1] = (uchar)0x02
$FuncAddr[2] = (uchar)0x03
$FuncAddr[3] = (uchar)0x04
$NullAddr[0] = (uchar)0x00
$NullAddr[1] = (uchar)0x00
$NullAddr[2] = (uchar)0x00
$NullAddr[3] = (uchar)0x00
$SendAddr[0] = (uchar)0xc0
$SendAddr[1] = (uchar)0x00
$SendAddr[2] = $FuncAddr[0]
$SendAddr[3] = $FuncAddr[1]
$SendAddr[4] = $FuncAddr[2]
$SendAddr[5] = $FuncAddr[3]


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
if ($G_Media == MEDIUM_TOKENRING)
{
   if ($Speed == 160000)   # 16MBit token ring
   {
      $NumMin = 160000
      $NumMed =  80000
      $NumMax =  16000
   }
   else                    # 4MBit token ring
   {
      $NumMin =  40000
      $NumMed =  20000
      $NumMax =   4000
   }
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

print "\nSend functional packets\n\n"

#
# send only performance test with functionally addressed packets
#
variation
performance $OpenInstance $SendAddr $SizeMin $NumMin 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

#
# now, do the functional receive tests
#
setpacketfilter $OpenInstance FUNCTIONAL
setfunctional $OpenInstance $FuncAddr
print "\nTest reception of functional packets\n"

#
# test 4 different speeds with tiny packets
#
$Delay = 0
while ($Delay < 14)
{
   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $SendAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $SendAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
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
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

setfunctional $OpenInstance $NullAddr

close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perf_fnc.tst
#-------------------------------------------------------------------



