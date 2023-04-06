class TSMSSession : TSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/smssessionmanager/TSMSSession.h>
%End
public:

	~TSMSSession();

	void setMessage(const std::string & message);

	void addPhoneNumber(const std::string & phoneNumber);

	void setPhoneNumberList(const StringList & phoneNumberList);

	StringList getPhoneNumberList() const;

signals:

	void smsStateSignal(std::string phoneNumber, EnumSMSState::SMSState state);

private:

	TSMSSession();

};
