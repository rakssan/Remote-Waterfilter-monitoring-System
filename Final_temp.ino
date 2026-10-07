#define BLYNK_TEMPLATE_ID "TMPL37DRUviKa"
#define BLYNK_TEMPLATE_NAME "Water Filter Remote Monitoring 1"
#define BLYNK_AUTH_TOKEN "8JMigaZ_r6v9gIre1Dqxi_ajEFpbV-GZ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Batman";
char pass[] = "menarebrave";

#define LED_PIN D1

BlynkTimer timer;

bool ledState = false;

void LED_ON()
{
  ledState = true;

  digitalWrite(LED_PIN, HIGH);

  Blynk.virtualWrite(V0, 1);

  Serial.println("LED ON - 3 seconds");

  // After 3 seconds, turn OFF
  timer.setTimeout(3000L, LED_OFF);
}

void LED_OFF()
{
  ledState = false;

  digitalWrite(LED_PIN, LOW);

  Blynk.virtualWrite(V0, 0);

  Serial.println("LED OFF - 2 seconds");

  // After 2 seconds, turn ON
  timer.setTimeout(2000L, LED_ON);
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Start the sequence
  LED_ON();
}

void loop()
{
  Blynk.run();
  timer.run();
}