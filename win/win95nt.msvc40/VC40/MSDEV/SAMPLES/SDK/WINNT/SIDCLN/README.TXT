Sample: Demonstration of the Win32 Security API Functions

Summary:

The SIDCLN sample demonstrates some of the Win32 security
API functions, and provides a sample of how a utility could
be written that recovers on-disk resources remaining
allocated to deleted user accounts.

More Information:

The on-disk resources recovered are:

   Files that are still owned by accounts that have been
   deleted are assigned ownership to the account logged on
   when this sample is run.

   ACEs for deleted accounts are edited (deleted) out of
   the ACLs of files to which the deleted accounts had been
   granted authorizations (eg., Read access)

It may be that running this sample as a utility has no
practical value in many environments, as the number of files
belonging to deleted user accounts will often be quite
small, and the number of bytes recovered on disk by editing
out ACEs for deleted accounts may well not be worth the time
it takes to run this sample.  The time it takes to run this
sample may be quite significant when processing an entire
hard disk or partition

Note:  This sample is not a supported utility.

TO RUN:

  You must log on using an account, such as Administrator,
  that has the priviledges to take file ownership and edit
  ACLs

  The ACL editing part of this sample can only be
  excercised for files on a partition that has ACLs NT
  processes:  NTFS

Typical test scenario:  Create a user account or two, log on
as each of these accounts in turn, while logged on for each
account, go to an NTFS partition, create a couple of files
so the test accounts each own a few files, use the file
manager to edit permissions for those files so that each
test user has some authorities (e.g., Read) explicitly
granted for those files.  Logon as Administrator, authorize
each test user to a few Administrator-owned files.  Delete
the test accounts.  Run the sample in the directories where
you put the files the test accounts owned or were authorized
to