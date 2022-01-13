//void setup()
//{
//  pinMode(2, INPUT_PULLUP);
//  Serial.begin(115200);
//}
//
//void loop()
//{
//  Serial.println(digitalRead(2));
//  delay(100);
//}


#include <OOCSI.h>

#define PIN 33
#define LED 32

// FOR OOCSI CONNECTION: 
// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Ziggo6781918";
// Password of your Wifi network.
const char* password = "byXdmrataac8";

//const char* ssid = "Wi Believe I can Fi";
//const char* password = "Chouhuashuoqiantou!";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_Guang_SENDER";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";

// OOCSI reference for the entire sketch
// To connect to an OOCSI network, that is, a running OOCSI server, you first need to create an OOCSI reference:
OOCSI oocsi = OOCSI();

void setup() {
  pinMode(PIN, INPUT_PULLUP);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
   
  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);
  oocsi.connect(OOCSIName, hostserver, ssid, password);
}

void loop() {
  // Wait a few seconds between measurements.
//  delay(2000);
  int button_pin = digitalRead(PIN);
  Serial.println(button_pin);
  if (button_pin == 0){
    digitalWrite(LED, HIGH);
    delay(600);
    }
    else {
      digitalWrite(LED, LOW);
      }
  delay(100);

  // make OOCSI message ready. Insert DF channel here
  oocsi.newMessage("esp-guangchannel2");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose

  // replace DEVICE with your reference ID from a device
  oocsi.addString("device_id", "DEVICE_button+");
  oocsi.addInt("button_status", button_pin);

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();
  
  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();
  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
}
