
#ifndef __ICOMMANDOBSERVER_IMPL_H__
#define __ICOMMANDOBSERVER_IMPL_H__

#include <string>

class ICommandObserver
{
public:

  virtual void DebugMessage(const std::string & message) = 0;

  virtual void SendFeedback(unsigned sessionID,
			    const std::string & state,
			    const std::string & param) = 0;

  virtual char *ConfGetChar(const std::string & prefID) = 0;

  virtual void ConfSetChar(const std::string & prefID,
			   const std::string & value) = 0;

};

#endif
