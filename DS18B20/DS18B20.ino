#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2 // NodeMCU D4 / GPIO02
#define sensor 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature Suhu(&oneWire);

float t;

void setup() {
  Serial.begin(9600);
  Suhu.begin();
}

void loop() {
  // DS18B20 Value
  Suhu.requestTemperatures();
  t = Suhu.getTempCByIndex(0);
  Serial.println(t);
  delay(1000);
}
