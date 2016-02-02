/*
This is the beta for Grow.0 I/O board "model zero":
->The goal of this beta is to read data from two soil
moisture sensors and the AM2301 (DHT21) to log environment
changes to a MySQL database at predetermined intervals.
->The next beta must read from the MySQL database (or be told
what to do by python serial?) in order to power any 
number of the 4 attached relays in response to environment
changes.
*/

#include <dht.h> // DHT Library for temp/humidity sensor
dht DHT;

int poll_soil = 5; // Loop will run this many times before checking soil (extending the life of the sensors)
int loop_delay = 30000; // ms delay at the end of the loop

#define DHT21_PIN 5
int soilpin[] = {A0, A1}; // Think modular, use arrays!
int relaypin[] = {2, 4, 7, 8}; // Using non-PWM pins for non-PWM device
int pwr_pin[] = {3, 6}; // pins to power soil moisture sensors only when needed
int counter = 0;
int TEMP;
int HUMI;
int SOIL_A;
int SOIL_B;

void setup() {
  pinMode(soilpin[0], INPUT);
  pinMode(soilpin[1], INPUT);
  pinMode(relaypin[0], OUTPUT);
  pinMode(relaypin[1], OUTPUT);
  pinMode(relaypin[2], OUTPUT);
  pinMode(relaypin[3], OUTPUT);
  pinMode(pwr_pin[0], OUTPUT);
  pinMode(pwr_pin[1], OUTPUT);  
  Serial.begin(115200);
}

int readSensor(int DID) {
  int output;
  
  switch (DID) {
    
    case 1:
    digitalWrite(pwr_pin[0], HIGH);
    delay(500);
    output = analogRead(soilpin[0]);
    digitalWrite(pwr_pin[0], LOW);
    break;

    case 2:
    digitalWrite(pwr_pin[1], HIGH);
    delay(500);
    output = analogRead(soilpin[1]);
    digitalWrite(pwr_pin[1], LOW);
    break;

    case 3:
    output = readDHT(1);
    break;

    case 4:
    output = readDHT(2);
    break;

    default:
    output = -999;
    break;    
  }
  return output;
}

int readDHT(int d) {
  float output;
  int chk = DHT.read21(DHT21_PIN);
    switch (chk)
    {
    case DHTLIB_OK:
        //Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.println("DHT Checksum error");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.println("DHT Time out error");
        break;
    case DHTLIB_ERROR_CONNECT:
        Serial.println("DHT Connect error");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.println("DHT Ack Low error");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.println("DHT Ack High error");
        break;
    default:
        Serial.print("Unknown error,\t");
        break;
    }
  
  switch (d) {
    case 1:
    output = DHT.temperature;
    break;
    case 2:
    output = DHT.humidity;
    break;
    default:
    output = -1;
    break;
  }
  
  return output; 
}

void loop() {
  if (counter == poll_soil) {
    SOIL_A = readSensor(1); // updates these values as they change, saves them for when they don't
    SOIL_B = readSensor(2);
    counter = 0;
  }
  TEMP = readSensor(3);
  delay(2000); // 2 sec delay for sensor to catch up
  HUMI = readSensor(4);
  Serial.print("Temperature (C): ");
  Serial.println(TEMP);
  Serial.print("Humidity: ");
  Serial.println(HUMI);
  Serial.print("Soil Moisture (A): ");
  Serial.println(SOIL_A);
  Serial.print("Soil Moisture (B): ");
  Serial.println(SOIL_B);
  Serial.println();
  counter ++;
  delay(loop_delay);
}
