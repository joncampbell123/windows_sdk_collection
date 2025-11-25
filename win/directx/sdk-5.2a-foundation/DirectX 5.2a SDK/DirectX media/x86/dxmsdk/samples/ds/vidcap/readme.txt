Vidcap, the VFW video capture filter sample, is not registered with the SDK
installation. To register it, run the command

  regsvr32 c:\dxmedia\bin\vidcap.ax

Vidcap.ax registers one entry per VFW capture device in the Video Capture
Sources category. See the function DllRegisterServer in vidcap.cpp for 
details.

