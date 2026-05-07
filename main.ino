#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <avr/wdt.h>

// Pin Definitions
const int G3X_MESSAGE_PIN = 2; // Discrete Output to G3X PFD

// --- Non-FIKI Proactive Thresholds ---
const float TEMP_DANGER_CELSIUS = 7.0;   // Increased to 7C for early activation
const float SPREAD_DANGER_CELSIUS = 5.0;  // Wider spread to catch humidity early
const float HYSTERESIS = 1.0;            

// Safety / Watchdog Variables
unsigned long lastSuccessfulRead = 0;
const unsigned long SENSOR_TIMEOUT_MS = 5000; 

Adafruit_BME280 bme; 
bool heaterActive = false;

void setup() {
  Serial.begin(115200);
  pinMode(G3X_MESSAGE_PIN, OUTPUT);
  digitalWrite(G3X_MESSAGE_PIN, LOW);

  // Attempt to start BME280 (Standard address 0x76)
  if (!bme.begin(0x76)) {
    Serial.println("CRITICAL: BME280 NOT FOUND!");
    handleSensorFailure();
  }

  lastSuccessfulRead = millis();
  
  // Enable internal hardware watchdog (8 seconds)
  wdt_enable(WDTO_8S); 
}

void loop() {
  wdt_reset(); // Signal MCU that code is running fine

  float oat = bme.readTemperature();
  float rh = bme.readHumidity();

  // Validate data integrity
  if (isnan(oat) || isnan(rh)) {
    if (millis() - lastSuccessfulRead > SENSOR_TIMEOUT_MS) {
      handleSensorFailure();
    }
  } else {
    lastSuccessfulRead = millis(); // Reset timeout on good data
    
    // Calculate Dew Point (Magnus-Tetens)
    float alpha = log(rh / 100.0) + (17.625 * oat) / (243.04 + oat);
    float dewPoint = (243.04 * alpha) / (17.625 - alpha);
    float spread = oat - dewPoint;

    // --- PROACTIVE DECISION LOGIC ---
    if (oat <= TEMP_DANGER_CELSIUS && spread <= SPREAD_DANGER_CELSIUS) {
      heaterActive = true;
    } 
    else if (oat > (TEMP_DANGER_CELSIUS + HYSTERESIS) || spread > (SPREAD_DANGER_CELSIUS + HYSTERESIS)) {
      heaterActive = false;
    }

    digitalWrite(G3X_MESSAGE_PIN, heaterActive ? HIGH : LOW);

    // Debugging stream for flight testing
    Serial.print("OAT: "); Serial.print(oat);
    Serial.print("C | Spread: "); Serial.print(spread);
    Serial.print("C | HEATER: "); Serial.println(heaterActive ? "ON" : "OFF");
  }

  delay(1000); 
}

void handleSensorFailure() {
  Serial.println("ALERT: SENSOR TIMEOUT - STARTING SIGNAL FLASH...");
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(G3X_MESSAGE_PIN, HIGH);
    delay(1000);
    digitalWrite(G3X_MESSAGE_PIN, LOW);
    delay(1000);
  }

  Serial.println("REBOOTING MCU...");
  
  // Exit loop and let Hardware Watchdog force a hard reset
  while (1) {} 
}
