#include <Firmata.h>
byte analogPin = 0;
void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.begin(57600);
}
void loop()
{
  Firmata.sendAnalog(analogPin, analogRead(analogPin));
  analogPin = analogPin + 1;
  if (analogPin >= TOTAL_ANALOG_PINS) analogPin = 0; 
}

