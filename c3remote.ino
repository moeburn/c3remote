//IR remote on an ESP32C3 Supermini

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = "mikesnet";
const char* password = "springchicken";

AsyncWebServer server(80);  //for easy web uploading new firmware

TFT_eSPI tft = TFT_eSPI();       //for TFT display

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

#define every(interval) \
    static uint32_t __every__##interval = millis(); \
    if (millis() - __every__##interval >= interval && (__every__##interval = millis()))

IRsend irsend(8); //+ side of infrared LED is on pin 8

// Command #1: Power
// Protocol: Panasonic, Parameters: S=0U D=160U F=61U
uint16_t repeat_Power[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #2: TV
// Protocol: Panasonic, Parameters: S=0U D=160U F=159U
uint16_t repeat_TV[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #3: VCR
// Protocol: Panasonic, Parameters: S=0U D=160U F=158U
uint16_t repeat_VCR[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #4: VDP
// Protocol: Panasonic, Parameters: S=0U D=160U F=162U
uint16_t repeat_VDP[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #5: Tuner
// Protocol: Panasonic, Parameters: S=0U D=160U F=146U
uint16_t repeat_Tuner[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #6: CD
// Protocol: Panasonic, Parameters: S=0U D=160U F=148U
uint16_t repeat_CD[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #7: Tape Mon
// Protocol: Panasonic, Parameters: S=0U D=160U F=170U
uint16_t repeat_Tape_Mon[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #8: Stop
// Protocol: Panasonic, Parameters: S=8U D=160U F=0U
uint16_t repeat_Stop[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #9: Rew/Prev
// Protocol: Panasonic, Parameters: S=8U D=160U F=2U
uint16_t repeat_Rew_Prev[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #10: Ffwd/Next
// Protocol: Panasonic, Parameters: S=8U D=160U F=3U
uint16_t repeat_Ffwd_Next[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #11: Rev
// Protocol: Panasonic, Parameters: S=8U D=160U F=11U
uint16_t repeat_Rev[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #12: Play
// Protocol: Panasonic, Parameters: S=8U D=160U F=10U
uint16_t repeat_Play[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #13: Ch-
// Protocol: Panasonic, Parameters: S=0U D=128U F=53U
uint16_t repeat_Ch_[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #14: Ch+
// Protocol: Panasonic, Parameters: S=0U D=128U F=52U
uint16_t repeat_Ch__1[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #15: Ant TV/VCR
// Protocol: Panasonic, Parameters: S=0U D=144U F=54U
uint16_t repeat_Ant_TV_VCR[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #16: Disc/Deck
// Protocol: Panasonic, Parameters: S=8U D=160U F=149U
uint16_t repeat_Disc_Deck[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #17: 1
// Protocol: Panasonic, Parameters: S=10U D=160U F=16U
uint16_t repeat__1[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #18: 2
// Protocol: Panasonic, Parameters: S=10U D=160U F=17U
uint16_t repeat__2[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #19: 3
// Protocol: Panasonic, Parameters: S=10U D=160U F=18U
uint16_t repeat__3[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #20: 4
// Protocol: Panasonic, Parameters: S=10U D=160U F=19U
uint16_t repeat__4[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #21: 5
// Protocol: Panasonic, Parameters: S=10U D=160U F=20U
uint16_t repeat__5[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #22: 6
// Protocol: Panasonic, Parameters: S=10U D=160U F=21U
uint16_t repeat__6[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #23: 7
// Protocol: Panasonic, Parameters: S=10U D=160U F=22U
uint16_t repeat__7[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #24: 8
// Protocol: Panasonic, Parameters: S=10U D=160U F=23U
uint16_t repeat__8[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #25: 9
// Protocol: Panasonic, Parameters: S=10U D=160U F=24U
uint16_t repeat__9[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #26: 0
// Protocol: Panasonic, Parameters: S=10U D=160U F=25U
uint16_t repeat__0[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #27: >10
// Protocol: Panasonic, Parameters: S=10U D=160U F=132U
uint16_t repeat__10[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 65535U };

// Command #28: Direct
// Protocol: Panasonic, Parameters: S=10U D=160U F=238U
uint16_t repeat_Direct[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 65535U };

// Command #29: Mute
// Protocol: Panasonic, Parameters: S=0U D=160U F=50U
uint16_t repeat_Mute[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #30: Vol+
// Protocol: Panasonic, Parameters: S=0U D=160U F=32U
uint16_t repeat_Vol_[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };

// Command #31: Vol-
// Protocol: Panasonic, Parameters: S=0U D=160U F=33U
uint16_t repeat_Vol__1[100] = { 3456U, 1728U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 432U, 1296U, 432U, 65535U };
  
  int indexnum = 1;

void drawScreen(){  //routine to draw the currently selected infrared button on the TFT display
  tft.setCursor(20, 120);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK, TFT_CYAN, true);

  switch (indexnum){
    case 1:
        tft.print("POWER ");
        tft.print(indexnum);
        break;
    case 2:
        tft.print("TV ");
        tft.print(indexnum);
        break;
    case 3:
        tft.print("VCR ");
        tft.print(indexnum);
        break;
    case 4:
        tft.print("VDP ");
        tft.print(indexnum);
        break;
    case 5:
        tft.print("TUNER ");
        tft.print(indexnum);
        break;
    case 6:
        tft.print("CD ");
        tft.print(indexnum);
        break;
    case 7:
        tft.print("TAPE MON ");
        tft.print(indexnum);
        break;
    case 8:
        tft.print("STOP ");
        tft.print(indexnum);
        break;
    case 9:
        tft.print("REW PREV ");
        tft.print(indexnum);
        break;
    case 10:
        tft.print("FFWD NEXT ");
        tft.print(indexnum);
        break;
    case 11:
        tft.print("REV ");
        tft.print(indexnum);
        break;
    case 12:
        tft.print("PLAY ");
        tft.print(indexnum);
        break;
    case 13:
        tft.print("CH- ");
        tft.print(indexnum);
        break;
    case 14:
        tft.print("CH+ ");
        tft.print(indexnum);
        break;
    case 15:
        tft.print("ANT/TV/VCR ");
        tft.print(indexnum);
        break;
    case 16:
        tft.print("DISC DECK ");
        tft.print(indexnum);
        break;
    case 17:
        tft.print("1 ");
        tft.print(indexnum);
        break;
    case 18:
        tft.print("2 ");
        tft.print(indexnum);
        break;
    case 19:
        tft.print("3 ");
        tft.print(indexnum);
        break;
    case 20:
        tft.print("4 ");
        tft.print(indexnum);
        break;
    case 21:
        tft.print("5 ");
        tft.print(indexnum);
        break;
    case 22:
        tft.print("6 ");
        tft.print(indexnum);
        break;
    case 23:
        tft.print("7 ");
        tft.print(indexnum);
        break;
    case 24:
        tft.print("8 ");
        tft.print(indexnum);
        break;
    case 25:
        tft.print("9 ");
        tft.print(indexnum);
        break;
    case 26:
        tft.print("0 ");
        tft.print(indexnum);
        break;
    case 27:
        tft.print(">10 ");
        tft.print(indexnum);
        break;
    case 28:
        tft.print("Direct ");
        tft.print(indexnum);
        break;
    case 29:
        tft.print("Mute ");
        tft.print(indexnum);
        break;
    case 30:
        tft.print("Vol+ ");
        tft.print(indexnum);
        break;
    case 31:
        tft.print("Vol- ");
        tft.print(indexnum);
        break;
  }
}

void sendCode () {  //routine to actually beam the selected infrared code through the IR LED
  switch (indexnum){
    case 1:
        irsend.sendRaw(repeat_Power, 99, 37);
        break;
    case 2:
        irsend.sendRaw(repeat_TV, 99, 37);
        break;
    case 3:
        irsend.sendRaw(repeat_VCR, 99, 37);
        break;
    case 4:
        irsend.sendRaw(repeat_VDP, 99, 37);
        break;
    case 5:
        irsend.sendRaw(repeat_Tuner, 99, 37);
        break;
    case 6:
        irsend.sendRaw(repeat_CD, 99, 37);
        break;
    case 7:
        irsend.sendRaw(repeat_Tape_Mon, 99, 37);
        break;
    case 8:
        irsend.sendRaw(repeat_Stop, 99, 37);
        break;
    case 9:
        irsend.sendRaw(repeat_Rew_Prev, 99, 37);
        break;
    case 10:
        irsend.sendRaw(repeat_Ffwd_Next, 99, 37);
        break;
    case 11:
        irsend.sendRaw(repeat_Rev, 99, 37);
        break;
    case 12:
        irsend.sendRaw(repeat_Play, 99, 37);
        break;
    case 13:
        irsend.sendRaw(repeat_Ch_, 99, 37);
        break;
    case 14:
        irsend.sendRaw(repeat_Ch__1, 99, 37);
        break;
    case 15:
        irsend.sendRaw(repeat_Ant_TV_VCR, 99, 37);
        break;
    case 16:
        irsend.sendRaw(repeat_Disc_Deck, 99, 37);
        break;
    case 17:
        irsend.sendRaw(repeat__1, 99, 37);
        break;
    case 18:
        irsend.sendRaw(repeat__2, 99, 37);
        break;
    case 19:
        irsend.sendRaw(repeat__3, 99, 37);
        break;
    case 20:
        irsend.sendRaw(repeat__4, 99, 37);
        break;
    case 21:
        irsend.sendRaw(repeat__5, 99, 37);
        break;
    case 22:
        irsend.sendRaw(repeat__6, 99, 37);
        break;
    case 23:
        irsend.sendRaw(repeat__7, 99, 37);
        break;
    case 24:
        irsend.sendRaw(repeat__8, 99, 37);
        break;
    case 25:
        irsend.sendRaw(repeat__9, 99, 37);
        break;
    case 26:
        irsend.sendRaw(repeat__0, 99, 37);
        break;
    case 27:
        irsend.sendRaw(repeat__10, 99, 37);
        break;
    case 28:
        irsend.sendRaw(repeat_Direct, 99, 37);
        break;
    case 29:
        irsend.sendRaw(repeat_Mute, 99, 37);
        break;
    case 30:
        irsend.sendRaw(repeat_Vol_, 99, 37);
        break;
    case 31:
        irsend.sendRaw(repeat_Vol__1, 99, 37);
        break;
  }  
}


void setup(void) {
  pinMode(9,INPUT_PULLUP); //set 3 buttons to input with internal pullup so we don't have to fuck around with external ones just for buttons
  pinMode(10,INPUT_PULLUP);
  pinMode(20,INPUT_PULLUP);
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
  tft.print("Connecting...");  //display wifi connection progress
  tft.setCursor(15, 25);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {  //delay code until wifi is connected
        delay(250);
        tft.print(".");
      } 
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(15, 10);
  tft.print("Connected!");
  tft.setCursor(15, 40);
  tft.print(ssid);
  tft.setCursor(15, 65);
  tft.print(WiFi.localIP());  //display SSID and IP and wait a sec
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

  tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	tft.setTextSize(2);
  drawScreen();
}



void loop(void) {
  if (digitalRead(20)==LOW) {  //if right button is pressed
    every(500){   //debounce button
      indexnum++;  //increase selection by one
      if (indexnum > 31) {indexnum=1;}  //wrap around at the end
      drawScreen();  //draw the new selection on screen
    }
  }

  if (digitalRead(9)==LOW) {  //if left button is pressed
    every(501){ //do all the above but the other way around
      indexnum--;
      if (indexnum < 1) {indexnum=31;}
      drawScreen();
    }
  }

  if (digitalRead(10)==LOW) {  //if middle button is pressed
    tft.drawCircle(200,20,5,TFT_YELLOW);  //draw a little yellow circle so we know we're beaming IR
    sendCode(); //beam the IR
  }
  else {tft.drawCircle(200,20,5,TFT_BLACK);}  //erase said yellow circle when we've let go of the button
  delay(10); //relax, take a breath
}
