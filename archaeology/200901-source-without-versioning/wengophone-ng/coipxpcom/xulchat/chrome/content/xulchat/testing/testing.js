
function mytesting() {
}

mytesting.prototype = {
  
  xc: null,
  suite: null,
  testingFeedback: null,
  // testing objs
  accountObj: null,

  Init: function() {

    // gui
    if (!top.window.opener || !top.window.opener.xc) {
      alert_error("GUI not founded. Quitting.");
      return;
    }
    this.xc = top.window.opener.xc;

    // suite
    if (!top || !top.jsUnitTestSuite) {
      alert_error("JsUnit not founded. Quitting.");
      return;
    }
    this.suite = new top.jsUnitTestSuite();
    if (!this.suite) {
      alert_error("Suite in invalid. Quitting.");
      return;
    }
    
    // register self
    this.xc.tt = this;
  },

  CreateSuite: function() {

    return new top.jsUnitTestSuite();
  },
  
  GetFeedback: function(sessionID, feed, param) {

    var arrFeed = feed.split(" ");
    
    this.xc.PrintDebug("------- feedback from " + sessionID + " = " + feed + " | " + param);
    
    if (!arrFeed[0]) {
      alert("invalid feedback");
      return;
    }

    /* possible account messages
    "account error default"
    "account error unknown"
    "account error server"
    "account error timeout"
    "account error authentification"
    "account connected"
    "account disconnected"
    */
    if (arrFeed[0] == "account") {

      if (arrFeed[1] == "error") {
	this.xc.tt.accountObj.ReceiveFeedback_account_ko(arrFeed[2]);
      }
      else {
	this.xc.tt.accountObj.ReceiveFeedback_account_ok(arrFeed[1]);
      }
    }

    /* possible chatSession messages
    "chatSession error null"
    "chatSession created sucessfully"
    */
    if (arrFeed[0] == "chatSession") {

      if (arrFeed[1] == "error") {
	this.xc.tt.accountObj.ReceiveFeedback_chatSession_ko(arrFeed[2]);
      }
      else {
	this.xc.tt.accountObj.ReceiveFeedback_chatSession_ok(arrFeed[1]);
      }
    }

    /* possible contact messages
    "contact added" + contactUUID
    "contact error no chat session"
    "contact error no common account found"
    "contact error unknown"
    */
    if (arrFeed[0] == "contact") {

      if (arrFeed[1] == "error") {
	this.xc.tt.accountObj.ReceiveFeedback_contact_ko(arrFeed[2]);
      }
      else {
	this.xc.tt.accountObj.ReceiveFeedback_contact_ok(arrFeed[1]);
      }
    }

  },

  Run: function() {
  }
}

var tt = new mytesting();
