# Internet of Things Device for Humidity Sensor: Iot THS
The primary goal of the project is to read temperature and humidity data from the DHT20 sensor, process this data, and then send it to a server or cloud service (implied by the Adafruit IO setup) via the ESP8266.
# Schematic
![Screenshot 2024-04-06 230513](https://github.com/adeyem1Ekund4/cpeg298-finalProject-udel/assets/166171920/9c108faf-6b6c-4b0c-bf3a-904a47088827)

# THS PCB View
### Front
![ELEG298_S23_Demo_shield_front](https://github.com/adeyem1Ekund4/cpeg298-finalProject-udel/assets/166171920/6d1b5edc-51f2-4fbc-bab8-1108e5776976)
### Back
![Screenshot 2024-04-06 225916](https://github.com/adeyem1Ekund4/cpeg298-finalProject-udel/assets/166171920/7afed081-bce0-42c4-8a09-7d4b1503581c)

# Arduino Code
The code sets up various parameters such as WiFi credentials, pins for LEDs, temperature thresholds, and initializes the DHT20 sensor.

In the setup() function:

The DHT20 sensor is initialized.
Pins 12 and 13 are set as output pins for LEDs.
Serial communication is started at a baud rate of 9600.
The ESP8266 module is initialized using software serial communication.
Various commands are sent to the ESP8266 module to set up WiFi connection, Adafruit IO credentials, and start the IoT connection.
If the connection to Adafruit IO fails, the program enters an infinite loop.
In the loop() function:

The code reads data from the DHT20 sensor every second.
The temperature and humidity values are obtained from the sensor.
The temperature value is sent to the ESP8266 module for further processing.
Based on the humidity level, different LED patterns are displayed.
The sensor data along with the status of the sensor reading is printed to the serial monitor.
The espData() function is used to send commands to the ESP8266 module and receive responses. It waits for a response within a specified timeout period and returns the response as a string.

# Adafruit Hub


# Future Changes
For the program, to improve readability and maintainability, break down the code into multiple functions, such as moving LED control logic into a separate function. Avoid using delays in the loop, as they can block other operations and affect system responsiveness. Improve error handling, especially for Wi-Fi and Adafruit IO connections, and provide alternative actions or retries in case of failure. Avoid hardcoding sensitive information and consider external methods for secure input. Restructure the espData function to be more efficient and reliable, and implement a feedback mechanism from the server/cloud to the Arduino for more interactive control. For battery-operated projects, use power-saving techniques to extend battery life. Add checks to ensure data read from the DHT20 sensor is within reasonable bounds before sending it to avoid transmitting erroneous data.
