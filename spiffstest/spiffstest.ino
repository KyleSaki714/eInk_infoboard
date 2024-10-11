#include <SPI.h>
#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>

#define FORMAT_SPIFFS_IF_FAILED true
#define FILENAME "/walterwhite.json"

  void listAllFiles(){
  
    File root = SPIFFS.open("/");
  
    File file = root.openNextFile();
  
    while(file){
  
        Serial.print("FILE: ");
        Serial.println(file.name());
  
        file = root.openNextFile();
    }
  
  }

void setup() {
  Serial.begin(9600);

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  SPIFFS.remove("/pkmndata.json");
  SPIFFS.remove("/test.json");

  listAllFiles();

  if (!SPIFFS.exists(FILENAME)) {
    Serial.println("File does not exist, creating file...");
    File createFile = SPIFFS.open(FILENAME, FILE_WRITE);
    if (!createFile) {
      Serial.println("Failed to create file");
      return;
    }

    JsonDocument createJson;
    createJson["hello"] = "world";
    createJson["breakingbad"] = "what if walter white found jesus?";

    serializeJson(createJson, createFile);

    createFile.close();
  }
  
  JsonDocument doc;

  File file = SPIFFS.open(FILENAME, FILE_READ);
  if (!file)
  {
    Serial.println("fail to open file");
    return;
  }
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    file.close();
    return;
  }
  file.close();

  String wwhite = doc["breakingbad"].as<String>();
  Serial.println(wwhite);


  // DynamicJsonDocument doc(4096);
  // doc.createNestedArray("dailyData");
  // doc.createNestedArray("weeklyData");
  // doc.createNestedArray("monthlyData");
  // doc.createNestedArray("yearlyData");
  // DynamicJsonDocument doc(200); // <- 200 bytes in the heap

  // doc["hello"] = "world";
  // doc["breakingbad"] = "what if walter white found jesus?";

  // // serializeJson(doc, Serial);
  // serializeJson(doc, file);
  // // file.close();

  // DeserializationError error = deserializeJson(doc, file);
  // if (error) {
  //   Serial.print("deserializeJson() failed: ");
  //   Serial.println(error.f_str());
  //   file.close();
  //   return;
  // }
  // file.close();

  // JsonObject mrwhite = doc["breakingbad"].as<JsonObject>();
  // Serial.println(mrwhite);


  Serial.println("Success");
}

void loop() {
  // put your main code here, to run repeatedly:

}
