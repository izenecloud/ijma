//only debug in the browser (without server)
var clientDebugMode = false;
if(window.location.href.indexOf("file:") == 0)
	clientDebugMode = true;