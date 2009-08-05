//only debug with client (no server)
var clientDebugMode = false; 

var defaultstring = "<?xml version=\"1.0\" encoding=\"utf-8\"?><jmacomp></jmacomp>";

var errorBgColor = "red";

var docId;

var lastSaveTime = null;

var modified = false;

var checkInterval = 30000; //in milliseconds

Array.prototype.indexof = function(obj) {
  var i = this.length;
  while (i--) {
    if (this[i] === obj) {
      return i;
    }
  }
  return -1;
}

function fireOnLoading()
{
	$('.saveAllBtn').attr('disabled', 'disabled');
	lastSaveTime = null;
	modified = false;
}

function fireUserEdit(isEdit)
{
	if(modified == isEdit)
		return;
	modified = isEdit;
	if(isEdit)
	{
		$(".saveAllBtn").attr('disabled', '');
	}
	else
	{
		$(".saveAllBtn").attr('disabled', 'disabled');
		lastSaveTime = new Date();
		updateSaveAllPromt();
	}
}

function updateSaveAllPromt()
{
	//update the last modified time
	if(lastSaveTime == null)
		$('.saveAllPromt').html('');
	else
	{
		var minutes = parseInt(((new Date()).getTime() - lastSaveTime.getTime())/60000);
		//var minutes = parseInt(((new Date()).getTime() - lastSaveTime.getTime())/1000);
		$('.saveAllPromt').html('(saved ' + minutes + ' minutes ago)');
	}
}

function onTimeoutCheck()
{
	if(modified)
	{
		saveAllChange();
	}
	
	updateSaveAllPromt();
	setTimeout(onTimeoutCheck, checkInterval);
}

function copyOrig(ele)
{
	var rootNode = ele.parentNode.parentNode.parentNode;
	var userInput = getFirstEleByIdItr(rootNode, "userInput");
	var origValue = getFirstEleByIdItr(rootNode, "origText").innerHTML;
	if(origValue == userInput.value)
		return;	
	if(userInput.value.length > 0)
	{
		if(!confirm("Are you sure to Overwrite the current Suggestion ?"))
			return;
	}
	
	userInput.value = origValue;
	//$("#"+id+" #userInput").val($("#"+id+" #origText").text());
}

function clearUserInput(ele)
{
	var rootNode = ele.parentNode.parentNode.parentNode;
	var userInput = getFirstEleByIdItr(rootNode, "userInput");
	if(userInput.value.length > 0)
	{
		if(confirm("Are you sure to Clear the current Suggestion?"))
			userInput.value = "";
	}
}

function gup( name )
{
	name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
	var regexS = "[\\?&]"+name+"=([^&#]*)";
	var regex = new RegExp( regexS );
	var results = regex.exec( window.location.href );
	if( results == null )
		return null;
	else
		return results[1];
}

function copyUrlToClipboard(s)
{
	if( window.clipboardData && clipboardData.setData )
	{
		clipboardData.setData("Text", s);
		alert("Copy to Clipboard Done!")
	}
	else
	{
		alert("Fail to copy to Clipboard Done!, please copy the url directly");
	}
}

function getSurroundedString(str, idx, radius)
{
	var begin = idx - radius;
	if(begin < 0)
		begin = 0;
	var end = idx + radius;
	if(end > str.length)
		end = str.length;
	return str.substring(begin, end);
}

