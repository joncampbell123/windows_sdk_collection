DPCHAT ReadMe

This is a simple chat program to demonstrate usage of the DirectPlay APIs.

Note: MSVC may include older versions of the DirectX header files and
libraries.  This sample requires DirectX 6.  In order to avoid compile errors,
make sure the path to the DirectX 6 header files and libraries are listed
BEFORE the MSVC header files and libraries through the
Tools -> Options -> Directories menu.

In order to support multi-byte character sets like Kanji, the dialogs in the
rc file need to be changed to use the "SYSTEM" font and the rc file needs to
be recompiled.

Note: The DPLAUNCH or BELLHOP sample that is part of the DirectX 6 SDK can be
used to exercise the lobby functionality of DPCHAT.

dpchat.mak - Make file to build DPCHAT.

lobby.cpp - sample code for enabling a DirectPlay application to be launched 
            and connected to a session by a lobby.

dialog.cpp - sample code for establishing a session connection by asking
            the user to supply information and make choices

dpchat.cpp - the core chat application


