/*
 * BfButtonManager.h
 * Analog button array manager
 * Copyright (c) 2019 Mickey Chan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Developed for Arduino-ESP32
 * Created by Mickey Chan (developer AT comicparty.com)
 */
#ifndef BfButtonManager_h
#define BfButtonManager_h

#include "Arduino.h"
#include "BfButton.h"

class BfButtonManager {
  public:
    /*
     * Constructor
     * Params:
     * pin          Analog pin for button array
     * btnNum       Number of buttons connected
     */
    BfButtonManager(uint8_t pin, uint8_t btnNum);
    ~BfButtonManager();
    
    /*
     * Set resolution for ADC.
     * The library will set build-in ADC for Arduino and ESP32 by default.
     */
    BfButtonManager& setADCResolution(uint16_t resolution);
    
    /*
     * Add button to button array.
     * Params:
     * btn                  BfButton instance,
     * minVoltageReading    Minimum voltage reading of the button.
     * maxVoltageReading    Maximum voltage reading of the button.
     */
    BfButtonManager& addButton(BfButton* btn, uint16_t minVoltageReading, uint16_t maxVoltageReading);
    BfButton& getButton(uint8_t id);
    
    /*
     * Prepare for button press detection
     * Clear first reading and attach button handler with detect edge
     * Should be put in setup() of the sketch
     */
    void begin();
    
    /*
     * Check analog pin reading for the buttons and update button state every 20 milliseconds
     * Should be put in loop() of the sketch
     */
    void loop();

    /*
     * Print analog button pin reading via serial debug console
     * when the reading greater than 100, and return the reading.
     */
    static uint16_t printReading(uint8_t pin);
    
  private:
    uint8_t _pin;
    uint8_t _buttonCount = 0;
    BfButton **_buttons;
    uint16_t *_btnVoltageLowerBounds;
    uint16_t *_btnVoltageUpperBounds;
    
    #if defined(ESP32)
    uint16_t _adcResolution = 4096;
    #else
    uint16_t _adcResolution = 1024;
    #endif
    
    unsigned long _lastLoop = 0;
    uint8_t _loopInterval = 20;
    
    /*
     * Check valid button press of the array and
     * rotate the button loop
     */
    void _updateButtonState();
    
    /*
     * Read analog value from button array pin 4 times and
     * return the avarage value for determine pressed button
     */
    int8_t _readButton();
};

#endif // BfButtonManager_h