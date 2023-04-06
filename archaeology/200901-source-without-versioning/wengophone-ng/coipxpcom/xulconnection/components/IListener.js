
const interfaces           = Components.interfaces;

const ILISTENER_CONTRACTID = '@openwengo.com/coiplistener;1';
const ILISTENER_CID = Components.ID('{44dafafa-f383-4d48-b58b-77ec9afe8ab1}');
const ILISTENER_IID = Components.interfaces.IListener;

const WINDOWMEDIATOR_CONTRACTID = "@mozilla.org/appshell/window-mediator;1";
const nsIWindowMediator    = interfaces.nsIWindowMediator;

const nsIDOMWindowInternal = interfaces.nsIDOMWindowInternal;

function nsIListener() {
}

nsIListener.prototype = {
  
  // attributs
  ui: null,
  // myObserver: null,
  // ObserverServ: null,

  Init: function() {
    
    this.ui = this._getWindow("xulconnection");

//     // Observer
//     myObserver = {
//       observe : function(subject, topic, data) {

// 	// traitements en fonction du topic...
// 	var windowManager = Components.classes[WINDOWMEDIATOR_CONTRACTID].getService();
// 	var windowManagerInterface = windowManager.QueryInterface(nsIWindowMediator);
// 	var enumerator = windowManagerInterface.getEnumerator("xulconnection");
	
// 	while (enumerator.hasMoreElements())
// 	  {
// 	    var mainWindow = enumerator.getNext().QueryInterface(nsIDOMWindowInternal);
// 	  }
// 	var box = mainWindow.document.getElementById("debugBox");
// 	box.value += data + "\n";
//       }
//     };

//     // récupération du service d'observation
//     ObserverServ = Components.classes["@mozilla.org/observer-service;1"].getService(Components.interfaces.nsIObserverService);
//     // enregistrement
//     ObserverServ.addObserver(myObserver, "un-topic", false);

  },

  AlertMessage: function(txt) {

    if (this.ui) {
      this.ui.alert(txt);
    }
  },

  DebugMessage: function(s) {

    var box = this.ui.document.getElementById("debugBox");
    box.value += s + "\n";
    
    // ObserverServ.notifyObservers(this, "un-topic", this);
    //this.ui.setTimeout(this.ui.alert, rand, s);
    //this.ui.setTimeout(addTextToBox, 0, this.ui, s);
  },

  LoginState: function(state) {

//     if (state == "Creating phoneline") {
      
//       // ok, we are connected
//       this.DebugMessage("GUI:" + "OK, we can now close the login window");
//       this.uiLogin = this._getWindow("login");
//       if (this.uiLogin) {
// 	this.uiLogin.close();
// 	this._islogged = true;
// 	this.DebugMessage("GUI: " + "Closing login window");
//       }
//     }
//     else if (state == "Login/password error") {
// 	this.DebugMessage("GUI: " + "Bad password, not logged");
//       this._islogged = false;
//     }
//     else {
//       this.DebugMessage("Unknown LoginState : " + state);
//     }

  },

  // local methods
  _getWindow: function(wid) {

    var ui = null;
    var windowManager = Components.classes[WINDOWMEDIATOR_CONTRACTID].getService();
    var windowManagerInterface = windowManager.QueryInterface(nsIWindowMediator);
    var enumerator = windowManagerInterface.getEnumerator(wid);
    
    while (enumerator.hasMoreElements())
      {
	ui = enumerator.getNext().QueryInterface(nsIDOMWindowInternal);
	break;
      }
    return ui;
  },

  QueryInterface: function(iid) {
    if (!iid.equals(Components.interfaces.nsISupports) &&
	!iid.equals(ILISTENER_IID))
      throw Components.results.NS_ERROR_NO_INTERFACE;
    return this;
  }
};

var nsIListenerModule = {
  
  registerSelf: function(compMgr, fileSpec, location, type) {
    compMgr = compMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar);
    compMgr.registerFactoryLocation(ILISTENER_CID,
				    "XulConnection Listener",
				    ILISTENER_CONTRACTID,
				    fileSpec,
				    location,
				    type);
  },
  
  getClassObject: function(compMgr, cid, iid) {
    if (!cid.equals(ILISTENER_CID))
      throw Components.results.NS_ERROR_NO_INTERFACE;
    if (!iid.equals(Components.interfaces.nsIFactory))
      throw Components.results.NS_ERROR_NOT_IMPLEMENTED;
    return nsIListenerFactory;    
  },
  
  canUnload: function(compMgr) { return true; }
};

var nsIListenerFactory = {
  createInstance: function(outer, iid) {
    if (outer != null)
      throw Components.results.NS_ERROR_NO_AGGREGATION;
    if (!iid.equals(ILISTENER_IID) &&
	!iid.equals(Components.interfaces.nsISupports))
      throw Components.results.NS_ERROR_INVALID_ARG;
    return new nsIListener();
  } 
};

function NSGetModule(comMgr, fileSpec) { 
  return nsIListenerModule;
}
