#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define TRIGGERPIN D1
#define ECHOPIN D2

long duration;
int distance, percentage;
const int minDistance = 2; // in cm
const int maxDistance = 15; // in cm
int initialDistance = -1; // Initialize to -1 to indicate no initial reading yet

// Debounce and notification variables
unsigned long lastDebounceTime = 0;
unsigned long lastNotificationTime = 0;
const unsigned long debounceDelay = 1000; // Debounce time in milliseconds (1 second)
const unsigned long messageInterval = 2 * 24 * 60 * 60 * 1000; // Notification interval (2 days in milliseconds)
const int significantChangeThreshold = 2; // Minimum distance change (cm) to trigger notification

char auth[] = "************";//from blynk template
char ssid[] = "*********"; // your WiFi name
char pass[] = "*****"; // your WiFi password

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGERPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  unsigned long currentTime = 0;

  // Check debounce time and if initial reading is available
  if (currentTime - lastDebounceTime >= debounceDelay) {
    distance = getDistance();
    percentage = (1.0 - (float(distance) - minDistance) / (maxDistance - minDistance)) * 100;
    Blynk.virtualWrite(V2, percentage);

    // Check for significant change or periodic notification
    if (abs(initialDistance - distance) > significantChangeThreshold || 
        (currentTime - lastNotificationTime >= messageInterval && initialDistance != -1)) {
      sendSensor();
      initialDistance = distance;
      lastNotificationTime = currentTime;
    }

    lastDebounceTime = currentTime;
  }

  Blynk.run();
}

int getDistance() {
  // Implement your ultrasonic sensor distance measurement function here
  // This example assumes digital pin connection and uses pulseIn

  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIGGERPIN, LOW);

  duration = pulseIn(ECHOPIN, HIGH);
  return duration * 0.034 / 2;
}

void sendSensor() {
  if (distance <= 4) {
    Blynk.logEvent("stock_full");
  } else if (distance >= 4 && distance <= 7) {
    Blynk.logEvent("half_done");
  } else if (distance > 7 && distance < 16) {
    Blynk.logEvent("low_stock");
  }
  Serial.print("Distance (cm): ");
  Serial.println(distance);
  Serial.print("percentage: ");
  Serial.println(percentage);
}
