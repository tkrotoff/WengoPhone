class StreetAddress {

%TypeHeaderCode
#include <coipmanager_base/profile/StreetAddress.h>
%End

public:

	StreetAddress();

	StreetAddress(const StreetAddress & streetAddress);

	~StreetAddress();

	bool operator==(const StreetAddress & streetAddress) const;

	void setStreet(const std::string & street);
	const std::string & getStreet() const;

	void setStateProvince(const std::string & stateProvince);
	const std::string & getStateProvince() const;

	void setCity(const std::string & city);
	const std::string & getCity() const;

	void setZipCode(const std::string & zipCode);
	const std::string & getZipCode() const;

	void setCountry(const std::string & country);
	const std::string & getCountry() const;

};
