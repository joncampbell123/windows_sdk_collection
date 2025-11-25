Asynchronous IO Support

ISAPI 1.0 support synchronous IO operations via callback 
methods ReadClient() and WriteClient(). The ability to 
support asynchronous operations is important; it frees 
up a server pool thread from being blocked to complete 
the IO operation. In addition, IIS server engine already 
has built in support to manage asynchronous IO operations 
using the completion ports and server thread pool. 

ISAPI 2.0 supports asynchronous write operation using 
the existing callback function WriteClient() with a special 
flag indicating that the operation has to be performed 
asynchronously. In addition, ISAPI 2.0 also provides a 
mechanism to request the server to transmit a file using 
the TransmitFile() .