function applyUserInput(ele)
{
	if(!confirm("Are you sure to Apply the current Suggestion?"))
		return;
	var rootNode = ele.parentNode.parentNode.parentNode;
	var userInput = getFirstEleByIdItr(rootNode, "userInput");
	var origValue = getFirstEleByIdItr(rootNode, "origText").innerHTML;
	
	var userValue = userInput.value;
	
	//validate the user input, only extra 
	var i = j = 0;
	for( ; i < userValue.length; ++i )
	{
		if(userValue[i] == " ")
			continue;
		
		if(j == origValue.length)
		{
			alert("Invalid User Input(Non-Space) \'" + userValue[i] + "\' at index " + i + " (" + getSurroundedString(userValue, i, 4) + ")");
			return;
		}
				
		
		while(origValue[j] == " ")
			++j;
		
		if(userValue[i] == origValue[j])
		{
			j++;
			continue;
		}
		alert("Invalid User Input(Non-Space) \'" + userValue[i] + "\' at index " + i + " (" + getSurroundedString(userValue, i, 4) + ")");
		return;
	}
	
	if(j != origValue.length)
	{
		alert("Missing the end of the original value: " + origValue.substring(j));
		return;
	}
	
	//apply the input
	var tokens = userValue.tokenize(" ", true, true);
	
	var lenArray = new Array(tokens.length);
	var singleLenArray = new Array(tokens.length);
	var lenTotal = 0;
	for(var i=0; i<tokens.length; ++i)
	{
		lenTotal += tokens[i].length;
		lenArray[i] = lenTotal;
		singleLenArray[i] = tokens[i].length;
	}
	
	var unitDiffers = getFirstEleByClassItr(rootNode, 'unitdiffers');
	var differBlocks = unitDiffers.childNodes;
	
	lenTotal = 0; //reset lenTotal here
	for(var j=0; j<differBlocks.length; ++j)
	{
		var differChild = differBlocks[j];
		if(differChild.className != "unitdifferblock")
			continue;
		var differBlock = differBlocks[j];
		var singleBlock = getFirstEleByClass(differBlock, 'unitdiffsingle');
		if(singleBlock != null)
		{
			var text = singleBlock.innerHTML;
			lenTotal += text.length;
			var idx = lenArray.indexof(lenTotal);
			if(idx >= 0 && singleLenArray[idx] == text.length)
				singleBlock.style.backgroundColor = "";
			else
				singleBlock.style.backgroundColor = errorBgColor;
			continue;
		}
		
		var upBlock = getFirstEleByClass(differBlock, 'unitdifferup');
		var downBlock = getFirstEleByClass(differBlock, 'unitdifferdown');
		
		var smallChildren = upBlock.childNodes;
		var tmpTotal = lenTotal;
		for(var k=0; k<smallChildren.length; ++k)
		{
			var smallChild = smallChildren[k];
			if(smallChild.className != "unitdiffsmall")
				continue;
			var text = smallChild.innerHTML;
			tmpTotal += text.length;
			var idx = lenArray.indexof(tmpTotal);
			if(idx >= 0 && singleLenArray[idx] == text.length)
				smallChild.style.backgroundColor = "";
			else
				smallChild.style.backgroundColor = errorBgColor;
		}
		
		smallChildren = downBlock.childNodes;
		for(var k=0; k<smallChildren.length; ++k)
		{
			var smallChild = smallChildren[k];
			if(smallChild.className != "unitdiffsmall")
				continue;
			var text = smallChild.innerHTML;
			lenTotal += text.length;
			var idx = lenArray.indexof(lenTotal);
			if(idx >= 0 && singleLenArray[idx] == text.length)
				smallChild.style.backgroundColor = "";
			else
				smallChild.style.backgroundColor = errorBgColor;
		}
	}
	
	//save the last applied user input
	var lastApplyInput = getFirstEleByIdItr(rootNode, "lastApplyUserInput");
	lastApplyInput.value = userInput.value;
	
	fireUserEdit(true);
}

function diffChange(ele)
{
	var node = ele;
	
	if(node.style.backgroundColor != errorBgColor)
		node.style.backgroundColor = errorBgColor;
	else
		node.style.backgroundColor = "";
		
	fireUserEdit(true);
}

function practiseDiffChange(ele)
{
	var node = ele;
	
	if(node.style.backgroundColor != errorBgColor)
		node.style.backgroundColor = errorBgColor;
	else
		node.style.backgroundColor = "";
}

