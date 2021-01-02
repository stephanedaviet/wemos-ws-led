#include <Adafruit_NeoPixel.h>

#include <global.h>
#include <sha1.h>
#include <MD5.h>
#include <Base64.h>
#include <WebSocketClient.h>
#include <WebSocketServer.h>

#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>      // https://github.com/morrissinger/ESP8266-Websocket

#define PIXEL_COUNT       30
#define WIFI_SSID         "Daviet-Cherveux"
#define WIFI_PASSWORD     "la vie est belle"
#define WEBSOCKET_HOST    "INSERT YOUR HOSTNAME HERE"
#define WEBSOCKET_PORT    80
#define WEBSOCKET_PATH    "/ws"

struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB rainbow[] = {
  {127, 0, 128},
  {0, 0, 255},
  {0, 255, 0},
  {255, 255, 0},
  {155, 100, 0},
  {255, 0, 0}
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, D4, NEO_GRB + NEO_KHZ800);
WebSocketClient wsClient;
WiFiClient client;

RGB color;
RGB lastColor;
int scrolling;

bool connected;

void renderLED(int i) {
  byte r = computeValueAt(lastColor.r, color.r, i);
  byte g = computeValueAt(lastColor.g, color.g, i);
  byte b = computeValueAt(lastColor.b, color.b, i);
  strip.setPixelColor(i, r, g, b);
  strip.show();
}

void renderLEDColor(int i, RGB color) {
  strip.setPixelColor(i, color.r, color.g, color.b);
  strip.show();
}

void setup() {
  //Serial.begin(9600); // Used for debugging messages

  // Initialize the colors
  lastColor = {0, 0, 0};
  color = {0, 0, 0};
  scrolling = 0;
  strip.begin();
  strip.show();

  //setup_wifi();
  //setup_websocket();
  
//  for (int i = 0; i < 6; i++) {
//    for (int j = 0; j < 5; j++) {
//      renderLEDColor(i - 6 , {0, 0, 0});
//      renderLEDColor(i - j, rainbow[5 - j]);
//      delay(10);
//    }
//  }
    for (int i = 0; i < strip.numPixels(); i++) {
      renderLEDColor(i, {255, 255, 220});
    }
}

void loop() {
    for (int i = 0; i < strip.numPixels(); i++) {
      renderLEDColor(i, {255, 255, 220});
    }
/*    for (int i = 10; i < 150; i += 1) {
      for (int j = 0; j < strip.numPixels(); j++) {
        renderLEDColor(j, {i, i, i});
      }
      delay(3);
    }
    delay(50);
    for (int i = 150; i > 10; i -= 1) {
      for (int j = 0; j < strip.numPixels(); j++) {
        renderLEDColor(j, {i, i, i});
      }
      delay(3);
    }
    delay(50);*/
}
//  // If connection fails / breaks, the first LED blinks red
//  if (!client.connected() || !connected) {
//    renderLEDColor(0, {255, 0, 0});
//    delay(500);
//    renderLEDColor(0, {0, 0, 0});
//    delay(500);
//  } else if (scrolling > 0) { // If scrolling is greater than 0, render LEDs
//    renderLED(strip.numPixels() - scrolling);
//    delay(20);
//    scrolling--;
//  } else {
//    // When receiving data, process it and start scrolling animation
//    // Expected format is: RRRGGGBBB (as in 255000000, 035127078, ...)
//    // There is no error handling
//    String data;
//    wsClient.getData(data);
//    if (data.length() > 0) {
//      //Serial.print("Received: ");
//      //Serial.println(data);
//      int red   = data.substring(0, 3).toInt() / 255.0 * 100;
//      int green = data.substring(3, 6).toInt() / 255.0 * 100;
//      int blue  = data.substring(6, 9).toInt() / 255.0 * 100;
//      //Serial.print("Red: ");
//      //Serial.println(red);
//      //Serial.print("Green: ");
//      //Serial.println(green);
//      //Serial.print("Blue: ");
//      //Serial.println(blue);
//
//      // Update colors
//      lastColor = color;
//      color = {red, green, blue};
//
//      // Start scrolling animation
//      scrolling = strip.numPixels();
//    }
//  }
//}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi is connected on ");
  Serial.println(WIFI_SSID);
  Serial.print("=> IP address : ");
  Serial.println(WiFi.localIP());
}

void setup_websocket() {
  wsClient.host = WEBSOCKET_HOST;
  wsClient.path = WEBSOCKET_PATH;
  Serial.println("Initiating connection...");
  if(client.connect(WEBSOCKET_HOST, WEBSOCKET_PORT)) {
    Serial.println("Connected.");
    Serial.println("Initializing websocket...");
    if(wsClient.handshake(client)) {
      Serial.println("Handshake successful.");
      connected = true;
    } else {
      Serial.println("Handshake failed.");
      connected = false;
    }
  } else {
    Serial.println("Connection failed.");
    connected = false;
  }
}

double diffAbs(byte last, byte current) {
  return (double) (last > current ? last - current : current - last);
}

byte computeValueAt(byte last, byte current, int i) {
  double ratio = (double) i / (double) strip.numPixels();
  return (current + (last > current ? 1 : -1) * (byte) (diffAbs(last, current) * ratio));
}
