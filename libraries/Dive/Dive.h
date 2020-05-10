/*
  Dive.h - Library for recording dives.
  Created by Nic Shackle, 12 April 2019.
  Released into the public domain.
*/
#ifndef Dive_h
#define Dive_h

#include "Arduino.h"

class Dive {
  public:
  	Dive();
    long startTime;
    long endTime;
    long length;
    bool active();
    void start();
    void stop();
    void update();
    long getDiveTime();
    long timeSince();
    int number;
  private:
  	bool _inDive;

};

#endif