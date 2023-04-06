class Time {
%TypeHeaderCode
#include <util/Time.h>
%End
public:

	Time();

	Time(const Time & time);

	Time(unsigned hour, unsigned minute, unsigned second);

	~Time();

	bool operator==(const Time & time) const;

	unsigned getHour() const;

	void setHour(unsigned hour);

	unsigned getMinute() const;

	void setMinute(unsigned minute);

	unsigned getSecond() const;

	void setSecond(unsigned second);

	std::string toString() const;

};
