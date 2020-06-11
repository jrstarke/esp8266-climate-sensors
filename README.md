# esp8266-climate-sensors

This is a small proof of concept for creating small ESP8266 based climate sensors.

My hardware consists of an [Adafruit Feather HUZZAH ESP8266](https://www.adafruit.com/product/2821) microcontroller and
a [DHT22](https://www.adafruit.com/product/385) temperature and humidity sensor.

The [dht22-influx-sketch](dht22-influx-sketch/dht22-influx-sketch.ino) Arduino Sketch sets up to connect to WIFI,
gather a temperature and humidity reading, then transmits the data to Influx DB, before going into a deep sleep for 60
seconds. This does mean that to use this, you will need to jumper the WAKE pin (Pin 16 on the Feather HUZZAH), to the
RST pin.


## Getting Started

### Hardware

#### Deep Sleep

For my purposes, I wanted to have my sensor running in low power, with the long term intent of running this off an 
18650 battery. My understanding is that in low power mode, this would be able to last for months, or up to a year on a
single charge. To accomplish this, when the clock wakes from deep sleep, it needs to reset the rest of the device so it
runs. To do this, I have a jumper between pin 16 (which triggers on WAKE) and the RST pin.

#### DHT22

The DHT22 is relatively simple. To my surprise, though it has 4 pins, only the pins 1, 2 and 4 are actually used.
Pin 1 should be connected to the 3V Pin. Pin 4 should be connected to GND. Pin 2 is the Data pin, but it requires a pull
up. With the DHT22, you'll receive a 4.7K - 10K resistor. One end of the resistor connects to 3V, the other to the Data
pin. Now you connect the data pin as well to your input pin. The resistor in this case gives an defined default input
state. In the sketch file, I used pin 5, but you could use 3, 4, 5, 12, 13 or 14.

### Software

#### Docker

The `docker-compose.yml` is set up with a container for
[InfluxDB](https://www.influxdata.com/products/influxdb-overview/) and [Grafana](https://grafana.com/).

InfluxDB and Grafana are configured to share the following values


| Env Var | Default | Description |
|---------|---------|-------------|
| DB_NAME | climate | The name of the InfluxDB database to create. (This will be written to by the sensors, and read by grafana) |
| DB_USER | db_user | The name of the database user to create. Will be granted read and write access. (The sensors and grafana will share this user) |
| DB_PASSWORD | db_password | The password for the database user |


#### Arduino Sketch

For the Arduino Sketch, you'll want to install a few libraries

- [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)
- [Adafruit Unified Sensor Lib](https://github.com/adafruit/Adafruit_Sensor)
- [ESP8266 Influxdb](https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino)

Next there are a few values you'll need to update, based on your own configuration

| Value | Description |
|-------|-------------|
| ssid | The SSID for your wireless network |
| password | The password for your wireless network |
| hostname | The name you want to give to your sensor (this value will be used to tag incoming data) |
| INFLUXDB_HOST | The Host (domain name or ip address) that will serve InfluxDB |
| INFLUXDB_DB | The name of the database to send the data to | 
| INFLUXDB_USER | The influxdb user to use for writing data to the database |
| INFLUXDB_PASSWORD | The influxdb user's password for writing to the database |
| INFLUXDB_PORT | the influxdb port to connect to the database |
| DHTPIN | The Digital Pin that you've connected the DHTs Data pin to |
| DHTTYPE | The DHT Type corresponding to your DHT model. Ensure that only your model is uncommented |

**Note**

When uploading to the ESP8266, you'll want to ensure that the jumper between the RST pin and Pin 16 is disconnected. I
found from personal experience that with this connected, my laptop was unable to connect to the ESP8266 for upload.

