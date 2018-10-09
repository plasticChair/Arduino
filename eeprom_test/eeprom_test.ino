/*
 * EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int addr = 0;
byte value;

void setup()
{
  // initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  EEPROM.begin(512);
}

void loop()
{
  float myNewFloat = 99;
  // read a byte from the current address of the EEPROM

addr += EEPROM.put(addr, 3.653234*addr);

delay(200);
EEPROM.commit();

delay(200);

addr += EEPROM.get(addr, myNewFloat);

  Serial.println(myNewFloat);

  addr +=addr+1;
/*
  EEPROM.write(address, analogRead(A0));
  delay(200);
  
  value = EEPROM.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  // advance to the next address of the EEPROM
  address = address + 1;

  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 512, wrap around to address 0
  if (address == 512)
    address = 0;

  delay(1000);

  EEPROM.commit();
*/

}
