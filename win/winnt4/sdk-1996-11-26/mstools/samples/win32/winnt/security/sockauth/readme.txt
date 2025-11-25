
SockAuth

Some of the APIs used in this sample are not Win32 APIs.  They are
specific APIs for Security Providers.

Documentation on sspi.h can be found in \mstools\help\spk.mvb --
use \mstools\help\infoview.exe spk.mvb to view.

This sample is distantly related to the httpauth sample which
demonstrates the use of the SSPI package to implement the client side
of NTLM authentication when interacting with IIS.  However, this sample
shows both the client and server conversations with the security
provider when just a socket connection is used.

The server side, in this sample, authenticates the client and provides
evidence of this by reporting the client's user name back to the client
via the socket connection.
