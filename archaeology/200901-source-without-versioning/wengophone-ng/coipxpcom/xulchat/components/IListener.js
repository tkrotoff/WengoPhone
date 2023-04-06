
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
  
  ui: null,

  Init: function() {
    
    this.ui = this._getWindow("xulchat");
  },

  DebugMessage: function(txt) {

    if (this.ui) {
      this.ui.xc.PrintDebug(txt);
    }
  },

  Feedback: function(sessionID, feed, param) {
    
    this.DebugMessage("++++ feedback de " + sessionID + " = " + feed + " | " + param);

    // for testing
    if (this.ui.xc.tt) {
      this.ui.xc.tt.GetFeedback(sessionID, feed, param);
    }
    // for production
    else {
      this.ui.xc.GetFeedback(sessionID, feed, param);
    }

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
