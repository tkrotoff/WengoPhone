class DesktopService /NoDefaultCtors/ {
%TypeHeaderCode
#include <qtutil/DesktopService.h>
%End
public:

	static DesktopService* getInstance();

	virtual ~DesktopService();

	enum DesktopIconType {
		FolderIcon,
		FileIcon
	};

	virtual QPixmap pixmapForPath(const QString & path, int size);

	virtual QPixmap desktopIconPixmap(DesktopIconType iconType, int size);

	virtual QStringList startFolderList();

	virtual QString userFriendlyNameForPath(const QString & path);

private:

	DesktopService();

};
