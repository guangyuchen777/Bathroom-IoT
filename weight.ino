#include <OOCSI.h>

#include <HX711_ADC.h>
#include "soc/rtc.h"
#include <EEPROM.h>


#define DOUT 13
#define SCK 12

//HX711 constructor:
HX711_ADC LoadCell(DOUT, SCK);
int weight = 0;

const int calVal_eepromAdress = 0;
unsigned long t = 0;

// FOR OOCSI CONNECTION: 
// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Ziggo6781918";
// Password of your Wifi network.
const char* password = "byXdmrataac8";

//const char* ssid = "Wi Believe I can Fi";
//const char* password = "Chouhuashuoqiantou!";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_Guang_SENDER4";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";

// OOCSI reference for the entire sketch
// To connect to an OOCSI network, that is, a running OOCSI server, you first need to create an OOCSI reference:
OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
//  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }

  
   
  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);
  oocsi.connect(OOCSIName, hostserver, ssid, password);
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      weight = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(weight);
      delay(100);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

 

  // make OOCSI message ready. Insert DF channel here
  oocsi.newMessage("esp-guangchannel4");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose

  // replace DEVICE with your reference ID from a device
  oocsi.addString("device_id", "DEVICE_CUP");
  oocsi.addInt("cup_weight", weight);

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();
  
  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();
  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
}
