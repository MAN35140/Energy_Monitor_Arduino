// Khai bao su dung cac thu vien
#include <SPI.h>  // https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/SPI
#include <LoRa.h> // https://github.com/sandeepmistry/arduino-LoRa

//* LoRa         Arduino Uno R3     ATmega328P
//* VCC              3.3V
//* GND              GND
//* RST               9              PB1 - 15
//* DIO0              2              PD2 - 4
//* EN/NSS            10             PB2 - 16 
//* MOSI              11             PB3 - 17 
//* MISO              12             PB4 - 18 
//* SCK               13             PB5 - 19 

// Khai bao ten cho cac chan
#define LED_System 7   // LED he thong, ON = sang, OFF = tat, PD7 - 13
#define LED_Work 8     // LED bao da gui du lieu xong, PB0 - 14

// Khai bao bien va du lieu
String LoRaData = ""; // Chuoi du lieu nhan ve qua LoRa
String device_id;     // Chua chuoi ma xac nhan Node ID
char ID_Node_1[] = "VNVP1"; // Ma nhan dang xac nhan ID Node1
char ID_Node_2[] = "VNVP2"; // Ma nhan dang xac nhan ID Node2 
String Vol_DC, Cur_DC, Power_DC;  // Cac bien du lieu DC
String Vol_AC, Cur_AC, Power_AC, Energy, Frequency, PowerFactor; // Cac bien du lieu AC
String Weather; // Bien du lieu thoi tiet
unsigned long last_time = 0;  // Gia tri time

// Khoi tao Struct
struct Node
{
  float Vol_DC;
  float Cur_DC;
  float Power_DC;
  float Vol_AC;
  float Cur_AC;
  float Power_AC;
  float Energy;
  float Frequency;
  float PowerFactor;
  float Weather;
};

struct Node Node1, Node2;

void setup()
{
  Serial.begin(115200); // Cau hinh Serial Port

  pinMode(LED_System, OUTPUT); // Khoi tao cau hinh LED ON/OFF he thong
  pinMode(LED_Work, OUTPUT); // Khoi tao cau hinh LED bao trang thai nhan du lieu
  digitalWrite(LED_System, HIGH); // Gateway Atmega328P ON

//  Serial.println("LoRa Receiver Callback");
  if (!LoRa.begin(433E6)) 
  {
//    Serial.println("Starting LoRa failed!");
//    while (1);
    asm volatile ("jmp 0");
  }

  // register the receive callback
  LoRa.onReceive(onReceive);
 
  // put the radio into receive mode
  LoRa.receive();
}

void loop() 
{
  // Khai bao cac bien chua vi tri cac ky tu dac biet
  int pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9, pos10;
    
  pos1 = LoRaData.indexOf('/'); // Xac dinh vi tri tu dau chuoi den '/' dau tien la 5
  device_id = LoRaData.substring(0, pos1); // Cat chuoi nhan duoc tu dau chuoi den vi tri thu 5, ket qua = "VNVP*"
  if(device_id == ID_Node_1 || device_id == ID_Node_2)  // Xac nhan dung ma ID 2 node
  {
    // Xac dinh vi tri cac ky tu dac biet trong chuoi
    pos2 = LoRaData.indexOf('~'); // Xac dinh vi tri tu dau chuoi den '~' dau tien la 10
    pos3 = LoRaData.indexOf('!');
    pos4 = LoRaData.indexOf('@');
    pos5 = LoRaData.indexOf('#');
    pos6 = LoRaData.indexOf('$');
    pos7 = LoRaData.indexOf('%');
    pos8 = LoRaData.indexOf('^');
    pos9 = LoRaData.indexOf('&');
    pos10 = LoRaData.indexOf('*');
    
    // Xu ly, tach chuoi nhan duoc va luu vao dung tung bien
    Vol_DC = LoRaData.substring(pos1 + 1, pos2);  // Cat chuoi nhan duoc tu vi tri 5+1 den vi tri thu 10
    Cur_DC = LoRaData.substring(pos2 + 1, pos3);
    Power_DC = LoRaData.substring(pos3 + 1, pos4);
    Vol_AC = LoRaData.substring(pos4 + 1, pos5);
    Cur_AC = LoRaData.substring(pos5 + 1, pos6);
    Power_AC = LoRaData.substring(pos6 + 1, pos7);
    Energy = LoRaData.substring(pos7 + 1, pos8);
    Frequency = LoRaData.substring(pos8 + 1, pos9);
    PowerFactor = LoRaData.substring(pos9 + 1, pos10);
    Weather = LoRaData.substring(pos10 + 1, LoRaData.length());
        
    if(device_id == ID_Node_1)  // Xac dinh dung chuoi nhan duoc la ID Node1
    {
      // Luu cac ket qua vao Struct Node1
      Node1.Vol_DC = Vol_DC.toFloat();
      Node1.Cur_DC = Cur_DC.toFloat();
      Node1.Power_DC = Power_DC.toFloat();
      Node1.Vol_AC = Vol_AC.toFloat();
      Node1.Cur_AC = Cur_AC.toFloat();
      Node1.Power_AC = Power_AC.toFloat();
      Node1.Energy = Energy.toFloat();
      Node1.Frequency = Frequency.toFloat();
      Node1.PowerFactor = PowerFactor.toFloat();
      Node1.Weather = Weather.toFloat();
    }
    if(device_id == ID_Node_2) // Xac dinh dung chuoi nhan duoc la ID Node2
    {
      // Luu cac ket qua vao Struct Node2
      Node2.Vol_DC = Vol_DC.toFloat();
      Node2.Cur_DC = Cur_DC.toFloat();
      Node2.Power_DC = Power_DC.toFloat();
      Node2.Vol_AC = Vol_AC.toFloat();
      Node2.Cur_AC = Cur_AC.toFloat();
      Node2.Power_AC = Power_AC.toFloat();
      Node2.Energy = Energy.toFloat();
      Node2.Frequency = Frequency.toFloat();
      Node2.PowerFactor = PowerFactor.toFloat();
      Node2.Weather = Weather.toFloat();
    }
  
    if((unsigned long)(millis() - last_time > 5000)) // Thuc hien sau moi 5s
    {
      // Gui du lieu 2 struct cho NodeMCU ESP8266 thong qua HardwareUART
      // Chan Arduino 1 - TX - PD1 - 3 ATmega328P ket noi RX NodeMCU ESP8266
      // Chan Arduino 0 - RX - PD0 - 2 ATmega328P ket noi TX NodeMCU ESP8266
      Serial.write((byte*)&Node1, sizeof(Node1));
      Serial.write((byte*)&Node2, sizeof(Node2));
        
      digitalWrite(LED_Work, HIGH); // received data
      delay(100);
      digitalWrite(LED_Work, LOW);
      
      LoRaData = "";  // Xoa chuoi nhan duoc
      last_time = millis();
    }
  }
}

void onReceive(int packetSize) 
{
  // received a packet
  LoRaData = LoRa.readString();
}
