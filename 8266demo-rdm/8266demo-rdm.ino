/***************************************************************************
  This is an example program for the sending a counter to Adafruit IO using
  an ESP8266 WiFi module.  You will need to correct the WiFi SSID and password
  and add your Adafruit IO username and Key.

  written by Theo Fleck and Rick Martin
  03/25/2020
 ***************************************************************************/
#include "Arduino.h"
#include <SoftwareSerial.h>		          //Allows us to use two GPIO pins for a second UART
SoftwareSerial espSerial(10,11);	      //Create software UART to talk to the ESP8266
String  IO_USERNAME = "";
String IO_KEY     =  "";
String WIFI_SSID = ""; 	    //Only need to change if using other network, eduroam won't work with ESP8266
String WIFI_PASS = ""; 		            //Blank for open network
float num = 1.0; 			                  //Counts up to show upload working
int pinAdc = A0;

void setup() {
	Serial.begin(9600);		// set up serial monitor with 9600 baud rate
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
	resp = espData("setup_feed=1,Week 4",2000,false);	//start the data feed
	Serial.println("------ Setup Complete ----------");
}

void loop() {

	// free version of Adafruit IO only allows 30 uploads/minute, it discards everything else
			// Wait 5 seconds between uploads
	Serial.print("Num is: ");
	Serial.println(num);
	String resp = espData("send_data=1,"+String(num),2000,false); //send feed to cloud

  long sum = 0;
    for(int i=0; i<32; i++)
    {
        num = analogRead(pinAdc);
    }
 
    //num >>= 5;
 
    Serial.println(num);
    delay(10);

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
