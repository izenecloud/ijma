var defaultstring = "<?xml version=\"1.0\" encoding=\"utf-8\"?><jmacomp></jmacomp>";

var errorBgColor = "red";

var docId;

var lastSaveTime = null;

var lastSaveTimePromt = "";

var modified = false;

var pageSize = 20;
var pageScope = 5;
var pageIdx = 0;
var maxPageIdx = 0;

var sentenceSize = 0;

var autoSaveInterval = 180000; // 3 min

var checkInterval = 30000; //in milliseconds
if(!clientDebugMode)
	checkInterval = 30000; // 30 s

var xmlDoc = null;
var xmlRoot = null;
var differsXml = null;

var promtDisplay = "none";

var saveAllBtns = null;
var sameAllPromts = null;

var showNBestPromt = "Show iJMA's N-best Results";
var hideNBestPromt = "Hide iJMA's N-best Results";

//statistical information object;
var idS;
var fileS;
var upTotalS;
var downTotalS;
var samePercentS;
var samePrecisionS;
var basisPrecisionS;
var jmaPrecisionS;

var isUploading = false;
var appendingFunc = null;

var isFirefox = navigator.userAgent.indexOf("Firefox") >= 0 || navigator.userAgent.indexOf("Shiretoko") >= 0;


function LoadJsCssFile(filename)
{
	if (strEndOf(filename.toLowerCase(), "js")){
		var fileref=document.createElement('script');
		fileref.setAttribute("type","text/javascript");
		fileref.setAttribute("src",filename);
		document.getElementsByTagName("head")[0].appendChild(fileref);
	}
	else if (strEndOf(filename.toLowerCase(), "css")){
		var fileref=document.createElement("link");
		fileref.setAttribute("rel", "stylesheet");
		fileref.setAttribute("type", "text/css");
		fileref.setAttribute("href",filename);
		document.getElementsByTagName("head")[0].appendChild(fileref);
	}

	
}


function chooseCss()
{	
	//alert("isFirefox " + isFirefox + "," + navigator.userAgent);
	if(isFirefox)
	{
		LoadJsCssFile("img/ff_spec.css");
		//document.write("<link href=\"img/ff_spec.css\" rel=\"stylesheet\" type=\"text/css\"/>");
	}
	else
	{
		LoadJsCssFile("img/ie_spec.css");
		//document.write("<link href=\"img/ie_spec.css\" rel=\"stylesheet\" type=\"text/css\"/>");
	}
}

Array.prototype.indexof = function(obj) {
  var i = this.length;
  while (i--) {
    if (this[i] === obj) {
      return i;
    }
  }
  return -1;
}

function showHidePromt(ele)
{
	if(ele.value == "Hide Instructions")
	{
		ele.value = "Show Instructions"
		promtDisplay = "none";
	}
	else
	{
		ele.value = "Hide Instructions";
		promtDisplay = "block";
	}
	
	$('.instructionDiv').each(function(index){
		this.style.display = promtDisplay;
	});
	$('.promtInfo').each(function(index){
		this.style.display = promtDisplay;
	});
}

function leavingCurrentPage()
{
	$('#differs').remove();
	$("#stat").after("<div id=\"loadingPromt\">Please Wait, modifications are being uploaded. The page will redirect to homepage after that.</div>");
}

function backToHomepage(ele)
{
	if(modified)
	{
		if(!confirm("There are some modifications unsaved, do you want to leave then current page?"))
		{
			return false;
		}
		
		if(confirm("Are you sure to save those modifications before leave?"))
		{
			var ret = saveAllChange(function(){window.location.href="index.jsp"}, true, function(){leavingCurrentPage();});
			appendingFunc = function() {
				saveAllChange(function(){window.location.href="index.jsp"}, true, function(){leavingCurrentPage();});
			};
            return false;
		}
	}

    ele.href = "index.jsp";
	return true;
}

function fireOnLoading()
{
	saveAllBtns.attr('disabled', 'disabled');
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
		saveAllBtns.attr('disabled', '');
	}
	else
	{
		saveAllBtns.attr('disabled', 'disabled');
		lastSaveTime = new Date();
		updateSaveAllPromt();
	}
}

