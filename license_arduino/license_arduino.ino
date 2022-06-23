#include "pitches.h"
#include "song.h"
#include "defs.h"
#include "DHT.h"
#include "RTClib.h"
#include <MCUFRIEND_kbv.h>

DHT dht;
RTC_DS3231 rtc;
MCUFRIEND_kbv tft;

void setup () 
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  dht.setup(DHT_PIN);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  uint16_t ID = tft.readID();
  tft.begin(ID);
  drawScreen();
}

void loop ()
{
  unsigned long currTime = millis();
  static unsigned long prevTime = 0;
  const unsigned long interval = 1000;
  if(currTime - prevTime >= interval) {
    prevTime += interval;
    //playSong();
    if(rtc.now().second() == 0) 
    {
      readTempHumid();
      drawScreen();
    }
  }
}

void readTempHumid() {
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" ");
  Serial.print(humidity, 1);
  Serial.print("%");
  Serial.print(" ");
  Serial.print(temperature, 1);
  Serial.println("C");
}

String getTime() {
  DateTime now = rtc.now();
  char format[] = "hh:mm";
  return now.toString(format);
}

String getDate() {
  DateTime now = rtc.now();
  char format[] = "DDD, MMM DD YYYY";
  return now.toString(format);
}

float getTemp() {
  return dht.getTemperature();
}

int getHumidity() {
  return dht.getHumidity();
}

void drawScreen() {
  tft.setRotation(1);
  int width = tft.width();
  int height = tft.height();
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, width, height, WHITE);
  drawDate();
  drawTime();
  drawTemperature();
  drawHumidity();
}

void drawTemperature() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(415, 10);
  tft.print(getTemp(), 1);
  tft.print("C");
}

void drawHumidity() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(330, 40);
  tft.print("Humidity:");
  tft.print(getHumidity());
  tft.print("%");
}

void drawDate() {
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 10);
  tft.print(getDate());
}

void drawTime() {
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
  tft.setCursor(150, 130);
  tft.print(getTime());
}
