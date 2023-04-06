class TConnectManager : QObject {
%TypeHeaderCode
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
%End
public:
	TConnectManager(TCoIpManager & tCoIpManager);
	~TConnectManager();
	bool checkValidity(const Account & account);
	void connect(const std::string & accountId);
	void disconnect(const std::string & accountId);
	void connectAllAccounts();
	void disconnectAllAccounts();
	void setHttpProxy(NetworkProxy networkProxy);
signals:
	void proxyNeedsAuthenticationSignal(NetworkProxy networkProxy);
	void accountConnectionStateSignal(std::string accountId,
		EnumConnectionState::ConnectionState state);
	void accountConnectionErrorSignal(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);
	void accountConnectionProgressSignal(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);
};
