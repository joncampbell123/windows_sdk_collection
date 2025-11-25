CVTDOC - ISAPI Automatic File Conversion Filter
-----------------------------------------------
Web content creators and webmasters often want to "publish" a document
or data file to the Web. However, it can be very inconvenient to 
constantly run a conversion program to generate new HTML each time 
the document or data file is updated.  Relying on the webmaster to run 
the conversion program for data that is often updated is also prone to error. 

CVTDOC is an ISAPI filter that converts documents to HTML dynamically as needed 
when the HTML file is accessed.  If the HTML document is out of date 
(older than the source document) or missing, it is generated automatically 
from the ISAPI filter, based on "conversion programs" registered for the source 
document type in the registry.  We provide sample conversion programs for Word, 
but its important to remember that this can be used for any document type for 
which you can find an HTML conversion program.

CONTENTS
--------
CVTDOC.DOC   - Documentation for CONVERT.DOC
CVTDOC.DLL   - The filter itself
CVTDOC.CPP   - The source code
MAKEFILE     - Builds CVTDOC.DLL
DOC2HTM.EXE  - Example conversion that converts Word DOCS to HTML files 
CVTSAMPL.HTM - Example use of filter to automatically convert SPECIALS.DOC
SPECIALS.DOC - Use with CVTSAMPL.HTM
XL2HTM.EXE   - Example conversion for Excel spreadsheets to HTML
TXT2HTML.BAT - Converts text files to HTML. Requires txt2html.pl and NT Perl.
TXT2HTML.PL  - Perl script called by TXT2HTML.BAT. Requires NT Perl.
README.TXT   - This file
