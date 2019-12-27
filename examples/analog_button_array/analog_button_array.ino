#include <BfButtonManager.h>
#include <BfButton.h>

const unsigned int btnPin = 35;
BfButtonManager manager(btnPin, 4);

BfButton btn1(BfButton::ANALOG_BUTTON_ARRAY, 0);
BfButton btn2(BfButton::ANALOG_BUTTON_ARRAY, 1);
BfButton btn3(BfButton::ANALOG_BUTTON_ARRAY, 2);
BfButton btn4(BfButton::ANALOG_BUTTON_ARRAY, 3);

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

void specialPressHandler (BfButton *btn, BfButton::press_pattern_t pattern) {
  Serial.print(btn->getID());
  Serial.println(" is special!!");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();

  manager.setADCResolution(4096);

  btn1.onPress(pressHandler);
  btn1.onDoublePress(pressHandler);
  btn1.onPressFor(pressHandler, 2000);
  manager.addButton(&btn1, 3000, 3150);
  
  btn2.onPress(pressHandler);
  btn2.onPressFor(pressHandler, 1500);
  manager.addButton(&btn2, 2190, 2350);

  // You may change event handler methods in any order!
  btn3.onPressFor(pressHandler, 1000)
      .onPress(pressHandler)
      .onDoublePress(specialPressHandler, 300);
  manager.addButton(&btn3, 1390, 1550);
  
  btn4.onPress(pressHandler);
  manager.addButton(&btn4, 600, 750);

  manager.begin();
}

void loop() {
  manager.loop();
}