function loadXmlStringToComp(xmlStr)
{
	var differsHtml = document.getElementById('differs');
	removeHtmlChildren(differsHtml);
	
	var xmlobject = loadXMLString(xmlStr);
	if(xmlobject == null)
		return;
	var rootNode = xmlobject.getElementsByTagName('jmacomp')[0];
	var differs = xmlobject.getElementsByTagName('differs')[0];
	
	var differsChildren = differs.childNodes;

	var totalSentences = 0;
	for(var i=0; i<differsChildren.length; ++i){
		if(differsChildren[i].tagName == "sentence")
			++totalSentences;
	}
	
	for(var i=0; i<differsChildren.length; ++i){
		var sentence = differsChildren[i];
		if(sentence.tagName != "sentence")
			continue;
		var senChildren = sentence.childNodes;
		
		var origText;
		var userInput = "";
		var unitdiffers = "";
		for(var j=0; j<senChildren.length; ++j){
			var senChild = senChildren[j];
			if(senChild.tagName == "origText")
			{
				origText = getText(senChild);
			}
			else if(senChild.tagName == "userInput")
			{
				userInput = getText(senChild);
			}
			else if(senChild.tagName == "single")
			{
				var bgColor = "";
				if(senChild.getAttribute("error") != "0")
					bgColor = "style=\"background-color:"+errorBgColor+"\"";
				unitdiffers += "<div class=\"unitdifferblock\"><div class=\"unitdiffsingle\" "+ bgColor +">" + 
						getText(senChild) +"</div></div>";
			}
			else if(senChild.tagName == "double")
			{
				var upNode = getChild(senChild, "up");
				var downNode = getChild(senChild, "down");
				
				unitdiffers += "<div class=\"unitdifferblock\"><div class=\"unitdifferup\">";
				
				var smallChildren = upNode.childNodes;
				for(var k=0; k<smallChildren.length; ++k)
				{
					var smallChild = smallChildren[k];
					if(smallChild.tagName != "small")
						continue;
					var bgColor = "";
				
					if(smallChild.getAttribute("error") != "0")
					{
						//alert("error: " + getText(senChild));
						bgColor = "style=\"background-color:"+errorBgColor+"\"";
					}
					unitdiffers += "<div class=\"unitdiffsmall\" "+ bgColor +">" + getText(smallChild) +"</div>";
				}
				
				unitdiffers += "</div><div class=\"unitdifferdown\">"; //close of the <div class="unitdifferup">
				
				smallChildren = downNode.childNodes;
				for(var k=0; k<smallChildren.length; ++k)
				{
					var smallChild = smallChildren[k];
					if(smallChild.tagName != "small")
						continue;
					var bgColor = "";
				
					if(smallChild.getAttribute("error") != "0")
					{
						//alert("error: " + getText(smallChild));
						bgColor = "style=\"background-color:"+errorBgColor+"\"";
					}
					unitdiffers += "<div class=\"unitdiffsmall\" "+ bgColor +">" + getText(smallChild) +"</div>";
				}
				
				unitdiffers += "</div></div>"; //close of the unitdifferdown and unitdifferblock;
			}
		}
		
	//++++++++++++ begin the compunit string
		var compunitStr = "<div class=\"compunit\">" + 
	"<div class=\"unitroottitle\">Sentence: " + (i+1)+"/"+totalSentences+"</div>" +
	"<div class=\"comprow\">" + 
		"<div class=\"unittitle\">Original Text: </div>" + 
		"<div class=\"unitcontent\" id=\"origText\"> " + origText + "</div>" + 
	"</div>" + 
	
	"<div class=\"compinforow\">" + 
		"<span class=\"promtInfo\"><strong>Recommended: </strong>Single click wrong segmented words (wrong words are with " +errorBgColor+ " background color) <strong>Below</strong>. Remove a wrong word by single click that word one more time.</span>" + 
	"</div>" +	
	"<div class=\"unitdiffers\">" + unitdiffers + "</div>" + 
	
	"<div class=\"compinforow\">" + 
		"<span class=\"promtInfo\"><strong>Not Recommended: </strong>Or input the correct segmentation directly, following the steps below: <ol> <li>Click &quot;Copy Origininal Text&quot; button to get the original text;<li>Separate the words with English spaces <strong>(Avoid to enter other characters and Make sure you are using English Input Method)</strong>; <li>Click &quot;Apply&quot; button to apply your input.</ol></span>" +
	"</div>" + 
	"<div class=\"comprow\">" + 
		"<div class=\"unittitle\">Suggestion: </div>" + 
		"<div class=\"unitcontent\">" + 
			//"<input type=\"text\" id=\"userInput\" size=\"60\" value=\"" + userInput + "\"/>" +
			"<textarea id=\"userInput\" cols=\"60\" rows=\"5\" wrap=\"soft\"></textarea>" +
			"<input type=\"hidden\" id=\"lastApplyUserInput\" value=\"" + userInput + "\" />" +
			"<input type=\"button\" class=\"userInputBtn\" value=\"Copy Origininal Text\" onclick=\"copyOrig(this);\"/>" + 
			"<input type=\"button\" class=\"userInputBtn\" value=\"Apply\" onclick=\"applyUserInput(this)\"/>" + 
			"<input type=\"button\" class=\"userInputBtn\" value=\"Clear\" onclick=\"clearUserInput(this);\"/>" +
		"</div>" + 
	"</div>" +
	
	"<div class=\"buttonsdiv\"><input type=\"button\" value=\"Save Changes\" class=\"saveAllBtn\"  onclick=\"saveAllChange()\"/><span class=\"saveAllPromt\"></span><a href=\"#stat\" class=\"stathref\">Statistical Information</a></div>" + 
"</div>";
	//+++++++++ end the compunit string
	
	$("#differs").append(compunitStr);
	
	}
		
	$(".unitdiffsingle,.unitdiffsmall").bind('click', function() { diffChange(this);});
	
	$('#idS').text(getXPathValue(rootNode, 'stat/id'));
	$('#fileS').text(getXPathValue(rootNode, 'stat/name'));
	var upTotal = parseInt(getXPathValue(rootNode, 'stat/upTotal'));
	var downTotal = parseInt(getXPathValue(rootNode, 'stat/downTotal'));
	var sameTotal = parseInt(getXPathValue(rootNode, 'stat/sameTotal'));
	var sameError = parseInt(getXPathValue(rootNode, 'stat/sameError'));
	var upDiffError = parseInt(getXPathValue(rootNode, 'stat/upDiffError'));
	var downDiffError = parseInt(getXPathValue(rootNode, 'stat/downDiffError'));
	updateStatInfo(upTotal, downTotal, sameTotal, sameError, upDiffError, downDiffError);
	
	fireOnLoading();
}

