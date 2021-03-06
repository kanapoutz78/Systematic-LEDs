#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <WebSocketsServer.h>
//#include <Hash.h>
#include <WiFiUdp.h>
#include <NeoPixelBus.h>

// Set to the number of LEDs in your LED strip
#define NUM_LEDS 226
// Maximum number of packets to hold in the buffer. Don't change this.
#define BUFFER_LEN 1500
// Toggles FPS output (1 = print FPS over serial, 0 = disable output)
#define PRINT_FPS 0

//NeoPixelBus settings
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266(set to 3 by default for DMA)

// Wifi and socket settings
const char* ssid     = "ASUS_VIVOBOOK";
const char* password = "T!PT)Psecret";
unsigned int localPort = 7778;
byte packetBuffer[BUFFER_LEN];
RgbColor ledDataBuffer[NUM_LEDS];

// LED strip
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> ledstrip(NUM_LEDS, PixelPin);

WiFiUDP port;

// Network information
// IP must match the IP in config.py
IPAddress ip(192, 168, 137, 200);
// Set gateway to your router's gateway
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  //WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("....START.....");
  // Connect to wifi and print the IP address over serial
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  port.begin(localPort);
  ledstrip.Begin();//Begin output
  ledstrip.Show();//Clear the strip for use
}

uint8_t N = 0;
#if PRINT_FPS
uint16_t fpsCounter = 0;
uint32_t secondTimer = 0;
#endif

void loop() {
  // Read data over socket
  int packetSize = port.parsePacket();
  // If packets have been received, interpret the command
  if (packetSize) {
    int len = port.read(packetBuffer, BUFFER_LEN);
    Serial.print("/");
    len /= 3;
    for (int i = 0; i < len; i += 1) {
      packetBuffer[len] = 0;
      //N = packetBuffer[i];
      RgbColor pixel((uint8_t)packetBuffer[3 * i], (uint8_t)packetBuffer[3 * i + 1], (uint8_t)packetBuffer[3 * i + 2]);
      ledstrip.SetPixelColor(i, pixel);
    }
    ledstrip.Show();
    Serial.print("/");
  }
}