function updateSaveAllPromt()
{
	//update the last modified time
	if(lastSaveTime == null)
		saveAllPromts.html('');
	else
	{		
		var minutes = 0;
		if(clientDebugMode)
			minutes = parseInt(((new Date()).getTime() - lastSaveTime.getTime())/1000);
		else
			minutes = parseInt(((new Date()).getTime() - lastSaveTime.getTime())/60000);
		
		lastSaveTimePromt = '(saved ' + minutes + ' minutes ago)';
		saveAllPromts.html(lastSaveTimePromt);
	}
}

function onTimeoutCheck()
{
	var lastSavePeriod = 0;
	if(lastSaveTime != null)
		lastSavePeriod = parseInt(((new Date()).getTime() - lastSaveTime.getTime()));
	
	if((lastSavePeriod > autoSaveInterval) && modified)
	{
		saveAllChange(null, true);
	}
	
	updateSaveAllPromt();
	setTimeout(onTimeoutCheck, checkInterval);
}

function copyOrig(ele)
{
	var rootNode = findParentByClass(ele, "compunit");
	var userInput = getFirstEleByIDXPath(rootNode, "suggestionRow/contentDiv/userInput");
	var origValue = getFirstEleByIDXPath(rootNode, "originRow/origText").innerHTML;
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
	var rootNode = findParentByClass(ele, "compunit");
	var userInput = getFirstEleByIDXPath(rootNode, "suggestionRow/contentDiv/userInput");
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
	var rootNode = findParentByClass(ele, "compunit");
	var userInput = getFirstEleByIDXPath(rootNode, "suggestionRow/contentDiv/userInput");
	var origValue = getFirstEleByIDXPath(rootNode, "originRow/origText").innerHTML;
	
	var userValue = userInput.value;
	
	//validate the user input, only extra 
	var i = j = 0;
	for( ; i < userValue.length; ++i )
	{
		if(userValue.charAt(i) == " ")
			continue;
		
		if(j == origValue.length)
		{
			alert("Invalid User Input(Non-Space) \'" + userValue.charAt(i) + "\' at index " + i + " (" + getSurroundedString(userValue, i, 4) + ")_1");
			return;
		}
				
		
		while(origValue.charAt(j) == " ")
			++j;
		
		if(userValue.charAt(i) == origValue.charAt(j))
		{
			j++;
			continue;
		}
		alert("Invalid User Input(Non-Space) \'" + userValue.charAt(i) + "\' at index " + i + " (" + getSurroundedString(userValue, i, 4) + ")_2");
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
			var text = getWordFromInnerHTML(singleBlock.innerHTML);
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
			var text = getWordFromInnerHTML(smallChild.innerHTML);
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
			var text = getWordFromInnerHTML(smallChild.innerHTML);
			lenTotal += text.length;
			var idx = lenArray.indexof(lenTotal);
			if(idx >= 0 && singleLenArray[idx] == text.length)
				smallChild.style.backgroundColor = "";
			else
				smallChild.style.backgroundColor = errorBgColor;
		}
	}
	
	//save the last applied user input
	var lastApplyInput = getFirstEleByIDXPath(rootNode, "titleRow/lastApplyUserInput");
	lastApplyInput.value = userValue;
	
	getFirstEleByIDXPath(rootNode, "titleRow/senModified").value = "1";
	fireUserEdit(true);
}

function diffChange(node)
{
	if(node.style.backgroundColor != errorBgColor)
		node.style.backgroundColor = errorBgColor;
	else
		node.style.backgroundColor = "";
	
	var rootNode = findParentByClass(node, "compunit");
	getFirstEleByIDXPath(rootNode, "titleRow/senModified").value = "1";
	
	fireUserEdit(true);
}

function feedbackChange(ele)
{
	var rootNode = findParentByClass(ele, "compunit");
	getFirstEleByIDXPath(rootNode, "titleRow/senModified").value = "1";
	fireUserEdit(true);
}