function appendXmlDiffBlcok(xmlDoc, father, newNodeName, text, isError)
{
	var node = createElementTextAndAppend(xmlDoc, father, newNodeName, text);
	if(isError)
		node.setAttribute("error", "1");
	else
		node.setAttribute("error", "0");
	return node;
}

function updateStatInfo(upTotal, downTotal, sameTotal, sameError, upDiffError, downDiffError)
{
	$('#upTotalS').text(upTotal);
	$('#downTotalS').text(downTotal);
	$('#samePercentS').text(((sameTotal/upTotal+sameTotal/downTotal)/2).toFixed(4));
	$('#samePrecisionS').text(((sameTotal - sameError)/sameTotal).toFixed(4));
	$('#basisPrecisionS').text(((upTotal - sameError - upDiffError)/upTotal).toFixed(4));
	$('#jmaPrecisionS').text(((downTotal - sameError - downDiffError)/downTotal).toFixed(4));
}

function saveAllChange()
{
	var differsHtml = document.getElementById('differs');
	var differsChildren = differsHtml.childNodes;
	
	var xmlDoc = loadXMLString(defaultstring);
	var xmlRoot = xmlDoc.getElementsByTagName('jmacomp')[0];
	var differsXml = createElementAndAppend(xmlDoc, xmlRoot, "differs");
	var statXml = createElementAndAppend(xmlDoc, xmlRoot, "stat");
	
	var upTotal = 0;
	var downTotal = 0;
	var sameTotal = 0;
	var sameError = 0;
	var upDiffError = 0;
	var downDiffError = 0;
	
	for(var i=0; i<differsChildren.length; ++i)
	{
		var differChild = differsChildren[i];
		if(differChild.className != "compunit")
			continue;
		var sentenceXml = createElementAndAppend(xmlDoc, differsXml, "sentence");
		var origText = getFirstEleByIdItr(differChild, 'origText').innerHTML;
		var userInput = getFirstEleByIdItr(differChild, 'lastApplyUserInput').value;
		createElementTextAndAppend(xmlDoc, sentenceXml, 'origText', origText);
		createElementTextAndAppend(xmlDoc, sentenceXml, 'userInput', userInput);
		
		var unitDiffers = getFirstEleByClassItr(differChild, 'unitdiffers');
		var differBlocks = unitDiffers.childNodes;
		for(var j=0; j<differBlocks.length; ++j)
		{
			var differChild = differBlocks[j];
			if(differChild.className != "unitdifferblock")
				continue;
			var differBlock = differBlocks[j];
			var singleBlock = getFirstEleByClass(differBlock, 'unitdiffsingle');
			if(singleBlock != null)
			{
				var text = singleBlock.innerHTML;
				var isError = singleBlock.style.backgroundColor == errorBgColor;
				appendXmlDiffBlcok(xmlDoc, sentenceXml, 'single', text, isError);
				
				++upTotal;
				++downTotal;
				++sameTotal;
				if(isError)
					++sameError;
				continue;
			}
			
			var upBlock = getFirstEleByClass(differBlock, 'unitdifferup');
			var downBlock = getFirstEleByClass(differBlock, 'unitdifferdown');
			
			var upXml = createElementAndAppend(xmlDoc, sentenceXml, "up");
			var downXml = createElementAndAppend(xmlDoc, sentenceXml, "down");
			
			var smallChildren = upBlock.childNodes;
			for(var k=0; k<smallChildren.length; ++k)
			{
				var smallChild = smallChildren[k];
				if(smallChild.className != "unitdiffsmall")
					continue;
				var text = smallChild.innerHTML;
				var isError = smallChild.style.backgroundColor == errorBgColor;
				appendXmlDiffBlcok(xmlDoc, upXml, 'small', text, isError);
				
				++upTotal;
				if(isError)
					++upDiffError;
			}
			
			smallChildren = downBlock.childNodes;
			for(var k=0; k<smallChildren.length; ++k)
			{
				var smallChild = smallChildren[k];
				if(smallChild.className != "unitdiffsmall")
					continue;
				var text = smallChild.innerHTML;
				var isError = smallChild.style.backgroundColor == errorBgColor;
				appendXmlDiffBlcok(xmlDoc, downXml, 'small', text, isError);
				
				++downTotal;
				if(isError)
					++downDiffError;
			}
		}
	}
	
	createElementTextAndAppend(xmlDoc, statXml, 'id', $('#idS').text());
	createElementTextAndAppend(xmlDoc, statXml, 'name', $('#fileS').text());
	createElementTextAndAppend(xmlDoc, statXml, 'upTotal', upTotal);
	createElementTextAndAppend(xmlDoc, statXml, 'downTotal', downTotal);
	createElementTextAndAppend(xmlDoc, statXml, 'sameTotal', sameTotal);	
	createElementTextAndAppend(xmlDoc, statXml, 'sameError', sameError);	
	createElementTextAndAppend(xmlDoc, statXml, 'upDiffError', upDiffError);
	createElementTextAndAppend(xmlDoc, statXml, 'downDiffError', downDiffError);
	
	updateStatInfo(upTotal, downTotal, sameTotal, sameError, upDiffError, downDiffError);	
	
	//alert(XMLtoString(xmlDoc));
	//save to the server
	if(!clientDebugMode)	
		uploadXml(XMLtoString(xmlDoc));
	
	
	fireUserEdit(false);
}


