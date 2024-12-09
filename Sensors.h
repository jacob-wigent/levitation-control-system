#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include "Adafruit_VL6180X.h"

// Constants for Sensor Calculations
const float FORCE_SLOPE = -5.03;
const float FORCE_INTERCEPT = 12.25;
const int MEDIAN_FILTER_SIZE = 15;

class Sensors {
private:
  // Pin Definitions
  const int POT_PIN = A1;
  const int FORCE_PIN = A0;
  const int BUTTON_PIN = 8;

  // Sensor Objects
  Adafruit_VL6180X distanceSensor;

  // Sensor Data
  float forceZeroOffset = 0.0;
  int potentiometerValue = 0;
  int heightValue = 0;
  float forceValue = 0.0;
  bool currentButtonState = 0;
  bool previousButtonState = 1;
  int initialDistance = 0;

  float heightOverride;
  bool heightOverrided = false;

  // Median Filter
  int distanceBuffer[MEDIAN_FILTER_SIZE];
  int bufferIndex = 0;

public:
  void initializeAll() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    while (!distanceSensor.begin()) {
      Serial.println("Error: VL6180X sensor not detected.");
      delay(2000);
    }

    // Zero force sensor
    forceZeroOffset = calculateForce();

    // Initialize the distance buffer with multiple readings
    const int NUM_READINGS = 20;  // Number of readings to average
    for (int i = 0; i < NUM_READINGS; ++i) {
      distanceBuffer[i % MEDIAN_FILTER_SIZE] = distanceSensor.readRange();
      delay(10);
    }

    initialDistance = medianFilter(distanceBuffer, MEDIAN_FILTER_SIZE) + 5.0;  // Set the initial distance to the median value
  }

  int getPotentiometerValue() const {
    return potentiometerValue;
  }

  float getHeightValue() const {
    return heightOverrided ? heightOverride : heightValue;
  }

  float getForceValue() const {
    return forceValue;
  }

  bool getButtonState() {
    bool value = currentButtonState && (currentButtonState != previousButtonState);
    previousButtonState = currentButtonState;
    return value;
  }

  bool getButtonStateRaw() const {
    return currentButtonState;
  }

  void updateAll() {
    potentiometerValue = analogRead(POT_PIN);
    forceValue = -(calculateForce() - forceZeroOffset);

    currentButtonState = !digitalRead(BUTTON_PIN);

    // Median filter for distance
    int rawDistance = distanceSensor.readRange();
    distanceBuffer[bufferIndex] = rawDistance;
    bufferIndex = (bufferIndex + 1) % MEDIAN_FILTER_SIZE;
    heightValue = initialDistance - medianFilter(distanceBuffer, MEDIAN_FILTER_SIZE);
  }

  float calculateForce() const {
    float rawValue = analogRead(FORCE_PIN);
    float voltage = rawValue / 1023.0 * 5.0;
    return FORCE_SLOPE * voltage + FORCE_INTERCEPT;
  }
  
  void setHeightOverride(float height) {
    heightOverrided = true;
    heightOverride = height;
  }

private:
  int medianFilter(const int *values, int size) const {
    int sorted[size];
    memcpy(sorted, values, size * sizeof(int));

    // Sort the array
    for (int i = 0; i < size - 1; ++i) {
      for (int j = i + 1; j < size; ++j) {
        if (sorted[i] > sorted[j]) {
          int temp = sorted[i];
          sorted[i] = sorted[j];
          sorted[j] = temp;
        }
      }
    }
    return sorted[size / 2];
  }
};

#endif