function switchVisibleOfFeedback(ele)
{
	var rootNode = ele.parentNode.parentNode;
	var advDiv = getFirstEleByClass(rootNode, "nbestDiv");
	//alert(advDiv + "," + advDiv.style.display);
	if(advDiv.style.display == "none")
	{
		advDiv.style.display = "block";
		ele.value = hideNBestPromt;
	}
	else
	{
		advDiv.style.display = "none";
		ele.value = showNBestPromt;
	}
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
	xmlDoc = loadXMLString(xmlStr);
	if(xmlDoc == null)
		return;
	xmlRoot = xmlDoc.getElementsByTagName('jmacomp')[0];
	differsXml = xmlDoc.getElementsByTagName('differs')[0];
	if(!differsXml)
	{
		return;
	}
	sentenceSize = differsXml.childNodes.length;
	
	pageIdx = 0;
	maxPageIdx = Math.ceil(sentenceSize / pageSize);
	
	//alert("maxpageidx " + maxPageIdx);
	
	updateDiffersHtml();
	
	setTimeout(onTimeoutCheck, checkInterval);
}

function moveTo(pPageIdx)
{
	//alert("moveto " + pPageIdx);
	saveAllChange();
	pageIdx = pPageIdx;
	updateDiffersHtml();
	return false;
}

/**
 * text likes word/pos
 * isSingle: true if it is single node in comparison xml, false if it is small node
 * This function is associated with getWordFromInnerHTML(innerHTML)
 */
function highlightPOS(text, isSingle, isUp)
{
	var idx = text.lastIndexOf("/");
	if(idx < 0)
		return text;
		
	var ret = text.substring(0, idx) + "&nbsp;/&nbsp;"
	if(isSingle)
	{
		var posText = text.substring(idx+1);
		//check whether contains to 
		var sepIdx = posText.indexOf(", ");
		if(sepIdx < 0)
			return ret + "<span class=\"possame\">" + posText + "</span>";
		return ret + "<span class=\"pos1\">" + posText.substring(0, sepIdx) + "</span>,&nbsp<span class=\"pos2\">" + posText.substring(sepIdx + 2) + "</span>";;
	}
	else if(isUp)
		return ret + "<span class=\"pos1\">" + text.substring(idx+1) + "</span>";		
	else
		return ret + "<span class=\"pos2\">" + text.substring(idx+1) + "</span>";
}

/**
 * This function is associated with highlightPOS(text, isSingle, isUp)
 */
function getWordFromInnerHTML(text)
{
	var idx = text.indexOf("&nbsp;/&nbsp;");
	if(idx < 0)
		return text;
	return text.substring(0, idx); // minut "&nbsp;"
}

