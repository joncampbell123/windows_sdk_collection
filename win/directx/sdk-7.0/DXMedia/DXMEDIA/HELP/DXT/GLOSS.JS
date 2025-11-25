//General variables
var g_win = new Object();	// Window Object for LoadImage()
var g_counter = 0;		// Slowdown counter
var g_sFileName = ''		// Glossary file name to load
var ID = 0;			// Timeout ID

//Slowdown open window process
function SlowDown_OpenWindow(){
    if(typeof(ID) !='undefined') {window.clearTimeout(ID)};
    if (g_counter < 1) {
	g_counter++;
	ID = window.setTimeout("SlowDown_OpenWindow()",175);
    }
    else {
	g_counter = 0;
	OpenWindow();
    }
}

//Open floating window
function OpenWindow() {
    var s = 'width=300,height=450,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes';
    g_win = window.open(g_sFileName, 'GLOS_SARY', s);
}

function ShowHelp(path){
    window.showHelp(path);
}

//Close down the floating window
function CloseWindow(){
    //IE4
    if ((g_isIE == true) && (g_iMaj >=4)) {
		self.g_win.close();
    }
	
    //IE3
    if ((g_isIE == true) && (g_iMaj < 4)) {
        //IE skips this step,
    }
	
    //NAVIGATOR 
    if (g_isNav == true){ 
	if(g_win.closed == true){} else{g_win.close();}
    }
    
    //OTHER
    if(g_win.closed == true){} else{g_win.close();}
}

/********************************/
function Glossary(path) { 
	g_sFileName = path;
	//IE4
    if ((g_isIE == true) && (g_iMaj >=4)) {
	lnkOver(path);
    }

    //NAVIGATOR
    if (g_isNav == true) {
        if (typeof(g_win.closed) == 'undefined') {
	    OpenWindow()
	 }
	 else{
	    CloseWindow();
	    SlowDown_OpenWindow();
	 }
     }

    //IE3, open window now and skip. Never close the window.
    if ((g_isIE == true) && (g_iMaj < 4 )) {
	OpenWindow(); 
    }
    
    //OTHER
    if ((g_isIE != true) && (g_isNav != true))
    	if (typeof(g_win.closed) == 'undefined') {
	    OpenWindow()
    	}
    	else{
    	    CloseWindow();
	    SlowDown_OpenWindow()
	    }
}
