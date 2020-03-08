// Arduino-unique functions for hardware settings (see 'https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/Arduino.h')
#include <Arduino.h>

// Program entry point
int main(const int argc, const char** argv){
  // Initialize Arduino hardware (pins, serial, etc.)
  init();
  // Setups for program
  
  // TODO: setup();
  
  // Idle event loop
  while(true){
  
  	// TODO: loop();
  
  }
  // Program ends successful
  return 0; 
}