function updateDiffersHtml()
{
	if(!differsXml)
	{
		return;
	}
	
	$('#differs').remove();
	
	var differs = xmlDoc.getElementsByTagName('differs')[0];
	
	var differsChildren = differsXml.childNodes;

	/*var totalSentences = 0;
	for(var i=0; i<differsChildren.length; ++i){
		if(differsChildren[i].tagName == "sentence")
			++totalSentences;
	}*/
	
	//add pages div
	var pageDiv = "<div class=\"pagesDiv\">";
	
	if(pageIdx >= 1)
	{
		pageDiv += "<a href=\"#\" onclick=\"return moveTo(0);\">First</a><a href=\"#\" onclick=\"return moveTo("+(pageIdx-1)+");\">Previous</a>";
	}	
	
	var idxBegin = pageIdx - pageScope;
	if(idxBegin<0)
		idxBegin = 0;
	var idxEnd = pageIdx + pageScope;
	if(idxEnd >= maxPageIdx)
		idxEnd = maxPageIdx - 1;

	for(var k=idxBegin; k<pageIdx; ++k)
	{
		pageDiv += "<a href=\"#\" onclick=\"return moveTo("+k+");\">"+(k+1)+"</a>";
	}
	
	pageDiv += "<span class=\"curPage\">"+(pageIdx+1)+"</span>";
	
	for(var k=pageIdx+1; k<=idxEnd; ++k)
	{
		pageDiv += "<a href=\"#\" onclick=\"return moveTo("+k+");\">"+(k+1)+"</a>";
	}
	
	if(pageIdx < maxPageIdx - 1)
	{
		pageDiv += "<a href=\"#\" onclick=\"return moveTo("+(pageIdx+1)+");\">Next</a><a href=\"#\" onclick=\"return moveTo("+(maxPageIdx-1)+");\">Last</a>";
	}
	
	pageDiv += "</div>"
	
	//end of the pageDiv
	
	var saveAllBtnDisabled = "";
	if(!modified)
		saveAllBtnDisabled = "disabled = \"disabled\"";
	
	var differsStr = "<div id=\"differs\">" + pageDiv;

	var xmlBegin = pageIdx * pageSize;
	var xmlEnd = xmlBegin + pageSize;
	if( xmlEnd > sentenceSize )
		xmlEnd = sentenceSize;
	for(var i=xmlBegin; i<xmlEnd; ++i){
		var sentence = differsChildren[i];
		if(sentence.tagName != "sentence")
			continue;
		var senChildren = sentence.childNodes;
		
		var origText;
		var userInput = "";
		var nbestStr = "";
		var feedback = "";
		var unitdiffers = "";
		var senSameError = 0;
		var senUpDiffError = 0;
		var senDownDiffError = 0;
		for(var j=0; j<senChildren.length; ++j){
			var senChild = senChildren[j];
			
			if(senChild.tagName == "single")
			{
				var bgColor = "";
				if(senChild.getAttribute("error") != "0")
				{
					bgColor = "style=\"background-color:"+errorBgColor+"\"";
					++senSameError;
				}
				unitdiffers += "<div class=\"unitdifferblock\"><div class=\"unitdiffsingle\" "+ bgColor +">" + 
						highlightPOS(getText(senChild), true, false) +"</div></div>";
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
						++senUpDiffError;
						bgColor = "style=\"background-color:"+errorBgColor+"\"";
					}
					unitdiffers += "<div class=\"unitdiffsmall\" "+ bgColor +">" + 
							highlightPOS(getText(smallChild), false, true) +"</span></div>";
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
						++senDownDiffError;
						bgColor = "style=\"background-color:"+errorBgColor+"\"";
					}
					unitdiffers += "<div class=\"unitdiffsmall\" "+ bgColor +">" + 
							highlightPOS(getText(smallChild), false, false) +"</span></div>";
				}
				
				unitdiffers += "</div></div>"; //close of the unitdifferdown and unitdifferblock;
			} // end if senChild.tagName == "double"
			else if(senChild.tagName == "origText")
			{
				origText = getText(senChild);
			}
			else if(senChild.tagName == "userInput")
			{
				userInput = getText(senChild);
			}
			else if(senChild.tagName == "jmaNBest")
			{
				nbestStr = getText(senChild);
				nbestStr = nbestStr.replace(/ /g,"&nbsp;&nbsp;&nbsp;").replace(/<score>/g, "<span class=\"scoreSpan\">").replace(/<\/score>/g,"</span>");
			}
			else if(senChild.tagName == "feedback")
			{
				feedback = getText(senChild);
			}
		}
		var feedBackPromt = "";
        if(feedback.length > 0)
            feedBackPromt = "&nbsp;(Have Feedback)";
	//++++++++++++ begin the compunit string
		var compunitStr = "<div class=\"compunit\">" + 
	"<div class=\"unitroottitle\" id=\"titleRow\"><span style=\"float:left;\">Sentence: " + (i+1)+"/"+sentenceSize + "</span>"+
			"<input type=\"hidden\" id=\"lastApplyUserInput\" value=\"" + userInput + "\" />" +
			"<input type=\"hidden\" id=\"senModified\" value=\"0\" />" +
			"<input type=\"hidden\" id=\"senSameError\" value=\"" + senSameError + "\" />" +
			"<input type=\"hidden\" id=\"senUpDiffError\" value=\"" + senUpDiffError + "\" />" +
			"<input type=\"hidden\" id=\"senDownDiffError\" value=\"" + senDownDiffError + "\" />" +
			"<a href=\"#\" class=\"unitroothref\" onclick=\"return backToHomepage(this);\" title=\"Go to the Homepage\">Home</a>" +
			"<a href=\"postable.htm\" target=\"_blank\" class=\"unitroothref\" title=\"View the POS Tag Sets\">POS Tag Sets</a></div>" +
	"<div class=\"comprow\" id=\"originRow\">" + 
		"<div class=\"unittitle\">Original Text: </div>" + 
		"<div class=\"unitcontent\" id=\"origText\"> " + origText + "</div>" + 
	"</div>" + 
	
	"<div class=\"compinforow\">" + 
		"<span class=\"promtInfo\" style=\"display:"+promtDisplay+";\"><strong>Recommended: </strong>Single click wrong segmented words (wrong words are with " +errorBgColor+ " background color) <strong>Below</strong>. Remove a wrong word by single click that word one more time.</span>" + 
	"</div>" +	
	"<div class=\"unitdiffers\" id=\"differsRow\">" + unitdiffers + "</div>" + 
	
	"<div class=\"compinforow\">" + 
		"<span class=\"promtInfo\" style=\"display:"+promtDisplay+";\"><strong>Not Recommended: </strong>Or input the correct segmentation directly, following the steps below: <ol> <li>Click &quot;Copy Origininal Text&quot; button to get the original text;<li>Separate the words with English spaces <strong>(Avoid to enter other characters and Make sure you are using English Input Method)</strong>; <li>Click &quot;Apply&quot; button to apply your input.</ol></span>" +
	"</div>" + 
	"<div class=\"comprow\" id=\"suggestionRow\">" + 
		"<div class=\"unittitle\">Suggestion (Input the correct segmentation here): </div>" + 
		"<div class=\"unitcontent\" id=\"contentDiv\">" + 
			"<textarea id=\"userInput\" cols=\"60\" rows=\"3\" wrap=\"soft\">"+userInput+"</textarea>" +			
			"<input type=\"button\" class=\"userInputBtn\" value=\"Copy Origininal Text\" onclick=\"copyOrig(this);\"/ title=\"Copy original text to the Seguestion area\">" + 
			"<input type=\"button\" class=\"userInputBtn\" value=\"Apply\" onclick=\"applyUserInput(this)\" title=\"Apply the Suggestion\"/>" + 
			"<input type=\"button\" class=\"userInputBtn\" value=\"Clear\" onclick=\"clearUserInput(this);\" title=\Clear the Suggestion area\"/>" +
		"</div>" + 
	"</div>" +
	 "<div class=\"comprow\" id=\"nbestRow\">" + 
		"<div class=\"unittitle\"><input type=\"button\" class=\"userInputBtn\" value=\"" + showNBestPromt +"\" onclick=\"switchVisibleOfFeedback(this)\" title=\"Show/Hide iJMA's N-best Results\">After show N-best Results, you can put your feedback below" + feedBackPromt + ". </div>" +
		"<div class=\"nbestDiv\" style=\"display:none;\" id=\"contentDiv\">" + 
			"<div class=\"showNbestDiv\">iJMA's N-Best Result: <ol> " + nbestStr+ "</ol>Please put your feedback on iJMA's N-best result into input box below.</div>" +
			"<textarea id=\"feedback\" cols=\"60\" rows=\"3\" wrap=\"soft\" onchange=\"feedbackChange(this)\">" + feedback + "</textarea>" +
		"</div>" + 
	"</div>" + 
	
	"<div class=\"buttonsdiv\"><input type=\"button\" value=\"Save Changes\" class=\"saveAllBtn\"  onclick=\"saveAllChangeByBtn();\" title=\"Save all the modifications\" "+saveAllBtnDisabled+"/><span class=\"saveAllPromt\">"+lastSaveTimePromt+"</span><a href=\"#stat\" class=\"stathref\" title=\"See the statistical information of the current file\">Statistical Information</a></div>" + 
