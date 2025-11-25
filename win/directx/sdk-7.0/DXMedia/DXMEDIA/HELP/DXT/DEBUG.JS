// Debugging Library 
 
// Outputs a string. Assumes txtStatus is a TEXTAREA
function Trace(cStatusText)
{
	if ((typeof(_debug_txtStatus) != 'object') || _debug_txtStatus.tagName != 'TEXTAREA')
		return;

/*
	if(typeof(x) != 'string')
	{
		x = '_debug_txtStatus';
	}
	else
	{
		alert(x);
	}
*/

	_debug_txtStatus.value = _debug_txtStatus.value + cStatusText + '\n';
}

// Initialize the debugger by dropping a TEXTAREA into the document
// Call this before calling Trace.
function InitDebug(iHeight, iWidth)
{
	var oColl = document.all('_debug_txtStatus');
//	if (oColl.tagName != 'undefined' || oColl.length > 0)
	if (oColl != null)
	{
		alert('Debug Error: Object with ID/NAME "_debug_txtStatus" already exists on the page');
		return;
	}

	if (InitDebug.arguments.length == 0)
	{
		iHeight = 300;
		iWidth = 300;
	}

	document.body.insertAdjacentHTML('beforeEnd', '<P><TEXTAREA STYLE="height:' + iHeight + ';width:' + iWidth + '" ID=_debug_txtStatus></TEXTAREA>');
}

function Debugger(iTop, iLeft, iHeight, iWidth)
{
	this.m_sTraceWindow = '_debug_txtStatus';
	this.Trace = Trace;

	var oColl = document.all(this.m_sTraceWindow);
	if (oColl != null)
	{
		alert('Debug Error: Object with ID/NAME "_debug_txtStatus" already exists on the page');
		return;
	}

	if (Debugger.arguments.length < 4)
	{
		sStyle = 'height:300;width:300';
	}
	else
	{
		sStyle = 'position:absolute;top:' + iTop + ';left:' + iLeft + ';height:' + iHeight + ';width:' + iWidth;
	}

	sDebugWindow = '<P><TEXTAREA STYLE="' + sStyle + '" ID=' + this.m_sTraceWindow + '></TEXTAREA>';
	document.body.insertAdjacentHTML('beforeEnd', sDebugWindow);

	Trace('Debugger Initialized...');
//	Trace.x = 'foobar';
}