function createRequest() {
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
	}
}

function uploadXml(xmlstring){
	createRequest();
	if(!request)
		return;
	var url = "docLogic.php?doc=save";
    request.open("POST", url, true);
	request.onreadystatechange = finishUploadXml;
    request.send(xmlstring);
}

function finishUploadXml(){
	if (request.readyState == 4){
       if (request.status == 200){
			var ret = request.responseText;
			if(ret.length > 0){
				alert("Error: "+ret);
			}
		}else{
			alert("Server Error Code:"+request.status);
		}
	}
}

function downloadloadXml(id){
	createRequest();
	if(!request)
		return;
	var url = "docLogic.php?doc=load&id="+id;
    request.open("POST", url, true);
	request.onreadystatechange = finishDownloadXml;
    request.send(null);
}

function finishDownloadXml(){
	if (request.readyState == 4){
       if (request.status == 200){
			var ret = request.responseText;
			if(ret.length > 0 && ret.charAt(0) != '<'){
				alert("Server Error Message: "+ret);
			}else{
				loadXmlStringToComp(ret);
			}
		}
	}
}
