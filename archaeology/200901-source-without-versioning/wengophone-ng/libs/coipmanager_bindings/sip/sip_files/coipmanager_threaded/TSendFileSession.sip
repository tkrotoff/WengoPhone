class TSendFileSession : TIFileSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/filesessionmanager/TSendFileSession.h>
%End
public:

signals:

	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	void fileTransferProgressionSignal(int percentage);

private:

	TSendFileSession();

};
