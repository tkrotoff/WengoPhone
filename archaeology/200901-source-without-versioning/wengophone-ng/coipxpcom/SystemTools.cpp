
#include "SystemTools.h"

#include <system/Startup.h>

SystemTools::SystemTools(const std::string & applicationName, 
			 const std::string & executablePath) {

  _startup = new Startup(applicationName, executablePath);
}

SystemTools::~SystemTools() {

  delete _startup;
}

bool SystemTools::setStartup(bool startup) {

  return _startup->setStartup(startup);
}

bool SystemTools::isStartup() {

  return _startup->isStartup();
}
