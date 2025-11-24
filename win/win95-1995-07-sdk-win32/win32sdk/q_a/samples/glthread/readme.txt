GLThread

    When implementing multiple threads in any type of application, it is
important to have adequate communication between threads. In OpenGL, it is
very important for two threads to know what the other thread is doing. For
example, it is common to clear the display window before drawing an OpenGL
scene. If both threads are called to draw portions of a scene and they both
try to call glClear before drawing, one thread's object may get erased by
another thread's call to glClear.

    The GLTHREAD sample will assign the glClear function to only one 
thread, and will ensure that the other thread does not perform and drawing
until glClear has been called. When a menu command message is sent to the
main window, the application will call CreateThread twice to create two
threads. Each thread will call GetDC(hwndMain) to obtain its own device
context to the main window. Then, each thread will call GLTHREAD's
bSetupPixelFormat function to set up the pixel format and finally call
wglCreateContext to create a new OpenGL Rendering Context. Now, each
thread has its own Rendering Context and both can call wglMakeCurrent to
make its new OpenGL rendering context its (the calling thread's) current
rendering context. All subsequent OpenGL calls made by the thread are drawn
on the device identified by the HDC returned from each thread's call to
GetDC(). Since only one thread should call glClear, GLTHREAD has thread
number one call it. The second thread is created "suspended" so it does
nothing until a call to ResumeThread is made. After thread one has called
glClear, it will enable thread two to resume by calling ResumeThread with a
handle to the second thread. The procedure in the main thread that created
the two other threads will wait until both threads are finished before
returning from the processing of the menu command message that is sent when
the user selects the "Draw Waves" menu selection from the "Test Threads"
menu. It will use the WaitForMultipleObjects function to do this.
