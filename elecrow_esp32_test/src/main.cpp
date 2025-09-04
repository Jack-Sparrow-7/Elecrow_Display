#include <LovyanGFX.hpp>
#include <Arduino.h>

// A custom configuration for your specific ESP32 display.
class LGFX_Custom : public lgfx::LGFX_Device
{
  // Set the panel driver to ILI9341
  lgfx::Panel_ILI9341     _panel_instance;

  // Use the SPI bus for communication
  lgfx::Bus_SPI           _bus_instance;

  // Use PWM for backlight control
  lgfx::Light_PWM         _light_instance;

  // Set the touch driver to XPT2046
  lgfx::Touch_XPT2046     _touch_instance;

public:
  // Constructor
  LGFX_Custom(void)
  {
    // === SPI Bus Configuration ===
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = VSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 27000000;  // 27MHz, from your working setup
      cfg.freq_read  = 16000000;
      cfg.pin_sclk = 14;           // Your SCLK pin
      cfg.pin_mosi = 13;           // Your MOSI pin
      cfg.pin_miso = 12;           // Your MISO pin
      cfg.pin_dc   = 2;            // Your D/C pin

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // === Display Panel Configuration ===
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;   // Your CS pin
      cfg.pin_rst          = -1;   // Your RST pin (-1 for not used)
      cfg.pin_busy         = -1;
      cfg.panel_width      = 240;
      cfg.panel_height     = 320;
      cfg.bus_shared       = true;

      _panel_instance.config(cfg);
    }

    // === Backlight Configuration ===
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 27;             // Your Backlight pin
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    // === Touch Screen Configuration ===
    {
      auto cfg = _touch_instance.config();
      cfg.x_min      = 300;   // Raw minimum X value for calibration
      cfg.x_max      = 3800;  // Raw maximum X value
      cfg.y_min      = 200;   // Raw minimum Y value
      cfg.y_max      = 3800;  // Raw maximum Y value
      cfg.pin_int    = -1;    // INT pin not used in your setup
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;

      // SPI settings for touch
      cfg.spi_host = VSPI_HOST;
      cfg.freq = 2500000;     // SPI clock for touch
      cfg.pin_sclk = 14;      // Must be same as display SCLK
      cfg.pin_mosi = 13;      // Must be same as display MOSI
      cfg.pin_miso = 12;      // Must be same as display MISO
      cfg.pin_cs   = 33;      // Your Touch CS pin

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

// Create an instance of our custom class.
LGFX_Custom display;

void setup(void)
{
  Serial.begin(115200);
  display.init();

  // Run touch calibration
  if (display.touch())
  {
    if (display.width() < display.height()) {
      display.setRotation(display.getRotation() ^ 1);
    }
    // A simple prompt on the screen
    display.setTextDatum(textdatum_t::middle_center);
    display.drawString("touch the arrow marker.", display.width() / 2, display.height() / 2);
    display.setTextDatum(textdatum_t::top_left);

    // Calibrate the touch screen
    display.calibrateTouch(nullptr, TFT_WHITE, TFT_BLACK, std::max(display.width(), display.height()) >> 3);
  }

  display.fillScreen(TFT_BLACK);
}

uint32_t count = 0;

void loop(void)
{
  // The original demo loop from your file
  display.setRotation(++count & 7);
  display.setColorDepth((count & 8) ? 16 : 24);

  display.setTextColor(TFT_WHITE);
  display.drawNumber(display.getRotation(), 16, 0);

  display.setTextColor(TFT_RED);
  display.drawString("R", 30, 16);
  display.setTextColor(TFT_GREEN);
  display.drawString("G", 40, 16);
  display.setTextColor(TFT_BLUE);
  display.drawString("B", 50, 16);

  display.drawRect(30, 30, display.width() - 60, display.height() - 60, TFT_YELLOW);

  int32_t x, y;
  if (display.getTouch(&x, &y)) {
    display.fillRect(x - 2, y - 2, 5, 5, TFT_CYAN);
  }
  delay(10);
}