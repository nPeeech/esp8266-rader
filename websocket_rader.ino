#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include "MYSSID.h"
#include "index_html.h"

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            break;
    }

}

void setup() {
    //Serial.begin(921600);
    Serial.begin(115200);

    //Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(MYSSID_ssid, MYSSID_pass);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(MYSSID_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", html_page);
    });

    server.begin();
    Serial.println("HTTP server started");

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
}

unsigned long last_10sec = 0;
unsigned int counter = 0;
int deg = 0;
int sign = 1;
int inc = 5;

void loop() {
    unsigned long t = millis();
    webSocket.loop();
    server.handleClient();

    if((t - last_10sec) > 10 * 10) {
        counter++;
        bool ping = (counter % 2);
        int i = webSocket.connectedClients(ping);
        Serial.printf("%d Connected websocket clients ping: %d\n", i, ping);
        last_10sec = millis();

        //dummy data
        if(deg>=180) sign=-1;
        else if(deg<=0) sign=1;
        deg+=sign*inc;
        
        JSONVar obj;
        obj["deg"] = deg;
        obj["dist"] = 3500;
        String json_str = JSON.stringify(obj);
        webSocket.broadcastTXT((const char *)json_str.c_str());
        Serial.println(obj);
    }
}
