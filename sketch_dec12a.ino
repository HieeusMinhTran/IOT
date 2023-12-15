#define BLYNK_TEMPLATE_ID "TMPL6qMyoxRQ3"
#define BLYNK_TEMPLATE_NAME "Water Pump Tree"
#define BLYNK_AUTH_TOKEN "laA86D2qcalJ8vPGbtnRqSj5UcxkOQ8t"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "laA86D2qcalJ8vPGbtnRqSj5UcxkOQ8t";
char ssid[] = "TP-Link_BDA4";
char pass[] = "73710790";

BlynkTimer timer;
bool pumpActive = false;

#define sensor A0
#define waterPump D3

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, soilMoistureSensor);
}

// Get the button value
BLYNK_WRITE(V1) {
  int relayState = param.asInt();

  if (relayState == 1) {
    digitalWrite(waterPump, LOW);
  } else {
    digitalWrite(waterPump, HIGH);
  }
}

// Get the soil moisture values
void soilMoistureSensor() {
  int moistureValue = analogRead(sensor);
  int mappedValue = map(moistureValue, 0, 1024, 0, 100);
  int humidityPercentage = (mappedValue - 100) * -1;

  Blynk.virtualWrite(V0, humidityPercentage);

  // If soil moisture is below 40% and pump is not active, turn on water pump for 40 seconds
  if (humidityPercentage < 40 && !pumpActive) {
    pumpActive = true;
    digitalWrite(waterPump, LOW);
    
    // Set timeout to 10000 milliseconds (10 seconds) if humidity is above 40%
    int timeout = (humidityPercentage > 40) ? 10000 : 40000;
    
    timer.setTimeout(timeout, []() {
      digitalWrite(waterPump, HIGH);
      pumpActive = false;
    });
  } else if (humidityPercentage > 50) {
    // If humidity is above 50%, turn off the water pump
    digitalWrite(waterPump, HIGH);
    pumpActive = false;
  }
}




void loop() {
  Blynk.run();
  timer.run();
}