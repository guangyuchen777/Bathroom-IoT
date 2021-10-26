#include <OOCSI.h>
#include "DHT.h"

// Example testing sketch for various DHT humidity/temperature sensors written by ladyada, modified by Eva van der Born
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - OOCSI for ESP Library: https://github.com/iddi/oocsi-esp

// Digital pin connected to the DHT sensor
#define DHTPIN 14
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
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

//// Initial dust sensor
//unsigned long duration;
//unsigned long starttime;
//unsigned long sampletime_ms = 2000; 
//unsigned long lowpulseoccupancy = 0;
//float ratio = 0;
//float concentration = 0;
//#define DustPIN 2

// Initial PIR sensor
int state = LOW;             // by default, no motion detected
int val = 0;                 // variable to store the sensor status (value)
#define PIRPIN 26                 // PIR Out pin 


// FOR OOCSI CONNECTION: 
// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Ziggo6781918";
// Password of your Wifi network.
const char* password = "byXdmrataac8";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_Guang_SENDER";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";

// OOCSI reference for the entire sketch
// To connect to an OOCSI network, that is, a running OOCSI server, you first need to create an OOCSI reference:
OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();

//  // dust sensor
//  pinMode(DustPIN,INPUT);
//  starttime = millis();

  // PIR sensor
  pinMode(PIRPIN, INPUT);
   
  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);
  oocsi.connect(OOCSIName, hostserver, ssid, password);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float heat_index_fahrenheit = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float heat_index_celsius = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(heat_index_celsius);
  Serial.print(F("°C "));
  Serial.print(heat_index_fahrenheit);
  Serial.println(F("°F"));

//  //dust sensor
//  duration = pulseIn(DustPIN, LOW);
//  lowpulseoccupancy = lowpulseoccupancy+duration;
//  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
//  {
//    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
//    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
//    Serial.print("Concentration = ");
//    Serial.print(concentration);
//    Serial.println(" pcs/0.01cf");
//    Serial.println("\n");
//    lowpulseoccupancy = 0;
//    starttime = millis();
//  }


  // PIR sensor
  val = digitalRead(PIRPIN);   // read sensor value
  Serial.println(val);
//  if (val == HIGH) {           // check if the sensor is HIGH
////    delay(100);                // delay 100 milliseconds 
//    Serial.println(val);
//    if (state == LOW) {
//      Serial.println("Motion detected!"); 
//      state = HIGH;       // update variable state to HIGH
//    }
//  } 
//  else {
////      delay(200);             // delay 200 milliseconds 
//      Serial.println(val);
//      if (state == HIGH){
//        Serial.println("Motion stopped!");
//        state = LOW;       // update variable state to LOW
//    }
//  }


  // make OOCSI message ready. Insert DF channel here
  oocsi.newMessage("esp-guangchannel");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose

  // replace DEVICE with your reference ID from a device
  oocsi.addString("device_id", "DEVICE_DHT+");
  
  oocsi.addFloat("Humidity", h);
  oocsi.addFloat("Temperature", t);
  oocsi.addFloat("HIC", heat_index_celsius);
//  oocsi.addLong("time", (long) millis());
// oocsi.addFloat("Concentration", concentration);

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();
  
  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();
  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
}
