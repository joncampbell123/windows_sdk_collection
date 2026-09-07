var tickDuration;
tickDuration = 50;

var activeObjectCount;
var activeObjects;
var itemDeactivated;

var tickGeneration;

activeObjects = new Array();
activeObjectCount = 0;
timerRefcount = 0;
itemDeactivated = false;

tickGeneration = 0;

function initializePath(e) {
 e.waypointX = new Array();
 e.waypointY = new Array();
 e.duration = new Array();

}

function addWaypoint(e, number, x, y, duration) {
 e.waypointX[number] = x;
 e.waypointY[number] = y;
 e.duration[number] = duration;
}

function compact() {
  var i, n, c;

  n = new Array();
  c = 0;
  itemDeactivated = false;
  for (i=0; i<activeObjectCount; i++)  {
     if (activeObjects[i].active == true) {
        n[c] = activeObjects[i];
        c++;
     }
  }

  activeObjects = n;
  activeObjectCount = c;
}

function tick(generation) {

  if (generation < tickGeneration) {
    // alert("Error "+generation);
     return;
  }

  //alert("tick: "+generation);

  if (itemDeactivated)
     compact();

  if (activeObjectCount == 0) {
     return;
  }
  else {
    for (i=0; i<activeObjectCount; i++) {
      moveElement(activeObjects[i]);
    }

    window.setTimeout("tick("+generation+");", tickDuration);
  }
}

function start(e) {
  if (itemDeactivated)
     compact();

  activeObjects[activeObjectCount] = e;
  activeObjectCount++;

  if (activeObjectCount == 1) { 
    tickGeneration++;
    tick(tickGeneration);
  }
}

function runWaypoint(e, startPoint, endPoint) {

  var startX, startY, endX, endY, duration;

  if (e.waypointX == null)  
    return;   

  startX = e.waypointX[startPoint];
  startY = e.waypointY[startPoint];
  endX = e.waypointX[endPoint];
  endY = e.waypointY[endPoint];

  duration = e.duration[endPoint];
  e.ticks = duration / tickDuration;

  e.endPoint = endPoint;
  e.active = true;
  e.currTick = 0;

  e.dx = (endX - startX) / e.ticks;
  e.dy = (endY - startY) / e.ticks;

  e.style.posLeft = startX;
  e.style.posTop = startY;

  start(e);
}  

function moveElement(e) {
  e.style.posLeft += e.dx;
  e.style.posTop += e.dy;

  e.currTick++;

  if (e.currTick > e.ticks) {
    e.active = false;
    itemDeactivated = true;
    if (e.onpathcomplete != null) {
       window.pathElement = e;
       e.onpathcomplete()
    }
  }
}
