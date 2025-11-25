# include       <macos\msvcmac.h>
# include       <Swap.h>

/* This is the outline of a typical GrowZone Proc. for a swappable
   application.  If the App. registers a GZ Proc., the Proc. is
   responsible for calling CbFreeSwapMem().  GZ Procs. must be in
   fixed (non-swappable) segments.
*/
long __pascal MyGZ(Size cb)
{
	Size cbFreed;

	cbFreed = 0;

	/* Do anything you might want to do to free memory without
	   unloading segments. */
	cbFreed += 0;

	if (cbFreed < cb)
		{
		cbFreed += CbFreeSwapMem(cb - cbFreed);
		}

	if (cbFreed < cb)
		{
		/* Do anything else you can to free memory.  If CbFreeSwapMem
		   returns a value lower than the one passed in, this indicates
		   that all swappable segments have been unloaded. */
		cbFreed += 0;
		}

	return(cbFreed);
}
