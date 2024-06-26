function getFirstEleById(ele, id){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.id == id)
			return child;
	}
	return null;
}

function getFirstEleByClass(ele, className){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.className == className)
			return child;
	}
	return null;
}

function getFirstEleByIdItr(ele, id){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.id == id)
			return child;
		if(child.childNodes.length>0){
			var ret = getFirstEleByIdItr(child,id);
			if(ret != null)
				return ret;
		}
	}
	return null;
}

function getFirstEleByClassItr(ele, className){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.className == className)
			return child;
		if(child.childNodes.length>0){
			var ret = getFirstEleByIdItr(child,className);
			if(ret != null)
				return ret;
		}
	}
	return null;
}

function getFirstEleByTag(ele, tag){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.tagName == undefined)
			continue;
		if(child.tagName.toUpperCase() == tag.toUpperCase())
			return child;
	}
	return null;
}

function getFirstEleByTagItr(ele, tag){
	var children = ele.childNodes;
	
	for(var i=0;i<children.length;++i){		
		var child = children[i];
		if(child.tagName != undefined && child.tagName.toUpperCase() == tag.toUpperCase())
			return child;
		if(child.childNodes.length>0){
			var ret = getFirstEleByTagItr(child,tag);
			if(ret != null)
				return ret;
		}
	}
	return null;
}

// if found nothing, return null
function findFatherWithTag(node, tag){
	var parent = node;
	while(true){
		parent = parent.parentNode;
		if(parent == undefined || parent == null)
			return null;
		if(strEqualsIgnoreCase(parent.tagName, tag))
			return parent;
	}
}

function findParentByClass(node, className)
{
	var parent = node.parentNode;
	while(parent != undefined && parent != null)
	{
		if(parent.className == className)
			return parent;
		parent = parent.parentNode;
	}
	return null;
}


function removeHtmlChildren(node){
	while(node.childNodes.length>0)
		node.removeChild(node.childNodes[0]);
}

/**
 * xpath is combined by id, like id1/id2
 */
function getFirstEleByIDXPath(rootNode, xpath){
	var names = xpath.split("/");
	var node = rootNode;
	for(var i=0;i<names.length;++i){
		node = getFirstEleById(node, names[i]);
		if(node == null)
			return null;
	}
	return node;
}