
#ifndef SYSTEMTOOLS_H
#define SYSTEMTOOLS_H

#include "dllexport.h"

#include <event/Trackable2.h>
#include <string>

class Startup;

class DLLEXPORT SystemTools : Trackable2 {
public:

	SystemTools(const std::string & applicationName,
		    const std::string & executablePath);
	~SystemTools();

 	bool setStartup(bool startup);
 	bool isStartup();

private:
	
	Startup *_startup;
};

#endif	//SYSTEMTOOLS_H
