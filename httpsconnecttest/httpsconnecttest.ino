// Kyle Santos
// Code adapted from https://randomnerdtutorials.com/esp32-http-get-open-weather-map-thingspeak-arduino/
// 9-26-24

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "spl-public";
const char* password = "";

//Your Domain name with URL path or IP address with path
const char* server = "https://eink-infoboard.onrender.com";

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIICCjCCAZGgAwIBAgIQbkepyIuUtui7OyrYorLBmTAKBggqhkjOPQQDAzBHMQsw\n" \
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n" \
"MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n" \
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n" \
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQA\n" \
"IgNiAATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzu\n" \
"hXyiQHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/l\n" \
"xKvRHYqjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud\n" \
"DgQWBBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNnADBkAjBqUFJ0\n" \
"CMRw3J5QdCHojXohw0+WbhXRIjVhLfoIN+4Zba3bssx9BzT1YBkstTTZbyACMANx\n" \
"sbqjYAuG7ZoIapVon+Kz4ZNkfF6Tpt95LY2F45TPI11xzPKwTdb+mciUqXWi4w==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, NULL);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.print("Connected to network with IP Address: ");
  Serial.println(WiFi.localIP());

  // USING INSECURE
  //client.setCACert(root_ca);
  client.setInsecure();

  // if(WiFi.status()== WL_CONNECTED){
  //   String content = httpGETRequest(server);
  //   Serial.println(content);
  //   // JSONVar pokeInfo = JSON.parse(content);
  // }

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.println("GET https://eink-infoboard.onrender.com HTTP/1.0");
    client.println("Host: eink-infoboard.onrender.com");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    String content = "";
    while (client.available()) {
      char c = client.read();
      content.concat(c);
    }
    Serial.println(content);
    client.stop();

    // Parse JSON
    JSONVar pokeInfo = JSON.parse(content);

    if (JSON.typeof(pokeInfo) == "undefined") {
      Serial.println("Parsing content json failed");
      return;
    }

    Serial.println(pokeInfo["firstAppearance"]);
    Serial.println(pokeInfo["height"]);
    Serial.println(pokeInfo["id"]);
    Serial.println(pokeInfo["sprite"]);
    Serial.println(pokeInfo["name"]);
    Serial.println(pokeInfo["types"]);
    Serial.println(pokeInfo["weight"]);

  }
}

void loop() {
  // put your main code here, to run repeatedly:

}