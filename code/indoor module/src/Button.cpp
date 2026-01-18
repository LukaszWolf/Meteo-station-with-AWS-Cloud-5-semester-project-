/**
 * @file Button.cpp
 * @brief Implementation of the Button class.
 */

#include "Button.h"

Button::Button(uint8_t id, int16_t x, int16_t y, int16_t w, int16_t h,
               ButtonCallback cb)
    : _id(id), _x(x), _y(y), _w(w), _h(h), _cb(cb), _pressedInside(false),
      _lastTouchX(0), _lastTouchY(0) {}

bool Button::contains(int16_t x, int16_t y) const {
  return (x >= _x && x <= (_x + _w) && y <= _y && y >= (_y - _h));
}

bool Button::updateTouch(int16_t touchX, int16_t touchY, bool isPressedNow) {
  bool inside = contains(touchX, touchY);
  bool click = false;

  if (isPressedNow) {
    if (inside) {
      if (!_pressedInside) {
        _pressedInside = true;
      }
      _lastTouchX = touchX;
      _lastTouchY = touchY;
    } else {
      if (_pressedInside) {
        _pressedInside = false;
      }
    }
  } else {
    if (_pressedInside) {
      click = true;
      if (_cb) {
        _cb(_id, _lastTouchX, _lastTouchY);
      }
    }
    _pressedInside = false;
  }

  return click;
}