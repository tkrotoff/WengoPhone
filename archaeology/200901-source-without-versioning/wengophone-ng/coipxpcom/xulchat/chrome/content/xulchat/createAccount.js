
var xc;
var protocol;

function Startup() {

  xc = window.opener.xc;

  // put every elements into gDialog
  GetUIElements();

  if (window.arguments.length != 1) {
    alert("no argument, closing");
    close();
  }
  protocol = window.arguments[0];

  // concat protocol name into the dialog
  var labelProtocol = gDialog.labelProtocol;
  labelProtocol.value += protocol;
}

function doOK() {

  var ret = xc.ICreateAccount(gDialog.login.value,
			      gDialog.password.value,
			      protocol);
  return ret;
}

function doCancel() {

  close();
}
