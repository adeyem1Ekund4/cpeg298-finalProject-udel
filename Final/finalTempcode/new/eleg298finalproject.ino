/***************************************************************************
  ELEG298 Final Project - OOP Version!

  written by Adeyemi Ekundayo
  05/10/2023
  Refactored with OOP techniques
 ***************************************************************************/

#include "DHT20.h"
#include "Arduino.h"
#include <SoftwareSerial.h>

// ========== LED Controller Class ==========
class LEDController {
private:
  int blueLedPin;
  int redLedPin;
  
public:
  LEDController(int bluePin, int redPin) : blueLedPin(bluePin), redLedPin(redPin) {
    pinMode(blueLedPin, OUTPUT);
    pinMode(redLedPin, OUTPUT);
  }
  
  void turnOff() {
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, LOW);
  }
  
  void blinkBlue(int delayTime) {
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    delay(delayTime);
    turnOff();
    delay(delayTime);
  }
  
  void blinkRed(int delayTime) {
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    delay(delayTime);
    turnOff();
    delay(delayTime);
  }
};

// ========== Temperature Thresholds Class ==========
class TemperatureThresholds {
public:
  static const int NOVA = 90;
  static const int SUMMER = 80;
  static const int HEAT = 70;
  static const int BEACH = 60;
  static const int CHILL = 30;
  static const int COLDEST = 20;
  static const int FROZEN = 10;
};

// ========== WiFi/IoT Manager Class ==========
class IoTManager {
private:
  SoftwareSerial* espSerial;
  String ioUsername;
  String ioKey;
  String wifiSSID;
  String wifiPass;
  
public:
  IoTManager(int rxPin, int txPin, String username = "", String key = "", 
             String ssid = "", String pass = "") 
    : ioUsername(username), ioKey(key), wifiSSID(ssid), wifiPass(pass) {
    espSerial = new SoftwareSerial(rxPin, txPin);
    espSerial->begin(9600);
  }
  
  ~IoTManager() {
    delete espSerial;
  }
  
  bool initialize() {
    Serial.println("Setting up IoT connection...");
    
    String resp = sendCommand("get_macaddr", 2000, true);
    resp = sendCommand("wifi_ssid=" + wifiSSID, 2000, true);
    resp = sendCommand("wifi_pass=" + wifiPass, 2000, true);
    resp = sendCommand("io_user=" + ioUsername, 2000, true);
    resp = sendCommand("io_key=" + ioKey, 2000, true);
    resp = sendCommand("setup_io", 15000, true);
    
    if(resp.indexOf("connected") < 0) {
      Serial.println("\nAdafruit IO Connection Failed");
      return false;
    }
    
    resp = sendCommand("setup_feed=1,ThermoStatus", 2000, false);
    Serial.println("------ IoT Setup Complete ----------");
    return true;
  }
  
  void sendData(float value) {
    String command = "send_data=1," + String(value);
    sendCommand(command, 2000, false);
  }
  
private:
  String sendCommand(String command, const int timeout, boolean debug) {
    String response = "";
    espSerial->println(command);
    long int time = millis();
    
    while ((time + timeout) > millis()) {
      while (espSerial->available()) {
        char c = espSerial->read();
        response += c;
        Serial.print(c);
      }
    }
    
    if (debug) {
      Serial.println("Resp: " + response);
    }
    response.trim();
    return response;
  }
};

// ========== Sensor Manager Class ==========
class SensorManager {
private:
  DHT20* dht;
  uint8_t readCount;
  
public:
  SensorManager() : readCount(0) {
    dht = new DHT20();
    dht->begin();
  }
  
  ~SensorManager() {
    delete dht;
  }
  
  bool isReadyToRead() {
    return (millis() - dht->lastRead() >= 1000);
  }
  
  int readSensor() {
    uint32_t start = micros();
    int status = dht->read();
    uint32_t stop = micros();
    
    printHeader();
    printData(start, stop, status);
    
    return status;
  }
  
