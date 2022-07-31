#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "nomedaredewifi";
const char* senhawifi = "senhadaredewifi";
const char* brokermqtt = "192.168.0.75";
#define mqtt_port 1883
#define usuariomqtt "pessoa_idosa_mqtt"
#define senhamqtt "1234"
#define canal "/ic/nm/serialdata/uno/"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void conectarwifi() {
  
    delay(10);
    WiFi.begin(ssid, senhawifi);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    randomSeed(micros());
}

void reconnect() {
  
  while (!client.connected()) {
    
    String clientId = "ESP01";
    clientId += String(random(0xffff), HEX);
      if (client.connect(clientId.c_str(),usuariomqtt,senhamqtt)) {
            client.publish("/ic/presence/nm/", "Esta conectado");
      } else {
            delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  conectarwifi();
  client.setServer(brokermqtt, mqtt_port);
  reconnect();
}

void publicardados(char *dados){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(canal, dados);
}
void loop() {
   client.loop();
   if (Serial.available() > 0) {
     char bfr[101];
     memset(bfr,0, 101);
     Serial.readBytesUntil( '\n',bfr,100);
     publicardados(bfr);
   }
 }
