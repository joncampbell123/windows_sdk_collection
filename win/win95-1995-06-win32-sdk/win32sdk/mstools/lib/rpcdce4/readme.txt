RPCDCE4.LIB is not installed with SDK setup.  If you are using this library,
please stop doing so, and begin to use RPCRT4.LIB which contains a superset
of the exports.  Notice that RPCDCE4.DLL simply forwards calls to RPCRT4.DLL.
To see this type "link -dump -exports rpcdce4.dll".

With the Cairo release of Windows NT, we do not expect to include RPCDCE4.DLL.
The time to stop using it is now.
