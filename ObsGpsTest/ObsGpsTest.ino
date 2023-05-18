/**
 * ObsGpsTest.ino
 * for details: see README.md
 */

#include <Arduino.h>
#include <U8g2lib.h>
//#include <Wire.h> //I2C for the display did not seem to work
#include <TinyGPSPlus.h>
//#include <SoftwareSerial.h>
#include "GpsSoftwareSerial.h"

// Make sure that the button pin is pulled-down via a hardware resistor (as a pressed button will connect to VCC on the OBS display module);
// otherwise the input pin will float around and the display may show garbage.
static const int ButtonPin = 2;
static const int GpsSerialRxPin = 16; // OBS' TX_NEO6M signal: IO16
static const int GpsSerialTxPin = 17; // OBS' RX_NEO6M signal: IO17
static const uint32_t GpsSerialBaudSlow = 9600;
static const uint32_t GpsSerialBaudFast = 115200;
static const int MAX_SATELLITES = 32;

bool fastBaudRate = false;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
GpsSoftwareSerial ss(GpsSerialRxPin, GpsSerialTxPin);

unsigned int ssStartupRxCount = 0;

// Choosing a specific U8g2 constructor for our display
// (The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); // does not seem to work

TinyGPSCustom totalGPGSVMessages(gps, "GPGSV", 1); // $GPGSV sentence, first element
TinyGPSCustom messageNumber(gps, "GPGSV", 2);      // $GPGSV sentence, second element
TinyGPSCustom gpsSatNumber[4];
TinyGPSCustom gpsElevation[4];
TinyGPSCustom gpsAzimuth[4];
TinyGPSCustom gpsSnr[4];


