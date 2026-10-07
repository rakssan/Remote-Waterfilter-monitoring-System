#include <WiFi.h>
#include <HTTPClient.h>

// =====================================================
// Wi-Fi Settings
// =====================================================

const char* WIFI_SSID = "Batman";
const char* WIFI_PASSWORD = "menarebrave";


// =====================================================
// Flask Server Settings
// Current laptop IPv4:
// 172.28.45.154
// =====================================================

const char* SERVER_URL =
  "http://172.28.45.154:5000/api/status";


// =====================================================
// PC817 Output Pins
//
// PC817 Pin 4 -> ESP32 GPIO
// PC817 Pin 3 -> ESP32 GND
//
// 10k pull-up:
// 3.3V -> 10k -> GPIO
//
// Output logic:
// PC817 ON  = GPIO LOW
// PC817 OFF = GPIO HIGH
//
// Therefore:
// !digitalRead()
// =====================================================

#define UV_PIN       27
#define OZONE_PIN    26
#define SAFE_PIN     25
#define SERVICE_PIN  32


// =====================================================
// Sending interval
// =====================================================

unsigned long previousMillis = 0;

const unsigned long SEND_INTERVAL = 1000;


// =====================================================
// CONNECT TO WI-FI
// =====================================================

void connectWiFi()
{
  Serial.println();
  Serial.println("================================");
  Serial.println("       Wi-Fi Connection");
  Serial.println("================================");

  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi CONNECTED!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Laptop Flask Server: ");
  Serial.println(SERVER_URL);

  Serial.println();
}


// =====================================================
// READ WATER PURIFIER STATUS
// =====================================================

bool readUV()
{
  return !digitalRead(UV_PIN);
}

bool readOzone()
{
  return !digitalRead(OZONE_PIN);
}

bool readSafeWater()
{
  return !digitalRead(SAFE_PIN);
}

bool readService()
{
  return !digitalRead(SERVICE_PIN);
}


// =====================================================
// SEND DATA TO FLASK SERVER
// =====================================================

void sendDataToServer()
{
  // ---------------------------------------------------
  // Check Wi-Fi
  // ---------------------------------------------------

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi disconnected!");
    connectWiFi();
    return;
  }


  // ---------------------------------------------------
  // Read PC817 signals
  // ---------------------------------------------------

  bool uv = readUV();
  bool ozone = readOzone();
  bool safeWater = readSafeWater();
  bool service = readService();


  // ---------------------------------------------------
  // Display status
  // ---------------------------------------------------

  Serial.println();
  Serial.println("========== STATUS ==========");

  Serial.print("UV         : ");
  Serial.println(uv ? "ON" : "OFF");

  Serial.print("OZONE      : ");
  Serial.println(ozone ? "ON" : "OFF");

  Serial.print("SAFE WATER : ");
  Serial.println(safeWater ? "SAFE" : "UNSAFE");

  Serial.print("SERVICE    : ");
  Serial.println(service ? "REQUIRED" : "NOT REQUIRED");


  // ---------------------------------------------------
  // Create JSON
  // ---------------------------------------------------

  String jsonData = "{";

  jsonData += "\"uv\":";
  jsonData += (uv ? "true" : "false");

  jsonData += ",\"ozone\":";
  jsonData += (ozone ? "true" : "false");

  jsonData += ",\"safe_water\":";
  jsonData += (safeWater ? "true" : "false");

  jsonData += ",\"service\":";
  jsonData += (service ? "true" : "false");

  jsonData += "}";


  // ---------------------------------------------------
  // Display JSON
  // ---------------------------------------------------

  Serial.print("JSON: ");
  Serial.println(jsonData);


  // ---------------------------------------------------
  // Send HTTP POST request
  // ---------------------------------------------------

  WiFiClient client;

  HTTPClient http;

  Serial.println("Connecting to Flask...");

  http.begin(client, SERVER_URL);

  http.addHeader("Content-Type", "application/json");


  int httpCode = http.POST(jsonData);


  // ---------------------------------------------------
  // Display HTTP result
  // ---------------------------------------------------

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);


  if (httpCode > 0)
  {
    String response = http.getString();

    Serial.print("Flask Response: ");
    Serial.println(response);
  }
  else
  {
    Serial.print("Connection Error: ");
    Serial.println(http.errorToString(httpCode));
  }


  http.end();
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("   SMART WATER PURIFIER MONITOR");
  Serial.println("========================================");


  // ---------------------------------------------------
  // Configure PC817 GPIOs
  // ---------------------------------------------------

  pinMode(UV_PIN, INPUT);
  pinMode(OZONE_PIN, INPUT);
  pinMode(SAFE_PIN, INPUT);
  pinMode(SERVICE_PIN, INPUT);


  // ---------------------------------------------------
  // Connect Wi-Fi
  // ---------------------------------------------------

  connectWiFi();


  // ---------------------------------------------------
  // Send first reading immediately
  // ---------------------------------------------------

  sendDataToServer();
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  if (millis() - previousMillis >= SEND_INTERVAL)
  {
    previousMillis = millis();

    sendDataToServer();
  }
}