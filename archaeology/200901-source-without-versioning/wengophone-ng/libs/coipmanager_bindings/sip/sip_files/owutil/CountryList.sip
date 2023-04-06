class CountryList /NoDefaultCtors/ {
%TypeHeaderCode
#include <util/CountryList.h>
%End
public:

	static StringList getCountryList();

private:

	CountryList();

};
