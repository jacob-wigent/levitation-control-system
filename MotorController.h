#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <Servo.h>

// Constants
const int PULSES_PER_REVOLUTION = 14; //PPR of motor encoder: https://www.revrobotics.com/rev-21-1651/
const int ENCODER_RPM_INTERVAL = 50; //Interval at which the RPM is calculated from pulses

class MotorController {
private:
    // ESC Object
    Servo esc;
    
    // Encoder Variables
    static volatile long pulseCount;
    unsigned long lastUpdateTime = 0;
    float rpm = 0;
    
    // Motor State
    int currentPower = 0;
    bool overrideMode = false;
    int overridePower = 0;
    bool locked = false;

    static void countPulse() {
      if (digitalRead(2) == digitalRead(3)) {
          pulseCount++;
      } else {
          pulseCount--;
      }
    }

public:
    // Methods
    void initialize() {
        esc.attach(9, 1000, 2000);  // Motor ESC PWM range
        pinMode(2, INPUT_PULLUP);
        pinMode(3, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(2), MotorController::countPulse, CHANGE);
    }

    bool setPowerFromInput(int analogValue) {
      return setPower(map(analogValue, 0, 1023, 1000, 2000));
    }

    bool setPower(int microseconds) {
      bool successful = true;

      currentPower = microseconds;

      // If the speed is locked or overidded, ignore the set power
      if (locked)
      {
        currentPower = 1000;
        successful = false;
      }
      else if (overrideMode) {
        currentPower = overridePower;
        successful = false;
      }

      esc.writeMicroseconds(currentPower);
      return successful;
    }

    void lock() {
      locked = true;
    }

    void unlock() {
      locked = false;
    }

    bool isLocked() const { 
      return locked; 
    }

    void setOverrideMode(bool enable, int power) {
        overrideMode = enable;
        overridePower = power;
    }

    int getCurrentPower() const { 
        return currentPower; 
    }

    void updateRPM() {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= ENCODER_RPM_INTERVAL) {
            noInterrupts();
            rpm = (pulseCount / static_cast<float>(PULSES_PER_REVOLUTION)) * (60.0/ENCODER_RPM_INTERVAL) * 1000;
            pulseCount = 0;
            lastUpdateTime = currentTime;
            interrupts();
        }
    }

    float getRPM() const { 
        return rpm;
    }
};

volatile long MotorController::pulseCount = 0;

#endif