#ifndef IMCONTACTMAP_H
#define IMCONTACTMAP_H

#include <map>
#include <string>

class IMAccount;
class IMContact;

class IMContactMap : std::multimap<IMAccount *, IMContact *>  {
public:

	IMContact * findIMContact(const IMAccount & imAccount, const std::string & contactId);
};

#endif /*IMCONTACTMAP_H*/
