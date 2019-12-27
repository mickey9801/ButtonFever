/*
 * This demo print voltage reading of analog pin when button in button array pressed
 */
#include <BfButtonManager.h>

uint16_t reading, avg;
uint16_t sum = 0;

const unsigned int pin = 35;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
}

void loop() {
  static unsigned int i = 0;
  reading = BfButtonManager::printReading(pin);
  if (reading > 100) { // button pressed
    sum += reading;
    if (i == 4) {
      avg = sum / 5;
      Serial.print("Avarage Reading: ");
      Serial.println(avg);
      sum = 0;
    }
    i++;
    if (i > 4) i = 0;
  } else { // button released
    sum = 0;
    i = 0;
  }
  delay(200);
}
