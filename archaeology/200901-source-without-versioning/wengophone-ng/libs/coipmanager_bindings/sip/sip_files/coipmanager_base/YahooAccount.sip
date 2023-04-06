class YahooAccount : IAccount {
%TypeHeaderCode
#include <coipmanager_base/account/YahooAccount.h>
%End
public:
	static const std::string DEFAULT_YAHOO_SERVER;
	static const std::string DEFAULT_YAHOO_JAPAN_SERVER;
	static const int DEFAULT_YAHOO_PORT;
	static const std::string DEFAULT_YAHOO_XFER_HOST;
	static const std::string DEFAULT_YAHOO_JAPAN_XFER_HOST;
	static const int DEFAULT_YAHOO_XFER_PORT;
	static const std::string DEFAULT_YAHOO_ROOM_LIST_LOCALE;
	YahooAccount();
	YahooAccount(const std::string & login, const std::string & password);
	YahooAccount(const YahooAccount & account);
	explicit YahooAccount(const IAccount * iAccount);
	virtual ~YahooAccount();
	virtual YahooAccount * clone() const /Factory/;
	void setServer(const std::string & yahooServer);
	std::string getServer() const;
	void setServerPort(int yahooServerPort);
	int getServerPort() const;
	void setYahooJapan(bool isYahooJapan);
	bool isYahooJapan() const;
	void setXferServer(const std::string & xferServer);
	std::string getXferHost() const;
	void setXferPort(int xferPort);
	int getXferPort() const;
	void setRoomListLocale(const std::string & locale);
	std::string getRoomListLocale() const;
protected:
	void copy(const YahooAccount & account);
};
