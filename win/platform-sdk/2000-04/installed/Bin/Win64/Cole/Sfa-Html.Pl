# ************************* GLOBAL ARRAYS ************************** 

#
# EM_array holds the error messages for each file.
# Dir_Content_Files holds the filenames in the directory
# Dir_Content_Subdirs holds the subdirs in the directory
# Top_Dirs holds the top-level directories
# Dir_Index holds the indices for the directories
# File_Error_Sums holds the initial sum of errors for each file
# Dir_Error_Sums holds the initial sum of errors for each directory
# File_Parents holds the index of the parent directory for each file
#

%EM_array = ();
%Dir_Content_Files = ();
%Dir_Content_Subdirs = ();
%Top_Dirs = ();
%Dir_Index = ();
@File_Error_Sums = ();
@Dir_Error_Sums = ();
@File_Parents = ();

# ************************ GLOBAL VARS *****************************

#
# NumErrorClasses holds the number of error classes for Cole. This
#                 is also held in error-file-to-html.pl
#

$NumErrorClasses = 14;

# ******************** SUBROUTINES *******************************

#
# MakeFileAbsolute converts a filename to the absolute file name
#
# Paramaters: file name, directory name
#
sub MakeFileAbsolute {

   #
   # Canonicalize working dir so that, e.g. c:foo\bar becomes c:\foo\bar.
   # Remove the trailing \ from the working directory
   #
   my($workingdir) = $_[1];
   if ($workingdir =~ /^[A-Za-z]:[^\\]/) 
   {
      substr($workingdir, 2, 0) = "\\";
   }
   if (substr($workingDir,-1,1) eq "\\") 
   {
      substr($workingDir,-1,1) = "";
   }

   $_ = $_[0];
   if (/^\.\./) 
   {  
      #
      # Relative filename of form ..\dir1\dir2
      # Recursively remove the ..s
      #
      my(@expandedWorkingDir) = split(/\\/,$workingdir);
      pop @expandedWorkingDir;
      my($newworkingdir) = join("\\", @expandedWorkingDir);
      substr($_[0], 0, 3) = "";
      MakeFileAbsolute($_[0], $newworkingdir);
   }
   elsif (/^[A-Za-z]:/) 
   {
      #
      # Absolute filename is of form  C:\dir\.... 
      #
   }  
   elsif (/^\\/)  
   {
      #
      # Absolute filename is of form  \dir1\dir2... or \\network\share\...
      #
   }  
   elsif (/^\.[^\.]/) 
   {  
      # 
      # Filename of form .\fribble, (not ..\dir...), replace . by working dir.
      #
      substr($_[0], 0, 1) = $workingdir;
   }
   else 
   {
      #
      # relative pathname of form dir1\dir2\... (or ..\dir1...), add in working dir.
      #
      substr($_[0], 0, 0) = $workingdir . "\\";
   }
}


#
# OpenLog opens the log file name and saves the handle in the global LOGFILE.
#
# Parameters:  logfile name, current working dir
#
sub OpenLog {

   my($logfilename) = $_[0];
   my($workingDir)  = $_[1];
   
   &MakeFileAbsolute($logfilename, $workingDir);
   open(LOGFILE, "<$logfilename") || die("\nCouldn't open log file $logfilename because $!");
}

#
# CreateOutputDirectory creates the directory and changes to it
#
# Parameters: directory name
#
sub CreateOutputDirectory {

   my($outDirectoryName) = $_[0];
   
   if ((!-d $outDirectoryName)  && system("mkdir $outDirectoryName")) 
   {
      &UserError("could not make output directory $outDirectoryName");
   }
   if ($outDirectoryName ne ".") 
   {
      chdir $outDirectoryName || die ("Cannot cd to $outDirectoryName.");
   }
}

#
# print_list prints a list; used for debugging
#
sub print_list {
   
   foreach (@_) 
   {
      print "\n";
      print ;      
   } 
}

# ******************** MAIN PROGRAM ******************************
#
# This script loads in all the .obj listed in the command line,  links all the 
# errors messages together, then produces .htm for all source files.  
# In a future version, it will write out the linked file and produce .htm files 
# on demand.
#

