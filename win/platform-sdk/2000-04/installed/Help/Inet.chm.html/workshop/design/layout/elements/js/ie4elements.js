var LMExportDict = null;

function LMErrorHandler()
{
   window.onerror=null; // set back to use the default error handler
   return true; // don't show an error dialog
}

function triggerLMEvent( engine, bvr )
{
   if ( engine == null ) return;
   var trigger = getLMExportBvr( engine, bvr );
   if ( trigger != null )
      DAControlelements.MeterLibrary.TriggerEvent( trigger, trigger );
}

function LMExitEnterHandler()
{
   if ( LMExportDict != null )
   {
      var i;
      for ( i = 1; i < LMExportDict.length; i += 4 )
         triggerLMEvent( LMExportDict[i], "ExitEnterControlEvent" );
   }
}

function setLMExportsAreDone(ctl)
{
   var i;
   for ( i = 0; i < LMExportDict.length; i += 4 )
   {
      if ( LMExportDict[i] == ctl )
      {
         LMExportDict[i + 3] = true;
         return;
      }
   }
}
 
function loadelements()
{
   if ( document.readyState == "complete" )
   { 
      DAControlelements.UpdateInterval = 0.0333; // Max frame rate of 30 fps

      var da40 = DAControlelements.MeterLibrary.VersionString == "5.01.15.0828" ? true : false;
      if ( !da40 )
      {
         // IE4.01 or later
         if ( navigator.appVersion.indexOf("Windows 95") > 0 )
            DAControlelements.TimerSource = 1;   // Use Trident Timer on Win95
      }

      LMReaderelements.NoExports=da40;
      LMReaderelements.Async=!da40;
 
      var dictEntry = new Array( DAControlelements, null, exportBvrIDselements, false );
      if ( LMExportDict == null )
         LMExportDict = dictEntry;
      else
         LMExportDict = LMExportDict.concat( dictEntry );
 
      window.onerror=LMErrorHandler;
      LMEngineelements = LMReaderelements.execute( "elements.x" );
      window.onerror=null; // set back to use the default error handler
 
      LMExportDict[LMExportDict.length - 3] = LMEngineelements;
   }
   else
   {
      setTimeout( "loadelements()", 50, "JavaScript" );
   }
}

var exportBvrIDselements = new Array(
   "ExitEnterControlEvent");
 
function getLMExportBvr(engine, id)
{
   var exportBvrIDs = null;
   var i;
   var j;
 
   if ( LMExportDict == null ) return null;
   for (i = 1; i < LMExportDict.length; i += 4)
   {
      if ( LMExportDict[i] == engine )
      {
         if ( !LMExportDict[i + 2] )
            return null;
         exportBvrIDs = LMExportDict[i + 1];
         break;
      }
   }
   if ( exportBvrIDs == null )
      return null;
   for (i = 0; i < exportBvrIDs.length; i++)
   {
      if (exportBvrIDs[i] == id)
      {
         var exports = engine.getBehavior( "Exports",
                                           new ActiveXObject("DirectAnimation.DAPair") );
         for (j = 0; j < i; j++)
            exports = exports.second;
         return exports.first;
      }
   }
   return null;
}

document.write( '<OBJECT ID="DAControlelements" STYLE="Width:420;Height:324"' );
document.write( '  CLASSID="CLSID:B6FFC24C-7E13-11D0-9B47-00C04FC2F51D">' );
document.write( '</OBJECT>' );
document.write( '<OBJECT' );
document.write( '   ID="LMReaderelements" STYLE="Display:none"' );
document.write( '   CLASSID="CLSID:183C259A-0480-11D1-87EA-00C04FC29D46">' );
document.write( '</OBJECT>' );
 
loadelements();
