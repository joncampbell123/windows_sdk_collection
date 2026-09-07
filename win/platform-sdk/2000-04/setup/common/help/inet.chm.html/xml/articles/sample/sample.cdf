<?xml version="1.0"?>
<!DOCTYPE Channel SYSTEM "http://www.w3c.org/Channel.dtd">
<!-- 
Note: You can replace the file path names to make the sample work on your machine.
-->
<CHANNEL HREF="http://www.microsoft.com/xml/articles/sample/homepage.htm" >
	<TITLE>Sample CDF Channel</TITLE>
	<LOGO HREF="/xml/articles/sample/book.ico" STYLE="ICON"/>
	<LOGO HREF="/xml/articles/sample/logo.gif" STYLE="IMAGE"/>
   	<ITEM HREF="/xml/articles/sample/page1.htm">
    		<LOGO HREF="/xml/articles/sample/red.ico" STYLE="ICON"/>
    		<Log VALUE="document:view"/>
    		<TITLE>The Red Page</TITLE>
    		<ABSTRACT>This is the abstract description for the red page.</ABSTRACT>
	</ITEM>
	<ITEM HREF="/xml/articles/sample/page2.htm">
		<LOGO HREF="/xml/articles/sample/blue.ico" STYLE="ICON"/>
		<Log VALUE="document:view" />
		<TITLE>The Blue Page</TITLE>
		<ABSTRACT>This is the abstract description for the blue page.</ABSTRACT>
	</ITEM>
	<ITEM HREF="/xml/articles/sample/page3.htm">
		<LOGO HREF="/xml/articles/sample/green.ico" STYLE="ICON"/>
		<Log VALUE="document:view" />
 		<TITLE>The Green Page</TITLE>
		<ABSTRACT>This is the abstract description for the green page.</ABSTRACT>
   	</ITEM>
	<ITEM HREF="/xml/articles/sample/scrnsave.htm">
		<USAGE VALUE="ScreenSaver"></USAGE>
	</ITEM>
	<SCHEDULE>
		<INTERVALTIME DAY="1" />
		<EARLIESTTIME HOUR="1" />
		<LATESTTIME HOUR="5" />
	</SCHEDULE>
</CHANNEL>