#define OBSLogo_width 128
#define OBSLogo_height 64
const unsigned char OBSLogo [] PROGMEM = {
  // 'OBS Logo S, 128x64px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xfc, 0xfc, 0x8d, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e,
  0xfc, 0xfd, 0x9d, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x03, 0x00, 0xee, 0xfc, 0xfd, 0x9d, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0xc6, 0x8c, 0x1d, 0x9c, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0xc7,
  0x8c, 0x1d, 0xbc, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xff, 0x3f, 0xc7, 0x8c, 0xfd, 0xbc, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x3f, 0xc7, 0xcc, 0xfd, 0xfc, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x3f, 0xc7,
  0xfc, 0xfd, 0xec, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x03, 0x00, 0xc7, 0xfc, 0x1c, 0xec, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0xc7, 0x3c, 0x1c, 0xec, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0xc6,
  0x0c, 0x1c, 0xcc, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x03, 0x00, 0xe6, 0x0c, 0x1c, 0xcc, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x0c, 0xfc, 0x8d, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c,
  0x0c, 0xfc, 0x8d, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x38, 0x0c, 0xfc, 0x0d, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x07, 0x33, 0xc6, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x33, 0xe6, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x33, 0xe7,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x39, 0x33, 0xe3, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0xb3, 0xe3, 0x00, 0x00, 0x80, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x39, 0xf3, 0xe1,
  0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x3f, 0xf3, 0xe1, 0xe7, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0xf3, 0xe1, 0xe7, 0xff, 0xff, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0xf3, 0xe3,
  0xe7, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x39, 0xf3, 0xe3, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0x73, 0xe3, 0x00, 0x00, 0x80, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x31, 0x33, 0xe7,
  0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x39, 0x33, 0xe7, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x33, 0xee, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x33, 0xee,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x07, 0x33, 0xcc, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x8f, 0x9f, 0x31, 0x3c, 0x38, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x9f, 0x9f, 0x71, 0x7e, 0x7c, 0xfc, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xbf, 0x9f, 0x73,
  0x7f, 0xfe, 0xfc, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x99, 0x83, 0x73, 0x27, 0xe6, 0xcc, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x81, 0x73, 0x03, 0xc7, 0x8c, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x81, 0x77,
  0x07, 0xc7, 0x8c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x87, 0x9f, 0x77, 0x1f, 0xc7, 0xcc, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x9f, 0x9f, 0x7f, 0x3e, 0xc7, 0xfc, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9f, 0x7d,
  0x7c, 0xc7, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xbc, 0x81, 0x7d, 0x78, 0xc7, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x81, 0x7d, 0x60, 0xc7, 0xcc, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x81, 0x79,
  0x62, 0xc6, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xb9, 0x9f, 0x79, 0x77, 0xee, 0xcc, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x9f, 0x9f, 0x71, 0x7f, 0x7e, 0xcc, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x9f, 0x9f, 0x71,
  0x3e, 0x7c, 0x8c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x06, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const uint8_t *smallTextFont = u8g2_font_chikita_tn; // info from u8g2: The FontStruction "Chikita" (http://fontstruct.com/Ffontstructions/show/52325) by "southernmedia" is licensed under a Creative Commons Attribution Share Alike license 
const uint8_t *textFont = u8g2_font_chikita_tf;
const uint8_t *glyphFont = u8g2_font_siji_t_6x10; // info from u8g2: Siji (https://github.com/stark/siji), License/Copyright: Siji icon font is available under the "GNU General Public License v2.0"
const uint8_t *glyphFont2 = u8g2_font_streamline_interface_essential_other_t;
const uint8_t *glyphFont3 = u8g2_font_streamline_map_navigation_t;
const uint8_t *glyphFont4 = u8g2_font_streamline_interface_essential_link_t;
const uint8_t *glyphFont5 = u8g2_font_unifont_t_animals;

typedef struct
{
  int no;
  int elevation;
  int azimuth;
  int snr;
  bool active;
} satellite_t;

static satellite_t sats[MAX_SATELLITES]; // sorted by satellite number
static satellite_t sortedSats[MAX_SATELLITES]; // sorted by signal strength


void drawSatAbsolute(uint8_t x, uint8_t y, bool filled=false)
{
  u8g2.drawLine(x-4, y, x-2, y);
  u8g2.drawLine(x+2, y, x+4, y);
  u8g2.drawLine(x-4, y-2, x-4, y+2);
  u8g2.drawLine(x+4, y-2, x+4, y+2);
  u8g2.drawLine(x-1, y-2, x+1, y-2);
  u8g2.drawLine(x-1, y+2, x+1, y+2);
  u8g2.drawLine(x-2, y-1, x-2, y+1);
  u8g2.drawLine(x+2, y-1, x+2, y+1);
  if(filled)
  {
    u8g2.drawBox(x-1, y-1, 3, 3);
  }
}

void drawSatConstellation(int numSats, satellite_t* drawSats)
{
  uint8_t outer_radius = 24;
  uint8_t center_x = 4+outer_radius;
  uint8_t center_y = 4+outer_radius;
  u8g2.drawCircle(center_x, center_y, outer_radius*3/3, U8G2_DRAW_ALL);
  u8g2.drawCircle(center_x, center_y, outer_radius*2/3, U8G2_DRAW_ALL);
  u8g2.drawCircle(center_x, center_y, outer_radius*1/3, U8G2_DRAW_ALL);

  for(size_t i = 0; i < numSats; i++)
  {
    float r = (90.0f-float(drawSats[i].elevation))/90.0f * outer_radius;
    float dx = sin(float(drawSats[i].azimuth) * M_PI / 180.0f) * r;
    float dy = cos(float(drawSats[i].azimuth) * M_PI / 180.0f) * r;
    drawSatAbsolute(center_x + dx, center_y - dy, drawSats[i].snr > 15);
  }
}

void drawSatSignalBars(int numActiveSats, satellite_t* drawSats)
{
  const int maxBars = 7; // only space for 7 bars on the small display
  const uint8_t xOffs = 45;
  const uint8_t height = 6;
  const uint8_t yInc = height+1;
  const uint8_t maxSnr = 28;
  static char charBuffer[8];
  uint8_t width;
  uint8_t xPos = 64;
  uint8_t yPos = 0;

  yPos += yInc;

  int numBars = (numActiveSats > maxBars) ? maxBars : numActiveSats;
  for(int i = 0; i < numBars; i++)
  {
    // satellite number ("name") as 1-2 decimal digits
    u8g2.drawStr(xPos, yPos, itoa(drawSats[i].no, charBuffer, 10));

    // signal strength bar
    width = constrain(drawSats[i].snr, 0, maxSnr);
    u8g2.drawBox(xPos+12, yPos-height, width, height);
    // draw an additional 1 pixel wide bar at the end to indicate that this signal is stronger than `maxSnr`
    // (and therefore the previous bar width has been constrained to a certain width)
    if(drawSats[i].snr > maxSnr)
    {
      u8g2.drawBox(xPos+13+maxSnr, yPos-height, 1, height);
    }

    // satellite SNR as decimal number
    u8g2.drawStr(xPos+xOffs, yPos, itoa(drawSats[i].snr, charBuffer, 10));
    yPos += yInc;
  }
}

void drawTime()
{
  static char charBuffer[10];

  uint8_t xpos = 5;
  uint8_t ypos = 62;

  u8g2.setFont(glyphFont);
  u8g2.drawGlyph(xpos, ypos, 0xe016); // clock icon  

  if(gps.time.isValid())
  {
    u8g2.setFont(smallTextFont);
    snprintf(charBuffer, 9, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    u8g2.drawStr(xpos+12, ypos-1, charBuffer);
  }
  else
  {
    u8g2.drawGlyph(xpos+12, ypos, 0xe25d); // dots icon
  }
  u8g2.setFont(smallTextFont);
}

void drawMsgIndicators(bool seenUbx, bool seenNmea)
{
  uint8_t xpos = 63;
  uint8_t ypos = 62;

  u8g2.setFont(textFont);
  u8g2.drawButtonUTF8(xpos, ypos, seenUbx ? U8G2_BTN_BW1 | U8G2_BTN_INV : U8G2_BTN_BW1, 22, 2, 2, "UBX");
  u8g2.drawButtonUTF8(xpos+30, ypos, seenNmea ? U8G2_BTN_BW1 | U8G2_BTN_INV : U8G2_BTN_BW1, 22, 2, 2, "NMEA");
}

void drawGraphics(int numActiveSats, satellite_t* drawSats)
{
  u8g2.clearBuffer();

  // white on black
  u8g2.setDrawColor(1);

  // print the button state as text
  u8g2.setFont(smallTextFont);

  drawSatConstellation(numActiveSats, drawSats);

  drawSatSignalBars(numActiveSats, drawSats);

  drawTime();

  drawMsgIndicators(ss.hasSeenUbx(), ss.hasSeenNmea());

  u8g2.sendBuffer();
}

int satCompareFunc(const void * a, const void * b)
{
  // sorting order:
  // 1. active satellites first, then inactive ones
  // 2. satellites with higher SNR first, then lower SNRs
  // 3. for satellites with same SNR: lower satellite numbers first
  
  satellite_t* orderA = (satellite_t*)a;
  satellite_t* orderB = (satellite_t*)b;

  if(!orderA->active && orderB->active)
  {
    // active before inactive
    return 1;
  }
  else if(orderA->active && !orderB->active)
  {
    // active before inactive
    return -1;
  }
  else
  {
    if(orderB->snr != orderA->snr)
    {
      // SNR is main sorting criteria; higher SNR first
      return (orderB->snr - orderA->snr);
    }
    else
    {
      // lower satellite number first
      return (orderA->no - orderB->no);
    }
  }
}

void drawSplashScreen(bool infill)
{
  uint8_t xPos = 85;
  uint8_t yPos = 42; // the answer to everything

  u8g2.clearBuffer();

  // draw the logo (white text on black background)
  u8g2.setDrawColor(1);
  u8g2.drawXBMP(0, 0, OBSLogo_width, OBSLogo_height, OBSLogo);

  // draw world icon and location icon
  u8g2.setFont(glyphFont2);
  u8g2.drawGlyph(xPos, yPos, 0x0034); // world icon
  u8g2.setFont(glyphFont3);
  u8g2.drawGlyph(xPos-3, yPos-21, 0x0032); // location icon

  // add some satellite icons
  drawSatAbsolute(xPos-8, yPos-9, infill);
  drawSatAbsolute(xPos-5, yPos+1, !infill);
  drawSatAbsolute(xPos+10, yPos+5, infill);
  drawSatAbsolute(xPos+23, yPos+1, !infill);
  drawSatAbsolute(xPos+29, yPos-7, infill);

  // indicators for fast and slow
  u8g2.setFont(glyphFont5);
  if(fastBaudRate)
  {
    u8g2.drawGlyph(xPos+20, yPos-24, 0x0027); // rabbit (=fast) icon
  }
  else
  {
    u8g2.drawGlyph(xPos+20, yPos-24, 0x002b); // snail (=slow) icon
  }

  // finally, some info text
  u8g2.setFont(textFont);
  u8g2.drawStr(xPos-11, yPos+18, "GPS TEST");

  u8g2.sendBuffer();
}

void drawErrorScreen(bool seenUbx, bool seenNmea)
{
  static char charBuffer[8];
  uint8_t xPos = 67;
  uint8_t yPos = 13;

  u8g2.clearBuffer();

  // draw the logo (white text on black background)
  u8g2.setDrawColor(1);
  u8g2.drawXBMP(0, 0, OBSLogo_width, OBSLogo_height, OBSLogo);

  u8g2.setFont(textFont);
  u8g2.drawStr(xPos-3, yPos, "BAUD");
  u8g2.drawStr(xPos+25, yPos, itoa(fastBaudRate ? GpsSerialBaudFast : GpsSerialBaudSlow, charBuffer, 10));

  u8g2.drawStr(xPos-3, yPos+10, "RX ST"); // RX count at startup
  u8g2.drawStr(xPos+33, yPos+10, itoa((ssStartupRxCount > 9999) ? 9999 : ssStartupRxCount, charBuffer, 10));

  u8g2.drawStr(xPos-3, yPos+20, "RX NOW"); // RX count now
  unsigned int rxCount = ss.getRxCount();
  u8g2.drawStr(xPos+33, yPos+20, itoa((rxCount > 9999) ? 9999 : rxCount, charBuffer, 10));

  u8g2.setFont(glyphFont4);
  u8g2.drawGlyph(xPos+5, yPos+41, 0x0032); // broken link icon

  // further indicators for fast and slow
  u8g2.setFont(glyphFont5);
  u8g2.drawGlyph(xPos+29, yPos+37, fastBaudRate ? 0x0027 : 0x002b); // rabbit (=fast) icon, snail (=slow) icon

  drawMsgIndicators(seenUbx, seenNmea);

  u8g2.sendBuffer();
}

void setupDisplay()
{
  // prepare display and show animated splash screen
  u8g2.begin();
  u8g2.setBitmapMode(false /* solid */);
  u8g2.setFlipMode(1); // rotate display content by 180 degrees
  drawSplashScreen(0);
}

int sortSats()
{
  // this function sorts all satellites and return number of active satellites;
  // drawing and logging will only operate on the (by then) sorted copy
  qsort(sortedSats, MAX_SATELLITES, sizeof(satellite_t), satCompareFunc);

  // count active ones
  int numActiveSats = 0;
  for (int i=0; i<MAX_SATELLITES; ++i)
  {
    if(sortedSats[i].active)
    {
      numActiveSats++;
    }
  }
  return numActiveSats;
}

void pollUbx(uint8_t msgClass, uint8_t msgId)
{
  // send an UBX (binar protocol) message
  // (and implicitely expect an ACK-* message back; please note that the RX side is not covered here)
  uint8_t ubxTxMsg[8] = {
    0xB5, // 1st sync char
    0x62, // 2nd sync char
    msgClass, // message class
    msgId, // message ID
    0x00, // 1st length byte
    0x00, // 2nd length byte
    0xDE, // 1st checksum byte 'CK_A' (to be calculated)
    0xAD, // 2nd checksum byte 'CK_B' (to be calculated)
  };

  uint16_t checksum = calcFletcherChecksum(&ubxTxMsg[2], /*len=*/4); // calculate checksum (start at the class field, i.e. at offset +2)
  ubxTxMsg[6] = (checksum >> 8) & 0x00FF; // update CK_A
  ubxTxMsg[7] = checksum & 0x00FF; // update CK_B

  // send those message bytes!
  for(size_t i=0; i<sizeof(ubxTxMsg)/sizeof(ubxTxMsg[0]); i++)
  {
    ss.write(ubxTxMsg[i]);
  }
}

void printSatInfo(int numActiveSats)
{
  for(int i=0; i<numActiveSats; i++)
  {
    Serial.print(F("Sat #"));
    Serial.print(sortedSats[i].no);
    Serial.print(F(",  Act: "));
    Serial.print(sortedSats[i].active ? F("yes") : F("no "));
    Serial.print(F(",  El: "));
    Serial.print(sortedSats[i].elevation);
    Serial.print(F(", Az: "));
    Serial.print(sortedSats[i].azimuth);
    Serial.print(F(", SNR: "));
    Serial.println(sortedSats[i].snr);
  }
  Serial.print(F("  # active sats seen: "));
  Serial.println(numActiveSats);
}

void pollMultiUbx()
{
  // this function can be used to narrow down if the GPS module responds to any UBX message poll request at all!
  // please note that some polling requests might need payload (which is not implemented (yet?));
  // currently NAV-* messages are polled and also some CFG-* messages

  // TODO: keep track of responses to individual message classes+IDs; that must be done in the software serial bridge (i.e. GpsSoftwareSerial)

  // TODO: separate functionality and UI in a more proper way;
  //       especially because of delays/ wait times and the caused delay for reading serial answers back from the GPS module

  static uint8_t entryCount = 0; // don't poll all messages at once; do it in multiple bursts
  static char charBuffer[8];
  uint8_t xPos = 8;
  uint8_t yPos = 9;
  uint8_t xOffset = 86;
  const uint16_t showUiWaitTimeMs = 1200;

  entryCount++;

  u8g2.clearBuffer();
  u8g2.setFont(textFont);
  u8g2.drawStr(xPos, yPos, "Polling UBX msgs.");
  u8g2.sendBuffer();

  // for timing purposes first draw all the text messages, then send the poll requests
  if(entryCount == 1)
  {
    u8g2.drawStr(xPos+xOffset, yPos, "(1/4)");
    xPos += 4;

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-POSCEF");
  
    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-POSLLH");
  
    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-STATUS");
  
    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-DOP");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-SOL");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-VELECEF");

    // draw and wait, then send -- not the other way around!
    u8g2.sendBuffer();
    delay(showUiWaitTimeMs);

    pollUbx(0x01, 0x01);
    pollUbx(0x01, 0x02);
    pollUbx(0x01, 0x03);
    pollUbx(0x01, 0x04);
    pollUbx(0x01, 0x06);
    pollUbx(0x01, 0x11);
  }
  else if(entryCount == 2)
  {
    u8g2.drawStr(xPos+xOffset, yPos, "(2/4)");
    xPos += 4;

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-VELNED");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-TIMEGPS");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-TIMEUTC");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-CLOCK");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-SVINFO");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-DPGPS");

    u8g2.sendBuffer();
    delay(showUiWaitTimeMs);

    pollUbx(0x01, 0x12);
    pollUbx(0x01, 0x20);
    pollUbx(0x01, 0x21);
    pollUbx(0x01, 0x22);
    pollUbx(0x01, 0x30);
    pollUbx(0x01, 0x31);
  }
  else if(entryCount == 3)
  {
    u8g2.drawStr(xPos+xOffset, yPos, "(3/4)");
    xPos += 4;

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-SBAS");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-EFKSTATUS");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "NAV-AOPSTATUS");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-MSG");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-INF");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-DAT");

    u8g2.sendBuffer();
    delay(showUiWaitTimeMs);

    pollUbx(0x01, 0x32);
    pollUbx(0x01, 0x40);
    pollUbx(0x01, 0x60);
    pollUbx(0x06, 0x01); // TODO: has param(s)! payload length=2
    pollUbx(0x06, 0x02); // TODO: has param(s)! payload length=1
    pollUbx(0x06, 0x06);
  }
  else if(entryCount == 4)
  {
    u8g2.drawStr(xPos+xOffset, yPos, "(4/4)");
    xPos += 4;

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-TP");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-RATE");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-FXN");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-RXM");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-EKF");

    yPos += 8;
    u8g2.drawStr(xPos, yPos, "CFG-PRT");

    u8g2.sendBuffer();
    delay(showUiWaitTimeMs);

    pollUbx(0x06, 0x07);
    pollUbx(0x06, 0x08);
    pollUbx(0x06, 0x0E);
    pollUbx(0x06, 0x11);
    pollUbx(0x06, 0x12);
    pollUbx(0x06, 0x00); // this one seems to have "worked for me" --> answered! --> put it at the very end; TODO: needs to be optimized, see below  
  }
}

