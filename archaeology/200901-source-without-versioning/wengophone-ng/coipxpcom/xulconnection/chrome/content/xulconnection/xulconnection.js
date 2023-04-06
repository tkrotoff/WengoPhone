
const kEVENTQUEUESERVICE_CID = '@mozilla.org/event-queue-service;1';
const kXPCOMPROXY_CID        = '@mozilla.org/xpcomproxy;1';
const interfaces           = Components.interfaces;
const nsIProtocolProxyService = interfaces.nsIProtocolProxyService;
const nsIEventQueueService = interfaces.nsIEventQueueService;
const nsIProxyObjectManager = interfaces.nsIProxyObjectManager;

function xulconnection() {
}

xulconnection.prototype = {

  gSession: null,
  is_logged: false,
  windowLogin: null,
  // debugLevel = 0: no alert || 1: errors || 2: debug
  debugLevel: 2,
  userLogin: "",

  Init: function() {

    this.gSession = GetXPCOM("@openwengo.com/coipsession;1", Components.interfaces.ISession);

    if (!this.gSession) {
      alert_error("no Session");
    }

    // init session
    this.gSession.Init();

    // ok, now let's launch the main application code
    this.Startup();
    
    // testing
    this.debugLevel = 1;
  },
  
  Startup: function() {

    if (this.debugLevel == 2) {
      //this.LaunchDebugWindow();
      // afficher ou pas le debugBox
    }

    if (!this.is_logged) {
      this.LaunchLoginWindow();
    }
  },
  
  PrintDebugWindow: function(visible) {

    //this.debugLogin = window.open("chrome://xulconnection/content/debug.xul", "debug", "chrome");
  },

  LaunchLoginWindow: function(username) {

    if (!this.gSession.listener.islogged) {
      this.windowLogin = window.open("chrome://xulconnection/content/login.xul", "login", "chrome");
    }
  },

  SetLogged: function(logged) {

  }

}

var xc = new xulconnection();

  // var eventQSvc = Components.classes[kEVENTQUEUESERVICE_CID].getService(nsIEventQueueService);
  // var uiQueue = eventQSvc.getSpecialEventQueue(nsIEventQueueService.UI_THREAD_EVENT_QUEUE);
  // var proxyMgr = Components.classes[kXPCOMPROXY_CID].getService(nsIProxyObjectManager);
	
  // First method, create the object directly
  //gCommand = proxyMgr.getProxy(uiQueueC, '@openwengo.com/coipcommand;1', gCommand, '{f40427f1-653e-4331-80ce-0db604b9b2e7}', proxyType);
  //gListener = proxyMgr.getProxy(uiQueueL, '@openwengo.com/coiplistener;1', gListener, '{44dafafa-f383-4d48-b58b-77ec9afe8ab1}', proxyType);

  // Second method, use an existing object
  // // create the objects
  // gListener = GetXPCOM("@openwengo.com/coiplistener;1", Components.interfaces.IListener);
  // gCommand = GetXPCOM("@openwengo.com/coipcommand;1", Components.interfaces.ICommand);

  // // assign them to another thread
  //proxyMgr.getProxyForObject(uiQueue, Components.interfaces.IListener, gListener, proxyType);

  //gCommand = proxyMgr.getProxyForObject(uiQueueC, Components.interfaces.IListener, ICommand, proxyType);
  //gListener = proxyMgr.getProxyForObject(uiQueueL, Components.interfaces.ICommand, gCommand, proxyType);

function Cleanup() {

  var debugBox = document.getElementById("debugBox");
  debugBox.value = "";
}

function addContactID() {
  
  var contactID = document.getElementById("contactID");
}
