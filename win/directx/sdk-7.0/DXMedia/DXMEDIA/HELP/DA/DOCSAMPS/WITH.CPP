<HTML>
<HEAD>
<TITLE>C++ Code with #import</TITLE>
</HEAD>
<BODY BGCOLOR=WHITE LINK=#0033CC TOPMARGIN=5 LEFTMARGIN=20>
<FONT FACE="Verdana, Arial, Helvetica" SIZE=3> 

<PRE>
#import "danim.dll" \
  exclude( "_RemotableHandle", "IMoniker", "IPersist", "ISequentialStream", \
  "IParseDisplayName", "IOleClientSite", "_FILETIME", "tagSTATSTG" ) \
  rename( "GUID", "DAGUID" ) \
  rename_namespace( "DAnim" )

using namespace DAnim;

IDAViewPtr _view;
IDAStaticsPtr e;

//Create the statics object
e.createInstance( L"DirectAnimation.DAStatics" );

//Create and establish the view object
_view.CreateInstance( L"DirectAnimation.DAView" );

IDAImagePtr = pbackImg= e->ImportImage(_bstr_t("myimage1.gif"));
IDAImagePtr = pfrontImg= e->ImportImage(_bstr_t("myimage2.gif"));

IDAImagePtr model = e->Overlay(pfrontImg, pbackImg);
_view->IDirectDrawSurface = ddsurf; 
_view->CompositeDirectlyToTarget = TRUE; 

//Start the model on this view
_view->StartModel(model, e->Silence, 0); 
</PRE>
</FONT>
</BODY>
</HTML>

