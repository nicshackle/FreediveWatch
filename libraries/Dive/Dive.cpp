#include "Arduino.h"
#include "Dive.h"

Dive::Dive() {
  _inDive = false;
}

void Dive::start() {
  startTime = millis();
  _inDive = true;
  number++;
}

void Dive::stop() {
  endTime = millis();
  _inDive = false;
}

void Dive::update(){
	if(_inDive)	length = millis() - startTime;
}

long Dive::getDiveTime() {
  return endTime - startTime;
}

bool Dive::active(){
	return _inDive;
}

long Dive::timeSince(){
	if(_inDive) return 0;
	else return millis() - endTime;
}