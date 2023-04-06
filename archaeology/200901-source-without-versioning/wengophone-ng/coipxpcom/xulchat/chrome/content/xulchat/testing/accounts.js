
/* 
** Assert tests
*/

function TestingAccountObj() {
}
TestingAccountObj.prototype = {

  cpt: 0,
  tested_protocol: "",
  tt: null,

  Init: function(tt) {
    this.tt = tt;
  },

  CreateAccount: function() {

    if (this.cpt == 0) {
      this.tested_protocol = "wengo";
      this.cpt++;
    }
    else if (this.cpt == 1) {
      this.tested_protocol = "msn";
      this.cpt++;
    }
    else if (this.cpt == 2) {
      this.tested_protocol = "jabber";
      this.cpt++;
    }

    if (this.tested_protocol == "wengo") {
      this.tt.xc.gSession.command.CreateAccount("(login)", 
						"(password)", 8);
    }
    else if (this.tested_protocol == "msn") {
      this.tt.xc.gSession.command.CreateAccount("(login)",
						"(password)", 2);
    }
    else if (this.tested_protocol == "jabber") {
      this.tt.xc.gSession.command.CreateAccount("(login)",
						"(password)", 5);
    }
  },

  ReceiveFeedback_account_ok: function(param) {

    // can I validate this accout creation
    //assertNotNull("Account created", "yes");
    this.tt.xc.PrintDebug("** Tests ** Account created successfully");

    // next account
    this.CreateAccount();
  },

  ReceiveFeedback_account_ko: function(error) {

    // can I validate this accout creation
    //assertNotNull("Account not created: " + error, null);
    this.tt.xc.PrintDebug("** Tests ** Error while creating this account");

    // next account
    this.CreateAccount();
  },

  End: function() {
  }
}

function testCreateAccount() {

  // init
  assertNotNull("testingObj must exist", tt);
  assertNotUndefined("testingObj must exist", tt);
  assertNotNull("gui must exist", tt.xc);
  assertNotNull("gui.session must exist", tt.xc.gSession);
  assertNotNull("gui.session.command must exist", tt.xc.gSession.command);
  assertNotNull("gui.session.listener must exist", tt.xc.gSession.listener);

  tt.xc.PrintDebug("** Tests ** Entering into testCreateAccount");

  tt.accountObj = new TestingAccountObj;
  tt.accountObj.Init(tt);
  tt.accountObj.CreateAccount();
}

