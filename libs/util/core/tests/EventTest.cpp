/**
 * This program is a simple and small example of what can be achieved
 * with custom events.  
 *
 * These events are wrappers around boost signals, they are defined in
 * Event.h.
 *
 * Commented lines are here to show the old syntax.
 *
 * You can build this example on GNU/Linux with the following command line in this directory:
 * g++ -I ../include/ -lboost_signals EventTest.cpp -o event_test

 */
#include <iostream>
#include <Event.h>
#include <string>

class MyTimer;
void showTime(MyTimer * sender, const std::string & time) {
  std::cout << time << std::endl;
}
 
class Clock {
public:
  void showTime(MyTimer * sender, const std::string & time) const {
    std::cout << "Clock: " << time << std::endl;
  }
};
 
class MyTimer {
public:
 
  Event<void (MyTimer *, const std::string &)> secondEvent;
  //boost::signal<void (const std::string &)> secondEvent;
 
  Event<void (MyTimer *, const std::string &)> minuteEvent;
  //boost::signal<void (const std::string &)> minuteEvent;
 
  void tick() {
    //Sends the signal, updates the observers
    secondEvent(this, "Second Event");
    minuteEvent(this, "Minute Event");
  }
};
 
int main(void) {
  MyTimer timer;
  Clock clock;
 
  timer.secondEvent += &showTime;
  //timer.secondEvent.connect(&showTime);
  timer.secondEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
  //timer.secondEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
  timer.minuteEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
  //timer.minuteEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
  timer.tick();
  //Print: Second Event
  //       Clock: Second Event
  //       Clock: Minute Event
 
  //timer.minuteEvent -= boost::bind(&Clock::showTime, &clock, _1, _2);
  //timer.minuteEvent.disconnect(boost::bind(&Clock::showTime, &clock, _1));
  timer.tick();
  //Print: Second Event
  //       Clock: Second Event
}
