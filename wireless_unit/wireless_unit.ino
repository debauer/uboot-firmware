/**
 * @example TCPClientSingleUNO.ino
 * @brief The TCPClientSingleUNO demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.03
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ESP8266.h"
#include <SoftwareSerial.h>

#define FABLAB

#ifdef FABLAB
  #define SSID        "Fablab Karlsruhe"
  #define PASSWORD    "foobar42" 
  #define HOST_NAME   "192.168.1.6"    
  #define HOST_PORT   (5002)
#endif

#ifdef SCHLAGER
  // @schlager
  #define SSID        "FRITZ!Box Fon WLAN 7390"
  #define PASSWORD    "9089879043196882" 
  #define HOST_NAME   "192.168.3.184"    
  #define HOST_PORT   (5002)
#endif

#ifdef RALLYE
  // @schlager
  #define SSID        "AOR 2015"
  #define PASSWORD    "AOR 2015" 
  #define HOST_NAME   "192.168.4.6"    
  #define HOST_PORT   (5002)
#endif

//SoftwareSerial mySerial(4, 5); /* RX:D3, TX:D2 */
ESP8266 wifi(Serial1,115200);

void setup(void)
{
    Serial.begin(115200);
    Serial.print("setup begin\r\n");
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}
 
void loop(void)
{
    uint8_t buffer[128] = {0};
    uint32_t len = 0;
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }

        char *hello = "GET /api/v1.0/ping HTTP/1.1\r\n\r\n";
        Serial.print("Sende:"); 
        Serial.println(hello);
        if(wifi.send((const uint8_t*)hello, strlen(hello))){
              len = 0;
              Serial.println("Senden OK"); 
              len = wifi.recv(buffer, sizeof(buffer),5000);
              Serial.print("Status Bytes: ");
              Serial.println(len);
              if (len > 0) {
                   Serial.print("empfangener String: ");
                   for(uint32_t i = 0; i < len; i++) {
                       Serial.print((char)buffer[i]);
                   }
                    len = wifi.recv(buffer, sizeof(buffer),5000);
                    Serial.print("Payload Bytes: ");
                    Serial.println(len);
                    if (len > 0) {
                         Serial.print("empfangener String: ");
                         for(uint32_t i = 0; i < len; i++) {
                             Serial.print((char)buffer[i]);
                         }
                    }
              }
        }
    
      if (wifi.releaseTCP()) {
          Serial.print("release tcp ok\r\n");
      } else {
          Serial.print("release tcp err\r\n");
      }
    delay(500);
}
     

