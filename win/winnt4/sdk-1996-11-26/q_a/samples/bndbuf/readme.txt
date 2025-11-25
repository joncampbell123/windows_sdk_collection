Sample: Distributed Bounded Buffer Solution (DBBS)



Summary:

The DBBS sample demonstrates the distributed version of the classical
Operating Systems producer-consumer problem. A centralized buffer
pool managed by the RPC server is used by the producers and consumers.
Counting semaphores are used to make sure that comsumptions take place
when there is at least one unconsumed string in the buffer pool and
productions take place when there is at least one empty slot in the
buffer pool. Synchronization to the shared buffer pool is coordinated
by means of a mutex.



More Information:

To use this program, the RPC locator service must be first started
using the following command line:

net start rpclocator

Next, start the application server by typing:

start bndbufs

Multiple clients could then be started by typing:

start bndbufc

for each client to be started.

The application uses while loops to run forever. Therefore, you need
to use ctrl-C to terminate each component.
