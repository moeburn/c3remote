// Demo using arcFill to draw ellipses and a segmented elipse
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = "mikesnet";
const char* password = "springchicken";

AsyncWebServer server(80);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

IRsend irsend(8);

#define PanasonicAddress      0x4004     // Panasonic address (Pre data) 
#define PanasonicPower        0x100BCBD  // Panasonic Power button

#define JVCPower              0xC5E8

uint16_t samsungProntoCode[72] = {
    0x0000, 0x006D, 0x0000, 0x0022,
    0x00ac, 0x00ac, 0x0016, 0x0040, 0x0016, 0x0040, 0x0016, 0x0040,
    0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015,
    0x0016, 0x0015, 0x0016, 0x0040, 0x0016, 0x0040, 0x0016, 0x0040,
    0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015,
    0x0016, 0x0015, 0x0016, 0x0040, 0x0016, 0x0015, 0x0016, 0x0015,
    0x0016, 0x0040, 0x0016, 0x0040, 0x0016, 0x0015, 0x0016, 0x0015,
    0x0016, 0x0040, 0x0016, 0x0015, 0x0016, 0x0040, 0x0016, 0x0040,
    0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0040, 0x0016, 0x0040,
    0x0016, 0x0015, 0x0016, 0x071c
};

// Panasonic Plasma TV Descrete code (Power On).
// Acquired from:
//   ftp://ftp.panasonic.com/pub/panasonic/drivers/monitors/Discrete-remote-control-codesProntoCCFformat.pdf
// e.g.
// 0000 0071 0000 0032 0080 003F 0010 0010 0010 0030 0010 0010 0010 0010 0010
// 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010
// 0010 0010 0010 0030 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010
// 0010 0010 0010 0010 0010 0010 0010 0010 0030 0010 0010 0010 0010 0010 0010
// 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0010 0030 0010
// 0030 0010 0030 0010 0030 0010 0030 0010 0010 0010 0010 0010 0010 0010 0030
// 0010 0030 0010 0030 0010 0030 0010 0030 0010 0010 0010 0030 0010 0A98
//
// Or the equiv. of sendPanasonic64(0x400401007C7D);
uint16_t panasonicProntoCode[104] = {
    0x0000, 0x0071, 0x0000, 0x0032,
    0x0080, 0x003F, 0x0010, 0x0010, 0x0010, 0x0030, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0030, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0030, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0030, 0x0010, 0x0030,
    0x0010, 0x0030, 0x0010, 0x0030, 0x0010, 0x0030, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0030, 0x0010, 0x0030,
    0x0010, 0x0030, 0x0010, 0x0030, 0x0010, 0x0030, 0x0010, 0x0010,
    0x0010, 0x0030, 0x0010, 0x0A98};


