/* WeMos DHT22 ThingSpeak Data Logger

   Connect to WiFi and send ThingSpeak Channel temperature and humidity reading

   Based on Adafruit ESP8266 Temperature / Humidity Webserver
   https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver

   Based on
   https://fablab.ruc.dk/wemos-intro/

   Depends on Adafruit DHT Arduino library
   https://github.com/adafruit/DHT-sensor-library
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTTYPE DHT22   // DHT Shield uses DHT 22
#define DHTPIN D4       // DHT Shield uses pin D4

int LED = D7;
int switchPin = D5;

// Existing WiFi network
const char* ssid     = "HMVVS";
const char* password = "4532550922";
//const char* ssid     = "PrettyFlyForAWifi";
//const char* password = "00000000";


// Thingspeak Channel
// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = "6UNJTJLEVG04IBQC";
const char* server = "api.thingspeak.com";


// Establish WiFi
WiFiClient client;

// Initialize DHT sensor
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// DHT variables
float humidity, temperature;                 // Raw float values from the sensor
char str_humidity[10], str_temperature[10];  // Rounded sensor values and as strings


// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;            // When the sensor was last read
const long interval = 20000;                  // Wait this long until reading and tranmitting again

void send_tspeak() {
  // Create the data string for Thingspeak
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(str_humidity);
    postStr += "&field2=";
    postStr += String(str_temperature);
    postStr += "\r\n\r\n";

    // send the data to Thingspeak
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(str_temperature);
    Serial.print(" degrees Celsius Humidity: ");
    Serial.print(str_humidity);
    Serial.println(".");
    Serial.println("Sending data to Thingspeak");
    delay(20000);
  }
}

void read_sensor() {
  
    // Reading temperature and humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();        // Read humidity as a percent
    temperature = dht.readTemperature();  // Read temperature as Celsius

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Convert the floats to strings and round to 2 decimal places
    dtostrf(humidity, 1, 2, str_humidity);
    dtostrf(temperature, 1, 2, str_temperature);

    Serial.print("Humidity: ");
    Serial.print(str_humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(str_temperature);
    Serial.println(" °C");
    delay(2000);

}

void setup(void)
{

  // initialize LED as output
  pinMode(LED, OUTPUT);
  analogWrite(LED, 10);

  pinMode(switchPin, INPUT);

  // Open the Arduino IDE Serial Monitor to see what the code is doing
  Serial.begin(9600);
  dht.begin();

  Serial.println("WeMos DHT Server");
  Serial.println("");

  // Connect to your WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  // Wait for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // Initial read
  read_sensor();

  // Handle http requests

}

void loop(void)
{

  // turn on LED with voltage HIGH
  // digitalWrite(LED, HIGH);

  // Flicker LED for every tranmsission
  if (digitalRead(switchPin)) {
    // Flicker LED for every tranmsission
    analogWrite(LED, 255);
    delay(100);
    analogWrite(LED, 150);
    delay(100);
    analogWrite(LED, 50);
    delay(100);
    analogWrite(LED, 10);
    delay(100);
  } else {
    digitalWrite(LED, 0);
  }


  read_sensor();

  send_tspeak();
}
