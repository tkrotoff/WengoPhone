
/* 
** Suite
*/

function ChatSuite() {

  var suite = tt.CreateSuite();
  suite.addTestPage("chrome://xulchat/content/testing/chat.xul");
  return suite;
}

/* 
** Assert tests
*/

function testCreateSession() {

  var gui = tt.gui;

  assertNotNull("gui must exist", gui);
  assertNotNull("gui.session must exist", gui.gSession);
  assertNotNull("gui.session.command must exist", gui.gSession.command);
  assertNotNull("gui.session.listener must exist", gui.gSession.listener);

  var session = gui.gSession;
  var command = gui.gSession.command;
  var listener = gui.gSession.listener;

  assertNotUndefined("command.CreateChatSession",
		     command.CreateChatSession);
  
  // no way to get feedback ?
  command.CreateChatSession(0);
  
//   assertNotUndefined("Created chatsession",
// 		     chatSession);
 
}

function testChatContacts() {


}
