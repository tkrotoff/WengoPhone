class TReceiveFileSession : TIFileSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/filesessionmanager/TReceiveFileSession.h>
%End
public:

	Contact getContact() const;

	void setFilePath(const std::string & path);

	std::string getFilePath() const;

	std::string getFileName() const;

	unsigned getFileSize() const;

signals:

	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	void fileTransferProgressionSignal(int percentage);

private:

	TReceiveFileSession();

};