# 
# Flush the buffers immediately
# 
$| = 1;

#
# Require error-file-to-html.pl for the routines declared in it
# 
my($ProgramName) = $0;
&MakeFileAbsolute($ProgramName, $workingDir);
my(@dir_executing_from) = split(/\\/, $ProgramName);
pop @dir_executing_from;
require join("\\", @dir_executing_from, "error-file-to-html.pl");

#
# First argument is name of error file.
# Second argument is name of output directory.
#
my($logfilename) = shift;
my($outDirectoryName) = shift || ".";

my($workingDir) = Win32::GetCwd();

#
# Open the log file
#
&OpenLog($logfilename, $workingDir);

#
# Create the output directory and change to it, if neccessary
#
&CreateOutputDirectory($outDirectoryName, $workingDir);

print "Cole Version 1.30.\nProcessing Cole Output Log...";

#
# Get the list of obj file names from the log into fileList.
#
my(@fileList);

{
   my($fcount) = 0;
   while (<LOGFILE>) 
   {
      chomp;
      if (/^%CD (.*)/) 
      {
         $workingDir = $1;
         if (substr($workingDir,-1,1) eq "\\") 
         {
            substr($workingDir,-1,1) = "";
         }
         if (++$fcount % 20 == 0) 
         {
           print $fcount, "...";
         }
      }
      else 
      {
         &MakeFileAbsolute($_, $workingDir);
         unshift(@fileList,$_);
      }
   }
}

my($title, $source_filename, $filename, $rest, $subdir, $objFilename);

#
# Get the list of source files that contain errors and the 
# update the directories that contain these files
#
foreach $objFilename (@fileList) 
{
   print ".";
   
   #
   # Get the source file name
   #
   open(ERRORFILE, "<$objFilename") || die("\nCouldn't open object file $objFilename.");
   $title = <ERRORFILE>;
   chop $title;
   if ($title =~ /^Error file for (.*)/) 
   {
      $source_filename = $1;
   }
   else 
   {
      die("\n$objFilename does not name a valid error file.\n");
   }
  
   #
   # Get the working directory in the obj file
   #
   $title = <ERRORFILE>;
   chop $title;
   if ($title =~ /^Working directory: (.*)/) 
   {
      $workingDir = $1;
   }
   else 
   {
      die("Second line of $ARGV ($title) is invalid.\n");
   }
  
   #
   # Get and sort porting messages.
   #     
   while (<ERRORFILE>) 
   {
      ($filename, $rest) = split(/\|/, $_, 2);
      &MakeFileAbsolute($filename, $workingDir);
      chop $rest;
      
      if (not $EM_array{$filename}) 
      {
         #
         # Update directory contents
         #      
         @dir_list = split(/\\/, $filename);
         pop(@dir_list);
         $subdir = join "\\", @dir_list;
         push(@{ $Dir_Content_Files{$subdir} }, $filename);         
      }
      
      $EM_array{$filename} .= "$rest\000";
   }
}

#
# Create the list of subdirectories and top-level directories
#
my(@dir_list) = sort keys %Dir_Content_Files;

# 
# Create a string of directory names separated by commas and with
# \ converted to ! to enable quick search of "subdirectory of" property
#
my($dir_string) = join ",", @dir_list, "";
$dir_string =~ s/\\/!/g;

#
# dir_list_index is a counter giving each directory an index
#
my($dir_list_index) = 0;
my($dir_name_i, $orig_name);

foreach $dir_name_i (@dir_list) 
{
   my($dir_name) = $dir_name_i;
   my(@dir_temp) = split(/\\/, $dir_name);
   
   $Dir_Index{$dir_name} = $dir_list_index++;
   
   #
   # Add this dir as a top level directory and remove it later if it
   # isn't one
   #
   push(@Top_Dirs, $dir_name_i);
   
   while (@dir_temp)
   {
      # 
      # Go up a level
      #
      pop(@dir_temp);
      $dir_name = join "!", @dir_temp;

      if ($dir_name && ($dir_string =~ /$dir_name,/))
      {

         #
         # Directory exists in the list, so the current directory has a parent
         #
         $dir_name =~ s/!/\\/g;     
         $orig_name = pop(@Top_Dirs);
         push(@{ $Dir_Content_Subdirs{$dir_name} }, $orig_name);
         last;
      }      
   }   
}

