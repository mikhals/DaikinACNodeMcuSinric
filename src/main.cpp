#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include <Sinric.h>

#include <SinricSwitch.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRDaikinESP ac(kIrLed);  // Set the GPIO to be used to sending the message


// To Do: Put in your credentials

const char* ssid = "####";
const char* pass = "####";

const char* api_key = "#####";

const char* switchId = "####";

// callbacks for devices...

void switchOff(){
  ac.off();
  Serial.println("AC switched off");
}


void switchOn(){
  ac.on();
  ac.setFan(kDaikinFanMed);
  ac.setMode(kDaikinAuto);
  ac.setTemp(25);
  ac.setSwingVertical(false);
  ac.setSwingHorizontal(false);
  Serial.println("AC switched on");
}



void onPowerState(const String& deviceId, bool state) {
  if(deviceId == switchId){
    if(state == true){
      switchOn();
    }else if(state == false){
      switchOff();
    }
    #if SEND_DAIKIN
      ac.send();
    #endif  // SEND_DAIKIN
  }
  Serial.printf("onPowerState(%s,%s) ", deviceId.c_str(), state?"true":"false");
  if (deviceId == switchId) {
    Serial.println("(Switch)");
  }
}
// WiFi setup
void setupWifi() {
  Serial.begin(115200);
  ac.begin();
  delay(1000);
  Serial.printf("\r\nConnecting WiFi (%s)", ssid);

  WiFi.persistent(false);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("connected!\r\n");

  IPAddress localIP = WiFi.localIP();
  Serial.printf("IP address: %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

// Sinric setup
void setupSinric() {
  Serial.print("Connecting Sinric");
  Sinric.begin(api_key);
  while (!Sinric.isConnected()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("connected!");

  // add new Switch
  SinricSwitch& mySwitch = Sinric.add<SinricSwitch>(switchId);
  // set the callback
  mySwitch.onPowerState(onPowerState);
  //mySwitch.onPowerState([](const char* deviceId, bool state) { Serial.printf("lambda power state %s,%s\r\n", deviceId, state?"true":"false"); } );
}


void setup() {
  delay(1000);
  Serial.begin(115200); Serial.println();
  setupWifi();
  setupSinric();
}

void loop() {
  ArduinoOTA.handle();
  Sinric.handle();
}
