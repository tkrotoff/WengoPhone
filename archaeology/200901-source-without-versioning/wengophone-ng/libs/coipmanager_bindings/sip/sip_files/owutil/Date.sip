class Date {
%TypeHeaderCode
#include <util/Date.h>
%End
public:
 Date();
 Date(const Date & date);
 Date(unsigned day, unsigned month, unsigned year);
 ~Date();
 unsigned getDay() const;
 void setDay(unsigned day);
 unsigned getMonth() const;
 void setMonth(unsigned month);
 unsigned getYear() const;
 void setYear(unsigned year);
 std::string toString() const;
private:
};
