class Path : NonCopyable {
%TypeHeaderCode
#include <util/Path.h>
%End
public:
	static std::string getApplicationDirPath();

	static std::string getConfigurationDirPath();

	static std::string getPathSeparator();

	static std::string getHomeDirPath();

%If (MACOSX_PLATFORM)
	static std::string getApplicationBundlePath();

	static std::string getApplicationPrivateFrameworksDirPath();

	static std::string getApplicationResourcesDirPath();

	static std::string getBundlePathFromId(const std::string & bundleId);
%End
};
