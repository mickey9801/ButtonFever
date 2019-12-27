/*
 * BfButtonManager.cpp
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
#include "Arduino.h"
#include "BfButtonManager.h"
#include "BfButton.h"

BfButtonManager::BfButtonManager(uint8_t pin, uint8_t btnNum) {
  this->_pin = pin;
  this->_buttonCount = btnNum;
  this->_buttons = new BfButton* [this->_buttonCount];
  this->_btnVoltageLowerBounds = new uint16_t[this->_buttonCount];
  this->_btnVoltageUpperBounds = new uint16_t[this->_buttonCount];
}

BfButtonManager::~BfButtonManager() {
  // release allocated memories
  delete [] this->_buttons;
  delete [] this->_btnVoltageLowerBounds;
  delete [] this->_btnVoltageUpperBounds;
}

BfButtonManager& BfButtonManager::setADCResolution(uint16_t resolution) {
  this->_adcResolution = resolution;
  return *this;
}
    
BfButtonManager& BfButtonManager::addButton(BfButton* btn, uint16_t minVoltageReading, uint16_t maxVoltageReading) {
  uint8_t _b = btn->getID();
  this->_buttons[_b] = btn;
  this->_btnVoltageLowerBounds[_b] = minVoltageReading;
  this->_btnVoltageUpperBounds[_b] = maxVoltageReading;
  return *this;
}

BfButton& BfButtonManager::getButton(uint8_t id) {
  return *this->_buttons[id];
}

void BfButtonManager::begin() {
  analogRead(this->_pin);
  delay(1);
}

void BfButtonManager::loop() {
  unsigned long curTime = millis();
  if (curTime - this->_lastLoop >= this->_loopInterval) {
    this->_lastLoop = curTime;
    this->_updateButtonState();
  }
}

uint16_t BfButtonManager::printReading(uint8_t pin) {
  int z;
  z = analogRead(pin);
  if (z > 100) Serial.println(z);
  return z;
}

void BfButtonManager::_updateButtonState() {
  int8_t _b = this->_readButton();
  for (int8_t i = 0; i < this->_buttonCount; i++) {
    if (i == _b) {
      this->_buttons[i]->updateState(BfButton::PRESSING);
    } else {
      this->_buttons[i]->updateState(BfButton::IDLE);
    }
    this->_buttons[i]->loop();
  }
}

int8_t BfButtonManager::_readButton() {
  int z, _sum;
  int8_t _button = -1;
  
  _sum = 0;
  for (int i=0; i < 4; i++) {
    _sum += analogRead(this->_pin);
  }
  z = _sum / 4;
  
  if (z >= 100 || z < this->_adcResolution) {
    for (int8_t _b = 0; _b < this->_buttonCount; _b++) {
      if (z > this->_btnVoltageLowerBounds[_b] && z < this->_btnVoltageUpperBounds[_b]) {
        _button = _b;
        break;
      }
    }
  }
  
  return _button;
}
