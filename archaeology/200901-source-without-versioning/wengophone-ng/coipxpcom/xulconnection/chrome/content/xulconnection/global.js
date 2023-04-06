
function alert_debug(txt) {

	if (debugLevel == 2) {
		alert(txt);
	}
}
function alert_error(txt) {

	if (debugLevel == 1 || debugLevel == 2) {
		alert(txt);
	}
}

function GetComponent(cid) {

	var obj = null;

	if (Components.classes[cid]) {
	
		try {
			obj = Components.classes[cid].createInstance();
		}
		catch (e) {}
	}
	return obj;
}

function GetInterface(component, intf) {
	
	var obj = null;
	
	try {
	    obj = component.QueryInterface(intf);
	}
	catch (ee) { }
	return obj;
}

function GetXPCOM(qcomp, qintf) {

	var comp = GetComponent(qcomp);
	if (!comp) {
		alert_debug("component : " + qcomp + " does not exist !");
		return null;
	}
	var intf = GetInterface(comp, qintf);
	if (!intf) {
		alert_debug("interface : " + qintf + " is not present for this component " + qcomp);
		return null;
	}
	return intf;
}
