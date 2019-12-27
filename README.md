# ButtonFever

Powerful button tools for managing various button events of standalone button or button array. Tested on ESP32 and should also work on Arduino too.  

**ButtonFever** is upgrade version of [*MultiButton*](https://github.com/mickey9801/MultiButtons) library, provide much powerful button features.  

`BfButton` class handled standalone button debouncing, trigger callback function for single press, double press, and long press events. The class can distinguish different pressing pattern and trigger corresponding callback, i.e. single press vs double press.  

`BfButtonManager` class manage multiple buttons with single analog pin. The class also provide `printReading()` method for you to check the analog pin reading. In theory you may add more buttons in the circuit.  

## Installation

1. [Arduino Library Manager (Recommended)](https://www.arduino.cc/en/Guide/Libraries)  
<!-- 2. [Download or clone this repository into your arduino libraries directory](https://help.github.com/articles/cloning-a-repository/)  -->

## Standalone Button Usage

`BfButton` class can be used alone to handle press event of a single digital button.  

1. **Include `BfButton` class**  

  ```cpp
  #include <BfButton.h>
  ```
  
2. **Create button object**  

   ```cpp
   BfButton(button_mode_t mode, uint8_t pin, bool pullup=true, uint8_t buttonLogic=LOW);
   ```
   
   By default, the object will use internal pullup of the board, but you may also use external pullup/pulldown as [example](examples/standalone_digital_button/) you concern, e.g. awake from deep sleep mode using button.  
   
   Parameters: 
   
   Parameter                     | Description
   ------------------------------|------------
   button\_mode\_t mode          | Declare button mode. Assign `BfButton::STANDALONE_DIGITAL` for standalone digital button.
   uint8\_t pin                  | GPIO of the button
   bool pullup=true              | Use internal pullup
   uint8\_t buttonLogic=LOW      | Button logic. Possible value: HIGH (pulldown) or LOW (pullup, default)
   
   ```cpp
   const unsigned int btnPin = 12;
   BfButton btn(BfButton::STANDALONE_DIGITAL, btnPin, false, HIGH); // using external pulldown
   ```   

3. **Declare button callback**  
   
   Button callback must contain 2 parameters:  
   
   Parameter                 | Description
   --------------------------|------------
   BfButton *btn             | BfButtons object itself
   BfButton::press\_pattern\_t pattern | Press pattern of the event. Possible value:<ul><li>BfButton::SINGLE\_PRESS</li><li>BfButton::DOUBLE\_PRESS</li><li>BfButton::LONG\_PRESS</li><ul>
   
   You may declare different callback for different press pattern, or you may use single callback for all pattern.  
   
   ```cpp
   void pressHandler (BfButton *btn, BfButton::press_pattern_t pattern) {
     Serial.print(btn->getID());
     switch (pattern)  {
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
   ```
   
4. **In setup() of the sketch, assign callback for button press events**  

   ```cpp
   void setup() {
     btn.onPress(pressHandler)
       .onDoublePress(pressHandler)     // default timeout
       .onPressFor(pressHandler, 1000); // custom timeout for 1 second
   }
   ```
   
   - BfButton& **onPress (** callback\_t **callback )**  

     Single press event handler  
     
   - BfButton& **onDoublePress (** callback\_t **callback,** unsigned long **timeout**=300 **)**  

     Detect double press within timeout.  
     
   - BfButton& **onPressFor (** callback\_t **callback,** unsigned long **timeout**=3000 **)**  

     Trigger long press event when continue pressed for a while (ie. `timeout`).  
      
5. **In loop() of the sketch, read button status.**  

   ```cpp
   void loop() {
     btn.read();
   }
   ```

### Example

```cpp
#include <BfButton.h>

const unsigned int btnPin = 12;
BfButton btn(BfButton::STANDALONE_DIGITAL, btnPin, false, HIGH); // I added a pulldown myself

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
```

## Button Array

`BfButtonManager` class manage multiple buttons with single analog pin.  

### Reference Circuit

![Multiple Button Circuit](https://4.bp.blogspot.com/_mhuuHR0dxnU/TF7Kesn5gmI/AAAAAAAAENg/JeRCtP2oNNs/s1600/analog_button_input.png)

### Determin Voltage Readings for Each Button

To determin voltage range for each button in the array, you may check the actual readings with `printReading()` method.  

The following sketch calculate the avarage reading of a button from 5 readings:  

```cpp
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
```

**NOTE:** Readings of analog pin may vary upon other connected devices. Voltage ranges for each button should determin based on measurement of the final circuit with all required devices initialized.  

### Usage

1. **Include `BfButton` class and `BfButtonManager` class**  
   
   ```cpp
   #include <BfButtonManager.h>
   #include <BfButton.h>
   ```
   
2. **Create button manager object**  

   ```cpp
   BfButtonManager(uint8_t pin, uint8_t btnNum);
   ```
   
   Parameters:  
   
   Parameter        | Description
   -----------------|------------
   uint8\_t pin     | Analog pin of the button array
   uint8\_t btnNum  | Number of buttons in the array
   
   ```cpp
   const unsigned int btnPin = 35;
   BfButtonManager manager(btnPin, 4);
   ```
   
3. **Create button objects and assign an ID for the button**  
   
   ```cpp
   BfButton(button_mode_t mode, uint8_t id);
   ```
   
   Parameters:  
   
   Parameter                     | Description
   ------------------------------|------------
   button\_mode\_t mode          | Declare button mode. Assign `BfButton::ANALOG_BUTTON_ARRAY` for button in button array.
   uint8\_t id                   | ID of the button

   ```cpp
   BfButton btn1(BfButton::ANALOG_BUTTON_ARRAY, 0);
   BfButton btn2(BfButton::ANALOG_BUTTON_ARRAY, 1);
   BfButton btn3(BfButton::ANALOG_BUTTON_ARRAY, 2);
   BfButton btn4(BfButton::ANALOG_BUTTON_ARRAY, 3);
   ```
   
4. **Declare button callback**  
   
   (see above: Standalone Button Usage)  
   
5. **In setup() of the sketch, assign callback to events**  
   
   (see above: Standalone Button Usage)  
   
6. **In setup() of the sketch, add button to button manager and provide the voltage range of the button**  

   ```cpp
   BfButtonManager& addButton(BfButton* btn, uint16_t minVoltageReading, uint16_t maxVoltageReading);
   ```
   
   Parameters:  
   
   Parameter                  | Description
   ---------------------------|------------
   BfButton* btn              | Button object
   uint16_t minVoltageReading | Minimum voltage reading of the button
   uint16_t maxVoltageReading | Maximum voltage reading of the button
   
   ```cpp
   manager.addButton(&btn1, 3000, 3150);
   ```
   
7. **In setup() of the sketch, initialize button manager for reading analog pin**  

   ```cpp
   manager.begin();
   ```
   
8. **In loop() of the sketch, update button state**  
   
   ```cpp
   void loop() {
     manager.loop();
   }
   ```
   
### Example

```cpp
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

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();

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
      .onDoublePress(pressHandler, 300);
  manager.addButton(&btn3, 1390, 1550);
  
  btn4.onPress(pressHandler);
  manager.addButton(&btn4, 600, 750);

  manager.begin();
}

void loop() {
  manager.loop();
}
```

## Other methods
### `BfButton` class

- uint8\_t **getID ()**  
  
  Return button pin number for digital button, or ID for button in button array.  

- uint8\_t **getPin ()**  
  
  Alias of getID().  
    
### `BfButtonManaget` class

- BfButtonManager& **setADCResolution (** uint16\_t **resolution )** 
  
  Set resolution for ADC. The library will set build-in ADC for Arduino and ESP32 by default.  

- *static* uint16\_t **printReading (** uint8\_t **pin )**
  
  Print analog pin reading through Serial port and return the reading.  
  
  ```cpp
  int reading = MultiButtons::printReading(14);
  ```


## Button Array In Theory
### Voltage Divider Rule

`Vout = Vin(R2/R1+R2)`

Vin = 3.3V # ESP32  
R1+R2 = 5K&#8486; = 5000&#8486;  

### Voltage of each button

- Button 1 Vout = 3.3(4000/5000) = 2.64V
- Button 2 Vout = 3.3(3000/5000) = 1.98V
- Button 3 Vout = 3.3(2000/5000) = 1.32V
- Button 4 Vout = 3.3(1000/5000) = 0.66V

### ADC convertion (12bit for ESP32)

0 ~ 3.3V = 0 ~ 4095  
3.3V/4095 = 0.81mV  

Button|MultiMeter Measurement|Expected Value
:----:|----------------------|-----------------
1     |2.62V                 |3259
2     |1.96V~1.97V           |2420~2432
3     |1.30V~1.31V           |1605~1617
4     |0.65V                 |802

It is required an adjustment for ESP32 ADC with the following equation:  

`Vout = e / 4095.0 * 3.3 + 0.1132`

Button|Circuit Measurement|Serial Debug Data|Calculated Voltage w' Adjustment
:----:|-------------------|-----------------|------------------
1     |2.61V              |3070~3103        |2.59V~2.61V
2     |1.95V~1.96V        |2237~2255        |1.92V~1.93V
3     |1.30V              |1456~1461        |~1.29V
4     |0.64V~0.65V        |658~664          |0.64V~0.65V

## Reference

- [Multiple button inputs using Arduino analog pin](https://rayshobby.net/wordpress/multiple-button-inputs-using-arduino-analog-pin/)
- [How to Debouce Six Buttons on One Analog Pin With Arduino (tcrosley)](https://electronics.stackexchange.com/a/101414)
