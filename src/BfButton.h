/*
 * BfButton.h
 * Standalone digital button module handle various button events
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
#ifndef BfButton_h
#define BfButton_h

#include "Arduino.h"

class BfButton {
  public:
    typedef enum {
      STANDALONE_DIGITAL,
      ANALOG_BUTTON_ARRAY
    } button_mode_t;
    
    typedef enum {
      IDLE,
      PRESSING
    } button_state_t;
    
    typedef enum {
      SINGLE_PRESS,
      DOUBLE_PRESS,
      LONG_PRESS
    } press_pattern_t;
    
    typedef void (*callback_t)(BfButton *btn, press_pattern_t pattern);
    
    /*
     * Constructor
     * Params:
     * mode         Button mode
     * pin          Analog pin for button array
     * pullup       Determin use internal pullup on the board or external pullup/pulldown
     * buttonLogic  Determin button logic for triggering button pressed events
     */
    BfButton(button_mode_t mode, uint8_t pin, bool pullup=true, uint8_t buttonLogic=LOW);
    
    /*
     * Assign event callbacks
     */
    BfButton& onPress(callback_t callback);
    BfButton& onDoublePress(callback_t callback, unsigned long timeout=300);
    BfButton& onPressFor(callback_t callback, unsigned long timeout=3000);
    
    /*
     * Public method for updating button object state
     */
    BfButton& updateState(button_state_t state);
    
    /*
     * Update button state every 20 milliseconds
     * Should be put in loop() of the sketch
     */
    void read(); // only for standalone digital button
    
    /*
     * Trigger button pressed events based on button state
     */
    void loop();
    
    /*
     * Return button pin number for digital button,
     * or ID for button in button array
     */
    uint8_t getID();
    
    /*
     * Alias of getID()
     */
    uint8_t getPin();
    
  private:
    // settings
    uint8_t _id;
    button_mode_t _mode;
    uint8_t _buttonLogic = LOW;
    unsigned long _lastLoop = 0;
    uint8_t _loopInterval = 20;
    unsigned long _doublePressTimeout = 300; // 0.3 seconds
    unsigned long _pressForTimeout = 3000; // 3 seconds
    // runtime
    button_state_t _prevState = BfButton::IDLE;
    button_state_t _state = BfButton::IDLE;
    uint8_t _pressCount = 0;
    unsigned long _firstPressedAt = 0;
    unsigned long _pressedSince = 0;
    bool _isDebouncing = false;
    // callbacks
    callback_t _onPressCallback;
    callback_t _onDoublePressCallback;
    callback_t _onPressForCallback;
};

#endif // BfButton_h