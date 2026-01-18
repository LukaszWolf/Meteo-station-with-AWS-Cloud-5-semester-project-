/**
 * @file Background.cpp
 * @brief Implementation of the Background class.
 */

#include "Background.h"
#include <LittleFS.h>
#include <TFT_eSPI.h>

int Background::s_offX = 0;
int Background::s_offY = 0;
uint16_t Background::s_lineBuf[480];
File Background::s_pngFile;
TFT_eSPI *Background::s_tft = nullptr;
PNG *Background::s_png = nullptr;

Background::Background(const String &path) : _path(path) {}

void Background::setPath(const String &path) { _path = path; }

String Background::path() const { return _path; }

void *Background::pngOpen(const char *filename, int32_t *pFileSize) {
  s_pngFile = LittleFS.open(filename, "r");
  if (!s_pngFile)
    return nullptr;
  *pFileSize = s_pngFile.size();
  return (void *)&s_pngFile;
}

void Background::pngClose(void *handle) {
  File *f = (File *)handle;
  if (f)
    f->close();
}

int32_t Background::pngRead(PNGFILE *pFile, uint8_t *pBuff, int32_t iLen) {
  File *f = (File *)pFile->fHandle;
  return f->read(pBuff, iLen);
}

int32_t Background::pngSeek(PNGFILE *pFile, int32_t iPosition) {
  File *f = (File *)pFile->fHandle;
  f->seek(iPosition);
  return iPosition;
}

int Background::pngDraw(PNGDRAW *p) {
  s_png->getLineAsRGB565(p, s_lineBuf, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
  s_tft->pushImage(s_offX, s_offY + p->y, p->iWidth, 1, s_lineBuf);
  return 1;
}
bool Background::drawPngFullScreen(const char *path, bool center) {
  if (!s_tft || !s_png) {
    return false;
  }

  if (s_png->open(path, pngOpen, pngClose, pngRead, pngSeek, pngDraw) !=
      PNG_SUCCESS) {
    return false;
  }

  s_offX = s_offY = 0;
  if (center) {
    int iw = s_png->getWidth();
    int ih = s_png->getHeight();
    if (iw < s_tft->width())
      s_offX = (s_tft->width() - iw) / 2;
    if (ih < s_tft->height())
      s_offY = (s_tft->height() - ih) / 2;
  }

  s_png->decode(NULL, 0);
  s_png->close();
  return true;
}

bool Background::draw(TFT_eSPI &tft, PNG &png, bool center) {
  s_tft = &tft;
  s_png = &png;
  return drawPngFullScreen(_path.c_str(), center);
}

void Background::listFS(const char *dir) {
  File root = LittleFS.open(dir);
  if (!root || !root.isDirectory())
    return;

  for (File f = root.openNextFile(); f; f = root.openNextFile())
    Serial.printf("%s (%uB)\n", f.name(), (unsigned)f.size());
}

void Background::addButton(const Button &btn) { _buttons.push_back(btn); }

void Background::clearButtons() { _buttons.clear(); }

bool Background::handleTouch(int16_t touchX, int16_t touchY,
                             bool isPressedNow) {
  bool anyClicked = false;
  for (auto &b : _buttons) {
    if (b.updateTouch(touchX, touchY, isPressedNow)) {
      anyClicked = true;
    }
  }
  return anyClicked;
}