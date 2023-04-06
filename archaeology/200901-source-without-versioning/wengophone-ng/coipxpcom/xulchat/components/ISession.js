
const interfaces           = Components.interfaces;

const ISESSION_CONTRACTID = '@openwengo.com/coipsession;1';
const ISESSION_CID = Components.ID('{690b7821-81c1-40ae-a902-daa053d11ec1}');
const ISESSION_IID = interfaces.ISession;

const ICOMMAND_CONTRACTID = '@openwengo.com/coipcommand;1';
const ICOMMAND_IID = interfaces.ICommand;

const ILISTENER_CONTRACTID = '@openwengo.com/coiplistener;1';
const ILISTENER_IID = interfaces.IListener;

const kEVENTQUEUESERVICE_CID = '@mozilla.org/event-queue-service;1';
const kPROTPROX_CID          = '{e9b301c0-e0e4-11D3-a1a8-0050041caf44}';
const kXPCOMPROXY_CID        = '@mozilla.org/xpcomproxy;1';

const WINDOWMEDIATOR_CONTRACTID = "@mozilla.org/appshell/window-mediator;1";
const nsIWindowMediator    = interfaces.nsIWindowMediator;

const nsIDOMWindowInternal = interfaces.nsIDOMWindowInternal;
const nsIProtocolProxyService = interfaces.nsIProtocolProxyService;
const nsIEventQueueService = interfaces.nsIEventQueueService;
const nsIProxyObjectManager = interfaces.nsIProxyObjectManager;

function nsISession() {
}

nsISession.prototype = {
  
  // attributs
  command: null,
  listener: null,

  Init: function() {
    
    this.command = Components.classes[ICOMMAND_CONTRACTID].createInstance(ICOMMAND_IID);
    this.listener = Components.classes[ILISTENER_CONTRACTID].createInstance(ILISTENER_IID);

    // register self as a listener
    this.command.SetListener(this._getProxyFromObjectOnUIThread(this.listener, Components.interfaces.IListener));

    // Call the Init method for each component
    this.command.Init();
    this.listener.Init();
  },
  
  _getProxyFromObjectOnUIThread: function(aObject, aInterface) {
    var eventQSvc = Components.classes[kEVENTQUEUESERVICE_CID].getService(nsIEventQueueService);
    var uiQueue = eventQSvc.getSpecialEventQueue(nsIEventQueueService.UI_THREAD_EVENT_QUEUE);
    var proxyMgr = Components.classes[kXPCOMPROXY_CID].getService(nsIProxyObjectManager);

    return proxyMgr.getProxyForObject(uiQueue, aInterface, aObject, 5);
  },

  QueryInterface: function(iid) {
    if (!iid.equals(Components.interfaces.nsISupports) &&
	!iid.equals(ISESSION_IID))
      throw Components.results.NS_ERROR_NO_INTERFACE;
    return this;
  }
};

var nsISessionModule = {
  
  registerSelf: function(compMgr, fileSpec, location, type) {
    compMgr = compMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar);
    compMgr.registerFactoryLocation(ISESSION_CID,
				    "XulConnection Session",
				    ISESSION_CONTRACTID,
				    fileSpec,
				    location,
				    type);
  },
  
  getClassObject: function(compMgr, cid, iid) {
    if (!cid.equals(ISESSION_CID))
      throw Components.results.NS_ERROR_NO_INTERFACE;
    if (!iid.equals(Components.interfaces.nsIFactory))
      throw Components.results.NS_ERROR_NOT_IMPLEMENTED;
    return nsISessionFactory;    
  },
  
  canUnload: function(compMgr) { return true; }
};

var nsISessionFactory = {
  createInstance: function(outer, iid) {
    if (outer != null)
      throw Components.results.NS_ERROR_NO_AGGREGATION;
    if (!iid.equals(ISESSION_IID) &&
	!iid.equals(Components.interfaces.nsISupports))
      throw Components.results.NS_ERROR_INVALID_ARG;
    return new nsISession();
  } 
};

function NSGetModule(comMgr, fileSpec) { 
  return nsISessionModule;
}
