function strEndOf(str, suffix){
	var supposeIndex = str.length - suffix.length;
	if(supposeIndex < 0)
		return false;
	return str.indexOf(suffix,supposeIndex) == supposeIndex;
}

function strEqualsIgnoreCase(s1, s2){
	if(s1 == undefined){
		return (s2 == undefined);
	}
	if(s2 == undefined)
		return false;
	return s1.toUpperCase() == s2.toUpperCase();
}

function arrayContains(array, str){
	for(var i=0;i<array.length;++i){
		if(array[i] == str)
			return true;
	}
	return false;
}