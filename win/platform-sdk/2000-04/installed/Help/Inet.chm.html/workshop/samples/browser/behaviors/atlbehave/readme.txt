AtlBehave - Sample binary DHTML behavior implemented using Active Template Library
==================================================================================

PURPOSE
=======

This sample illustrates how to: 

 - attach a binary DHTML behavior to an HTML element.  
 - implement a binary DHTML behavior for Internet Explorer 5.  
 - access the document object model.
 - handle events from the document object model.

FILES SUPPLIED
==============

This is a Visual C++ 6.0 project.
 
 - FACTORY.CPP,FACTORY.H - IElementBehaviorFactory interface implementation.
 - BEHAVIOR.CPP,BEHAVIOR.H - IElementBehavior interface implementation.
 - EVENTSINK.CPP,EVENTSINK.H - HTMLElementEvents event sink implementation.
 - BEHAVE.HTM - test HTML file which demonstrates the sample behavior.

BUILDING THE SAMPLE
==================

 - Requires that the Internet Explorer 5 header and library files are installed. These are available from  the Site Builder Network (http://msdn.microsoft.com/workshop/), Downloads area.

 - to compile the sample with Visual C++ 5.0, select Options...Directories from the Tools menu and add paths to the Internet Explorer 5 include and library files in the following order:
    - IE 5 include files (..\SBN\include)
    - NT 5 include files (..\SBN\include\NT50)
    - Visual C++ include files

 - to compile the sample with Visual C++ 6.0, copy the Visual C++ 6.0 versions of unknwn.h & unknwn.idl to a temporary folder, e.g. SBN\include\VC6, then select Options...Directories from the Tools menu and add paths to the Internet Explorer 5 include and library files in the following order:
    - IE 5 include files (..\SBN\include)
    - Visual C++ 6.0 UNKNWN.IDL & UNKNWN.H files (..\SBN\include\VC6)
    - NT 5 include files (..\SBN\include\NT50)
    - Visual C++ include files

RUNNING THE SAMPLE
==================

 - Open BEHAVE.HTM.

 - Moving the mouse over the main text will show the attached behavior. In this sample, the selected system colors are used to highlight the text.

 - Use the two listboxes to select a different system color to be used as the background or text color.

NOTES
=====

This sample overides the default ATL implementation of ObjectSafety::SetInterfaceSafetyOptions() so you are not presented with a Security Alert dialog box on opening the sample HTML page. This is because Internet Explorer 5 queries the behavior factory to set the safety options for the IDispatchEx/IDispatch and IPersistPropertyBag interfaces and IPersistPropertyBag is not implemented in this sample.

This sample does not support providing the two properties (backColor and textColor) as expando properties, for example:

<SPAN ID=target STYLE="behavior:url(#behave1); font-size:xx-large"  backColor=5 textColor=24>