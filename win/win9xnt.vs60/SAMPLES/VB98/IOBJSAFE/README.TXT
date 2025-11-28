                  Visual Basic 6 IObjectSafety Interface Sample
                                Readme document
                                March 19, 1998

Summary
-------
   IObjSafe.OCX implements the IObjectSafety interface, which exposes functionality 
   to Internet Explorer 4.x's Safe For Scripting and Safe For Initialization 
   Security features.

IObjectSafety definition
------------------------
   IObjectSafety should be implemented by objects that have interfaces which
   support "untrusted" clients (for example, scripts). It allows the owner of
   the object to specify which interfaces need to be protected from untrusted
   use. Examples of interfaces that might be protected in this way are:

   IID_IDispatch         - "Safe for automating with untrusted automation 
                            client or script"
   IID_IPersist*         - "Safe for initializing with untrusted data"
   IID_IActiveScript     - "Safe for running untrusted scripts"

Safety Design Issues
--------------------
   There are three possible safety scripting scenarios necessary to consider.

   i.   Object is always safe for scripting.
        If you know that all of your object's methods and properties are definitely
        safe for scripting, you should notify the client host application by not 
        returning an error in the IObjectSafety_SetInterfaceSafetyOptions interface 
        method.  This will allow your object to successfully be created and run.

   ii.  Object is never safe for scripting.
        If you know that none of your object's methods and properties are safe 
        for scripting, you will want to notify the client host application 
        by returning an error, E_Fail, in the IObjectSafety_SetInterfaceSafetyOptions 
        interface method. This will not allow any methods or properties to be accessed.
        
   iii. Object is partially safe for scripting.
        At least one method or property is known to be safe for scripting.  In this
        case you have two choices to consider.  
        1.  Notify the client application that your object is not safe for scripting.
            This will not allow any methods or properties to be accessed.
        2.  Notify the client application that your object is safe for scripting but
            disable the methods and properties that are known not to be safe for 
            scripting.  This will allow your object to successfully be created and
            only allow access to those methods and properties that you know to be 
            safe for scripting.

Testing and Running Samples
---------------------------
   It is recommended that you have Internet Explorer 4.0 or greater to test this
   sample.  An HTML sample have been included to demonstrate the use and testing
   of the IObjSafe.OCX sample Visual Basic project.

Project Description
-------------------
   IObjSafe.vbp is an ActiveX OCX sample that implements the IObjectSafety
   interface defined in the IObjectSafety.TLB type library.  The HTML file 
   IObjSafe.HTM tests the IObjectSafety interface implemented in the 
   IObjSafe.OCX by creating and hosting the UCObjectSafety control  and 
   references the Safe and UnSafe properties with VBScript in the HTML document.
