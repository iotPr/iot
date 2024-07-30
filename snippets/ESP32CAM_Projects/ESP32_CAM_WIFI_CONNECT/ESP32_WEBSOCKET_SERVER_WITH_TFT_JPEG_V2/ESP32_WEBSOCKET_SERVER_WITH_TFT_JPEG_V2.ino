#include <SPIFFS.h>
#include <SPI.h>

#include <ArduinoWebsockets.h>
#include <WiFi.h>

#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>

const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";
const char* imageFileName = "/camimage.jpeg";

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
boolean SDInited = false;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS init failed!");
    while(1) yield();
  }else{
    SDInited = true;
  } 

  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_RED);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
  
  Serial.println();
  Serial.println("Setting AP...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP Address : ");
  Serial.println(IP);

  server.listen(8888);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(SDInited){
    if(server.poll()){
      client = server.accept();
    }

    if(client.available()){
      client.poll();

      WebsocketsMessage msg = client.readBlocking();
      boolean isSaved = fileWrite(imageFileName, msg);
      if(isSaved){
        
        uint32_t t = millis();

        // Get the width and height in pixels of the jpeg if you wish
        uint16_t w = 0, h = 0;
        TJpgDec.getFsJpgSize(&w, &h, imageFileName); // Note name preceded with "/"
        Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);
      
        // Draw the image, top left at 0,0
        TJpgDec.drawFsJpg(0, 0, imageFileName);
      
        // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms
        t = millis() - t;
        Serial.print(t); Serial.println(" ms");
      }
    }
  }
}

boolean fileWrite(String name, WebsocketsMessage content){
  File file = SPIFFS.open(name.c_str(), "w");
  if(!file){
    String errorMsg = "Can't open file";
    Serial.println(errorMsg);
    return false;
  }else{
    file.write((const uint8_t*) content.c_str(), content.length());
    file.close();
    return true;
  }
}
