/*
 # Owner  : argosaroso
*/

//ThingSpeak
#include <ESP8266WiFi.h>;
#include <WiFiClient.h>;
#include <ThingSpeak.h>;

const char* ssid = "YourSSID";               // Network SSID
const char* password = "YourPassword";       // Network Password

WiFiClient client;

unsigned long myChannelNumber = Yourchannelnumeber;        // Channel Number
const char * myWriteAPIKey = "YourwriteAPIKey";            // Write API Key

//DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2 //D4 on NodeMCU GPIO02
#define sensor 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature Tmp(&oneWire);

float t;

//SEN0161
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define volt9 1.750             //Voltage Value pH = 9.18 
#define volt7 1.360             //Voltage Value pH = 7
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40      //times of collection

int pHArray[ArrayLenth];    //Store the average value of the sensor feedback
int pHArrayIndex=0;

void setup()
{
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  Tmp.begin();
  delay(10);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
}

void loop()
{
  // DS18B20 Value
  Tmp.requestTemperatures();
  t = Tmp.getTempCByIndex(0);
  // Print DS18B20 on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(t);                 
  Serial.println("*C");
  delay(1000);

  // SEN0161 Value
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*3.3/1024;
      pHValue = 7.00 - ((volt7-voltage) / ((volt9 - volt7)/(9.18-7)));   
   // pHstep = ((Voltage value pH = 9.18 - Voltage value pH = 7)/(PH9.18-PH7));
   // pHValue = 7.00 - (Voltage value pH = 7 - recent voltage) / pHstep;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
    Serial.print(voltage,3);
    Serial.print("    pH value: ");
    Serial.println(pHValue,2);
    digitalWrite(LED,digitalRead(LED)^1);
    printTime=millis();

    //Send Data to ThingSpeak
    ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
    ThingSpeak.writeField(myChannelNumber, 2, pHValue, myWriteAPIKey);
    delay(100);
  }
}
double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
