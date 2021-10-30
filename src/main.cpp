#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>

#include <WiFiFunctions.h>

#include <PinDefinitions.h>
#include <CpuDefinitions.h>
#include <CpuController.h>

// All accesspoints in the order they should be tried
WiFiInfo WiFiAccessPoints[3] = {{"FRITZBox Thomas 2,4 Ghz", "4858035152347806"},
                                {"FRITZbox Schwark 2,4 Ghz", "4858035152347806"},
                                {"Speedport Schwark 2,4 GHz", "4858035152347806"}};

// The ESP8266 HTTP Web server on port 80
ESP8266WebServer server(80);

// Default HTTP response types saved in SRAM
static const char RESPONSE_TEXT[] PROGMEM = "text/plain";
static const char RESPONSE_JSON[] PROGMEM = "application/json";

// Variables for the 8Bit cpu
CpuController cpu;

/* This returns the current control word. */
void getControlWord()
{
  String buf;
  DynamicJsonDocument doc(255);

  doc["controlWord"] = cpu.getControlWord();

  serializeJson(doc, buf);

  server.send(200, F("application/json"), buf);
}

/* This sets the current control word. */
// void setControlWord()
// {
//   // Check if body was received
//   if (server.hasArg("plain") == false)
//   {
//     server.send(400, FPSTR(RESPONSE_TEXT), F("Body not received!"));
//     return;
//   }
//
//   // Deserialize body and check for deserialization errors
//   DynamicJsonDocument doc(512);
//   DeserializationError error = deserializeJson(doc, server.arg("plain"));
//   if (error)
//   {
//     String errorText(error.f_str());
//     server.send(400, FPSTR(RESPONSE_TEXT), "Deserialization error: " + errorText);
//     return;
//   }
//
//   server.send(200, FPSTR(RESPONSE_TEXT), F("Succesfully set control word!"));
// }

/* This returns the actual instruction. */
void getInstruction()
{
  String buf;
  DynamicJsonDocument doc(512);

  doc["flags"] = cpu.getFlags();
  doc["instruction"] = cpu.getInstruction();
  doc["instructionStep"] = cpu.getInstructionStep();

  serializeJson(doc, buf);

  server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* This returns the current settings. */
void getSettings()
{
  DynamicJsonDocument doc(512);

  doc["ip"] = WiFi.localIP().toString();
  doc["gw"] = WiFi.gatewayIP().toString();
  doc["sm"] = WiFi.subnetMask().toString();

  if (server.arg("signalStrength") == "true")
  {
    doc["signalStrengh"] = WiFi.RSSI();
  }

  if (server.arg("chipInfo") == "true")
  {
    doc["chipId"] = ESP.getChipId();
    doc["flashChipId"] = ESP.getFlashChipId();
    doc["flashChipSize"] = ESP.getFlashChipSize();
    doc["flashChipRealSize"] = ESP.getFlashChipRealSize();
  }

  if (server.arg("freeHeap") == "true")
  {
    doc["freeHeap"] = ESP.getFreeHeap();
  }

  String buf;
  serializeJson(doc, buf);
  server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* Define routing */
void restServerRouting()
{
  // Server default response
  server.on("/", HTTP_GET, []()
            { server.send(200, F("text/html"), F("Welcome to the REST Web Server")); });

  // Server endpoints
  server.on(F("/controlword"), HTTP_GET, getControlWord);
  server.on(F("/instruction"), HTTP_GET, getInstruction);

  server.on(F("/settings"), HTTP_GET, getSettings);

  // Set not found response
  server.onNotFound([]()
                    {
                      String message = F("File Not Found\n\n");

                      message += "URI: ";
                      message += server.uri();
                      message += "\nMethod: ";
                      message += server.method();
                      message += "\nArguments: ";
                      message += server.args();
                      message += "\n";

                      for (uint8_t i = 0; i < server.args(); i++)
                      {
                        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
                      }

                      server.send(404, FPSTR(RESPONSE_TEXT), message);
                    });
}

/* Initializes the rest server. */
void restServerInit()
{
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local -> Doesn't work?!
  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started!");
  }

  // Set server routing
  restServerRouting();

  // Start server
  server.begin();

  Serial.println("HTTP Server started!");
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  // Initialize the cpu controller
  cpu.init();

  // Set the wifi mode
  WiFi.mode(WIFI_STA);

  // Set a static ip and gateway
  WiFi.config((IPAddress){192, 168, 2, 50}, (IPAddress){192, 168, 2, 1}, (IPAddress){255, 255, 255, 0}, (IPAddress){192, 168, 2, 1});

  // Try to connect to a access point
  WiFiConnect(WiFiAccessPoints, sizeof(WiFiAccessPoints));

  // Initialize the rest server
  restServerInit();
}

void loop()
{
  server.handleClient();
  cpu.executeInstruction();
}