unsigned int checkCommunication(bool dataAvailable, bool trap, bool alwaysLogStatus=false)
{
  static bool statusLogged = false;
  static unsigned int checkCount = 0;

  ++checkCount;

  if(alwaysLogStatus || !dataAvailable || (dataAvailable && !statusLogged))
  {
    statusLogged = true; // do this only once and not cyclically

    bool seenUbx = ss.hasSeenUbx();
    bool seenNmea = ss.hasSeenNmea();

    unsigned int rxCount = ss.getRxCount();
    Serial.print(F("Current RX count: "));
    Serial.println(rxCount);

    if(!seenUbx && !seenNmea)
    {
      Serial.println(F("No RX data at all: check wiring."));
    }
    else
    {
      Serial.print(F("UBX RX "));
      if(!seenUbx)
      {
        Serial.print(F("not "));
      }
      Serial.println(F("seen."));

      Serial.print(F("NMEA RX "));
      if(seenNmea)
      {
        if(dataAvailable)
        {
          Serial.println(F("seen, data available. Everything seems to work."));
        }
        else
        {
          Serial.println(F("seen, but seems to be invalid or incomplete."));
        }
      }
      else
      {
        Serial.print(F("not seen."));
      }
    }

    if(!seenUbx)
    {
      pollMultiUbx();
    }

    if(!dataAvailable)
    {
      drawErrorScreen(seenUbx, seenNmea);

      // dump RX memory
      unsigned int len = ss.getRxStartupMemLen();
      Serial.print(F("RX startup memory length: "));
      Serial.println(len);
      Serial.print(F("RX count: "));
      Serial.println(rxCount);

      unsigned int loopCnt = (len < rxCount) ? len : rxCount;
      int* pMem = ss.getRxStartupMem();

      // dump received bytes as HEX
      for(unsigned int i=0; i<loopCnt; i++)
      {
        if(i % 8 == 0)
        {
          Serial.println(); // line break
        }
        Serial.print(" 0x");
        Serial.print(*pMem, HEX);
        pMem++;
      }
      Serial.println(); // final line break
      
      // trap the error permanently until reset
      if(trap)
      {
        Serial.println(F("Trapped."));
        while(true);
      }
    }    
  }

  return checkCount;
}

