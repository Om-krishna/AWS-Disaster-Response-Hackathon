#include <RF24.h>
//#include <RF24Network.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN A0
#define DHTTYPE DHT11
#define trigPin 5
#define echoPin 4

DHT dht(DHTPIN, DHTTYPE);       

//int temp, hum;
int calibValue;                 //Measured height after fixing it on a height on street pole
int currentValue;

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
const byte addresses[][6] = {"00001", "00002"};
struct DataPackage{             //Data pack for transmission
  byte temp;
  byte hum;
  int level;
};

DataPackage data;               //Object of Data pack

void setup() {
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  CalibrationUltra();
}
int ultra(){                    //Measuring distance using ultrasonic sensor
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
  Serial.println(calibValue);
}

void loop() {
                 
  data.hum = dht.readHumidity();
  data.temp = dht.readTemperature();
  currentValue = ultra();
  data.level = calibValue-currentValue;               //Calculating level of water
  //data.level=ultra();
  Serial.print("Water level: ");
  Serial.print(data.level);
  Serial.print("Humidity: ");
  Serial.print(data.hum);
  Serial.print(", Temperature: ");
  Serial.println(data.temp);

  radio.write(&data, sizeof(DataPackage));
  delay(2000);
}
