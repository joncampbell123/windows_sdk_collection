#define MAX_ITEM 10
#define MAX_SIZE 20

#define SUCCESS 1
#define FAILURE 0


#define MAXCALLS    20


// Synchronization primitives to shared buffer

HANDLE hMutex;		// Handle to mutex
HANDLE hEmptySem;	// Handle to empty semaphore
HANDLE hFullSem;		// Handle to full semaphore


// This is the shared buffer

char buffer[MAX_ITEM][MAX_SIZE];


// The current producer and consumer position in the buffer pool

int pro_buf_pos;
int con_buf_pos;
