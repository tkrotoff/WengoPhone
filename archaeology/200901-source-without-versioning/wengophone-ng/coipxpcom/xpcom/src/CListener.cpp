
#include "CListener.h"

//#include "nsStringAPI.h"
//#include <string>
//using namespace std;

//#include <stdio.h>
//#include <nsIServiceManager.h>
//#include <nsISomething.h>

// this interface is implemented in Javascript

NS_IMPL_ISUPPORTS1(CListener, IListener)

CListener::CListener()
{
}

CListener::~CListener()
{
}

NS_IMETHODIMP
CListener::NotifyMessage(const nsAString & message)
{
  // STUB STUB STUB
  return NS_OK;
}

/*
void CListener::GetListener() {
{
   static const char szContractId[] = "@openwengo.com/coiplistener;1";
   nsresult rv;

   // Initialize XPCOM and check for failure ...
   rv = NS_InitXPCOM(nsnull, nsnull);
   if ( NS_FAILED(rv) )
   {
     printf("Calling NS_InitXPCOM returns [%x].\n", rv);
     return -1;
   }
   
   // optional autoregistration - forces component manager to check for new components.
   (void)nsComponentManager::AutoRegister(nsIComponentManager::NS_Startup, nsnull);

   // Create an instance of our component
   nsCOMPtr mysample = do_CreateInstance(szContractId, &rv);
   if (NS_FAILED(rv))
   {
      printf("Creating component instance of %s fails.\n", szContractId);
      return -2;
   }

   // Do something useful with your component ...
   mysample->NotifyMessage("de la bombe");

   // (main body of code goes here)

   // Released any interfaces.

   // Shutdown XPCOM
   NS_ShutdownXPCOM(nsnull);
   return 0;
}
*/

/*
NS_IMETHODIMP
CListener::Init()
{
  // STUB STUB STUB
  return NS_OK;
}

NS_IMETHODIMP
CListener::loggerMessageAddedEvent(const nsAString & message)
{
  // STUB STUB STUB
  return NS_OK;
}

NS_IMETHODIMP
CListener::chatMessageReceivedEvent(const nsAString & message)
{
  // STUB STUB STUB
  return NS_OK;
}
*/
