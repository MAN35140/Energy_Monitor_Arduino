// Khai bao su dung cac thu vien
#include <ThingSpeak.h>   // https://github.com/mathworks/thingspeak-arduino
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi

char ssid[] = "TP-Link_1CC4";   // your network SSID (name) 
char pass[] = "76767676";       // your network password

WiFiClient  client;

unsigned long myChannelNumber_Wind = 1679967;  // Channel ID Channel Wind
const char * myWriteAPIKey_Wind = "XVUYKD4293ZRATMT"; // Write API Key Channel Wind

unsigned long myChannelNumber_Solar = 1678412;  // Channel ID Channel Solar
const char * myWriteAPIKey_Solar = "TV81HAV44XK6J1PP"; // Write API Key Channel Solar

unsigned long myChannelNumber_3 = 1679968;  // Channel ID Channel PF, Weather
const char * myWriteAPIKey_3 = "FZH5P53DUT56TDAZ"; // Write API Key Channel PF, Weather

// Khai bao Struct
struct Node
{
  float DCVoltage;
  float DCCurrent;
  float DCPower;
  float ACVoltage;
  float ACCurrent;
  float ACPower;
  float Energy;
  float Frequency;
  float PowerFactor;
  float Weather;
};

struct Node Node1, Node2;

unsigned long last_time = 0;  // Gia tri time

// Ham ngat nhan du lieu tu ATmega328P thong qua HardwareUART
void SerialEvent()
{
  while (Serial.available()) 
  {
    Serial.readBytes((byte*)&Node1, sizeof(Node1));
    Serial.readBytes((byte*)&Node2, sizeof(Node2));
  }
}

// Ham gui du lieu len Server ThingSpeak
void WriteDataToThingSpeak()
{
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      delay(5000);     
    } 
  }
  else
  {
    if((unsigned long)(millis() - last_time > 17000)) // Wait 17000 miliseconds to update the channel again
    {
      // set the fields with the values
      ThingSpeak.setField(1, Node1.DCVoltage);
      ThingSpeak.setField(2, Node1.DCCurrent);
      ThingSpeak.setField(3, Node1.DCPower);
      ThingSpeak.setField(4, Node1.ACVoltage);
      ThingSpeak.setField(5, Node1.ACCurrent);
      ThingSpeak.setField(6, Node1.ACPower);
      ThingSpeak.setField(7, Node1.Energy);
      ThingSpeak.setField(8, Node1.Frequency);
    
      // write to the ThingSpeak channel Solar
      int x = ThingSpeak.writeFields(myChannelNumber_Wind, myWriteAPIKey_Wind);
  
      // set the fields with the values
      ThingSpeak.setField(1, Node2.DCVoltage);
      ThingSpeak.setField(2, Node2.DCCurrent);
      ThingSpeak.setField(3, Node2.DCPower);
      ThingSpeak.setField(4, Node2.ACVoltage);
      ThingSpeak.setField(5, Node2.ACCurrent);
      ThingSpeak.setField(6, Node2.ACPower);
      ThingSpeak.setField(7, Node2.Energy);
      ThingSpeak.setField(8, Node2.Frequency);
      
      // write to the ThingSpeak channel Solar
      int y = ThingSpeak.writeFields(myChannelNumber_Solar, myWriteAPIKey_Solar);
  
      ThingSpeak.setField(1, Node2.PowerFactor);
      ThingSpeak.setField(2, Node1.PowerFactor);
      ThingSpeak.setField(3, Node2.Weather);
      ThingSpeak.setField(4, Node1.Weather);
      
      // write to the ThingSpeak channel PowerFactor, Weather
      int z = ThingSpeak.writeFields(myChannelNumber_3, myWriteAPIKey_3);
  
      Node1.DCVoltage = 0;
      Node1.DCCurrent = 0;
      Node1.DCPower = 0;
      Node1.ACVoltage = 0;
      Node1.ACCurrent = 0;
      Node1.ACPower = 0;
      Node1.Energy = 0;
      Node1.Frequency = 0;
      Node1.PowerFactor = 0;
      Node1.Weather = 0;

      Node2.DCVoltage = 0;
      Node2.DCCurrent = 0;
      Node2.DCPower = 0;
      Node2.ACVoltage = 0;
      Node2.ACCurrent = 0;
      Node2.ACPower = 0;
      Node2.Energy = 0;
      Node2.Frequency = 0;
      Node2.PowerFactor = 0;
      Node2.Weather = 0;
      
      digitalWrite(16,0);
      delay(100);
      digitalWrite(16,1);
      last_time = millis();
    }
  }
}

void setup()
{
  Serial.begin(115200);  // Initialize serial
  pinMode(16, OUTPUT);   // Cau hinh chan LED bao hieu
  digitalWrite(16,1);    // LED OFF
  
  WiFi.mode(WIFI_STA); 
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() 
{
  SerialEvent();
  WriteDataToThingSpeak();
}
