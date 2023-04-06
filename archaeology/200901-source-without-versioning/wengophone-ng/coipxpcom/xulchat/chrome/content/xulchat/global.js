
var debugLevel = 2;

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
	catch (e) { }
	return obj;
}

function GetXPCOM(qcomp, qintf) {

	if (!qcomp) {
		alert_error("The requested component [" + qcomp + "]does not exist.");
		return false;
	}
	if (!qintf) {
		alert_error("The requested interface [" + qintf + "] does not exist.");
		return false;
	}
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

function inArray(array, needed) {
  
  for (var i = 0; i < array.length; i++)
    {
      if (needed == array[i]) {
	return i;
      }
    }
  return false;
}
