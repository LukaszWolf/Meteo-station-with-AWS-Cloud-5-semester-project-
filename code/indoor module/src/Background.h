/**
 * @file Background.h
 * @brief Manages full-screen background images and interactive buttons.
 */

#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>
#include <vector>

#include "Button.h"

/**
 * @class Background
 * @brief Handles loading background PNGs and managing associated touch buttons.
 */
class Background {
public:
  /**
   * @brief Constructs a new Background object.
   * @param path Path to the background PNG file.
   */
  Background(const String &path = "");

  /**
   * @brief Sets the file path for the background image.
   * @param path The new file path (e.g., "/images/bg.png").
   */
  void setPath(const String &path);

  /**
   * @brief Gets the current file path.
   * @return The file path as a String.
   */
  String path() const;

  /**
   * @brief Draws the background image to the TFT screen.
   *
   * @param tft Reference to the TFT object.
   * @param png Reference to the PNG decoder object.
   * @param center Whether to center the image on the screen (default: true).
   * @return true if drawing was successful, false otherwise.
   */
  bool draw(TFT_eSPI &tft, PNG &png, bool center = true);

  /**
   * @brief Lists files in a directory to Serial for debugging purposes.
   * @param dir Directory path to list (e.g., "/").
   */
  static void listFS(const char *dir);

  /**
   * @brief Adds an interactive button region to this background.
   * @param btn The Button object defining the hitbox and callback.
   */
  void addButton(const Button &btn);

  /**
   * @brief Removes all registered buttons from this background.
   */
  void clearButtons();

  /**
   * @brief Processes touch input for all registered buttons.
   *
   * Iterates through all buttons added to this background and updates their
   * state.
   *
   * @param touchX X-coordinate of the touch event.
   * @param touchY Y-coordinate of the touch event.
   * @param isPressedNow Current state of the touch (true=pressed,
   * false=released).
   * @return true if any button was clicked (action triggered), false otherwise.
   */
  bool handleTouch(int16_t touchX, int16_t touchY, bool isPressedNow);

private:
  String _path; ///< Path to the background image file.
  std::vector<Button>
      _buttons; ///< List of interactive buttons on this background.

  // --- Static members for PNGdec callbacks ---
  // These must be static because PNGdec requires C-style function pointers.

  static int s_offX;              ///< X offset for centering the image.
  static int s_offY;              ///< Y offset for centering the image.
  static uint16_t s_lineBuf[480]; ///< Buffer for one line of decoded pixels.
  static File s_pngFile;          ///< Handle to the open PNG file.
  static TFT_eSPI *s_tft; ///< Pointer to the display driver used during draw.
  static PNG *s_png;      ///< Pointer to the PNG decoder instance.

  // --- PNGdec Callback Wrappers ---
  static void *pngOpen(const char *filename, int32_t *pFileSize);
  static void pngClose(void *handle);
  static int32_t pngRead(PNGFILE *pFile, uint8_t *pBuff, int32_t iLen);
  static int32_t pngSeek(PNGFILE *pFile, int32_t iPosition);
  static int pngDraw(PNGDRAW *p);

  /**
   * @brief Internal helper to setup and execute the PNG drawing process.
   * @param path File path.
   * @param center Centering flag.
   * @return true on success.
   */
  bool drawPngFullScreen(const char *path, bool center);
};