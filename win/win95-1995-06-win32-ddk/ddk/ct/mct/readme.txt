Use the WinDDK forum on Compuserv for technical support
and release information for the latest version of the
modem hct.

Important notes for mhct 2.8:
  The forced ec test and the speed adjust test change the state
of the slave machine.  The slave must be reset after running
either one of these tests.  To reset the slave choose Test, Slave
to turn slave mode off, then choose Test, Slave to turn slave
mode back on.  Failure to do this will invalidate tests run after
forced ec or speed adjust.
  The tone/pulse test requires an analog line that can support
both tone and pulse dialing.
  The blind dial test is timing dependent.  Press ok on phone
cord removal dialog at the same time the phone cord is removed
from the modem.

Important notes on new .inf file changes since build 122:
1) In the All section, change HKR,, DevLoader,, VCOMM
			   to HKR,, DevLoader,, *vcomm
2) In the All section, add: HKR,, PortSubClass, 1, 02
3) Change all occurances of serial.386 to serial.vxd
4) Remove all occurances of HKR,, Settings, Originate,, D
5) Remove all occurances of HKR,, PortName,, COM5
6) Don't use these characters in model names in the Strings section:
   <>:/\|
7) In the model AddReg section (e.g. Modem1.AddReg), add:
   HKR,, Contention,, *vcd
8) In the manufacturer section, use quotes around device IDs which
   include a comma.
9) Use %% instead of % winthin registry strings.  Likewise, use
   a double-quote when a single quote is required within a string.
10) Be sure to include E0 in your Init strings to suppress command
   echo. (Unimodem does not need it)
11) Be sure to include V0 in your Init strins.  List all numeric
   response codes supported by the modem in the Responses section.
   Use the last dialog of the MODEM.DOT WinWord template to generate
   these.  Note that if your modem can do better negotiation progress
   reporting with verbose result codes, you can use these instead.
