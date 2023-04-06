
// Main object
function xc_login_impl() {
}

xc_login_impl.prototype = {

  accountList: null,
  chatTabbox: null,
  chatTabs: null,
  chatTabpanels: null,
  arraySessions: null,

  Init: function() {
    
    this.accountList = document.getElementById("account-list");
    this.chatTabbox = document.getElementById("chat-tabbox");
    this.chatTabs = document.getElementById("chat-tabs");
    this.chatTabpanels = document.getElementById("chat-tabpanels");
    
    this.arraySessions = new Array();
  },

  AddAccount: function(login) {

    this.accountList.appendItem(login);
  },

  RemoveSelected: function() {

    if (this.accountList.selectedIndex >= 0) {
      this.accountList.removeItemAt(this.accountList.selectedIndex);
    }
  },

  AppendMessage2Frame: function(iframe, msg) {

    var logDoc = iframe.contentDocument;
    logDoc.body.innerHTML += msg;
    
    var w = iframe.contentWindow;
    if (w.scrollMaxY) {
      w.scrollBy(0, w.scrollMaxY);
    }
  },

  SendChatMessage: function(id, msg) {

    var contact = this.arraySessions[id];
    var iframe = document.getElementById("history_" + id);

    msg = "<p>" + msg + "</p>";
    this.AppendMessage2Frame(iframe, msg);
  },

  AddChatSession: function(email) {

    this.arraySessions.push(email);
    var chatSessionID = this.arraySessions.length - 1;

    // COIP add chat session...
    xc.ICreateChatSession(chatSessionID);

    return chatSessionID;
  },

  AddChatContact: function(sessionID, email, protocol) {
    
    // TODO : check email format (must only be chars)

    var unifier = "_" + sessionID;

    var tab = this.chatTabs.appendItem(email, sessionID);
    //this.chatTabs.selectedItem(email, sessionID);
    //tab.selected = true;

    var tabpanel = document.createElement("tabpanel");

    var vbox = document.createElement("vbox");
    vbox.setAttribute("flex", "1");

    var iframe = document.createElement("iframe");
    iframe.setAttribute("flex", "1");
    iframe.setAttribute("id", "history" + unifier);
    iframe.setAttribute("style", "height: 250px;");
    vbox.appendChild(iframe);

    var splitter = document.createElement("splitter");
    splitter.setAttribute("collapse", "none");
    splitter.setAttribute("resizeafter", "farthest");
    vbox.appendChild(splitter);

    var hbox = document.createElement("hbox");
    hbox.setAttribute("flex", "1");

    var textSend = document.createElement("textbox");
    textSend.setAttribute("id", "message" + unifier);
    hbox.appendChild(textSend);

    var buttonSend = document.createElement("button");
    buttonSend.setAttribute("label", "Send");
    buttonSend.setAttribute("oncommand", "composeKeyPressForChat('" + sessionID + "');");
    hbox.appendChild(buttonSend);

    vbox.appendChild(hbox);

    tabpanel.appendChild(vbox);

    this.chatTabpanels.appendChild(tabpanel);

    // COIP add chat session...
    xc.IAddContact2ChatSession(sessionID, email, protocol);

  },

  End: function() {
  }
};
  
// Implementation
var xc = null;
var xc_login = null;

function Startup() {
  
  xc = window.opener.xc;
  xc_login = new xc_login_impl();
  xc_login.Init();
}

function CreateAccount(protocol) {

  xc.dialogCreateAccount = window.openDialog("chrome://xulchat/content/createAccount.xul",
					     "_blank", "chrome,dialog=yes,titlebar=yes,modal=no,resizable=no",
					     protocol);
}

function RemoveAccount() {

  // need to inform COIP...
  xc_login.RemoveSelected();
}

function AddChatContact(protocol) {

  var accountList = document.getElementById("account-list");
  
  var accountNumber = accountList.getRowCount();
  
  if (!accountNumber) {

    alert("You must, at least, have an account.");
    return;
  }

  var contact = document.getElementById("chat-contact");

  if (contact.value.length) {

    // create session
    var chatSessionID = xc_login.AddChatSession(contact.value);
    // create tabs
    xc_login.AddChatContact(chatSessionID, contact.value, protocol);
  }
}

function RemoveCurrentTab() {

}

function composeKeyPressForChat(id)
{
  var message = document.getElementById("message_" + id);

  xc_login.SendChatMessage(id, message.value);
}

