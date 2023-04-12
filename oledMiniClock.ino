/*****************************************************************
  WiFi clock with 64x48 mini oled display shield. 

  Simple clock with cute pig during day time and of course
  night owl during night time.

  Based on: NTP TZ DST - bare minimum
  NetWork Time Protocol - Time Zone - Daylight Saving Time
  https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm

  Code by http://tg-music.neocities.org
  potatopasty@pm.me

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see https://www.gnu.org/licenses/.
******************************************************************/

/* Necessary Includes */

#include <ESP8266WiFi.h>            // we need wifi to get internet access
#include <WifiUDP.h>
#include <time.h>                   // time() ctime()
#include <WiFiManager.h>            // Use this, do not hard code your wifi name and password
#include <NTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "bitmaps.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* Configuration of NTP */
#define MY_NTP_SERVER "europe.pool.ntp.org"      // Europe   https://www.pool.ntp.org
#define MY_TZ "EET-2EEST,M3.5.0/3,M10.5.0/4"     // Helsinki https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, MY_NTP_SERVER);

/* Globals */
time_t now;                         // this is the epoch
tm tm;                              // the structure tm holds time information in a more convient way

void showTime() {
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);      // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);         // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);         // days since Sunday 0-6
  if (tm.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nWiFi-clock");

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setRotation(0);

  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

  // Set your own password and network name. IP-Address: 192.168.4.1 for Wifi setup.
  WiFiManager wifiManager;
  wifiManager.autoConnect("AP-NAME", "AP-PASSWORD");

  // start network
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ( "." );
  }
  Serial.println("\nWiFi connected");
  // by default, the NTP will be started after 60 secs
}

void loop() {
  showTime();

  display.clearDisplay();
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40, 20);
  if (tm.tm_hour < 10) {
    display.print(" ");
  }
  display.print(tm.tm_hour);
  display.print(":");
  if (tm.tm_min < 10) {
    display.print("0");
  }
  display.print(tm.tm_min);
  display.print(":");
  if (tm.tm_sec < 10) {
    display.print("0");
  }
  display.print(tm.tm_sec);


  if ((tm.tm_hour <= 7) || (tm.tm_hour >= 21)) {
    display.drawBitmap(31, 15, owl, 64, 48, 1);
  }
  else {
    display.drawBitmap(31, 15, pig, 64, 48, 1);
  }

  display.display();
}
