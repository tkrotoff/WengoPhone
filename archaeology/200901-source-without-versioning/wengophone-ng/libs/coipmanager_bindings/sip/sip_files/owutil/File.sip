class File : NonCopyable {
%TypeHeaderCode
#include <util/File.h>
%End
public:
	enum Encoding {
		EncodingDefault,
		EncodingUTF8
	};
 File(const std::string & filename, Encoding encoding = File::EncodingDefault);
 File(const File & file);
 virtual ~File();
 std::string getExtension() const;
 bool move(const std::string & newPath, bool overwrite = false);
 bool remove();
 bool copy(const std::string & path);
 static bool copyFile(const std::string & dst, const std::string & src);
 std::string getPath() const;
 std::string getFullPath() const;
 std::string getFilename() const;
 StringList getDirectoryList() const;
 StringList getFileList() const;
 unsigned getSize() const;
 static std::string normalize(const std::string & path, bool forceDir = false);
 static std::string convertPathSeparators(const std::string & path);
 static std::string getPathSeparator();
 static void createPath(const std::string & path);
 static File createTemporaryFile();
 static bool exists(const std::string & path);
 static bool isDirectory(const std::string & filename);
protected:
};
class IFile : Interface {
%TypeHeaderCode
#include <util/File.h>
%End
public:
	virtual void close() = 0;
protected:
	virtual bool open() = 0;
	virtual bool isOpen()  = 0;
};
class FileReader : File {
%TypeHeaderCode
#include <util/File.h>
%End
public:
 FileReader(const std::string & filename);
 FileReader(const FileReader & fileReader);
 FileReader(const File & file);
 ~FileReader();
 bool open();
 std::string read();
 void close();
private:
 bool isOpen();
};
class FileWriter : File {
%TypeHeaderCode
#include <util/File.h>
%End
public:
 FileWriter(const std::string & filename, bool binaryMode = true);
 FileWriter(const FileWriter & fileWriter, bool binaryMode = true);
 FileWriter(const File & file, bool binaryMode = true);
 ~FileWriter();
 bool write(const std::string & data);
 void close();
 bool setAppendMode(bool appendMode);
private:
 bool open();
 bool isOpen();
};