function initialize()
{
	//check whether is firefox
	var isFirefox = navigator.userAgent.indexOf("Firefox") >= 0 || navigator.userAgent.indexOf("Shiretoko") >= 0;
	if(!isFirefox)
	{
		$('#header').before("<div id=\"warnheader\">Only <strong>Firefox</strong> support all the functions! You should: "+
			"<ol><li>Go to <a href=\"http://www.mozilla.com/firefox/\" target=\"_blank\">www.mozilla.com/firefox/</a> to download firefox and install it.</li>" + 
			"<li>Open <span style=\"color:blue;\">"+window.location.href +"</span> (<a href=\"#\" onclick=\"copyUrlToClipboard(&quot;"+window.location.href+"&quot)\">Copy to Clipboard</a>) in the firefox.</li></ol></div>");
		$("#header").remove();
		$("#stat").remove();
		$("#differs").remove();
		alert("You should open this page in Firefox!");
		
		return;
	}
	
		
	if(clientDebugMode)
		loadXmlStringToComp(sampleXml);
	else
	{
		docId = gup('id');
		if(docId == null)
		{
			alert("Require Parameter 'id'!");
			return;
		}
		downloadXml(docId);
	}
	
	setTimeout(onTimeoutCheck, checkInterval);
}

$(function() {
	initialize();	
});
