### The file, whose entry point is ErrorFile2HTMFile, translates error files
### into HTML files (Duh!)  We want it to work on errors that have
### been either produced by Cole (port64.exe) and produced by the PERL linker,
### it looks at the first line to determine which file it is producing error messages
### for, and only processes error message in the error file for that file.
### (For those who haven't read all the specs, yet:  the error files from Cole
### contain error reports for all files #included in a given source during parsing.
### The linker links together all the error files, then sort according to file name,
### and spits everything back out.)

$|=1;

# ************************ GLOBAL VARS *****************************

#
# NumErrorClasses holds the number of error classes for Cole. This
#                 is also held in error-file-to-html.pl
#

$NumErrorClasses = 14;

# ************************ GLOBAL ARRAYS ***************************

#
# new_errorInfo contains the descriptions of all the errors. See port64 for a description
#               of the error types.
#

@new_error_info =
    (
     "P32 failures, code will break under a P32 model",   ## ecP32Failure
     "P32 suspicious code fragment",                      ## ecP32Warning
     "P64 failures, code will break under a P64 model",   ## ecP64Failure
     "P64 suspicious code fragment",                      ## ecP64Warning
     "Truncating Casts that may lose data",               ## ecTruncatingCast,
     "Casts into and out of void *",                      ## ecVoidPtrCast,
     "Casts of small constants (0x0001 thru 0x0777) to pointers", ##  ecSmallConstCast,
     "Casts of medium constants (0x1000 thru 0x7FFFFFFF) to pointers", ##  ecMidConstCast,
     "Casts of very suspicious constants (0x80000000 to FFFFFFFF) to pointers", ##  ecSuspiciousConstCast,
     "Suspicious expansive casts of unsigned values (e.g., ULLFoo= ULLFoo & (ULBar - 1))", ## ecSuspiciousExpansiveCastofUnsigned,
     "Casts from pointer-to-non-pointer to pointer-to-pointer", ## ecMultilevelCast,
     "Cast from pointers to x_PTR values and vice-versa", ## ecPointer2x_PtrCast,
     "General programming warnings and errors",           ## ecGenericError     
     "Cole internal error reports"                        ## ecColeError
);

%error_info = 
  ("Cast operation from non-64 bit integral to pointer", "#FF0000:0:\"courier new\"",    ##This one is on the cast.
   "Cast operation from non-_PTR integral to pointer.", "FF0000:0:\"courier new\"",    
   "Redeclare integral variable as ", "#FF0000:2:tahoma",  ## This one is on symbol.
   "Suspect Cast of integral constant to pointer value", "FF0000:0:\"courier new\"",    ## This one is on the constant.
   "Cast from Pointer-to-non-pointer to Pointer-to-pointer", "FF0000:0:\"courier new\"", ## SFA is start of cast operation
   "Change declaration of this variable to ", "#FF0000:2:tahoma", ## This one is on symbol.
   "Type Error: you must cast pointer to INT_PTR, not to int.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Type Error: you must cast pointer to UINT_PTR, not to unsigned int.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Type Error: you must cast pointer to LONG_PTR, not to long.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Type Error: you must cast pointer to ULONG_PTR, not to unsigned long.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Type Error: you must cast pointer to INT_PTR, not to __int64.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Type Error: you must cast pointer to UINT_PTR, not to unsigned __int64.", "FF0000:-2:\"courier new\"", ## SFA is on start of cast.
   "Cast from Pointer-to-x to Pointer-to-void", "FF0000:-2:\"courier new\"",
   "Cast from Pointer-to-void to Pointer-to-pointer", "FF0000:-2:\"courier new\"",
   "Casting 32 bit unsigned result of complement operator (~) to 64 bits.", "FF0000:-2:\"courier new\"",
   "Casting 32 bit unsigned result of complement operator (~) to <x>_PTR. Potential P64 problem.", "FF0000:-2:\"courier new\"",
   "Casting <x>_PTR to", "FF0000:-2:\"courier new\"", ##    "Casting <x>_PTR to %s, possible loss of data.",
   "Casting 64 bit value ", "FF0000:-2:\"courier new\"",  ## "Casting 64 bit value (%s) to %s, possible loss of data."
   "Casting ptrdiff_t to ", "FF0000:-2:\"courier new\"",  ##  "Casting ptrdiff_t to %s, possible loss of data."
   "Truncating cast of value of type ptrdiff_t, possible loss of data on a 64 bit platform.", "FF0000:-2:\"courier new\"",
   "Truncating cast, possible loss of data.", "FF0000:-2:\"courier new\""
   );  

