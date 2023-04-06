class ImageSelector : QDialog {
%TypeHeaderCode
#include <qtutil/ImageSelector.h>
%End
public:
	static const int START_DIR_ICON_SIZE;

	ImageSelector(QWidget * parent);

	~ImageSelector();

	QString path() const;

	void addStartDirItem(const QString& dir, const QString& name, const QPixmap& pixmap);

	virtual void accept();

public slots:

	void setCurrentDir(const QString & currentDir);

};