"</div>";
	//+++++++++ end the compunit string
	
	differsStr += compunitStr;
	
	}
	

	
	//alert(differsStr.length);
	$("#stat").after(differsStr + pageDiv + "</div>");
		
	$(".unitdiffsingle,.unitdiffsmall").bind('click', function() { diffChange(this);});
		
	idS.text(getXPathValue(xmlRoot, 'stat/id'));
	fileS.text(getXPathValue(xmlRoot, 'stat/name'));
	var upTotal = parseInt(getXPathValue(xmlRoot, 'stat/upTotal'));
	var downTotal = parseInt(getXPathValue(xmlRoot, 'stat/downTotal'));
	var sameTotal = parseInt(getXPathValue(xmlRoot, 'stat/sameTotal'));
	var sameError = parseInt(getXPathValue(xmlRoot, 'stat/sameError'));
	var upDiffError = parseInt(getXPathValue(xmlRoot, 'stat/upDiffError'));
	var downDiffError = parseInt(getXPathValue(xmlRoot, 'stat/downDiffError'));
	updateStatInfo(upTotal, downTotal, sameTotal, sameError, upDiffError, downDiffError);
	
	saveAllBtns = $('.saveAllBtn');
	saveAllPromts = $('.saveAllPromt');
	
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
	upTotalS.text(upTotal);
	downTotalS.text(downTotal);
	samePercentS.text(((sameTotal/upTotal+sameTotal/downTotal)/2).toFixed(4));
	samePrecisionS.text(((sameTotal - sameError)/sameTotal).toFixed(4));
	basisPrecisionS.text(((upTotal - sameError - upDiffError)/upTotal).toFixed(4));
	jmaPrecisionS.text(((downTotal - sameError - downDiffError)/downTotal).toFixed(4));
}


