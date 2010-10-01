function XMLtoString(elem){
	var serialized;
	try {
		// XMLSerializer exists in current Mozilla browsers
		serializer = new XMLSerializer();
		serialized = serializer.serializeToString(elem);
	} 
	catch (e) {
		// Internet Explorer has a different approach to serializing XML
		serialized = elem.xml;
	}
	
	return serialized;
}

function loadXMLString(txt) {
	try{ //Internet Explorer 
	  xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
	  xmlDoc.async="false";
	  xmlDoc.loadXML(txt);
	  return(xmlDoc); 
	}catch(e){
		try{ //Firefox, Mozilla, Opera, etc.
			parser=new DOMParser();
			xmlDoc=parser.parseFromString(txt,"text/xml");
			return(xmlDoc);
	 	}catch(e) {alert(e.message)}
	} 
	return(null);
}

function getText(node){
	if(node.firstChild == null)
		return null;
	return node.firstChild.nodeValue;
}

function setText(node, val)
{
	node.firstChild.nodeValue = val;
}

function getChild(node, childName){	
	var children = node.childNodes;
	for(var i=0; i<children.length; ++i){
		if(children[i].tagName == childName)
			return children[i];
	}
	return null;
}

function getChildText(node, childName){
	var child = getChild(node, childName);
	return (child == null) ? null : child.firstChild.nodeValue;
}

function getNotNullChildText(node, childName){
	var child = getChild(node, childName);
	return (child == null) ? "" : child.firstChild.nodeValue;
}

function getAllChildText(node, childName){
	var children = node.childNodes;
	var ret = new Array(children.length);
	var retIndex = 0;
	for(var i=0;i<children.length;++i){
		var child = children[i];
		//if(node.tagName == "serviceType")
			//alert("Compare " + child.tagName + " with "+childName+" equals "+(child.tagName == childName));
		if(child.tagName == childName){
			ret[retIndex] = getText(child);
			++retIndex;
		}
	}
	
	if(retIndex == ret.length)
		return ret;
	return ret.slice(0,retIndex);
}

function getTextUnderNode(node){
	var children = node.childNodes;
	var ret = new Array(children.length);
	for(var i=0;i<children.length;++i){
		ret[i] = getText(children[i]);
	}
	return ret;
}

function createElementAndAppend(xmlDoc, father, newNodeName){
	//alert("new ele with name "+newNodeName);
	var newel=xmlDoc.createElement(newNodeName);
	father.appendChild(newel);
	return newel;
}

function createElementTextAndAppend(xmlDoc, father, newNodeName, text){
	var newCDATA=xmlDoc.createCDATASection(text);	
	var newel=xmlDoc.createElement(newNodeName);
	newel.appendChild(newCDATA);
	father.appendChild(newel);
	return newel;
}

function getXPathNode(rootNode, xpath){
	var names = xpath.split("/");
	var node = rootNode;
	for(var i=0;i<names.length;++i){
		node = getChild(node, names[i]);
		if(node == null)
			return null;
	}
	return node;
}

function getXPathValue(rootNode, xpath){
	var node = getXPathNode(rootNode, xpath);
	if(node == null){
		return null;
	}
	return getText(node);
}

function setXPathValue(rootNode, xpath, val){
	var node = getXPathNode(rootNode, xpath);
	if(node == null){
		return;
	}
	return setText(node, val);
}