uint16_t calcFletcherChecksum(uint8_t* pData, size_t len)
{
  // calculate checksum using 8 bit Fletcher algorithm
  uint8_t ckA = 0;
  uint8_t ckB = 0;
  for(size_t i=0; i<len; i++)
  {
    ckA += *pData;
    ckB += ckA;
    pData++;
  }
  return ((uint16_t)ckA << 8) | (uint16_t)ckB;
}

void setup(void)
{
  unsigned int setupTime = millis();
  pinMode(ButtonPin, INPUT);
  if(digitalRead(ButtonPin) == HIGH)
  {
    fastBaudRate = true;
  }
  else
  {
    fastBaudRate = false;
  }

  // start communication with GPS module (either fast or slow)
  ss.begin(fastBaudRate ? GpsSerialBaudFast : GpsSerialBaudSlow);

  // before setting anything else up, use the time directly after startup 
  // to check for serial activity (and try to find NMEA or UBX);
  // only read from software serial, do not use for moving to GPS decoder;
  // the UBX-speaking modules dump some info on startup but are quiet from then on;
  // don't even setup the display before
  while(millis() <= (setupTime+2000))
  {
    // Dispatch incoming characters
    if (ss.available() > 0)
    {
      ss.read();
    }
  }

  setupDisplay();

  // use hardware serial for logging
  Serial.begin(115200);
  Serial.print(F("Using TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  if(fastBaudRate)
  {
    Serial.print(F("Using fast"));
  }
  else
  {
    Serial.print(F("Using slow"));
  }
  Serial.println(F(" baudrate w/ GPS module."));
  ssStartupRxCount = ss.getRxCount();
  Serial.print(F("Rx'ed no. of bytes during startup: "));
  Serial.println(ssStartupRxCount);
  //Serial.print(F("Setup time was: "));
  //Serial.println(setupTime);

  drawSplashScreen(1);

  // Initialize all the uninitialized TinyGPSCustom objects
  for (int i=0; i<4; ++i)
  {
    gpsSatNumber[i].begin(gps, "GPGSV", 4 + 4 * i);
    gpsElevation[i].begin(gps, "GPGSV", 5 + 4 * i);
    gpsAzimuth[i].begin(gps, "GPGSV", 6 + 4 * i);
    gpsSnr[i].begin(gps, "GPGSV", 7 + 4 * i);
  }

  for (int i=0; i<MAX_SATELLITES; ++i)
  {
    sats[i].no = -1;
    sats[i].active = false;
  }

  u8g2.setFont(smallTextFont);
}

void loop(void)
{
  static bool dataAvailable = false;
  static const int downsamplingFactor = 1; // base rate usually is 1 Hz, i.e. every second
  static int downsampleCounter = downsamplingFactor-1;

  // Dispatch incoming characters
  if (ss.available() > 0)
  {
    gps.encode(ss.read());

    if (totalGPGSVMessages.isUpdated())
    {
      dataAvailable = true;

      //Serial.println("got an update!"); // debugging only
      for (int i=0; i<4; ++i)
      {
        int no = atoi(gpsSatNumber[i].value());
        if (no >= 1 && no <= MAX_SATELLITES)
        {
          int elev = atoi(gpsElevation[i].value());
          int azimuth = atoi(gpsAzimuth[i].value());
          int snr = atoi(gpsSnr[i].value());
          sats[no-1].no = no;
          sats[no-1].active = true;
          sats[no-1].elevation = elev;
          sats[no-1].azimuth = azimuth;
          sats[no-1].snr = snr;
        }
      }
  
      int totalMessages = atoi(totalGPGSVMessages.value());
      int currentMessage = atoi(messageNumber.value());

      if (totalMessages == currentMessage)
      {
        // make sure we do not draw and print to the console too often
        ++downsampleCounter;
        if( downsampleCounter == downsamplingFactor )
        {
          downsampleCounter = 0;

          // get a copy for sorting and set all satellites back to inactive until seen again
          memcpy(sortedSats, sats, sizeof(sats));
          for (int i=0; i<MAX_SATELLITES; ++i)
          {
            sats[i].active = false;
          }

          int numActiveSats = sortSats();

          // uncomment for verbose log messages
          //printSatInfo(numActiveSats);

          drawGraphics(numActiveSats, sortedSats);
        }
      }
    }
  }

  // check for connection errors; forced poll of UBX messages when no UBX seen at all
  const uint32_t firstComCheckTimeMs = 10000;
  const uint32_t comCheckIntervalMs = 7000;
  if(millis() > firstComCheckTimeMs)
  {
    static int checkCount = 0;

    if(checkCount == 0)
    {
      checkCount = checkCommunication(dataAvailable, /*trap=*/false);
    }

    if(millis() > (firstComCheckTimeMs+comCheckIntervalMs) && checkCount == 1)
    {
      checkCount = checkCommunication(dataAvailable, /*trap=*/false, true); // don't trap yet
    }

    if(millis() > (firstComCheckTimeMs+2*comCheckIntervalMs) && checkCount == 2)
    {
      checkCount = checkCommunication(dataAvailable, /*trap=*/false, true); // still don't trap...
    }

    if(millis() > (firstComCheckTimeMs+3*comCheckIntervalMs) && checkCount == 3)
    {
      checkCount = checkCommunication(dataAvailable, /*trap=*/true, true); // finally trap...
    }
  }
}
