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

/* This returns the current mode of the cpu controller. */
void getMode()
{
	String buf;
	StaticJsonDocument<255> doc;

	doc["executeMode"] = cpu.getExecuteMode();
	doc["loadCodeMode"] = cpu.getLoadCodeMode();

	serializeJson(doc, buf);

	server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* This sets the mode of the cpu controller. */
void postMode()
{
	if (!server.hasArg("mode"))
	{
		server.send(400, FPSTR(RESPONSE_TEXT), F("mode argument missing!"));
		return;
	}

	// Reset all modes
	cpu.setExecuteMode(false);
	cpu.setLoadCodeMode(false);

	// Set the cpu to execute mode
	if (server.arg("mode") == "execute")
		cpu.setExecuteMode(true);

	// Set the cpu to load code mode
	if (server.arg("mode") == "loadcode")
		cpu.setLoadCodeMode(true);

	getMode();
}

/* This sets code the cpu controller should load into RAM. */
void postCode()
{
	// Check if the cpu is in load code mode 
	if (!cpu.getLoadCodeMode())
	{
		server.send(405, FPSTR(RESPONSE_TEXT), F("CPU is not in load code mode!"));
		return;
	}

	//	Check if body was received
	if (server.hasArg("plain") == false)
	{
		server.send(400, FPSTR(RESPONSE_TEXT), F("Body not received!"));
		return;
	}

	// Deserialize body and check for deserialization errors
	const size_t CAPACITY = JSON_ARRAY_SIZE(255);
	DynamicJsonDocument doc(CAPACITY);
	DeserializationError error = deserializeJson(doc, server.arg("plain"));
	if (error)
	{
		String errorText(error.f_str());
		server.send(400, FPSTR(RESPONSE_TEXT), "Deserialization error: " + errorText);
		return;
	}

	// Get the json array and copy it to a standard C array
	JsonArray array = doc.as<JsonArray>();
	uint8_t buffer[array.size()];

	copyArray(array, buffer, array.size());
	
	// Load code into RAM
	cpu.loadCodeToRam(buffer, sizeof(buffer));

	// Create the response text
	String startMessage = F("Succesfully received code array of ");
	String message = startMessage + sizeof(buffer);

	message += F(" byte(s).\n\tContent: { ");

	for(uint8_t i = 0; i < sizeof(buffer); i++)
	{
		message += "0x";
		message += String(buffer[i], HEX);
		message += ", ";
	}

	message += "}";

	server.send(200, FPSTR(RESPONSE_TEXT), message);
}

/* This returns the current control word of the cpu controller. */
void getControlWord()
{
	String buf;
	StaticJsonDocument<255> doc;

	doc["controlWord"] = cpu.getControlWord();

	serializeJson(doc, buf);

	server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* This returns the actual instruction of the cpu controller. */
void getInstruction()
{
	String buf;
	StaticJsonDocument<255> doc;

	doc["flags"] = cpu.getFlags();
	doc["instruction"] = cpu.getInstruction();
	doc["instructionStep"] = cpu.getInstructionStep();

	serializeJson(doc, buf);

	server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* This returns the actual amount of code to load and how many is already loaded to the cpu controller. */
void getCodeLoadStatus()
{
	String buf;
	StaticJsonDocument<255> doc;

	doc["loadCodeMode"] = cpu.getLoadCodeMode();
	doc["codeToLoad"] = cpu.getCodeToLoad();
	doc["codeLoaded"] = cpu.getCodeLoaded();

	serializeJson(doc, buf);

	server.send(200, FPSTR(RESPONSE_JSON), buf);
}

/* This resets the cpu controller. */
void postReset()
{
	cpu.reset();
	getInstruction();
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

/* Define routing for web API. */
void restServerRouting()
{
	// Server default response
	server.on("/", HTTP_GET, []()
			  { server.send(200, F("text/html"), F("8-Bit CPU REST API v1")); });

	// Server endpoints
	server.on(F("/mode"), HTTP_GET, getMode);
	server.on(F("/control"), HTTP_POST, postMode);
	server.on(F("/code"), HTTP_POST, postCode);

	server.on(F("/reset"), HTTP_POST, postReset);

	server.on(F("/controlword"), HTTP_GET, getControlWord);
	server.on(F("/instruction"), HTTP_GET, getInstruction);
	server.on(F("/code"), HTTP_GET, getCodeLoadStatus);

	server.on(F("/settings"), HTTP_GET, getSettings);

	// Set not found response
	server.onNotFound([]()
					  {
						  String message = F("File Not Found\n\n");

						  message += F("URI: ");
						  message += server.uri();
						  message += F("\nMethod: ");
						  message += server.method();
						  message += F("\nArguments: ");
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
		Serial.println(F("MDNS responder started!"));
	}

	// Set server routing
	restServerRouting();

	// Start server
	server.begin();

	Serial.println(F("HTTP Server started!"));
	Serial.println();
}

/* Basic setup of the ESP8266. */
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

/* Main loop */
void loop()
{
	server.handleClient();
	cpu.handleInstructions();
}
