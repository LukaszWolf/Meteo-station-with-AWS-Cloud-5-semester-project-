/**
 * @file Button.h
 * @brief Defines a touch-interactive button area.
 */

#pragma once
#include <Arduino.h>

/**
 * @brief Callback function type for button events.
 * @param id Button ID.
 * @param x X-coordinate of the touch.
 * @param y Y-coordinate of the touch.
 */
using ButtonCallback = void (*)(uint8_t id, int16_t x, int16_t y);

/**
 * @class Button
 * @brief Represents a rectangular touch-sensitive area with a callback.
 */
class Button {
public:
  /**
   * @brief Constructs a new Button object.
   * @param id Unique identifier for the button.
   * @param x X-coordinate of top-left corner.
   * @param y Y-coordinate of top-left corner.
   * @param w Width of the button.
   * @param h Height of the button.
   * @param cb Callback function to execute on click.
   */
  Button(uint8_t id, int16_t x, int16_t y, int16_t w, int16_t h,
         ButtonCallback cb = nullptr);

  uint8_t id() const { return _id; }
  void setCallback(ButtonCallback cb) { _cb = cb; }

  /**
   * @brief Checks if a point is within the button's bounds.
   */
  bool contains(int16_t x, int16_t y) const;

  /**
   * @brief Updates the button state based on current touch input.
   *
   * @param touchX Current touch X coordinate.
   * @param touchY Current touch Y coordinate.
   * @param isPressedNow Whether the screen is currently touched.
   * @return true if the button was clicked (pressed and released inside), false
   * otherwise.
   */
  bool updateTouch(int16_t touchX, int16_t touchY, bool isPressedNow);

private:
  uint8_t _id;            ///< Unique ID of the button.
  int16_t _x, _y, _w, _h; ///< Dimensions and position of the button.
  ButtonCallback _cb;     ///< Function to call when button is clicked.
  bool _pressedInside;    ///< Tracks if the button is currently being pressed.
  int16_t _lastTouchX;    ///< X-coordinate of the last touch within bounds.
  int16_t _lastTouchY;    ///< Y-coordinate of the last touch within bounds.
};