close(ERRORFILE);

#
# Generate the files list
#
open (GLOBALHTML, ">Cole-Files.htm") || die("Can't open HTML file \"Cole-Files.htm\"");
print GLOBALHTML <<"EndOfHeader";
<HTML>
<HEAD>
<TITLE>Error Files</TITLE>

<script language="JavaScript">

  function compute_err_counts(box_number) {
    
    if (box_number != -1) 
    {
       //
       // For box clicks
       //
       for (var fileindex = 0; fileindex < filelist.length; fileindex++) 
       {
          var fn = filelist[fileindex];
          if (top.ckHide[box_number]) 
          {
              file_err_sum[fileindex] -= fn[box_number];
              dir_err_sum[file_parent[fileindex]] -= fn[box_number];
          } 
          else 
          {
              file_err_sum[fileindex] += fn[box_number];
              dir_err_sum[file_parent[fileindex]] += fn[box_number];          
          } 

          document.all["file" + fileindex].innerText = file_err_sum[fileindex];
          if (show_files[fileindex]) 
          {
              if (file_err_sum[fileindex] == 0) 
              { 
                 document.all["fileline" + fileindex].style.display = "none";
              } 
              else 
              {  
                 document.all["fileline" + fileindex].style.display = "";
              }
          } 
       }                        
    } 
    else 
    {
       //
       // For the load scenario
       //
       for (var fileindex = 0; fileindex < filelist.length; fileindex++) 
       {
          document.all["file" + fileindex].innerText = file_err_sum[fileindex];
          if (file_err_sum[fileindex] == 0) 
          {
             document.all["fileline" + fileindex].style.display = "none";
          } 
          else 
          { 
             document.all["fileline" + fileindex].style.display = "";
          }
       }
       
       //
       // Reset the ckHide array
       //
       for (var i = 0; i < $NumErrorClasses; i++) 
       { 
          top.ckHide[i] = 0;
       }
       var code_pane = top.frames["code-pane"];
       var filename  = new String(code_pane.location);
       var endpos    = filename.length;
       var startpos  = filename.lastIndexOf("/")+1;
       
       if ((startpos < endpos) && (filename.substring(startpos,endpos) == "cole-option.htm"))
       {
          code_pane.reloadHook();
       }              
    }
    
    //
    // Update the directory counts
    //
    update_dir_err_sums();           
  }

  function update_dir_err_sums()
  {
    for (var dirindex = 0; dirindex < dir_err_sum.length; dirindex++)
    {
       var total = dir_err_sum[dirindex];
       var dn_subdir = dirdl[dirindex];
    
       for (var i = 0; i < dn_subdir.length; i++) 
       {
          total += dir_err_sum[dn_subdir[i]];
       }
       
       document.all["dir" + dirindex].innerText = total;      
       if (show_dirs[dirindex])
       {
          if (total == 0) 
          {
             document.all["dirline" + dirindex].style.display = "none";
          } 
          else 
          { 
             document.all["dirline" + dirindex].style.display = "";
          }        
       }
    }  
  }

  function hide_dir(dir_index, hide_this_dir)
  {
    var i;
    var dn_subdir = dirdl[dir_index];
    var dn_file   = dirfl[dir_index];    
    show_dirs[dir_index] = 0;

    if (hide_this_dir == 1) 
    {
       document.all["dirline" + dir_index].style.display = "none";
    } 
    else
    {
       document.all["dirpl" + dir_index].innerText = "+";       
    }
     
    for (i = 0; i < dn_file.length; i++)
    {
       show_files[dn_file[i]] = 0;
       document.all["fileline" + dn_file[i]].style.display = "none";
    }   
    for (i = 0; i < dn_subdir.length; i++) 
    {    
       hide_dir(dn_subdir[i], 1);      
    } 
  }
  
  function show_dir(dir_index)
  {
    var i;
    var dn_subdir = dirdl[dir_index];
    var dn_file   = dirfl[dir_index];       
    show_dirs[dir_index] = 1;
    
    document.all["dirline" + dir_index].style.display = "";
    document.all["dirpl" + dir_index].innerText = "-";    
    for (i = 0; i < dn_file.length; i++)
    {
       show_files[dn_file[i]] = 1;
       if (file_err_sum[dn_file[i]] != 0)
       {
          document.all["fileline" + dn_file[i]].style.display = "";
       }
    }   
    for (i = 0; i < dn_subdir.length; i++) 
    {
       show_dir(dn_subdir[i]);
    }  
  }
  

  function show_hide_dirs(dir_index) 
  {
    var codepane = top.frames["code-pane"];
    var filenamepane = top.frames["filename-pane"];
    
    codepane.location.href= "about:blank";
    filenamepane.change_file_name(""); 
    if (show_dirs[dir_index] == 0) 
    {
       show_dir(dir_index); 
    }
    else
    {
       hide_dir(dir_index, 0);
    }
  } 


