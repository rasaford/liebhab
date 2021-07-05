#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <base64.h>

#include "arduino_secrets.h"

//
// WiFi params
//
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

//
// Server params
//
const String authUsername = AUTH_USERNAME;
const String authPassword = AUTH_PASSWORD;
const char *endpoint = API_ENDPOINT;
const char *root_ca = ROOT_CA;

//
// Light Params
//
#define SWITCH_PIN 13
#define ADC_PIN 32
// if light is currently on
bool light = true;
bool light_target = true;
// power reading params
#define MW_THRESHOLD 100.0

//
// Misc Params
//
#define WIFI_RETRY_MS 10 * 1000
#define WAIT_MS 3 * 60 * 1000
#define ERROR_RETRY_MS 10 * 100

//
// ArduinoJSON params
//
#define ARDUINOJSON_USE_LONG_LONG 0
#define ARDUINOJSON_USE_DOUBLE 0

// WIFI
void connect_wifi()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to the WiFi network ");
  Serial.println(ssid);
}

/*
True if the light is consuming more power than the threshold
*/
bool is_on()
{
  int adc_value = analogRead(ADC_PIN);
  float mV = ((float)adc_value) / 4095 * 3.3 * 1000;
  float mW = mV * 5;
  light = mW > MW_THRESHOLD;
  return light;
}

/*
Pulses SWITCH_PIN once (active low)
*/
void pulse()
{
  digitalWrite(SWITCH_PIN, LOW);
  delay(100);
  digitalWrite(SWITCH_PIN, HIGH);
}

/*
Toggles the light, insuring that the desired target state
is reached by measuring the consumed power.
*/
void toggle_light()
{
  // update target state
  light_target = !light_target;
  Serial.print("Toggling light: ");
  Serial.print(light);
  Serial.print(" -> ");
  Serial.println(light_target);

  while (light != light_target)
  {
    pulse();
    // wait 2s to let light state settle.
    delay(2000);
    // measure power consumption
    is_on();

    Serial.print("try toggle, light_state: ");
    Serial.println(light);
  }
  Serial.print("toggled light state: ");
  Serial.println(light);
}

void setup()
{
  Serial.begin(115200);
  pinMode(SWITCH_PIN, OUTPUT);

  connect_wifi();

  // setup light toggle
  light = true;
  toggle_light();
}

void loop()
{
  // Check the current connection status
  if ((WiFi.status() != WL_CONNECTED))
  {
    connect_wifi();
    delay(WIFI_RETRY_MS);
  }

  // connect to endpoint
  HTTPClient http;
  http.begin(endpoint, root_ca);

  String auth = base64::encode(authUsername + ":" + authPassword);
  http.addHeader("Authorization", "Basic " + auth);

  const int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK)
  {
    Serial.print("Unexpected return code: ");
    Serial.println(httpCode);
    http.end();
    delay(ERROR_RETRY_MS);
    return;
  }

  // read payload
  String payload = http.getString();
  // Free HTTP ressources
  http.end();

  Serial.print("Got messages form server: ");
  Serial.println(payload);
  
  if (payload == "[]")
  {
    // Serial.println("Array has 0 elements, skipping");
    delay(ERROR_RETRY_MS);
    return;
  }

  // deserialize JSON
  StaticJsonDocument<384> doc; // size computed with https://arduinojson.org/v6/assistant/
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.print("ERROR: Could not deserialize json: ");
    Serial.println(error.c_str());
    delay(ERROR_RETRY_MS);
    return;
  }

  JsonObject root_0 = doc[0];
  bool sent_out = root_0["sent_out"];

  if (!sent_out) {
    Serial.println("Message sent out not true");
    delay(ERROR_RETRY_MS);
  }

  // turn light on and off again
  Serial.print("Light on for ");
  Serial.print(WAIT_MS);
  Serial.println(" ms");
  toggle_light(); // on

  delay(WAIT_MS);

  toggle_light(); // off
}
