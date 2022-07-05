#include "song.h"
#include "defs.h"
#include "DHT.h"
#include "RTClib.h"
#include <MCUFRIEND_kbv.h>
DHT dht;
RTC_DS3231 rtc;
MCUFRIEND_kbv tft;
bool alarmScreen = false;
int hour, mins = 0;
int timeSelector = 0;
void setup () 
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ALARM_BUTTON, INPUT);
  pinMode(SELECT_BUTTON, INPUT);
  pinMode(INCREMENT_BUTTON, INPUT);
  pinMode(OK_BUTTON, INPUT);
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
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  drawScreen();
}
void loop ()
{
  if (rtc.alarmFired(1) == true){
 //playSong();
rtc.clearAlarm(1);
}
  unsigned long currTime = millis();
  static unsigned long prevTime = 0;
  const unsigned long interval = 1000;
  if(currTime - prevTime >= interval) {
    prevTime += interval;
    //playSong();
    if(!alarmScreen) {
      if(rtc.now().second() == 0) 
      {
        drawScreen();
      }
    }
  }
  readButtons();
}
void readButtons() {
  static byte alarm_prev_state;
  static byte select_prev_state;
  static byte increment_prev_state;
  static byte ok_prev_state;
  
  byte alarm_state = digitalRead(ALARM_BUTTON);
  byte select_state = digitalRead(SELECT_BUTTON);
  byte increment_state = digitalRead(INCREMENT_BUTTON);
  byte ok_state = digitalRead(OK_BUTTON);
  if(alarm_state && !alarm_prev_state && !alarmScreen) {
    Serial.println("Going to alarm screen");
    alarmScreen = true;
    drawAlarmScreen();
  }  else
  if(alarm_state && !alarm_prev_state && alarmScreen) {
    Serial.println("Going back to main screen");
    alarmScreen = false;
    drawScreen();
  }
  if(select_state && !select_prev_state && alarmScreen) {
    // Switch between hour and minutes, also redraw screen.
    
    if(timeSelector == 0){
        timeSelector = 1;
        Serial.println("Select minutes");
        }
        else
    if(timeSelector == 1){
        timeSelector = 0;
        Serial.println("Select hour");
        }
  }
  if(increment_state && !increment_prev_state && alarmScreen) {
    // Increment the hours/minutes depending on what's selected, also redraw screen.
    if(timeSelector == 0) {
        if(hour + 1 > 23)
            hour = 0;
        else
          hour += 1;
    }
    else {
        if(mins + 5 > 59)
            mins = 0;
        else
          mins += 5;
    }
    drawAlarmScreen();
  }
  if(ok_state && !ok_prev_state && alarmScreen) {
    // Set the alarm and go back to the main screen.
    rtc.setAlarm1(DateTime(2022, 6, 25, hour, mins, 0), DS3231_A1_Hour);
    alarmScreen = false;
    drawScreen();
  }
  
  alarm_prev_state = alarm_state;
  select_prev_state = select_state;
  increment_prev_state = increment_state;
  ok_prev_state = ok_state;
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
  if(temp > 30
  )
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
  tft.setTextSize(7);
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
  tft.setTextSize(7);
  tft.setTextColor(WHITE);
  tft.setCursor(150, 130);
  tft.print(hour);
  tft.print(":");
  tft.print(mins);
}
