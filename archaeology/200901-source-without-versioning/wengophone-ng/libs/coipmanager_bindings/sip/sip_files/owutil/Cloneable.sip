class Cloneable : Interface {
%TypeHeaderCode
#include <util/Cloneable.h>
%End
public:
 virtual ~Cloneable();
	virtual Cloneable * clone() const = 0 /Factory/;
};
