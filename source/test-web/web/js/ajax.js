function createRequest() {
	var request = null;
	try {
		request = new XMLHttpRequest();
	} catch (trymicrosoft) {
		try {
			request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (othermicrosoft) {
			try {
				request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (failed) {
				request = false;
			}
		}
	}
	
	if (!request){
		alert("Error initializing XMLHttpRequest!");
		return null;
	}
	return true;
}

function uploadXml(xmlstring, callbackFun){
    var request = createRequest();
	if(!request)
		return;
	var url = "jmaslxml";
    request.open("POST", url, true);
	request.onreadystatechange = function() {finishUploadXml(request, callbackFun);};
    request.send(xmlstring);
}

function finishUploadXml(request, callbackFun){
	if (request.readyState == 4){
		if (request.status == 200){
			var ret = request.responseText;
			if(ret.length > 0){
				alert("Error: "+ret);
			}else{
				if(callbackFun != undefined){
					callbackFun();
                }
			}
		
		}else{
			alert("Server Error Code:"+request.status);
		}
	}
}

function downloadXml(id){
	var request = createRequest();
	if(!request)
		return;
	var url = "jmaslxml?id="+id;
    request.open("POST", url, true);
	request.onreadystatechange = function() {finishDownloadXml(request);};
    request.send(null);
}

function finishDownloadXml(request){
	if (request.readyState == 4){
       if (request.status == 200){
			var ret = request.responseText;
			if(ret.length > 0 && ret.charAt(0) != '<'){
				alert("Server Error Message: "+ret);
			}else{
                //alert("finish download");
                loadXmlStringToComp(ret);
			}
		}
	}
}
