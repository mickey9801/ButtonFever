/*
 * BfButton.cpp
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
#include "Arduino.h"
#include "BfButton.h"

BfButton::BfButton(button_mode_t mode, uint8_t pin, bool pullup, uint8_t buttonLogic) {
  this->_id = pin;
  this->_mode = mode;
  if (this->_mode == BfButton::STANDALONE_DIGITAL) {
    if (pullup) {
      pinMode(this->_id, INPUT_PULLUP);
    } else {
      pinMode(this->_id, INPUT); // for external pullup or pulldown
    }
    this->_buttonLogic = buttonLogic;
  }
}

BfButton& BfButton::onPress(callback_t callback) {
  this->_onPressCallback = callback;
  return *this;
}

BfButton& BfButton::onDoublePress(callback_t callback, unsigned long timeout) {
  this->_onDoublePressCallback = callback;
  this->_doublePressTimeout = timeout;
  return *this;
}

BfButton& BfButton::onPressFor(callback_t callback, unsigned long timeout) {
  this->_onPressForCallback = callback;
  this->_pressForTimeout = timeout;
  return *this;
}

BfButton& BfButton::updateState(button_state_t state) {
  this->_prevState = this->_state;
  this->_state = state;
  return *this;
}

// only for standalone digital button
void BfButton::read() {
  if (this->_mode != BfButton::STANDALONE_DIGITAL) return;
  unsigned long curTime = millis();
  if (curTime - this->_lastLoop >= this->_loopInterval) {
    this->_lastLoop = curTime;
    int x = digitalRead(this->_id);
    if (x == this->_buttonLogic) {
      this->updateState(BfButton::PRESSING);
    } else {
      this->updateState(BfButton::IDLE);
    }
    this->loop();
  }
}

void BfButton::loop() {
  unsigned long curTime = millis();
  // debouncing
  if (this->_prevState == BfButton::IDLE && this->_state == BfButton::PRESSING) {
    this->_isDebouncing = true;
    return;
  }
  
  // double press timeout, may trigger last single press event (if no pressFor callback)
  if (this->_pressCount > 0 &&
    !(this->_onDoublePressCallback && curTime - this->_firstPressedAt <= this->_doublePressTimeout) &&
    !(this->_onPressForCallback && this->_pressedSince != 0 && this->_state == BfButton::PRESSING)) {
      if (this->_onPressCallback) this->_onPressCallback(this, BfButton::SINGLE_PRESS);
      this->_pressCount = 0;
      this->_firstPressedAt = 0;
  }
  
  if (this->_prevState == BfButton::PRESSING && this->_state == BfButton::PRESSING) {
    // confirm pressed
    if (this->_isDebouncing) {
      this->_pressCount++;
      if (this->_firstPressedAt == 0) this->_firstPressedAt = curTime;
      if (this->_pressedSince == 0) this->_pressedSince = curTime;
      this->_isDebouncing = false;
    }
    
    // Press For
    if (this->_onPressForCallback && this->_pressCount > 0 &&
      (curTime - this->_pressedSince >= this->_pressForTimeout)) {
        this->_onPressForCallback(this, BfButton::LONG_PRESS);
        this->_pressCount = 0;
        this->_firstPressedAt = 0;
        this->_pressedSince = 0;
        return;
    }
    
    // Double Press
    if (this->_onDoublePressCallback && this->_pressCount > 1 &&
      (curTime - this->_firstPressedAt <= this->_doublePressTimeout)) {
        this->_onDoublePressCallback(this, BfButton::DOUBLE_PRESS);
        this->_pressCount = 0;
        this->_firstPressedAt = 0;
        return;
    }
  }
  
  // release the button
  if (this->_prevState == BfButton::PRESSING && this->_state == BfButton::IDLE) {
    this->_pressedSince = 0; // reset pressFor timer
  }
}

uint8_t BfButton::getID() {
  return this->_id;
}

uint8_t BfButton::getPin() {
  return this->getID();
}
