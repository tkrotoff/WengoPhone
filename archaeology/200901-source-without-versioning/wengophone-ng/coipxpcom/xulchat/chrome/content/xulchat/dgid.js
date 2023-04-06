// we store all globals in a gDialog object for convenience
var gDialog = {};

function GetUIElements()
{
  var elts = document.getElementsByAttribute("id", "*");
  for (var i = 0; i < elts.length; i++)
  {
    dgid(elts.item(i).getAttribute("id"));
  }
}

// that one is really very useful
function dgid(aStr)
{
  gDialog[aStr] = document.getElementById(aStr);
}
