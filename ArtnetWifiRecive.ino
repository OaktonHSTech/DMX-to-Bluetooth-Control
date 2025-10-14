#include "arduino_secrets.h"
/*
Example, transmit all received ArtNet messages (Op-Code DMX) out of the
serial port in plain text.
Function pointer with the C++11 function-object "std::function".

Stephan Ruloff 2019
https://github.com/rstephan/ArtnetWifi
*/

#include <ArtnetWifi.h>
#include <Arduino.h>

//Wifi settings
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
// Set your Static IP address
IPAddress local_IP(10, 101, 100, 71);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 5);

IPAddress subnet(255, 255, 0, 0);
WiFiUDP UdpSend;
ArtnetWifi artnet;

void onDmxFrame(
  uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data
) {
  int address = 193
  //intensity
  int intensity = data[address - 1];
  Serial.println("");
  Serial.print("intensity: ");
  Serial.println(intensity);
  //red
  int red = data[address];
  int green = data[address + 1];
  int blue = data[address + 2];
  Serial.println("");
  Serial.print("color: ");
  Serial.println(red, green, blue);
  //effect
  int effect = data[address + 3];
  Serial.println("");
  Serial.print("effect: ");
  Serial.println(effect);
}

// connect to wifi – returns true if successful or false if not
bool ConnectWifi(void)
{
  bool state = true;
  int i = 0;
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void setup()
{
  // set-up serial for debug output
  Serial.begin(115200);
  ConnectWifi();

  // this will be called for each packet received
  artnet.setArtDmxFunc([](DMX_FUNC_PARAM){
    bool tail = false;

    Serial.print("DMX: Univ: ");
    Serial.print(universe, DEC);
    Serial.print(", Seq: ");
    Serial.print(sequence, DEC);
    Serial.print(", Data (");
    Serial.print(length, DEC);
    Serial.print("): ");

    if (length > 16) {
      length = 16;
      tail = true;
    }
    // send out the buffer
    for (uint16_t i = 0; i < length; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    if (tail) {
      Serial.print("...");
    }
    Serial.println();
  });
  artnet.begin();
artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}
