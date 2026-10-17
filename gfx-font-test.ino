class Utils {
  private:
    static unsigned long lastPrintln;
  public:
    // Slows everything down to make Serial output readable.
    static void publish(String s) {
      if (millis() < lastPrintln + 1000) {
        delay(1000);
      }
      Serial.println(s);
      lastPrintln = millis();
    }
};
unsigned long Utils::lastPrintln = 0;

const int COLOR_WHITE = 0x65535;
const int COLOR_BLACK = 0x0;
const int COLOR_RED = 0xF800;
const int COLOR_GREEN = 0x07E0;
const int COLOR_BLUE = 0x1F;
#include "Arduino_GigaDisplay_GFX.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"

GigaDisplay_GFX display_;

class OLEDWrapper {
  private:
    uint16_t currentColor = COLOR_WHITE;
    const int DEFAULT_FONT_SIZE = 3;
  public:
    void clear() {
      fillScreen(COLOR_BLACK);
    }
    void fillScreen(uint16_t color) {
      display_.fillScreen(color);
    }
    void startup() {
      delay(1000);
      display_.begin(); //init library
      clear();
      display_.setRotation(1);
    }
    void getTextBox(const GFXfont* font, String string, int textSize, int16_t x0, int16_t y0,
                    int16_t* x, int16_t* y, uint16_t* w, uint16_t* h) {
      display_.setFont(font);
      display_.setTextSize(textSize);
      display_.getTextBounds(string, x0, y0, x, y, w, h);
      // https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA/issues/43
      *w += (textSize * 4);
      *h += (textSize * 4);
    }
    void drawLine(int x0, int y0, int x1, int y1) {
      // Rotate not happening automatically?
      display_.drawLine(y0, x0, y1, x1, currentColor);
    }
    void fillRect(int x0, int y0, int x1, int y1, int color) {
      display_.fillRect(x0, y0, x1 - x0, y1 - y0, color); // is there an off-by-one error here?
    }  
    void fillRectWH(int x0, int y0, int w, int h, int color) {
      display_.fillRect(x0, y0, w, h, color);
    }  
    int getHeight() {
      return display_.height();
    }
    int getWidth() {
      return display_.width();
    }
    void displayAtXY(const GFXfont* font, String previousString, String s, int textSize,
                uint16_t x0, uint16_t y0, int textColor, int backgroundColor, boolean diagnose = false) {
      int16_t   x;
      int16_t   y;
      uint16_t  w;
      uint16_t  h;

      if (previousString.length() > 0) {
        getTextBox(font, previousString, textSize, x0, y0, &x, &y, &w, &h);
        display_.fillRect(x0, y0, w < getWidth() ? w : getWidth(), h, backgroundColor);
        if (diagnose) {
          String msg(previousString);
          msg.concat(" x0:"); msg.concat(x0); msg.concat(", y0:"); msg.concat(y0);
          msg.concat(", x:"); msg.concat(x);  msg.concat(", y:");  msg.concat(y);
          msg.concat(", w:"); msg.concat(w);  msg.concat(", h:");  msg.concat(h);
          Utils::publish(msg);
        }
      }
      getTextBox(font, s, textSize, x0, y0, &x, &y, &w, &h);
      display_.setTextColor(textColor);
      display_.setCursor(x0, y0 + h);
      display_.setFont(font);
      display_.setTextSize(textSize);
      display_.print(s);
      if (diagnose) {
        String msg(s);
        msg.concat(" x0:"); msg.concat(x0); msg.concat(", y0:"); msg.concat(y0);
        msg.concat(", x:"); msg.concat(x);  msg.concat(", y:");  msg.concat(y);
        msg.concat(", w:"); msg.concat(w);  msg.concat(", h:");  msg.concat(h);
        Utils::publish(msg);
      }
    }
   void stringsTest() {
      fillScreen(COLOR_BLUE);
      const int N_ITEMS = 5;
      String vals[N_ITEMS] = {"abc", "123", "xyz", "Ayz", ".;:"};
      for (int i = 0; i < N_ITEMS; i++) {
        String previousString = i > 0 ? String(vals[i - 1]) : "";
        int rgbVal = 255 - (i * (127 + 63) / N_ITEMS); // white -> 75% gray
        int color = display_.color565(rgbVal, rgbVal, rgbVal);
        displayAtXY(&FreeSans24pt7b, previousString, String(vals[i]), 1, 0, 0, color, COLOR_RED, true);
        delay(5000);
      }
   }

};
OLEDWrapper* oledWrapper = nullptr;

void setup() {
    Serial.begin(115200);
    oledWrapper = new OLEDWrapper();
    oledWrapper->startup();
}

bool isFirstLoop = true;
void loop() {
  if (isFirstLoop) {
//    isFirstLoop = false;
    oledWrapper->stringsTest();
  }
}
