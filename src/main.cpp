#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <Wire.h>
#include <string.h>

#define SLAVE_ADDRESS 8

AsyncWebServer server(80);

const char *ssid = "Ceyentra Wifi 3";
const char *password = "CeyTec@3";

bool isProcessing = false;
String slots;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"error\":\"Not Found\"}");
}

/*
POST '/medicines'
*/
AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/medicines", [](AsyncWebServerRequest *request, JsonVariant &json) {
  // json.prettyPrintTo(Serial);

  Wire.beginTransmission(SLAVE_ADDRESS);

  JsonArray &arr = json.as<JsonArray>();

  const int arr_size = arr.size();

  int i = 0;

  slots = "";

  for (auto value : arr)
  {
    // const char *value_description = value["description"];
    // const char *value_image_link = value["image_link"];
    // const char *value_name = value["name"];
    // int value_price = value["price"];
    const char *value_slot = value["slot"];

    Serial.println(value_slot);

    Wire.write(value_slot);

    if (i < arr_size - 1)
    {
      Wire.write("*");
    }

    String temp(value_slot);
    temp += "\t";

    slots += temp;

    i++;
  }
  Wire.endTransmission();

  isProcessing = true;

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();

  root["success"] = "Successfully Started.";
  root.printTo(*response);

  request->send(response);
});

void setup()
{

  Serial.begin(115200);

  //I2C Communication
  Wire.begin(4, 5);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(LED_BUILTIN, OUTPUT);

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  /*
  GET '/progress'

  Returns current progress by fetching it from arduino through I2C. 
  */
  server.on("/progress", HTTP_GET, [](AsyncWebServerRequest *request) {
    Wire.requestFrom(SLAVE_ADDRESS, 4);

    String str;

    while (Wire.available())
    {
      char c = Wire.read();
      str += c;
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;

    JsonObject &root = jsonBuffer.createObject();

    if (str.equalsIgnoreCase("proc"))
    {
      root["progress"] = "processing";
    }
    else
    {
      root["progress"] = "done";
    }

    root.printTo(*response);

    request->send(response);
  });

  server.addHandler(handler);

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{

  Wire.requestFrom(SLAVE_ADDRESS, 4);

  String str;

  while (Wire.available())
  {
    char c = Wire.read();
    str += c;
  }

  if (str.equalsIgnoreCase("proc"))
  {
    
    Serial.println("Processing...");
    Serial.println(slots);
  }
  else
  {
    Serial.println("Waiting for new Request...");
  }

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}