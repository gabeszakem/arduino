/*
    This is an extension for Karadio https://github.com/karawin/Ka-Radio
   You can control karadio wit an esp-01 or esp8266.
   My program basic is "TelnetServer.ino for ESP8266 NodeMCU" by Rudolf Reuter
   and  IRremoteESP8266: IRrecvDemo.
   You need install library: //https://github.com/markszabo/IRremoteESP8266
   This is the first tme version(Prototype) it need to develop and test
 * *********************************************************
   ESP-01         ESP8266(Karadio)
   TX             RX
   RX             TX
   GND            GND
   VCC            3.3V
   CH_PD          3.3V
 * *********************************************************
   IR Receiver VS1838b        ESP-01    ESP8266(Karadio)
   (From left to right)
   PIN1                       GPIO2
   PIN2                                 GND
   PIN3                                 3.3V
 * *********************************************************

   You need configure your ssid and password
   You configure your ir code

   Gábriel Kovács   gabeszakem@gmail.com
   /


  // File: TelnetServer.ino for ESP8266 NodeMCU
  // 2015-12-07 Rudolf Reuter www.rudiswiki.de/wiki9 (search for "wifi")
  // 2015-12-17 RR, structure copied from example WiFiTelnetToSerial.ino
  //
  // Developed for debug purpose, use like the Arduino Serial Monitor.
  // Needs Arduino 1.6.5/6 to compile.
  //
  /*
     This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
   IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   An IR detector/demodulator must be connected to the input RECV_PIN.
   Version 0.1 Sept, 2015
   Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
*/


/* IR Receiver VS1838b   out/gnd/V
    I use universal remote control with amplifier setup
*/



#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "settings.h"

// declare telnet server (do NOT put in setup())
WiFiServer TelnetServer(23);
WiFiClient TelnetClient;
/* Setup for the Connector/Arduino */
//WiFiClient client;            // Use this for WiFi instead of EthernetClient
int RECV_PIN = 2; //an IR detector/demodulatord is connected to GPIO pin 2


IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long startTime = millis();
unsigned long uartTime = startTime;
unsigned long buttonTime = startTime;

char irStr[4];


void setup() {
  delay(1000);
  Serial.begin(115200);
  connectWifi();
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
  irrecv.enableIRIn(); // Start the receiver


} // setup()

void loop() {
  if (millis() - startTime > 1000) { // run every 2000 ms
    startTime = millis();
    // Test
    if (TelnetServer.hasClient()) {
      if (debug) Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");
      if (!TelnetClient || !TelnetClient.connected()) {
        if (TelnetClient) {
          TelnetClient.stop();
          if (debug)     Serial.println("TelnetClient Stop");
        }
        TelnetClient = TelnetServer.available();
        telnetClientWelcome();
      } else {
        TelnetServer.available().stop();
        if (debug)      Serial.println("TelnetServer Stop");
      }
    }
    if (TelnetClient && TelnetClient.connected() && TelnetClient.available()) {
      while (TelnetClient.available()) { // get data from Client
        String msg = TelnetClient.readStringUntil('\r');
        Serial.print(msg);
        if (msg != lastMSG) {
          lastMSG = msg;
        }
      }
    }
    if (Serial.available()) {
      String msg = Serial.readString();
      if (TelnetClient.connected()) {  // send data to Client
        TelnetClient.print(msg);
      }
      if (msg != lastMSG) {
        lastMSG = msg;
      }
    }
    if (millis() - uartTime > 100) { // run every 2000 ms
      uartTime = millis();
      if (irrecv.decode(&results)) {
        /*Uncomment for test your ir code*/
        //Serial.println(results.value);
        switch (results.value)
        {
          case 88370221:  /*(" LEFT");*/  irStr[0]; Serial.print("cli.next\r");/* sql("INSERT INTO karadio.message (message,sender) VALUES (  cli.next, IR)"); */break;
          case 88386797: /*(" DOWN");*/  irStr[0]; Serial.print("cli.vol-\r"); /*sql("INSERT INTO karadio.message (message,sender) VALUES (  cli.vol-, IR)");*/  break;
          case 88353901: /*(" UP");*/  irStr[0]; Serial.print("cli.vol+\r"); /*sql("INSERT INTO karadio.message (message,sender) VALUES (  cli.vol+, IR)");*/  break; // volume +
          case 88403117:  /*(" RIGHT");*/  irStr[0]; Serial.print("cli.prev\r");/* sql("INSERT INTO karadio.message (message,sender) VALUES (  cli.prev, IR)"); */break;
          case 85994505:  /*(" LEFT");*/  irStr[0]; Serial.print("cli.next\r");/* sql("INSERT INTO karadio.message (message,sender) VALUES ( cli.next, IR)"); */break;
          case 86027401:  /*(" RIGHT");*/  irStr[0]; Serial.print("cli.prev\r");/* sql("INSERT INTO karadio.message (message,sender) VALUES ( cli.prev  , IR)");*/ break;
          case 87595173:  /*(" 0"); STOP*/  nbStation('0'); break;
          case 87558197:  /*(" 1");*/  nbStation('1'); break;
          case 87591093:  /*(" 2");*/  nbStation('2'); break;
          case 87574645:  /*(" 3");*/  nbStation('3'); break;
          case 87607541:  /*(" 4");*/  nbStation('4'); break;
          case 87566357:  /*(" 5");*/  nbStation('5'); break;
          case 87599253:  /*(" 6");*/  nbStation('6'); break;
          case 87582805:  /*(" 7");*/  nbStation('7'); break;
          case 87615701:  /*(" 8");*/  nbStation('8'); break;
          case 87562277:  /*(" 9");*/  nbStation('9'); break;
          default:;
            //Serial.print("cli.play(\"2\") \r")
            /*Serial.println(" other button   ");*/
        };// End Case
        buttonTime = millis();
        irrecv.resume(); // Receive the next value
      }
      if ((millis() - buttonTime) > 1000) {
        if (strlen(irStr) > 0)
        {
          Serial.print("cli.play(\""); Serial.print(irStr); Serial.print("\")\r");
          irStr[0] = 0;
        }
      }

    }
  }
}

//----------------------- WiFi handling
void connectWifi() {
  if (debug) Serial.print("Connecting as wifi client to SSID: ");
  if (debug) Serial.println(ssid);

  // use in case of mode problem
  WiFi.disconnect();
  // switch to Station mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin ( ssid, password );

  if (debug ) WiFi.printDiag(Serial);

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  unsigned long uartTime = startTime;
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    if (debug) Serial.print(".");
  }
  if (debug) Serial.println("");
  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    if (debug)  Serial.print("WiFi connected; IP address: ");
    if (debug)  Serial.println(WiFi.localIP());
  } else {
    if (debug) Serial.print("WiFi connect failed to ssid: ");
    if (debug)  Serial.println(ssid);
    if (debug) Serial.print("WiFi password <");
    if (debug) Serial.print(password);
    if (debug)  Serial.println(">");
    if (debug) Serial.println("Check for wrong typing!");
  }
}  // connectWiFi()

void nbStation(char nb)
{
  if (strlen(irStr) >= 3) irStr[0] = 0;
  uint8_t id = strlen(irStr);
  irStr[id] = nb;
  irStr[id + 1] = 0;
}


void telnetClientWelcome() {
  if (TelnetClient.connected()) {  // send data to Client
    TelnetClient.print("UART interface for the Wifi Webradi\n");
    TelnetClient.print("Interface information: http://karadio.karawin.fr/Interface.txt");
  }
}

