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
#include <SoftwareSerial.h>
#include <StreamUtils.h>
#include <AsyncJson.h>

#define D6 6
#define D5 5

SoftwareSerial s(D6, D5); // (Rx, Tx)

AsyncWebServer server(80);

const char *ssid = "Ceyentra Wifi 3";
const char *password = "CeyTec@3";

bool isProcessing = false;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"error\":\"Not Found\"}");
}

AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/medicines", [](AsyncWebServerRequest *request, JsonVariant &json) {
  
    // json.prettyPrintTo(Serial);

  JsonArray &arr = json.as<JsonArray>();

  for (auto value : arr)
  {
    // const char *value_description = value["description"]; 
    // const char *value_image_link = value["image_link"];   
    // const char *value_name = value["name"];               
    // int value_price = value["price"];                     
    const char *value_slot = value["slot"];

    Serial.println(value_slot);
  }

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

  s.begin(9600);

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

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

  server.on("/progress", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;

    JsonObject &root = jsonBuffer.createObject();

    if (isProcessing)
    {
      root["progress"] = "processing";
    }else
    {
      root["progress"] = "done";
    }
    
    root.printTo(*response);

    request->send(response);
  });

  server.addHandler(handler);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound(notFound);

  server.begin();
}

int i = 0;

void loop()
{
  if (isProcessing && i==10)
  {
    isProcessing = false;
    i = 0;
  }

  if (isProcessing)
  {
    i++;
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Server is Running...");
  delay(1000);       

  digitalWrite(LED_BUILTIN, LOW); 
  delay(1000);
}