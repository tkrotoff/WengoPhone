class Identifiable : Cloneable {
%TypeHeaderCode
#include <util/Identifiable.h>
%End
public:
 Identifiable();
 Identifiable(const Identifiable & identifiable);
 virtual Identifiable * clone() const;
 std::string getUUID() const;
 void setUUID(const std::string & uuid);
protected:
 void copy(const Identifiable & identifiable);
private:
};
