#include <Arduino.h>
#include <GxEPD.h>
#include <GxGDEP015OC1/GxGDEP015OC1.cpp> // 1.54" b/w 
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include <YoutubeApi.h>
#include <ESP8266WiFi.h>
#include "./youtubelogo.h"
#include <ArduinoJson.h>
#include "./credentials.h"

void showFontCallback();

#define PROJECT_NAME "YOUTUBE-SUBSCRIBER-DISPLAY2"
#define TIME_ZONE (+2)
#define DEBUG
#ifdef DEBUG
#define DEBUG_LOG(x) Serial.print(x)
#else
#define DEBUG_LOG(x)
#endif

#if defined(ESP8266)
GxIO_Class io(SPI, SS, D3, D4); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io); // default selection of D4(=2), D2(=4)
#endif

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs = 600000; // mean time between api requests
String subscribers, viewCount, videoCount;

void setup() {
    Serial.begin(115200);
    DEBUG_LOG("\nsetup");
    display.init();
    display.setRotation(2);
    DEBUG_LOG("\nsetup done");

    WiFi.persistent(false);
    //WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.hostname(PROJECT_NAME);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    DEBUG_LOG("\nWiFi connecting: ");
    while(WiFi.status() != WL_CONNECTED) {
        DEBUG_LOG(".");
        delay(500);
    }

    DEBUG_LOG("\nWiFi connected ");
    DEBUG_LOG(WiFi.localIP());
    DEBUG_LOG("\n");
}

void loop() {
    DEBUG_LOG("\nYoutube Abruf gestartet\n");
    if(api.getChannelStatistics(CHANNEL_ID)) {
        subscribers = api.channelStats.subscriberCount;
        viewCount = api.channelStats.viewCount;
        videoCount = api.channelStats.videoCount;
        display.drawPaged(showFontCallback);
        DEBUG_LOG("\nSubscriber Count: ");
        DEBUG_LOG(subscribers);
        DEBUG_LOG("\nView Count: ");
        DEBUG_LOG(viewCount);
        DEBUG_LOG("\nVideo Count: ");
        DEBUG_LOG(videoCount);
    }
    delay(api_mtbs);
}

void showFontCallback()
{
  const GFXfont* f24pt = &FreeSansBold24pt7b;
  const GFXfont* f9pt = &FreeSans9pt7b;

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.drawLine(0, 60, 200, 60, GxEPD_BLACK);
  display.setFont(f24pt);
  int x = (subscribers.length() * 30) / 2; // make the text in the center of the display
  display.setCursor(100 - x, 100);
  display.println(subscribers);
  display.setFont(f9pt);
  display.setCursor(45, 120);
  display.setTextColor(GxEPD_BLACK);
  display.println("Subscribers");

  display.drawBitmap(30, 10, gImage_ytlogo1, 46, 32, GxEPD_BLACK);
  display.drawBitmap(76, 11, gImage_ytlogo2, 94, 29, GxEPD_BLACK);

  display.drawLine(0, 140, 200, 140, GxEPD_BLACK);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(0, 165);
  display.println("Views:   " + viewCount);
  display.setCursor(0, 185);
  display.println("Videos:  " + videoCount);
}
