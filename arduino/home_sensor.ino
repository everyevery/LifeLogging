#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"

const byte DATA_TYPE_TEMPERATURE = 0x01;
const byte DATA_TYPE_HUMIDITY = 0x02;

const int MAX_PACKET_SIZE = 64;

const int SENSING_INTERVAL = 15;
const int SENSING_EACH_COUNT = 3;
const int SENSING_EACH_INTERVAL = 3;

/* Home Server */
const IPAddress HOME_SERVER_IP(192,168,0,10);
const int HOME_SERVER_PORT = 8090;
WiFiUDP udp;

/* WIFI */
char ssid[] = "lee24g";
char pass[] = "pi31415926535";
int status = WL_IDLE_STATUS;
byte mac[6] = {0, };
  
/* STATUS LED */
int led = 13;

/* DHT22 */
#define DHTPIN  2
#define DHTTYPE  DHT22


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // initialize serial and wait for the port to open:
  Serial.begin(9600);
  while(!Serial) ;

  // STATUS LED SETTING
  pinMode(led, OUTPUT);
  
  // WiFi
  Serial.println("Initializing Wifi...");
  getMacAddress();

  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  
  // DHT
  dht.begin();
}

void loop() {
  float temperature = 0.0;
  float humidity = 0.0;
  float temporary;
  int count_temperature = 0;
  int count_humidity = 0;
  unsigned long start_time;
  unsigned long end_time;
  
  start_time = millis();
  digitalWrite(led, HIGH);
  for (int idx=0; idx < SENSING_EACH_COUNT; idx++) {
    temporary = dht.readTemperature(false);
    if ( !isnan(temporary) ) {
      temperature += temporary;
      count_temperature++;
    }
    temporary = dht.readHumidity();
    if ( !isnan(temporary) ) {
      humidity += temporary;
      count_humidity++;
    }
    delay(SENSING_EACH_INTERVAL*1000);
  }
  temperature = average(temperature, count_temperature, 1000);
  humidity = average(humidity, count_humidity, 1000);
  printData(temperature, humidity);
  
  if ( 1 == connectWifi() ) {
    if ( 1 == sendDataToServer(start_time, temperature, humidity) ) {
      Serial.println("Success to send data");
    }
    else {
      Serial.println("Fail to send data");
    }
    disconnectWifi(); 
  }
  else {
    Serial.println("Fail to connect Wifi");
  }
  
  digitalWrite(led, LOW);
  end_time = millis();

  delay(calculateDelay(SENSING_INTERVAL, start_time, end_time));
}

int sendDataToServer(unsigned long start_time, float temperature, float humidity) {
  int result = 0;
  int sended = 0;
  
  udp.beginPacket(HOME_SERVER_IP, HOME_SERVER_PORT);
  sended += udp.write(mac, sizeof(mac));
  sended += udp.write((byte*)&start_time, sizeof(unsigned long));
  sended += udp.write((byte*)DATA_TYPE_TEMPERATURE, sizeof(byte));
  sended += udp.write((byte*)&temperature, sizeof(float));
  sended += udp.write((byte*)DATA_TYPE_HUMIDITY, sizeof(byte));
  sended += udp.write((byte*)&humidity, sizeof(float));
  for(int idx=sended; idx<MAX_PACKET_SIZE; idx++) {
    udp.write(byte(0));  //TODO: byte를 보낼 때는 보낸 값이 리턴된나?
  }
  result = udp.endPacket();
  udp.stop();
  return result;
}

int connectWifi() {
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("##### Couldn't get a wifi connection");
    return 0;
  }
  else {
    Serial.println("##### Connected to network");
    return 1;
  }
}

int disconnectWifi() {
 WiFi.disconnect(); 
}

int calculateDelay(int delay_time, unsigned long start_time, unsigned long end_time) {
  if ( start_time <= end_time ) {
    return (delay_time * 1000) - (end_time - start_time);
  }
  else {
    return (delay_time * 1000) - (start_time + (4294967295 - end_time));
  }
}

float average(float sum, int count, float error) {
  if ( 0 == count ) {
    return error;
  }
  else {
    return sum / count;
  }  
}

void printData(float temperature, float humidity) {
  Serial.print("Humidity: "); 
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temperature);
  Serial.println(" *C");
}

void getMacAddress() {
  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    Serial.println(WiFi.encryptionType(thisNet));
  }
}
