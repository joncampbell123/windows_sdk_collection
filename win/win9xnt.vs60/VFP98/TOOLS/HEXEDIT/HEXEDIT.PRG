**  The HexEdit form accepts a parameter containing the path to the
**  file to edited.  If an SCX is the main program in an APP file, 
**  it does not accept parameters passed from outside the app.  So 
**  this is just a shell prg to pass the parameter to the form.  This 
**  prg becomes the main program in the app.
PARAMETER Param1
DO FORM HEXEDIT WITH PARAM1
