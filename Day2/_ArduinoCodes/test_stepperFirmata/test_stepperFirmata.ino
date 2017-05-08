#include <Firmata.h>
#include <Stepper.h>
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);
void analogWriteCallback(byte pin, int value)
{
  if (IS_PIN_PWM(pin)) {
    if(pin == 9){
      myStepper.step(map(value,0,255,0,2048));
    }else if(pin ==11){
      myStepper.step(map(value,0,255,0,-2048));  
    }
  }
}

void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.begin(57600);
  myStepper.setSpeed(5);
}

void loop()
{
  while (Firmata.available()) {
    Firmata.processInput();
  }
}

