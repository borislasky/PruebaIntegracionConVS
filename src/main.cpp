#include <Arduino.h>
#include "WiFiSinc.h"

WiFiSinc miwifi;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("hola");
  miwifi.IpEstatica("192.168.43.111", "255.255.255.0", "192.168.43.1");
  miwifi.ConectaWiFi();
}

void loop() {
  if(miwifi.estado() != WL_CONNECTED)
    miwifi.ConectaWiFi();
  delay(1000);
}