uint16_t repeat__Copy_to_Clipboard_[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #2: (Copy to Clipboard)$1
// Protocol: Panasonic, Parameters: S=0U D=160U F=62U
uint16_t repeat__Copy_to_Clipboard__1[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #3: (Copy to Clipboard)$2
// Protocol: Panasonic, Parameters: S=0U D=160U F=163U
uint16_t repeat__Copy_to_Clipboard__2[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #4: (Copy to Clipboard)$3
// Protocol: Panasonic, Parameters: S=0U D=160U F=148U
uint16_t repeat__Copy_to_Clipboard__3[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #5: (Copy to Clipboard)$4
// Protocol: Panasonic, Parameters: S=0U D=160U F=146U
uint16_t repeat__Copy_to_Clipboard__4[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #6: (Copy to Clipboard)$5
// Protocol: Panasonic, Parameters: S=0U D=176U F=129U
uint16_t repeat__Copy_to_Clipboard__5[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #7: (Copy to Clipboard)$6
// Protocol: Panasonic, Parameters: S=0U D=176U F=128U
uint16_t repeat__Copy_to_Clipboard__6[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #8: (Copy to Clipboard)$7
// Protocol: Panasonic, Parameters: S=0U D=176U F=133U
uint16_t repeat__Copy_to_Clipboard__7[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #9: (Copy to Clipboard)$8
// Protocol: Panasonic, Parameters: S=0U D=176U F=134U
uint16_t repeat__Copy_to_Clipboard__8[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #10: (Copy to Clipboard)$9
// Protocol: Panasonic, Parameters: S=0U D=176U F=135U
uint16_t repeat__Copy_to_Clipboard__9[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #11: (Copy to Clipboard)$10
// Protocol: Panasonic, Parameters: S=0U D=176U F=136U
uint16_t repeat__Copy_to_Clipboard__10[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #12: (Copy to Clipboard)$11
// Protocol: Panasonic, Parameters: S=0U D=160U F=32U
uint16_t repeat__Copy_to_Clipboard__11[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #13: (Copy to Clipboard)$12
// Protocol: Panasonic, Parameters: S=0U D=160U F=33U
uint16_t repeat__Copy_to_Clipboard__12[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #14: (Copy to Clipboard)$13
// Protocol: Panasonic, Parameters: S=0U D=160U F=50U
uint16_t repeat__Copy_to_Clipboard__13[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #15: (Copy to Clipboard)$14
// Protocol: Panasonic, Parameters: S=0U D=176U F=155U
uint16_t repeat__Copy_to_Clipboard__14[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #16: (Copy to Clipboard)$15
// Protocol: Panasonic, Parameters: S=18U D=160U F=187U
uint16_t repeat__Copy_to_Clipboard__15[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #17: (Copy to Clipboard)$16
// Protocol: Panasonic, Parameters: S=0U D=176U F=130U
uint16_t repeat__Copy_to_Clipboard__16[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #18: (Copy to Clipboard)$17
// Protocol: Panasonic, Parameters: S=0U D=160U F=57U
uint16_t repeat__Copy_to_Clipboard__17[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #19: (Copy to Clipboard)$18
// Protocol: Panasonic, Parameters: S=0U D=160U F=48U
uint16_t repeat__Copy_to_Clipboard__18[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #20: (Copy to Clipboard)$19
// Protocol: Panasonic, Parameters: S=0U D=160U F=178U
uint16_t repeat__Copy_to_Clipboard__19[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 65535U };



void setup(void) {
  irsend.begin();
  Serial.begin(115200);
    tft.init();
  tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.print("Connecting...");
  tft.setCursor(15, 25);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        tft.print(".");
      } 
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(15, 10);
  tft.print("Connected!");
  tft.setCursor(15, 40);
  tft.print(ssid);
  tft.setCursor(15, 65);
  tft.print(WiFi.localIP());
  delay(1000);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
  randomSeed(analogRead(2));

  tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(TFT_MAGENTA);
	tft.setTextSize(2);

}



void loop(void) {
   // tft.fillScreen(TFT_YELLOW);
	tft.setCursor(20, 120);
	tft.setTextColor(TFT_BLACK, TFT_YELLOW, true);
	tft.setTextSize(2);
  tft.print("SAMSUNG POWER");
  Serial.println("Sending a Samsung TV 'on' command.");
  irsend.sendPronto(samsungProntoCode, 72);
  delay(2000);
  tft.setCursor(20, 120);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK, TFT_MAGENTA, true);
  tft.print("PANASONIC POWER");
  Serial.println("Sending a Panasonic Plasma TV 'on' command.");
  irsend.sendPronto(panasonicProntoCode, 104);
  delay(2000);


  int indexnum = 0;
        tft.setCursor(20, 120);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard_, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__1, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__2, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__3, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__4, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__5, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__6, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__7, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__8, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__9, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__10, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__11, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__12, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__13, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__14, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__15, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__16, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__17, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__18, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_CYAN, true);
        tft.print("TECHNICS ");
        tft.print(indexnum);
        indexnum++;
        irsend.sendRaw(repeat__Copy_to_Clipboard__19, 99, 37);  // Send a raw data capture at 38kHz.
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 120);
        tft.setTextColor(TFT_BLACK, TFT_GREEN, true);
        tft.print("TEST COMPLETE");
        while(1);

}
