#include "pitches.h"
#include "song.h"
#include "defs.h"
#include "DHT.h"

DHT dht;

void setup ()
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  dht.setup(DHT_PIN);
}

void loop ()
{
  //playSong();
  readTempHumid();
}

void readTempHumid() {
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  Serial.print(humidity, 1);
  Serial.print("%");
  Serial.print("\t");
  Serial.print(temperature, 1);
  Serial.println("C");
}
