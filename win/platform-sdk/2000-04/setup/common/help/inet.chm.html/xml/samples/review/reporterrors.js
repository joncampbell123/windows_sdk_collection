// Parse error formatting function
function reportParseError(error)
{
  var s = "";
  for (var i=1; i<error.linepos; i++) {
    s += " ";
  }
  r = "<font face=Verdana size=2><font size=4>XML Error loading '" + 
      error.url + "'</font>" +
      "<P><B>" + error.reason + 
      "</B></P></font>";
  if (error.line > 0)
    r += "<font size=3><XMP>" +
    "at line " + error.line + ", character " + error.linepos +
    "\n" + error.srcText +
    "\n" + s + "^" +
    "</XMP></font>";
  return r;
}

// Runtime error formatting function
function reportRuntimeError(exception)
{
  return "<font face=Verdana size=2><font size=4>XSL Runtime Error</font>" +
      "<P><B>" + exception.description + "</B></P></font>";
}