
#include "nsIGenericFactory.h"
#include "CCommand.h"
#include "nsSystemTools.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(CCommand)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSystemTools)

static const nsModuleComponentInfo components[] =
{
	{
		COMMAND_CLASSNAME,
		COMMAND_CID,
		COMMAND_CONTRACTID,
		CCommandConstructor
	},
	{
		SYSTEMTOOLS_CLASSNAME,
		SYSTEMTOOLS_CID,
		SYSTEMTOOLS_CONTRACTID,
		nsSystemToolsConstructor
	}
};

NS_IMPL_NSGETMODULE("xulconnectionModule", components)