  float getHumidity() {
    return dht->getHumidity();
  }
  
  float getTemperature() {
    return dht->getTemperature();
  }
  
private:
  void printHeader() {
    if ((readCount % 10) == 0) {
      readCount = 0;
      Serial.println();
      Serial.println("Type\tHumidity (%)\tTemp (°C)\tTime (µs)\tStatus");
    }
    readCount++;
  }
  
  void printData(uint32_t start, uint32_t stop, int status) {
    Serial.print("DHT20 \t");
    Serial.print(dht->getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(dht->getTemperature(), 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    
    switch (status) {
      case DHT20_OK:
        Serial.print("OK");
        break;
      case DHT20_ERROR_CHECKSUM:
        Serial.print("Checksum error");
        break;
      case DHT20_ERROR_CONNECT:
        Serial.print("Connect error");
        break;
      case DHT20_MISSING_BYTES:
        Serial.print("Missing bytes");
        break;
      case DHT20_ERROR_BYTES_ALL_ZERO:
        Serial.print("All bytes read zero");
        break;
      case DHT20_ERROR_READ_TIMEOUT:
        Serial.print("Read time out");
        break;
      case DHT20_ERROR_LASTREAD:
        Serial.print("Error read too fast");
        break;
      default:
        Serial.print("Unknown error");
        break;
    }
    Serial.print("\n");
  }
};

// ========== Temperature Control System Class ==========
class TemperatureControlSystem {
private:
  LEDController* ledController;
  SensorManager* sensorManager;
  IoTManager* iotManager;
  
public:
  TemperatureControlSystem() {
    ledController = new LEDController(12, 13);  // Blue LED pin 12, Red LED pin 13
    sensorManager = new SensorManager();
    iotManager = new IoTManager(10, 11);  // RX pin 10, TX pin 11
  }
  
  ~TemperatureControlSystem() {
    delete ledController;
    delete sensorManager;
    delete iotManager;
  }
  
  bool initialize() {
    Serial.begin(9600);
    Serial.println(__FILE__);
    Serial.print("DHT20 LIBRARY VERSION: ");
    Serial.println(DHT20_LIB_VERSION);
    Serial.println();
    delay(1000);
    
    return iotManager->initialize();
  }
  
  void run() {
    if (!sensorManager->isReadyToRead()) {
      return;
    }
    
    int status = sensorManager->readSensor();
    if (status != DHT20_OK) {
      return;  // Skip if sensor reading failed
    }
    
    float humidity = sensorManager->getHumidity();
    iotManager->sendData(humidity);
    
    controlLEDs(humidity);
  }
  
private:
  void controlLEDs(float humidity) {
    if (humidity > TemperatureThresholds::NOVA) {
      ledController->blinkBlue(10);
    }
    else if (humidity > TemperatureThresholds::SUMMER) {
      ledController->blinkBlue(50);
    }
    else if (humidity > TemperatureThresholds::HEAT) {
      ledController->blinkBlue(100);
    }
    else if (humidity > TemperatureThresholds::BEACH) {
      ledController->blinkBlue(500);
    }
    else if (humidity > TemperatureThresholds::CHILL && humidity < TemperatureThresholds::BEACH) {
      ledController->turnOff();
      delay(500);
    }
    else if (humidity < TemperatureThresholds::CHILL) {
      ledController->blinkRed(500);
    }
    else if (humidity < TemperatureThresholds::COLDEST) {
      ledController->blinkRed(100);
    }
  }
};

// ========== Global Objects ==========
TemperatureControlSystem* tempSystem;

// ========== Arduino Setup and Loop ==========
void setup() {
  tempSystem = new TemperatureControlSystem();
  
  if (!tempSystem->initialize()) {
    Serial.println("System initialization failed!");
    while(1);  // Halt execution
  }
  
  Serial.println("System ready!");
}

void loop() {
  tempSystem->run();
}