# ******************************************************************

@error_keys = keys(%error_info);
%error_indices = ();
{
    my($eindex) = 0;
    my($key);
    foreach $key (@error_keys) {
        $error_indices{$key} = $eindex++;}
};

sub HTMLQuoteString {
    $_ = $_[0];
    s/&/&amp;/g;
    s/\042/&quot;/g;
    s/</&lt;/g;
    s/>/&gt;/g;
    return $_;
}
    
sub SpewErrorTableEntries {
    my($errorTableFilename) = @_;
    for ($i=0; $i < @new_error_info; $i++) {
        my($description, $color, $size, $font) = split(/:/,$new_error_info[$i]);
        printf $errorTableFilename "  <tr><td nowrap><input type=checkbox id=ck%d checked>\n<label for=ck%d>%s (%d instance%s)</label>\n",
        $i, $i, &HTMLQuoteString($description), $global_error_count[$i], ($global_error_count[$i] == 1) ? "" : "s";}
}

sub SpewStyleEntries {
    my($sourceHTMFilename) = @_;
    for ($i=0; $i < @new_error_info; $i++) {
        printf $sourceHTMFilename ".ec$i {color:red; font-weight:bold; font-size:120%%; font-family:\"courier new\"}\n";
        }
}

sub sourcePath2otherPath {
   my($path, $extension) = @_;
   my(@expandedPath) = split(/\\/, $path);
   $_ = $expandedPath[$#expandedPath]; # grab last component

   # This ain't UNIX; dots in filenames are anti-social
   tr/./_/;

   # ensure that no base name ends in _err; change to _er_
   if (/_err$/) { substr($_,-1,1) = "_"; }

   # Need to make sure that we don't use the
   # same otherPath for different source paths. (RFC)
   my($prevpath) = $used_bases{$_};
   my($i) = 0;
   while ($prevpath && ($prevpath ne $path)) {
      $prevpath = $used_bases{$_ . ++$i};
   }
   return $_ . ($i . $extension);
}

## Only valid on compressed error files.
sub ErrorFile2LinkedErrorFile {
  local($errorFilename) = @_;
  open(ERRORFILE, "<$errorFilename") || die("Couldn't open error file $errorFilename.");  
  local($title) = scalar <ERRORFILE>;
  chop $title;
  if ($title =~ /^Error file for (.*)/) {
    $source_filename = $1;
    $compressed = 0;}
  elsif ($title =~ /^Compressed error file for (.*)/) {
    $source_filename = $1;
    $compressed = 1;}
  else {die("$errorFilename does not name a valid error file.\n");}

  my($htmFilename) = &sourcePath2otherPath($source_filename, "_err.htm");
  my($sourceHTMFilename) = &sourcePath2otherPath($source_filename, ".htm");
  open(HTMLFILE, ">$htmFilename") || die("Couldn't open linked error output file $htmFilename.");

# <link REL="stylesheet" TYPE="text/css" HREF="foo.css">
  print HTMLFILE <<"EndOfHeader";
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"><html><head>
<title>Error listing for file $source_filename </title>
<style id=local>
</style>
<script language="JavaScript">
  function restyle() {
    var localSheet = document.styleSheets.local;
    for (var i = localSheet.rules.length - 1; i >= 0; i--)
      localSheet.removeRule(i);
    for (var i = 0; i < $NumErrorClasses; i++) {
       if (top.ckHide[i]) {
          localSheet.addRule("TR.ec"+i, "display:none")
       }
    }
  }
</script>

<script language="JavaScript">
  function loadfunction() {
    var codepane = top.frames["code-pane"];
    codepane.location.href=\"$sourceHTMFilename\";
    restyle();      
  }   
</script>

</head>

<body bgcolor=#FFFFFF onload="loadfunction();">

<table cellspacing=0 cellpadding=0>
EndOfHeader
  ## Error message are sorted
  ## Loop records, annotating source.
  $prior_record = "";
  local($filename) = $source_filename;
  while (<ERRORFILE>) {
    ## Eliminate error messages for same sfa due to macro expansion.  FIX: don't issue duplicates.
    chop;
    if ($_ eq $prior_record) {next;}
    $prior_record = $_;
    if ($compressed) {($row_number, $column_number, $errorclass, $link, $error_message) = split(/\|/);}
    else {($filename, $row_number, $column_number, $errorclass, $link, $error_message) = split(/\|/);}
    $hfilename = &sourcePath2otherPath($filename, ".htm");
    printf HTMLFILE "<tr class=ec%d>", $errorclass;
    my($fakelink) = &hashlink("$row_number-$column_number");
    print HTMLFILE "<td nowrap><a target=\"code-pane\" title=\"$error_message\" HREF=\"$hfilename#$fakelink\">$error_message</a>\n";}
        
  ## Print standard closer.
  printf(HTMLFILE "</table>\n</body></html>\n");
  close HTMLFILE;
}

### Verify errorfile is a porting report, and get the source filename
sub ErrorFile2HTMFile {
  local($errorFilename) = @_;
  open(ERRORFILE, "<$errorFilename") || die("Couldn't open error file $errorFilename.");  
  local($title) = scalar <ERRORFILE>;
  chop $title;
  if ($title =~ /^Error file for (.*)/) {
    $source_filename = $1;
    $compressed = 0;}
  elsif ($title =~ /^Compressed error file for (.*)/) {
    $source_filename = $1;
    $compressed = 1;}
  else {die("$errorFilename does not name a valid error file.\n");}

  if (!open(SOURCEFILE, "<$source_filename"))
        {
        # close(ERRORFILE); return;
        die("Couldn't open source file \"$source_filename\".");
        }
  my($htmFilename) = &sourcePath2otherPath($source_filename, ".htm");
  open(HTMLFILE, ">$htmFilename") || die("Couldn't open output file $htmFilename.");

  print HTMLFILE <<"EndOfHeader";
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"><html><head>
<title>Porting report for file $source_filename </title>
<style TYPE="text/css">
EndOfHeader
  SpewStyleEntries(HTMLFILE);
  
  #
  # Make a copy of the source filename to convert \'s to \\'s
  #
  my($title_name) = $source_filename;
  $title_name =~ s/\\/\\\\/g;

  print HTMLFILE <<"EndOfHeader";
</style>

<style ID=local TYPE="text/css">
<!-- this style sheet is for hiding and unhiding things. -->
</style>

<script language="JavaScript">
  function restyle() {
    var localSheet = document.styleSheets.local;
    for (var i = localSheet.rules.length - 1; i >= 0; i--)
      localSheet.removeRule(i);
    for (var i = 0; i < $NumErrorClasses; i++) {
       if (top.ckHide[i]) {
          localSheet.addRule(".ec"+i,
                             "color:green; font-weight:bold; font-size:120%; font-family:\\"courier new\\"");
          }
       else {
          localSheet.addRule(".ec"+i,
                             "color:red; font-weight:bold; font-size:120%; font-family:\\"courier new\\"");
          }
       }
  }
</script>

<script language="JavaScript">

var filenamepane = top.frames["filename-pane"];    
filenamepane.change_file_name("$title_name"); 

restyle();

</script>
</head>


<body>

<pre>
EndOfHeader

  ## Get and sort porting messages.  
  ## Note that for compressed error files, the records are already sorted.
  local(@records) = ();
  @records = <ERRORFILE>;
  chop @records;
  @records = sort by_SFA @records;

  ## Loop records, annotating source.
  $source_row_number = 0;
  $prior_row_number = -1;
  $prior_column_number = -1;
  while (@records) {
    $record = shift(@records);
    if ($compressed) {($row_number, $column_number, $errorclass, $link, $error_message) = split(/\|/,$record);}
    else {($filename, $row_number, $column_number, $errorclass, $link, $error_message) = split(/\|/,$record);}
    ## Eliminate error messages for same sfa due to macro expansion.  FIX: do better in the future.
    if ($row_number == $prior_row_number && $column_number == $prior_column_number) {next;}
    $prior_row_number = $row_number;
    $prior_column_number = $column_number;
    # while (@records && same_sfa($record, @records[0])) {shift(@records);}
    ## If not Compressed Only worry about errors in source file, not in included headers.
    if ($compressed || $filename eq $source_filename) {&annotate_source_line();}}
  ## We've run out of records, spit out the rest of the source file.
  unless ($source_row_number == 0) {
    ## FIX: Abstract out < and > nonsense into a separate print subroutine.
    $source_row =~ s/\001/&lt;/g;
    $source_row =~ s/\002/&gt;/g;
    print HTMLFILE $source_row;}
  while ($inline = <SOURCEFILE>) {  ## Maybe be real PERL hackers and use $_?
    $inline =~ s/</&lt;/g;
    $inline =~ s/>/&gt;/g;
    print HTMLFILE $inline;}  
        
  ## Print standard closer.
  printf(HTMLFILE "\n</pre></body></html>\n");
}

## arguments will eventually be $row_number, $column_number, $error_message, ,$errorclass, SOURCEFILE, $source_row_number, $source_row, HTMLFILE, $link
sub annotate_source_line
{
  while ($row_number != $source_row_number) {
    unless ($source_row_number == 0) {
      ## finish fixup of < and >
      $source_row =~ s/\001/&lt;/g;
      $source_row =~ s/\002/&gt;/g;
      print HTMLFILE $source_row;}
    $source_row = <SOURCEFILE>;
    ## deal with < and > in source code.  Can't just do fixup now, or it would break indices into the string.
    $source_row =~ tr/<>/\001\002/;
    $source_row_number++;}

  ## now that we've got the line, isolate token
  ## Fail safe if SFA info is screwed up and we can detect it.
  if ($column_number > length($source_row)) { # Bug: SFA screwed up
    substr($source_row, -1, 0) = "<font color=red size=size+3><b>COLE BUG C0023, PLEASE REPORT TO COLEBUG</b></font>";
    return; }
  $tokenstart = substr($source_row, $column_number);
  $firstchar = substr($source_row, $column_number, 1);
  $tokenlength = 0;
  ## if it's a paren (usually the start of a cast), look naively for closing paren.
  if ($firstchar eq "(") {
    $tokenlength = index($tokenstart, ")");  ## Just find first close paren on the line.
    if ($tokenlength <= 0) {$tokenlength = 0;}
    $tokenlength++;}
  else {
    $tokenstart =~ m/^(->|[=<>]+|[+-]+|[&]+|[\|]+|[a-zA-Z_][a-zA-Z0-9_]*|0x[0-9A-Fa-f]+|[0-9][0-9.eE]*|.)(.*)$/;
    $token = $1;
    $tokenlength = length($token);}

  ## Fail safe if SFA info is screwed up and we can detect it.
  if ($column_number + $tokenlength> length($source_row)) { # Bug: SFA screwed up
    substr($source_row, -1, 0) = "<font color=red size=size+3><b>COLE BUG C0024, PLEASE REPORT TO COLEBUG</b></font>";
    return; }

  ## highlight it.
  local($linkToUse, $enterhighlight, $exithighlight);
  ## No link
  if (!$link)  {
    $enterhighlight = "<span class=ec$errorclass title=\"$error_message\">";
    $exithighlight = "</span>";}
  ## Link to different file
  elsif ($link =~ /(.*)\#(.*)/) { 
    $linkToUse = &sourcePath2otherPath($1, ".htm") . "#". &hashlink($2);
    $enterhighlight = "<a href=\"$linkToUse\" class=ec$errorclass title=\"$error_message\">";
    $exithighlight = "</a>";}
  ## Link to same file
  else {
    $enterhighlight = "<a href=#" . &hashlink($link) . " class=ec$errorclass title=\"$error_message\">";
    $exithighlight = "</a>";}
  $shift_string = "<a name=\"" . &hashlink("$row_number-$column_number") . "\"></a>$enterhighlight";

  #Now do the work.
  substr($source_row, $column_number + $tokenlength, 0) = $exithighlight;
  substr($source_row, $column_number, 0) = $shift_string;
}


############################################################################################
#####

## It is important that higher columns sort before lower columns on same line since
## we add annotation text from right to left.
sub by_SFA
{local(@left_fields) = split(/\|/,$a);
 local(@right_fields) = split(/\|/,$b);
 if ($compressed) {
   @left_fields[0] <=> @right_fields[0] || @right_fields[1] <=> @left_fields[1];}
 else {
   @left_fields[1] <=> @right_fields[1] || @right_fields[2] <=> @left_fields[2];}
}


sub same_sfa
{local(@left_fields) = split(/\|/,@_[0]);
 local(@right_fields) = split(/\|/,@_[1]);
 $same = @left_fields[1] == @right_fields[1] && @right_fields[2] == @left_fields[2];
 return $same;
}

sub locate_error_info {
  my($error_message) = @_[0];
  my($key);
  foreach $key (@error_keys) {
    if (index($error_message, $key) == 0) {
      return $error_info{$key};}}
  return "";}

sub locate_error_index {
  my($error_message) = @_[0];
  my($key);
  foreach $key (@error_keys) {
    if (index($error_message, $key) == 0) {
      return $error_indices{$key};}}
  return "";}

sub locate_error_color {
  my($error_class) = @_[0];
  ($temp = $new_error_info[$error_class]) || return "";
  ($description, $color, $size) = split(/:/,$temp);
  return $color;}

sub locate_error_size {
  my($error_class) = @_[0];
  ($temp = $new_error_info[$error_class]) || return "";
  ($description, $color, $size) = split(/:/,$temp);
  return $size;}

sub locate_error_face {
  my($error_class) = @_[0];
  ($temp = $new_error_info[$error_class]) || return "";
  ($description, $color, $size, $face) = split(/:/,$temp);
  return $face;}

## Those using NT 5.0 and an early version of IE are hosed on long names.
## So we play a little game.  Hope there aren't more than 1000 links on a page...
my(%linkHashTable) = ();
my($nexthashvalue) = 1;

## This was a workaround for an IE bug with #names and the file:// protocol.
##sub hashlink {
##    my($link) = $_[0];
##    my($hashval) = $linkHashTable{$link};
##    if ($hashval ne "") {return $hashval;}
##    my($newlink) = &ThreeLetterLink($nexthashvalue++);
##    $linkHashTable{$link} = $newlink;
##    return $newlink;
##}

sub hashlink {
    return $_[0];
}

my(@trans) = ("0" .. "9", "a" .. "z");

##sub ThreeLetterLink {
##    my($arg) = $_[0];
##    my($highDigit) = int($arg / 1296);
##    $arg = $arg % 1296;
##    my($midDigit) = int($arg / 36);
##    my($lowDigit) = $arg % 36;
##    return $trans[$highDigit] . $trans[$midDigit] . $trans[$lowDigit];
##}
