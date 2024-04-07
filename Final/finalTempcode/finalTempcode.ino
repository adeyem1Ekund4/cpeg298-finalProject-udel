/***************************************************************************
  ELEG298 Final Project!

  written by Adeyemi Ekundayo
  05/10/2023
 ***************************************************************************/

#include "DHT20.h"
#include "Arduino.h"
#include <SoftwareSerial.h>		          //Allows us to use two GPIO pins for a second UART
SoftwareSerial espSerial(10,11);	      //Create software UART to talk to the ESP8266
String  IO_USERNAME = "";
String IO_KEY     =  "";
String WIFI_SSID = ""; 	    //Only need to change if using other network, eduroam won't work with ESP8266
String WIFI_PASS = ""; 		            //Blank for open network
DHT20 DHT;
uint8_t count = 0;

int Temperature;
int ledB = 12;
int ledR = 13;

int nova = 90;
int summer = 80;
int heat = 70;
int beach = 60;


int frozon = 10;
int coldest = 20;
int chill = 30;


void setup()
{
  DHT.begin();    //  ESP32 default pins 21 22
  pinMode(12,OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.print("DHT20 LIBRARY VERSION: ");
  Serial.println(DHT20_LIB_VERSION);
  Serial.println();
  delay(1000);
	espSerial.begin(9600);		// set up software UART to ESP8266 @ 9600 baud rate
	Serial.println("setting up");
	String resp = espData("get_macaddr",2000,true);	//get MAC address of 8266
	resp = espData("wifi_ssid="+WIFI_SSID,2000,true);	//send Wi-Fi SSID to connect to
	resp = espData("wifi_pass="+WIFI_PASS,2000,true);	//send password for Wi-Fi network
	resp = espData("io_user="+IO_USERNAME,2000,true);	//send Adafruit IO info
	resp = espData("io_key="+IO_KEY,2000,true);
	resp = espData("setup_io",15000,true);			//setup the IoT connection
	if(resp.indexOf("connected") < 0) {
		Serial.println("\nAdafruit IO Connection Failed");
		while(1);
	}
	resp = espData("setup_feed=1,ThermoStatus",2000,false);	//start the data feed
	Serial.println("------ Setup Complete ----------");

}

void loop()
{
  if (millis() - DHT.lastRead() >= 1000)
  {
    // READ DATA
    uint32_t start = micros();
    int status = DHT.read();
    uint32_t stop = micros();
    

    if ((count % 10) == 0)
    {
      count = 0;
      Serial.println();
      Serial.println("Type\tTemperature (°F)\tHumid (%)\tTime (µs)\tStatus");
    }
    count++;
    
    Temperature=DHT.getHumidity();
    String resp = espData("send_data=1,"+String(Temperature),2000,false);

    if((DHT.getHumidity()) > nova){
      digitalWrite(12, HIGH);
      digitalWrite(13, LOW);
      delay(10);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(10);
    }
    else if((DHT.getHumidity()) > summer){
      digitalWrite(12, HIGH);
      digitalWrite(13, LOW);
      delay(50);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(50);
    }
    else if((DHT.getHumidity()) > heat){
      digitalWrite(12, HIGH);
      digitalWrite(13, LOW);
      delay(100);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(100);
    }
    else if((DHT.getHumidity()) > beach){
      digitalWrite(12, HIGH);
      digitalWrite(13, LOW);
      delay(500);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(500);
    }
    else if(((DHT.getHumidity()) > chill) && ((DHT.getHumidity()) < beach)){
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(500);
    }
    else if((DHT.getHumidity()) < chill){
      digitalWrite(12, LOW);
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(500);
    }
    else if((DHT.getHumidity()) < coldest){
      digitalWrite(12, LOW);
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      delay(100);
    }
    

    Serial.print("DHT20 \t");
    // DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT.getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    switch (status)
    {
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
}

 String espData(String command, const int timeout, boolean debug) {
	String response = "";
	espSerial.println(command);	//send data to ESP8266 using serial UART
	long int time = millis();
	while ( (time + timeout) > millis()) {	//wait the timeout period sent with the command
		while (espSerial.available()) {	//look for response from ESP8266
			char c = espSerial.read();
			response += c;
			Serial.print(c);	//print response on serial monitor
		}
	}
	if (debug) {
		Serial.println("Resp: "+response);
	}
	response.trim();
	return response;
}

  

