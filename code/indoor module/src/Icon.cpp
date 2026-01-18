#include "Icon.h"

extern PNG png;
Icon *Icon::_active = nullptr;
static File _iconFile;

Icon::Icon(TFT_eSPI *tft, const char *path, uint8_t trR, uint8_t trG,
           uint8_t trB)
    : _tft(tft), _sprite(tft), _path(path),
      _transparent565(rgb888To565(trR, trG, trB)), _loaded(false), _w(0),
      _h(0) {}

uint16_t Icon::rgb888To565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void *Icon::_pngOpen(const char *filename, int32_t *size) {
  _iconFile = LittleFS.open(filename, "r");
  if (!_iconFile) {
    Serial.printf("[Icon] open fail: %s\n", filename);
    *size = 0;
    return nullptr;
  }
  *size = _iconFile.size();
  return (void *)&_iconFile;
}

void Icon::_pngClose(void *handle) {
  File *f = (File *)handle;
  if (f && *f) {
    f->close();
  }
}

int32_t Icon::_pngRead(PNGFILE *file, uint8_t *buf, int32_t len) {
  File *f = (File *)file->fHandle;
  return f->read(buf, len);
}

int32_t Icon::_pngSeek(PNGFILE *file, int32_t pos) {
  File *f = (File *)file->fHandle;
  f->seek(pos);
  return pos;
}

int Icon::_pngDrawToSprite(PNGDRAW *pDraw) {
  if (!_active)
    return 0;

  Icon *self = _active;
  static uint16_t lineBuf[480];

  if (pDraw->iWidth > (int)(sizeof(lineBuf) / sizeof(lineBuf[0]))) {
    Serial.println("[Icon] Line too wide!");
    return 0;
  }

  png.getLineAsRGB565(pDraw, lineBuf, PNG_RGB565_LITTLE_ENDIAN, 0x0000);

  for (int x = 0; x < pDraw->iWidth; x++) {
    uint16_t c = lineBuf[x];
    if (c != self->_transparent565) {
      self->_sprite.drawPixel(x, pDraw->y, c);
    }
  }

  return 1;
}
bool Icon::loadFromFS() {

  if (_loaded) {
    _sprite.deleteSprite();
    _loaded = false;
  }

  Icon::_active = this;

  int res = png.open(_path.c_str(), Icon::_pngOpen, Icon::_pngClose,
                     Icon::_pngRead, Icon::_pngSeek, Icon::_pngDrawToSprite);

  if (res != PNG_SUCCESS) {
    Serial.printf("[Icon] PNG open failed for '%s' (err=%d)\n", _path.c_str(),
                  res);
    Icon::_active = nullptr;
    return false;
  }

  _w = png.getWidth();
  _h = png.getHeight();

  _sprite.setColorDepth(16);

  if (!_sprite.createSprite(_w, _h)) {
    Serial.println("[Icon] createSprite FAILED (RAM?)");
    png.close();
    Icon::_active = nullptr;
    return false;
  }

  _sprite.fillSprite(_transparent565);
  png.decode(NULL, 0);
  png.close();

  _loaded = true;
  Icon::_active = nullptr;

  Serial.printf("[Icon] Loaded '%s' (%dx%d)\n", _path.c_str(), _w, _h);

  return true;
}

void Icon::draw(int16_t x, int16_t y, uint16_t bgColor) {
  if (_loaded) {
    _sprite.pushSprite(x, y, bgColor);
  }
}

void Icon::unload() {
  if (_loaded) {
    _sprite.deleteSprite();
    _loaded = false;
  }
}