function saveAllChangeByBtn()
{
	var ret = saveAllChange(null, true);
	if(!ret)
	{
		appendingFunc = function() {
			saveAllChange(null, true);
		};
		alert("The previous modifications are being uploaded, current modifications would be saved after that automatically!");
	}
}

/**
  * must given isUpload = true if want to upload
  */
function saveAllChange(callbackFun, isUpload, beforeUploadFun)
{
	if(!differsXml)
	{
		return false;
	}
	
	if(isUploading)
		return false;
	
	if(!modified)
		return false;
		
	isUploading = true;
	var differsHtml = document.getElementById('differs');
	var differsChildren = differsHtml.childNodes;
	
	var sameError = parseInt(getXPathValue(xmlRoot, 'stat/sameError'));
	var upDiffError = parseInt(getXPathValue(xmlRoot, 'stat/upDiffError'));
	var downDiffError = parseInt(getXPathValue(xmlRoot, 'stat/downDiffError'));
	
	var xmlBegin = pageIdx * pageSize;
	var xmlEnd = xmlBegin + pageSize;
	
	var sentenceXml = differsXml.childNodes[xmlBegin];
	for(var i=0; i<differsChildren.length; ++i)
	{
		var differChild = differsChildren[i];
		if(differChild.className != "compunit")
			continue;
		//ignore the unmodified sentence
		if(getFirstEleByIDXPath(differChild, "titleRow/senModified").value != "1")
		{
			sentenceXml = sentenceXml.nextSibling;
			continue;
		}
		//reset the modified flag
		getFirstEleByIDXPath(differChild, "titleRow/senModified").value = "0";		
		
		var senSameError = 0;
		var senUpDiffError = 0;
		var senDownDiffError = 0;
		
		var userInput = getFirstEleByIDXPath(differChild, 'titleRow/lastApplyUserInput').value;
		setText(sentenceXml.childNodes[1], userInput);
	
		var feedback = getFirstEleByIDXPath(differChild, 'nbestRow/contentDiv/feedback').value;
		setText(sentenceXml.childNodes[3], feedback);
		
		var unitDiffers = getFirstEleByIDXPath(differChild, 'differsRow');
		var differBlocks = unitDiffers.childNodes;
		var differBlockXml  = sentenceXml.childNodes[4];
		
		for(var j=0; j<differBlocks.length; ++j)
		{
			var differBlock = differBlocks[j];
			if(differBlock.className != "unitdifferblock")
				continue;
			//alert("Current: " + differBlockXml.tagName+", " + getText(differBlockXml));
			
			
			var singleBlock = getFirstEleByClass(differBlock, 'unitdiffsingle');
			if(singleBlock != null)
			{
				if(singleBlock.style.backgroundColor == errorBgColor)
				{
					differBlockXml.setAttribute("error", "1");
					++senSameError;
				}
				else
					differBlockXml.setAttribute("error", "0");
				
				//alert("single " + differBlockXml.tagName+", " + getText(differBlockXml));
				differBlockXml = differBlockXml.nextSibling;
				continue;
			}
			
			var upBlock = getFirstEleByClass(differBlock, 'unitdifferup');
			var downBlock = getFirstEleByClass(differBlock, 'unitdifferdown');
			
			var upXml = differBlockXml.firstChild;
			var downXml = upXml.nextSibling;
			
			
			var smallChildren = upBlock.childNodes;
			var smallXml = upXml.firstChild;
			for(var k=0; k<smallChildren.length; ++k)
			{
				var smallChild = smallChildren[k];
				if(smallChild.className != "unitdiffsmall")
					continue;
				if(smallChild.style.backgroundColor == errorBgColor)
				{
					smallXml.setAttribute("error", "1");
					++senUpDiffError;
				}
				else
					smallXml.setAttribute("error", "0");
				smallXml = smallXml.nextSibling;
			}
			
			smallChildren = downBlock.childNodes;
			smallXml = downXml.firstChild;
			for(var k=0; k<smallChildren.length; ++k)
			{
				var smallChild = smallChildren[k];
				if(smallChild.className != "unitdiffsmall")
					continue;
				if(smallChild.style.backgroundColor == errorBgColor)
				{
					smallXml.setAttribute("error", "1");
					++senDownDiffError;
				}
				else
					smallXml.setAttribute("error", "0");
				smallXml = smallXml.nextSibling;
			}
			
			differBlockXml = differBlockXml.nextSibling;
		}
		
		var senSameErrorInput = getFirstEleByIDXPath(differChild, "titleRow/senSameError");
		sameError = sameError - parseInt(senSameErrorInput.value) + senSameError;
		senSameErrorInput.value = "" + senSameError;
		
		var senUpDiffErrorInput = getFirstEleByIDXPath(differChild, "titleRow/senUpDiffError");
		upDiffError = upDiffError - parseInt(senUpDiffErrorInput.value) + senUpDiffError;
		senUpDiffErrorInput.value = "" + senUpDiffError;
		
		var senDownDiffErrorInput = getFirstEleByIDXPath(differChild, "titleRow/senDownDiffError");		
		downDiffError = downDiffError - parseInt(senDownDiffErrorInput.value) + senDownDiffError;
		senDownDiffErrorInput.value = "" + senDownDiffError;

        sentenceXml = sentenceXml.nextSibling;
	}
	
	setXPathValue(xmlRoot, 'stat/sameError', sameError);
	setXPathValue(xmlRoot, 'stat/upDiffError', upDiffError);
	setXPathValue(xmlRoot, 'stat/downDiffError', downDiffError);
	
	var upTotal = parseInt(getXPathValue(xmlRoot, 'stat/upTotal'));
	var downTotal = parseInt(getXPathValue(xmlRoot, 'stat/downTotal'));
	var sameTotal = parseInt(getXPathValue(xmlRoot, 'stat/sameTotal'));
	updateStatInfo(upTotal, downTotal, sameTotal, sameError, upDiffError, downDiffError);	
	
	//save to the server
	if(isUpload){
		if(!clientDebugMode)
		{
			if(beforeUploadFun != undefined)
				beforeUploadFun();
			uploadXml(XMLtoString(xmlDoc), callbackFun);
		}
		if(fireUserEdit != undefined)
			fireUserEdit(false);
	}
	else
	{
		fireUserEdit(true);
	}
	
	isUploading = false;
	if(clientDebugMode)
        alert(XMLtoString(xmlDoc));
	
	if(appendingFunc)
	{
		var opFun = appendingFunc;
		appendingFunc = null;
		opFun();
	}
	
	return true;
}


function initialize()
{
	//check whether is firefox
	/*var isFirefox = navigator.userAgent.indexOf("Firefox") >= 0 || navigator.userAgent.indexOf("Shiretoko") >= 0;
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
	*/
	chooseCss();
	if(clientDebugMode)
	{
		LoadJsCssFile("a.js");	
		if(isFirefox)
			LoadJsCssFile("js/firebug-lite-compressed.js");
	}
	
	idS = $('#idS');
	fileS = $('#fileS');
	upTotalS = $('#upTotalS');
	downTotalS = $('#downTotalS');
	samePercentS = $('#samePercentS');
	samePrecisionS = $('#samePrecisionS');
	basisPrecisionS = $('#basisPrecisionS');
	jmaPrecisionS = $('#jmaPrecisionS');

	
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
}

$(function() {
	initialize();	
});
