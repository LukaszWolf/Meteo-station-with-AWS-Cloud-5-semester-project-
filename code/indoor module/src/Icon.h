/**
 * @file Icon.h
 * @brief Handles loading and drawing PNG icons from LittleFS to TFT sprites.
 *
 * This class uses the PNGdec library to decode PNG images and render them
 * onto a TFT_eSprite for fast, flicker-free drawing with transparency support.
 */

#pragma once
#include <FS.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>

extern PNG png;

/**
 * @class Icon
 * @brief Represents a graphical icon loaded from the filesystem.
 */
class Icon {
public:
  /**
   * @brief Constructs a new Icon object.
   *
   * @param tft Pointer to the TFT_eSPI object.
   * @param path File path to the PNG image in LittleFS.
   * @param trR Red component of the transparent color (0-255).
   * @param trG Green component of the transparent color (0-255).
   * @param trB Blue component of the transparent color (0-255).
   */
  Icon(TFT_eSPI *tft, const char *path, uint8_t trR, uint8_t trG, uint8_t trB);

  /**
   * @brief Loads the PNG image from LittleFS into a sprite.
   * @return true if loading was successful, false otherwise.
   */
  bool loadFromFS();

  /**
   * @brief Draws the icon at the specified coordinates.
   *
   * @param x X-coordinate.
   * @param y Y-coordinate.
   * @param bgColor Background color for the sprite push (default TFT_BLACK).
   */
  void draw(int16_t x, int16_t y, uint16_t bgColor = TFT_BLACK);

  /**
   * @brief Unloads the icon and frees memory (deletes sprite).
   */
  void unload();

  /**
   * @brief Gets the width of the loaded icon.
   * @return Width in pixels.
   */
  int16_t width() const { return _w; }

  /**
   * @brief Gets the height of the loaded icon.
   * @return Height in pixels.
   */
  int16_t height() const { return _h; }

private:
  static Icon *_active; ///< Pointer to the active Icon instance for callbacks.

  // PNGdec callbacks
  static void *_pngOpen(const char *filename, int32_t *size);
  static void _pngClose(void *handle);
  static int32_t _pngRead(PNGFILE *file, uint8_t *buf, int32_t len);
  static int32_t _pngSeek(PNGFILE *file, int32_t pos);
  static int _pngDrawToSprite(PNGDRAW *pDraw);

  static uint16_t rgb888To565(uint8_t r, uint8_t g, uint8_t b);

  TFT_eSPI *_tft;           ///< Pointer to the display driver.
  TFT_eSprite _sprite;      ///< Sprite buffer for the icon.
  String _path;             ///< File path of the icon image.
  uint16_t _transparent565; ///< Transparent color key in RGB565 format.
  bool _loaded; ///< Flag indicating if the icon is currently loaded.
  int16_t _w;   ///< Icon width.
  int16_t _h;   ///< Icon height.
};