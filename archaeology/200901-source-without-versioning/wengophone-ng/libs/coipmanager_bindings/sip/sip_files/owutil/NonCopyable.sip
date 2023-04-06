class NonCopyable {
%TypeHeaderCode
#include <util/NonCopyable.h>
%End
protected:
 NonCopyable();
 ~NonCopyable();
private:
	NonCopyable(const NonCopyable &);
};
