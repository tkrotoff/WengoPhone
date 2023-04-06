class TIFileSession : TSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager_threaded/filesessionmanager/TIFileSession.h>
%End
public:

	void setFile(const File & file);

	File getFile() const;

	IFileSession::IFileSessionEvent getLastEvent() const;

private:

	TIFileSession();

};
