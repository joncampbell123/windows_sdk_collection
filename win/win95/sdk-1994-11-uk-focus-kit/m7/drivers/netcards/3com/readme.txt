Remarks:

1. The NET3COM.INF is based on Windows 95 build 189. It has been
   modified to support 3Com's new 3C589B PCMCIA adapter with the
   new driver, namely, 'ELPC3.VXD'.

2. The 3Com's developed driver is named 'ELPC3.VXD' and can work
   for both 3C589 and 3C589B adapters.

3. The new 'ELPC3.VXD' driver has new features to support auto-selection
   of media type for both 3C589 and 3C589B adapters. It also supports
   32K SRAM for 3C589B adapter.

4. The diagnostic utility is put under \DIAG. It can run on both
   3C589 and 3C589B adapters.

5. To use, copy net3com1.inf to your windows\inf directory.  Install,
   and when asked by setup, type in the directory in which this file
   is found.
