Direct Music Readme
-------------------

This release of DirectX 6 contains the beta 1 release of DirectMusic. 
The DirectMusic DLLs are automatically installed when you run the SDK setup.
However, the DirectMusic DLLs are not included in the DirectX 6 redist.
If for some reason, you need to install the DirectMusic DLLs and do not want
to run the SDK setup, the redist setup for DirectMusic can be found here, 
in the extras\dmusic directory on the DirectX 6 SDK CD.  

The documentation for the DirectMusic API is included with the DirectX 6 
documentation. The DirectMusic sample code is also included as part of the 
DirectX 6 SDK.  Both of these options are available in the DX6 SDK setup.

DirectMusic Producer, the authoring tool for DirectMusic, is also included 
with this release.  It has a separate install which can be found in the 
extras\DMProducer directory on the DirectX 6 CD.  For more information 
on DirectMusic Producer, please see the readme.txt which can be found in the
same directory as the DirectMusic Producer setup program.

In this release, DirectMusic does not yet support reverberation.  Also, WDM 
support for DirectMusic is still under construction.

We are still working on improving the latency of the synthesizer under 
DirectSound.  Currently, the latency is around 100ms on systems with 
DirectSound drivers. We are working on making the latency scale to the 
driver implementation, so it will always be optimal.

Please remember that as this is a beta, it should not be distributed in any
form until the final is released.  These files will expire on 31 October, 1998.