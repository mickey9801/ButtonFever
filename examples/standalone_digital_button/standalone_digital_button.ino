#include <BfButton.h>

const unsigned int btnPin = 12;
BfButton btn(BfButton::STANDALONE_DIGITAL, btnPin, false, HIGH); // I added a pull-down myself

void pressHandler (BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println(" pressed.");
      break;
    case BfButton::DOUBLE_PRESS:
      Serial.println(" double pressed.");
      break;
    case BfButton::LONG_PRESS:
      Serial.println(" long pressed.");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();

  btn.onPress(pressHandler)
     .onDoublePress(pressHandler) // default timeout
     .onPressFor(pressHandler, 1000); // custom timeout for 1 second
}

void loop() {
  btn.read();
}
