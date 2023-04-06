
var xc = null;

function Startup() {
  
  xc = window.opener.xc;
}

function userLogin() {

  var userLogin = document.getElementById("userLogin");
  var userPassword = document.getElementById("userPassword");
  
  xc.userLogin = userLogin;
  xc.gSession.command.CreateAccount(userLogin.value, userPassword.value);
}
