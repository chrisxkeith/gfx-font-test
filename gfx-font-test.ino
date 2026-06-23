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
    }
    static void waitForSerial(String s) {
        delay(1000);
        String msg(s);
        msg.concat(", Press any character + Enter to continue: ");
        Utils::publish(msg);
        while (Serial.read() == -1) {
          delay(100);
        }
    }
    static String msToString(unsigned long ms) {
      int totalSeconds = ms / 1000;
      int secs = totalSeconds % 60;
      int minutes = (totalSeconds / 60) % 60;
      int hours = (totalSeconds / 60) / 60;
    
      char buf[100];
      sprintf(buf, "%02u:%02u:%02u", hours, minutes, secs);
      return String(buf);
    }
};
unsigned long Utils::lastPrintln = 0;

const int COLOR_WHITE = 0x65535;
const int COLOR_BLACK = 0x0;
const int COLOR_RED = 0xF800;
#include "Arduino_GigaDisplay_GFX.h"
#include "Fonts/FreeSans18pt7b.h"

GigaDisplay_GFX display_;

class OLEDWrapper {
  private:
    uint16_t currentColor = COLOR_WHITE;
    const int DEFAULT_FONT_SIZE = 3;
  public:
    void clear() {
      display_.fillScreen(COLOR_BLACK);
    }
    void startup() {
      delay(1000);
      display_.begin(); //init library
      clear();
      display_.setRotation(1);
    }
    void display(String s, const GFXfont* font, int textSize, uint8_t x, uint8_t y) {
      display_.setCursor(x, y);
      display_.setFont(font);
      display_.setTextSize(textSize);
      display_.print(s);
    }
    void display(String s, int textSize, uint8_t x, uint8_t y) {
      display(s, nullptr, textSize, x, y);
    }
    void display(String s) {
      display(s, DEFAULT_FONT_SIZE, 10, 10);
    }
    void display(String s[], int nStrings) {
      for (int i = 0; i < nStrings; i++) {
        display(s[i], DEFAULT_FONT_SIZE, 10, 32 + (i * 32));
      }
    }
    void setDrawColor(int color) {
      currentColor = color;
    }
    void setFont(const GFXfont* font) {
      display_.setFont(font);
    }
    void getTextBoundsWH(String string, const GFXfont* font, int textSize,
                          int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) {
      display_.setFont(font);
      display_.setTextSize(textSize);
      display_.getTextBounds(string, x, y, x1, y1, w, h);
    }
    void getTextBounds(String string, const GFXfont* font, int textSize,
                          int16_t* x1, int16_t* y1, uint16_t* x2, uint16_t* y2) {
      uint16_t w;
      uint16_t h;
      getTextBoundsWH(string, font, textSize, 0, 0, x1, y1, &w, &h);
      *x2 = *x1 + w;
      *y2 = *y1 + h;
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
    void dump() {
      String s("OLEDWrapper: getHeight(): ");
      s.concat(getHeight());
      s.concat(", getWidth(): ");
      s.concat(getWidth());
      Utils::publish(s);
    }
    void oneFontTest(const GFXfont* font, String fontName) {
      int16_t x;
      int16_t y;
      uint16_t w;
      uint16_t h;

      clear();
      setFont(font);
      display_.setTextSize(1);
      display_.getTextBounds(fontName, 0, 0, &x, &y, &w, &h);
      if (y < 0) {
        y = 0;
      }
      fillRectWH(x, y, w, h, COLOR_RED);
      String s(fontName);
      s.concat("\n");
      s.concat("x: ");
      s.concat(x);
      s.concat(", y: ");
      s.concat(y);
      s.concat(", w: ");
      s.concat(w);
      s.concat(", h: ");
      s.concat(h);
      display(s, font, 1, 0, h);
      Utils::waitForSerial(s);
    }
    void fontTest() {
      oneFontTest(&FreeSans18pt7b, "FreeSans18pt7b");
      oneFontTest(&FreeSans18pt7b, "acemnorsuvwxz");
      oneFontTest(&FreeSans18pt7b, "abcdefghijklmnopqrstuvwxyz");
      setFont(nullptr);
    }
};
OLEDWrapper* oledWrapper = nullptr;

void setup() {
    Serial.begin(115200);
    Utils::publish("setup() : started.");
    oledWrapper = new OLEDWrapper();
    oledWrapper->startup();
    Utils::publish("setup() : finished.");
}

void loop() {
	oledWrapper->fontTest();
}
