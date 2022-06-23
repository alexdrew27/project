#include "song.h"
#include "defs.h"
#include "DHT.h"
#include "RTClib.h"
#include <MCUFRIEND_kbv.h>

DHT dht;
RTC_DS3231 rtc;
MCUFRIEND_kbv tft;

bool alarmScreen = false;

void setup () 
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ALARM_BUTTON, INPUT);

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
    readButtons();
    //playSong();
    if(rtc.now().second() == 0) 
    {
      drawScreen();
    }
  }
  readButtons();
}

void readButtons() {
  static byte alarm_prev_state;
  //static byte cancel_prev_state;
  //static byte min10_prev_state;
  //static byte min_prev_state;
  
  byte alarm_state = digitalRead(ALARM_BUTTON);
  //byte cancel_state = digitalRead(CANCEL);
  //byte min10_state = digitalRead(MIN10);
  //byte min_state = digitalRead(MIN);

  if(alarm_state && !alarm_prev_state && !alarmScreen) {
    Serial.println("Going to alarm screen");
    alarmScreen = true;
    drawAlarmScreen();
  }

  if(alarm_state && !alarm_prev_state && alarmScreen) {
    Serial.println("Going back to main screen");
    alarmScreen = false;
    drawScreen();
  }
  
  alarm_prev_state = alarm_state;
  //min_prev_state = min_state;
  //sec10_prev_state = sec10_state;
  //start_prev_state = start_state;
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
  float temp = getTemp();
  if(temp > 25)
    digitalWrite(RED_LED, HIGH);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(415, 10);
  tft.print(temp, 1);
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

void drawAlarmScreen() {
  tft.setRotation(1);
  int width = tft.width();
  int height = tft.height();
  tft.fillScreen(BLACK);
  tft.drawRect(0, 0, width, height, WHITE);
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
  tft.setCursor(150, 130);
  tft.print("00:00");
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
  tft.setCursor(120, 70);
  tft.print("Alarm set");
  Serial.println("Alarm set");
  delay(5000);
}
