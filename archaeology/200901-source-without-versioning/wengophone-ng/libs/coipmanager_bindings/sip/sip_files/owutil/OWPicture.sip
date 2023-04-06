class OWPicture {
%TypeHeaderCode
#include <util/OWPicture.h>
%End
public:
	OWPicture();

	OWPicture(const OWPicture & picture);

	static OWPicture pictureFromData(const std::string & data);

	static OWPicture pictureFromData(const QByteArray & byteArray);

	static OWPicture pictureFromFile(const std::string & filename);

	QByteArray getData() const;

	std::string getFilename() const;

	void setFilename(const std::string & filename);

private:

	void copy(const OWPicture & picture);

};
