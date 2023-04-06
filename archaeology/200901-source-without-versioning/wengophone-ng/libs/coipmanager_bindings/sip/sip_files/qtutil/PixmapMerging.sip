class PixmapMerging /NoDefaultCtors/ {
%TypeHeaderCode
#include <qtutil/PixmapMerging.h>
%End
public:

	static QPixmap merge(const QByteArray & foregroundPixmapData, const std::string & backgroundPixmapFilename);

private:

	PixmapMerging();

};
