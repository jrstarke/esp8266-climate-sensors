// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
// - ESP8266 Influxdb: https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <InfluxDb.h>

const char* ssid     = "<ssid>";
const char* password = "<password>";
const char* hostname = "<sensor_name>";

#define INFLUXDB_HOST "example.com"
#define INFLUXDB_DB "climate"
#define INFLUXDB_USER "db_user"
#define INFLUXDB_PASSWORD "db_password"
#define INFLUXDB_PORT 8086

#define DHTPIN 5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

Influxdb influx(INFLUXDB_HOST);

void setupInfluxDB() {
  influx.setDbAuth(INFLUXDB_DB, INFLUXDB_USER, INFLUXDB_PASSWORD);
  influx.setVersion(1);
  influx.setPort(INFLUXDB_PORT);
}

void setupWifi() {  
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void transmitMeasurement(String metric, float measurement) {
  Serial.print("Transmitting ");
  Serial.print(metric);
  Serial.print(" with measurement ");
  Serial.print(measurement);
  Serial.println(" to Influx");
  
  InfluxData row(metric);
  row.addTag("host", hostname);
  row.addValue("value", measurement);
  if (influx.write(row)) {
    Serial.println("Transmittion Succeeded");
  } else {
    Serial.println("Transmittion Failed");
    Serial.println(influx.getLastErrorMessage());
  }
}

void sendSensorData() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h;
  do {
    h = dht.readHumidity();
  } while (isnan(h));
  transmitMeasurement("humidity", h);
  
  // Read temperature
  float t;
  do {
    t = dht.readTemperature();
  } while (isnan(t));
  transmitMeasurement("temperature", t);
  
  float heatIndex = dht.computeHeatIndex(t, h, false);
  transmitMeasurement("heat_index", heatIndex);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Heat index: "));
  Serial.print(heatIndex);
  Serial.println(F("°C "));
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  setupWifi();

  setupInfluxDB();

  sendSensorData();
  
  ESP.deepSleep(60e6);
}

void loop() {
  // The deepSleep will result in setup running again on wake
}
