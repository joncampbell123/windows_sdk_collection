Sample: OvComm

PURPOSE:    To demonstrate overlapped serial 
            communication in multiple threads.


MORE INFO:

    To Use:
       Start the application from the command line.  The application will
       use COM1 9600,N,8,1 if the command line doesn't specify 
       otherwise.  If desired, the comm port and serial specification 
       can be supplied on the command line.
       The application uses the console to display received data.  
       Keyboard input from the console's standard input handle
       is used to get data to send to the comm port.

    What Happens:
       The program utilizes 4 threads.  A main thread initializes the comm
       port and console, creates 3 worker threads and waits for the threads
       to terminate.  Each worker thread controls a single facet of 
       serial communications.

       A Status Thread is created to monitor status events.
       When an event occurs, the event is communicated to the user via 
       OutputDebugString.

       A Reader Thread is created to read the comm port and display the
       received data on the console output.
       
       A Writer Thread is created to accept keyboard input and send it
       out the comm port.

       Control-Break causes program termination.

   Caution:
       This sample utilizes multiple threads only for the sake of simplicity.
       Each thread does one logical task and that is all.  A more efficient
       method for this program would be to have a single thread perform
       all i/o requests and then wait on multiple handles for each operation
       to complete.  Creating multiple threads just for the sake of simplicity
       in a full blown application may not be best implementation.
