
const kEVENTQUEUESERVICE_CID = '@mozilla.org/event-queue-service;1';
const kXPCOMPROXY_CID        = '@mozilla.org/xpcomproxy;1';
const interfaces           = Components.interfaces;
const nsIProtocolProxyService = interfaces.nsIProtocolProxyService;
const nsIEventQueueService = interfaces.nsIEventQueueService;
const nsIProxyObjectManager = interfaces.nsIProxyObjectManager;
//const nsIDOMEventListener = interfaces.nsIDOMEventListener;

function gui() {
}

gui.prototype = {

  gSession: null,
  is_logged: false,
  // windows
  windowLogin: null,
  dialogCreateAccount: null,
  // debugLevel = 0: no alert || 1: errors || 2: debug
  debugLevel: 2,
  unitTestingStartup: false,
  tt: null,

  Init: function() {
    
    // if we need to debug before everything
    //alert("this is an alert message to help debugging");
    
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
    }

    if (!this.is_logged) {
      this.LaunchLoginWindow();
    }

    if (this.unitTestingStartup) {
      this.LaunchTestingWindow();
    }

  },
  
  // GUI
  
  PrintDebugWindow: function(visible) {

    //this.debugLogin = window.open("chrome://xulchat/content/debug.xul", "debug", "chrome");
  },

  LaunchLoginWindow: function(username) {

    this.windowLogin = window.open("chrome://xulchat/content/login.xul", "login", "chrome");
  },

  LaunchTestingWindow: function() {

    window.open("chrome://xulchat/content/jsunit/testRunner.html?testpage=xulchat/content/testing/testing.xul&autorun=false", "jsunit",
		"chrome,width=800,height=600,resize=yes,modal=no");
  },
  
  PrintDebug: function(txt) {

    var box = document.getElementById("debugBox");
    box.value = txt + "\n" + box.value;

    if (box.scrollMaxY) {
      box.scrollBy(0, box.scrollMaxY);
    }

  },

  /// COIP
  GetFeedback: function(sessionID, feed, param) {

    /* possible messages
    "account error default"
    "account error unknown"
    "account error server"
    "account error timeout"
    "account error authentification"
    "account connected"
    "account disconnected"

    "chatSession is invalid"
    "chatSession already exist"
    "chatSession is null"
    "chatSession created sucessfully"

    "contact added" + contactUUID
    "contact error no chat session"
    "contact error no common account found"
    "contact error unknown"
    */

    // log it
    this.PrintDebug("**** GetFeedback  [" + sessionID + "] **** " + feed);

//     if (feed == "chatSession created sucessfully") {
//       ;
//     }
//     else if (feed == "Account connected") {

//       var gDialog = this.dialogCreateAccount.gDialog;
//       if (!gDialog || !gDialog.login || !gDialog.password) {
// 	alert("cannot continue because the createAccount dialog is not valid");
// 	return;
//       }
//       // add this valid account into the list of accounts
//       if (this.windowLogin) {
// 	this.windowLogin.xc_login.AddAccount(gDialog.login.value);
//       }
//       if (this.dialogCreateAccount) {
// 	this.dialogCreateAccount.close();
// 	this.dialogCreateAccount = null;
//       }
//     }
//     if (state == "user_login_ok") {      
//       var gDialog = this.dialogCreateAccount.gDialog;
//       if (!gDialog || !gDialog.login || !gDialog.password) {
// 	alert("cannot continue because the createAccount dialog is not valid");
// 	return;
//       }
//       // add this valid account into the list of accounts
//       if (this.windowLogin) {
// 	this.windowLogin.xc_login.AddAccount(gDialog.login.value);
//       }
//       if (this.dialogCreateAccount) {
// 	this.dialogCreateAccount.close();
// 	this.dialogCreateAccount = null;
//       }
//     }
  },

  ICreateAccount: function(login, passwd, protocolStr) {
    
    // order is important
    var allProtocols = new Array("unknown", "all", "msn", "yahoo",
				 "aimicq", "jabber", "sipsimple", 
				 "sip", "wengo", "iax");
    
    var protocol = inArray(allProtocols, protocolStr);
    if (protocol === false) {
      alert("This protocol is not valid [" + protocolStr + "]");
      return false;
    }
    if (!login.length) {
      alert("The login cannot be empty [" + login + "]");
      return false;
    }
    
    this.gSession.command.CreateAccount(login, passwd, protocol);

    // return false because it will be managed by an event
    return false;
  },

  ICreateChatSession: function(chatSessionID) {
    
    this.gSession.command.CreateChatSession(chatSessionID);
  },

  IAddContact2ChatSession: function(sessionID, contactID, protocol) {
    
    this.gSession.command.ChatSession_addContact(sessionID, 
						 contactID, 
						 protocol);
  },
  

  End: function() {
  }
}

// globals
var xc = new gui();
var debugLevel = 2;
xc.Init();

// local implementation
function Cleanup() {

  var debugBox = document.getElementById("debugBox");
  debugBox.value = "";
}

function addContactID() {
  
  var contactID = document.getElementById("contactID");
}

function Login() {
  
  xc.LaunchLoginWindow();
}

function Testing() {
  
  xc.LaunchTestingWindow();
}
