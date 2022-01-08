#include <OOCSI.h>

// Initial dust sensor
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
#define DustPIN 12

// Initial LDR sensor
#define LDRPIN 14



// FOR OOCSI CONNECTION: 
// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Ziggo6781918";
// Password of your Wifi network.
const char* password = "byXdmrataac8";

//const char* ssid = "Wi Believe I can Fi";
//const char* password = "Chouhuashuoqiantou!";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_OOCSI_Guang_SENDER3";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";

// OOCSI reference for the entire sketch
// To connect to an OOCSI network, that is, a running OOCSI server, you first need to create an OOCSI reference:
OOCSI oocsi = OOCSI();

void setup() {
  Serial.begin(115200);

  // dust sensor
  pinMode(DustPIN,INPUT);
  starttime = millis();
  
  // LDR sensor
  pinMode(LDRPIN,INPUT);


   
  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);
  oocsi.connect(OOCSIName, hostserver, ssid, password);
}

void loop() {
  // Wait a few seconds between measurements.
//  delay(2000);

  //dust sensor
  duration = pulseIn(DustPIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
    Serial.print("Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
  }


  // LDR sensor
  int ldrStatus = digitalRead(LDRPIN);
  Serial.println(ldrStatus);


  // make OOCSI message ready. Insert DF channel here
  oocsi.newMessage("esp-guangchannel3");

  // add data (primitive data types int, float, long, string)
  // the labels such as "count" or "timestamp" are completely free to choose

  // replace DEVICE with your reference ID from a device
  oocsi.addString("device_id", "DEVICE_DHT+");
  oocsi.addFloat("Concentration", concentration);
  oocsi.addFloat("LDRstatus", ldrStatus);

  // this command will send the message; don't forget to call this after creating a message
  oocsi.sendMessage();
  
  // prints out the raw message (how it is sent to the OOCSI server)
  // oocsi.printSendMessage();
  // needs to be checked in order for OOCSI to process incoming data.
  oocsi.check();
}
