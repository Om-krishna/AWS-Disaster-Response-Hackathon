#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <String.h>

#define DHTPIN A0
#define DHTTYPE DHT11
#define trigPin 5
#define echoPin 4

DHT dht(DHTPIN, DHTTYPE);
RF24 radio(10, 9); // CE, CSN
SoftwareSerial myserial(3, 2);  

const byte addresses[][6] = {"00001", "00002"};
int avgTemp, avgHum, avgLevel;
int calibValue;                 //Measured height after fixing it on a height on street pole
int currentValue;

struct DataGate{
  byte temp;
  byte hum;
  int level;
};
struct DataNode1{
  byte temp;
  byte hum;
  int level;
};

DataNode1 dataN1;
DataGate dataG;

void setup() {
  myserial.begin(9600); 
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00001
  radio.openReadingPipe(1, addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  CalibrationUltra();
}
void ShowSerialData()
{
  while (myserial.available() != 0)
    Serial.write(myserial.read());
  delay(1000);
 
}

// Fucntion for sending data using GET request
void GetRequest(){
  Serial.println("");
  Serial.println("********************");
 
 /*******GSM Communication Starts*******/

  if (myserial.available())
  Serial.write(myserial.read());
 
  myserial.println("AT");
  delay(3000);

  myserial.println("AT+CFUN=?");
  delay(3000);
 
  myserial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+CSTT=\"airtelgprs.com\",\"\",\"\"");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=1,1");
  delay(6000);
  ShowSerialData();

  myserial.println("AT+HTTPINIT");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CID\",1");
  delay(6000);
  ShowSerialData();

  String str = "AT+HTTPPARA=\"URL\",\"http://floodgateway.herokuapp.com/data?gatewayid=1069&temperature="+String(avgTemp)+"&humadity="+String(avgHum)+"&waterlevel="+String(avgLevel)+"\"";
  //myserial.println("AT+HTTPPARA=\"URL\",\"http://floodgateway.herokuapp.com/data?gatewayid=1069&temperature=70&humadity=60&waterlevel=50\""); //Server address
  myserial.println(str);
  delay(15000);
  ShowSerialData();
 
  myserial.println("AT+HTTPACTION=0");
  delay(20000);
  ShowSerialData();
 
  myserial.println("AT+HTTPREAD");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPTERM");
  delay(10000);
  ShowSerialData;
 
  /*******GSM Communication Stops*******/
  
}

int ultra(){
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  return distance;
}

void CalibrationUltra(){        //Taking intial measurment 
  int value;
  for(int i =0;i<5;i++){ 
    value = ultra();
    delay(500);
  }
  calibValue = value;
  Serial.print("Calibrated Value: ");
  Serial.println(calibValue);
}

void loop() {

  dataG.hum = dht.readHumidity();
  dataG.temp = dht.readTemperature();
  currentValue = ultra();
  dataG.level = calibValue-currentValue;
  Serial.print("Temperature Gate: ");
  Serial.println(dataG.temp);
  Serial.print("Humidity Gate: ");
  Serial.println(dataG.hum);
  Serial.print("Distance Gate: ");
  Serial.println(dataG.level);  
  if (radio.available()) {
    radio.read(&dataN1, sizeof(DataNode1));
    Serial.print("Temperature: ");
    Serial.println(dataN1.temp);
    Serial.print("Humidity: ");
    Serial.println(dataN1.hum);
    Serial.print("Distance: ");
    Serial.println(dataN1.level);
  }

  avgTemp = (dataG.temp+dataN1.temp)/2;
  avgHum = (dataG.hum+dataN1.hum)/2;
  avgLevel = (dataG.level + dataN1.level)/2;
  Serial.print("Average Temprature: ");
  Serial.println(avgTemp);
  Serial.print("Average Humidity: ");
  Serial.println(avgHum);
  Serial.print("Average Level: ");
  Serial.println(avgLevel);
  Serial.println("*****************---------------------********************");
  delay(4000);
  GetRequest();
}
