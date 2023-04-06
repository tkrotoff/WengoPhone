
#include "nsSystemTools.h"

#include "nsGUIEvent.h"

// to del
//#include <nsIObserverService.h>
//#include <nsIProxyObjectManager.h>

NS_IMPL_ISUPPORTS1(nsSystemTools, nsISystemTools)

nsSystemTools::nsSystemTools()
{
  _systemTools = NULL;
}

nsSystemTools::~nsSystemTools()
{
  exit(0);
//   if (_systemTools) {
//     delete _systemTools;
//   }
}

NS_IMETHODIMP nsSystemTools::Init(const nsAString & applicationName,
				  const nsAString & executablePath)
{
  _systemTools = new SystemTools(
    NS_ConvertUTF16toUTF8(applicationName).get(),
    NS_ConvertUTF16toUTF8(executablePath).get()
  );
  return NS_OK;
}

NS_IMETHODIMP nsSystemTools::SetStartUp(PRBool startUp)
{
  bool bstartUp = startUp;

//   if (startUp) {
//     bstartUp = true;
//   }

  if (_systemTools) {
    _systemTools->setStartup(bstartUp);
  }
  return NS_OK;
}

NS_IMETHODIMP nsSystemTools::IsStartup(PRBool *ret)
{
  bool _ret;

  *ret = false;
  if (_systemTools) {
    _ret = _systemTools->isStartup();
    if (_ret) {
      *ret = true;
    }
  }
  return NS_OK;
}
