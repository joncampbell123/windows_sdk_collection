The idea behind this custom server is that it provides a smart search buisness object which knows how and where to look across a corporate network for a customer who may be located in 1 of 50 different databases (e.g., like a Gopher). 

The scenario used by this server simulates a simple search strategy, only the search paths being used are a local vs. a remote DBC. Local being Testdata and Remote being Tastrade.

What actually happens is that we are swapping out the current DE in a form and replacing the bound datasource properties with new ones from the server. This strategy allows us to use VFP in a 3-tier model where User services are not tightly bound to Data services as in much of today's client-server approaches.

Take a look at the BeforeOpenTables event of the Form's DataEnvironment.

To use:

1. Make sure you change the Database path in the SearchData program to accommodate your VFP working directory.

2. You will need to rebuilt the Foxsearch project and create a local server (EXE) so that it gets registered. The code which invokes the server is in the BeforeOpenTables event of the WING1.SCX DataEnvironment.

3. Run the form.


IMPORTANT!!! This very simple sample is meant to demonstrate the concept of a gopher and only works with a local server. Do not try to use this for a remote automation server because a block will occur when the client is waiting for a call to complete while the server attempts to callback into it. A better example which shows off OLE Callbacks with remote automation is the pool manager sample.