</script>

</HEAD>

<body leftmargin=2 rightmargin=0 onload="compute_err_counts(-1);">
<font size=2>
<nobr>
<h2>File List</h2>

EndOfHeader

print "\nGenerating HTML files.";

#
# Reset the global error counts
#
@global_error_count = ();
for ($i=0; $i < @new_error_info; $i++) 
{
   push(@global_error_count, 0);
}

#
# Recursively generate the references for all the directories and files
#
$file_index = 0;
foreach $top_level_dir (@Top_Dirs)
{
   $file_index = &SpewDirectoryRefs($top_level_dir, $file_index, "", 1);
}

# 
# Create the lists the JavaScript will be using
#
print GLOBALHTML "\n<script>var filelist =[";
for ($i = 0; $i < $file_index; $i++)
{
   print GLOBALHTML "file$i";
   if (($i + 1) != $file_index)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], file_err_sum =[";
for ($i = 0; $i < $file_index; $i++)
{
   print GLOBALHTML "$File_Error_Sums[$i]";
   if (($i + 1) != $file_index)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], file_parent =[";
for ($i = 0; $i < $file_index; $i++)
{
   print GLOBALHTML "$File_Parents[$i]";
   if (($i + 1) != $file_index)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], show_files =[";
for ($i = 0; $i < $file_index; $i++)
{
   print GLOBALHTML "1";
   if (($i + 1) != $file_index)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], dirfl =[";
$dir_len = (keys %Dir_Content_Files);
for ($i = 0; $i < $dir_len; $i++)
{
   print GLOBALHTML "dirf$i";
   if (($i + 1) != $dir_len)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], dirdl =[";
for ($i = 0; $i < $dir_len; $i++)
{
   print GLOBALHTML "dird$i";
   if (($i + 1) != $dir_len)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], dir_err_sum =[";
for ($i = 0; $i < $dir_len; $i++)
{
   print GLOBALHTML "$Dir_Error_Sums[$i]";
   if (($i + 1) != $dir_len)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "], show_dirs =[";
for ($i = 0; $i < $dir_len; $i++)
{
   print GLOBALHTML "1";
   if (($i + 1) != $dir_len)
   {
      print GLOBALHTML ", ";
   }
}

print GLOBALHTML "]; </script>\n</body></html>\n";
close GLOBALHTML;

&SpewColeReport();  ## Writes ColeReport.htm, which is all boilerplate.
&SpewMiscData();    ## Writes cole-misc.htm, more boilerplate.
&SpewNameFile();    ## Writes cole-fname.htm. It holds the name of the file in the source window.
&SpewColeOptions(); ## Writes cole-option.htm, which is mostly Javascript code.
&SpewColeOptionExplantion(); ## Writes cole-error-classes.htm, which is just text.


# ******************** SUBROUTINES *******************************

sub SpewColeOptions {
open (MISCHTML, ">cole-option.htm") || die("Can't open misc HTML file \"cole-option.htm\"");
print MISCHTML <<"EndOfHeader";
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<TITLE>Cole options</TITLE>
  
<script language="JavaScript">
  function realSoonNow() {
    var expDate = new Date();
    expDate.setDate(15+expDate.getDate());
    return expDate.toGMTString();
  }
  function onck() { 
    var box = event.srcElement;
    // alert("|"+escape(box.id) + "|=" + (box.checked ? "1" : "0") + ";");
    // If the event is not from a check box, ignore it.
    if (box.id.substring(0,2) == "ck") {
      var boxNumber = box.id.substring(2,box.id.length) - 0;
      top.document.cookie = escape(box.id) + "=" + (box.checked ? "1" : "0") + "; expires=", realSoonNow(), ";";
      top.ckHide[boxNumber] = !box.checked;
      // alert("Box = "+box.id+"\\nCookie = "+top.document.cookie+"\\nHide = "+ (top.ckHide[boxNumber] ? "TRUE" : "FALSE"));
      var targ = top.frames["Errors-pane"];
      if (targ.location != "about:blank")
        targ.restyle();
      targ = top.frames["files-pane"];
      if (targ.location != "about:blank")
        targ.compute_err_counts(boxNumber);
      }
  }

</script>
</head>

<body bgcolor=#FFFFFF onload="reloadHook();">
<form name="options">
<table onclick="onck()">
  <caption>
    <em>Show which classes of errors?</em><br>
    <a href="cole-error-classes.htm" target="code-pane">(Explain Error Classes)</a>
  </caption>

EndOfHeader
&SpewErrorTableEntries(\*MISCHTML);
print MISCHTML <<"EndOfHeader";
</table>

<script language="JavaScript">

function reloadHook() {
  var ck;
  var inp;
  for (var i = 0; i < $NumErrorClasses; i++) {
       inp = document.options["ck"+i];
       if (top.ckHide[i]) {inp.checked = false;}
       else {inp.checked = true;}
  }
}

</script>

</html>
EndOfHeader
close MISCHTML;
}

sub SpewDirectoryRefs {

   my($dir)        = $_[0];
   my($dir_index)  = $Dir_Index{$dir};
   my($file_index) = $_[1];
   my($tab_string) = $_[2];
   my($top_level)  = $_[3];
   my($i, $dirlen, $subdir);

   #
   # Print the reference for this directory
   #
   print GLOBALHTML "<a target=\"Errors-pane\" id=dirline$dir_index onclick = show_hide_dirs($dir_index)";
   print GLOBALHTML " href=\"about:blank\">$tab_string<span id=dirpl$dir_index>-</span> $dir (<span id=dir$dir_index></span> issues)<br></a>\n";
   print GLOBALHTML "<script>var dird$dir_index = [";
   
   $dirlen = $#{ $Dir_Content_Subdirs{$dir} } + 1;

   for ($i = 0; $i < $dirlen; $i++)
   {
      print GLOBALHTML "$Dir_Index{$Dir_Content_Subdirs{$dir}[$i]}";
      if (($i + 1) != $dirlen)
      {
         print GLOBALHTML " ,";
      } 
   }
   print GLOBALHTML "], dirf$dir_index = [";
   
   $dirlen = $#{ $Dir_Content_Files{$dir} } + 1;
   for ($i = 0; $i < $dirlen; $i++)
   {
      print GLOBALHTML $file_index+$i;
      if (($i + 1) != $dirlen)
      {
         print GLOBALHTML " ,";
      } 
   }   
   print GLOBALHTML "]</script>\n";

   #
   # Increase the tab level for files and subdirs
   #
   $tab_string = $tab_string . "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp";

   #
   # Print references for the files
   #
   foreach $filename (@{ $Dir_Content_Files{$dir} })
   {
      $errFilename       =  &sourcePath2otherPath($filename, ".err");
      $linkedErrFilename =  &sourcePath2otherPath($filename, "_err.htm");
      $htmFilename       =  &sourcePath2otherPath($filename, ".htm");
      
      local(@errors);
      @errors = (split(/\0/, $EM_array{$filename}));
      
      #
      # The error messages are not tagged with a filename.
      #
      $compressed = 1;  
      @errors = sort by_SFA @errors;

      my(@unique_error_count) = ();
      for ($i=0; $i < @new_error_info; $i++) 
      {
         push(@unique_error_count, 0);
      }
      
      local($prior_row, $prior_column, $prior_message);
      $prior_row = -1;
      my($row, $column, $errorclass, $link, $message);
      open (ERRFILE, ">$errFilename") || 
           die("Can't open output Cole error file $errFilename for $filename");
  
      print ERRFILE "Compressed error file for $filename\n";
      foreach $error_message (@errors) 
      {
         #
         # remove duplicate error messages.  Don't consider link when calculating duplicates.
         # Fix: remove duplicates before sorting to make sorting go faster.
         #
         ($row, $column, $errorclass, $link, $message) = split(/\|/,$error_message);
         if ($row != $prior_row || $column != $prior_column || $prior_message ne $message) 
         {
            print ERRFILE $error_message, "\n";
            $prior_message = $message;
            $prior_row = $row;
            $prior_column = $column;
            $unique_error_count[$errorclass]++;
         }
      }
      close ERRFILE;

      #
      # avoid stupid problem of 0 printing as the null (empty) string.
      #
      print GLOBALHTML "<a target=\"Errors-pane\" id=fileline$file_index href=\"$linkedErrFilename\">";
      print GLOBALHTML "$tab_string$filename (<span id=file$file_index></span> issues)<br></a>\n";
      print GLOBALHTML "<script>var file$file_index = [";
  
      for ($i=0; $i < $#new_error_info; $i++) 
      {
         print GLOBALHTML "$unique_error_count[$i], ";
         $File_Error_Sums[$file_index] += $unique_error_count[$i];
      }
      print GLOBALHTML "$unique_error_count[$#unique_error_count]]</script>\n";
      $File_Error_Sums[$file_index] += $unique_error_count[$#unique_error_count];
      
      for ($i=0; $i < @new_error_info; $i++) 
      {
         $global_error_count[$i] += $unique_error_count[$i];
      }

      #
      # because we don't generate the linked error file on demand, we do it eagerly now.
      #
      print "E";
      &ErrorFile2LinkedErrorFile($errFilename);

      #
      # because we don't generate the html file on demand, we do it eagerly now.
      #
      print "S";
      &ErrorFile2HTMFile($errFilename);
      
      #
      # Update the File_Parents and Dir_Error_Sums arrays
      #
      $File_Parents[$file_index] = $dir_index;
      $Dir_Error_Sums[$dir_index] += $File_Error_Sums[$file_index];
      $file_index++;
   }
      
   #
   # Recursively call each of the subdirs
   #
   foreach $subdir (@{ $Dir_Content_Subdirs{$dir} })
   {
      $file_index = &SpewDirectoryRefs($subdir, $file_index, $tab_string, 0);
   }
   
   return $file_index; 
}

sub SpewColeReport {
open (GLOBALHTML, ">ColeReport.htm") || die("Can't open HTML file \"ColeReport.htm\"");

print GLOBALHTML <<"EndOfHeader";
<HTML>
<HEAD>
<TITLE>Porting Report from Cole</TITLE>

<script language="JavaScript">
  // master list of what errors are hidden
  var ckHide = new Array();
EndOfHeader
for ($i=0; $i < @new_error_info; $i++) {printf GLOBALHTML "  ckHide[$i] = false\n";}
print GLOBALHTML <<"EndOfHeader";

  function loadCookie() {
    // alert("Cookie = "+document.cookie);
    var cookieList  = document.cookie.split("; ");
    for (var i = 0; i < cookieList.length; ++i) {
        var namepair = cookieList[i].split("=");
        var name = unescape(namepair[0]);
        var value = unescape(namepair[1]);
        if (name.substring(0,2) == "ck") {
           var index = name.substring(2,name.length) - 0;
           ckHide[index] = ("0"==value);
       }
    }
  }
  
//  loadCookie();
</script>

</HEAD>

<frameset rows="50,*" FRAMEBORDER=no border=0>
  <frame SRC="cole-misc.htm" name="tool-pane" FRAMEBORDER=0 SCROLLING=no>
  <FRAMESET COLS="300,*" FRAMEBORDER=yes BORDER=0 bordercolor=blue>
    <FRAMESET rows="3*,3*" frameborder=0>
      <FRAME SRC="Cole-Files.htm" NAME="files-pane" FRAMEBORDER=1 SCROLLING=AUTO>
      <FRAME                      NAME="Errors-pane" FRAMEBORDER=1 SCROLLING=AUTO>
    </FRAMESET>
    <frameset rows="50,*" frameborder=0>
      <frame src="cole-fname.htm"  name="filename-pane" frameborder=1 scrolling=no>
      <FRAME                       NAME="code-pane" FRAMEBORDER=1 SCROLLING=AUTO>
    </FRAMESET>
  </FRAMESET>
</frameset>

</HTML>
EndOfHeader

close GLOBALHTML;
}

sub SpewColeOptionExplantion {
#
# Write out a bunch of text explaining error messages.
#
open (EXPLAINHTML, ">cole-error-classes.htm") ||
      die("Can't open error explanation file \"cole-error-classes.htm\"");
print EXPLAINHTML <<"EndOfHeader";
<HTML>
<HEAD>
<TITLE>Cole Error Class Explanation</TITLE>
</HEAD>
<body>
<h1>
Cole Error Class Explanation
</h1>
<ul>
  <li> <b>P32 failures, code will break under a P32 model:</b>
    This includes, for example, casting a 64 bit integral value to a pointer value.
  </li>
  <li> <b>P32 suspicious code fragment:</b>
    This fragment is OK for P32, but there may be platform configuration issues Cole cannot
    detect.  For example, casting a pointer to a 64 bit integral value (where the
    type of the value is something other than INT_PTR, LONG_PTR, etc).  
  </li>
  <li> <b>P64 failures, code will break under a P64 model:</b>
    This includes, for example, casting a pointer to a 32 bit integral value.
  </li>
  <li> <b>P64 suspicious code fragment:</b>
    This fragment is OK for P64, but indicative of a problem elsewhere in the code,
    for example, casting a 32 bit integral value to a pointer (where
    the type of the 32 bit value is something other than INT_PTR, LONG_PTR, etc.).
  </li>
  <li> <b>General programming warnings and errors:</b>
    This includes warnings on wrong usage of format directives in the printf format string,
    checks for unsigned values polluting a signed arithmetic expression etc.
  </li>
  <li> <b>The other errors are self-explanatory.</b>
  </li> 
</ul>
<p>
If you are running Cole over code that will only be run under P64 (aka LLP64), turn off
the reporting for P32 failures and P32 suspicious code fragments.
</p>
<p>
If you are running Cole over code that will only be run under both P32 (aka ILP32) and P64 
(aka LLP64), leave on all P32 and P64 error reporting.
</p>
</body>
</HTML>
EndOfHeader
close EXPLAINHTML;
}


sub SpewMiscData {
#
# Write data to display cole options
#
open (MISCHTML, ">cole-misc.htm") || die("Can't open misc HTML file \"Cole-misc.htm\"");
print MISCHTML <<"EndOfHeader";
<HTML>
<HEAD>
<TITLE>Porting Report from Cole</TITLE>
</HEAD>
<body style="margin-top:0;background-color:yellow">
<h1 style="background-color:yellow;font-size:30">
Cole Visual Report 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  
<a href="cole-option.htm" target="code-pane" style="font-size:60%">(Error Display Options)</a>
</h1>
</body>
</HTML>
EndOfHeader
close MISCHTML;
}

sub SpewNameFile {
#
# The name file 
#
open (MISCHTML, ">cole-fname.htm") || die("Can't open misc HTML file \"Cole-fname.htm\"");
print MISCHTML <<"EndOfHeader";
<HTML>
<HEAD>
<TITLE>Porting Report from Cole</TITLE>

<script language="JavaScript">

  function change_file_name(new_name) 
  {
     document.all("filenametarget").innerText = new_name;
  }

</script>

</HEAD>
<body style="background-color:yellow">
<span style="font-size:30" id=filenametarget>
</span>
</body>
</HTML>
EndOfHeader
close MISCHTML;
}
