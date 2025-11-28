	Author:                 Calvin Hsia
	emailname:      v-calvh
	Creation Date:  9/28/94
	Description:
		This includes both the formatting and the documenting wizards.
		There will not be a 3.0 version of Foxdoc.
		
	To run the app, you need WZFOXDOC.APP

       All the CPP and HPP files are the source code.
       FDKeywrd is the table of keywords.


The code analyzer is a tool to allow users to navigate through their Foxpro programs.

It takes the output from the formatting wizard and presents it in an interactive way. It consists of a single form with a 2 page pageframe.

The first page has a listbox displaying all user symbols, and a grid that shows where the currently selected listbox symbol exists within your code. For example, if there's a symbol FOO in your code, you can choose FOO in the listbox, and the grid below will show in which file there is a reference to this symbol, whether it's in a program, menu file, screen file, class library, or database container. If you double-click on the item in the grid, the referenced file is opened for your perusal.

There's an Analyzer menu pad which allows you to navigate around your code. Hit Ctrl-N to go to the next occurrence of FOO. While looking in a VCX, you see a line:

	FOO=BAR

Highlight the BAR with your cursor, then hit Ctrl-D to go to the definition of BAR. Ctrl-R will go to any references.

The second page has an outline control that will display either the calling tree hierarchy of your program or the class hierarchy.

