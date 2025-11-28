This is an example of a Pool Manager using Custom OLE Servers. The idea here is that you (client) can send jobs to this Pool Manager (VFP Custom OLE Server) which might be remote and have it handle the Job Processing. This allows you to continue working because the pool manager is handling all the work. The cool thing here is that you the client does not need to have some sort of timer checking to see if a job is done because the Pool Manager makes a call back into you. The Jobs are dummy simulation jobs.

Note: there is a chart job option which requires Excel.

To Use:

1. You need to make sure you build the POOL.EXE file from the Project so that it Registers as Custom OLE Servers.

2. Run the POOL form.