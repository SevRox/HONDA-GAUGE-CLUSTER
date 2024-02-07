#include <PWM.h>

int pin = 9;                // the pin that the pin is attached to
int32_t frequency = 10; //frequency (in Hz)

void setup()
{
  InitTimersSafe(); 
  SetPinFrequencySafe(pin, frequency);
}

void loop()
{
  int i = 10;
  for(i = 10; i <= 250; i=i+1) {
    frequency = i;
    SetPinFrequencySafe(pin, frequency);
    pwmWrite(pin, 230); 
    delay(25);
  }

  for(i; i > 0; i=i-1) {
    frequency = i;
    SetPinFrequencySafe(pin, frequency);
    pwmWrite(pin, 230); 
    delay(25);
  }

  // SetPinFrequencySafe(pin, 30);
  // pwmWrite(pin, 128); 
}

