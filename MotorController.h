#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <Servo.h>

// Constants
const int PULSES_PER_REVOLUTION = 14; //PPR of motor encoder: https://www.revrobotics.com/rev-21-1651/
const unsigned long STOP_TIMEOUT = 100000; // Timeout for detecting stopped motor in microseconds (100 ms)

class MotorController {
private:
    // ESC Object
    Servo esc;
    
    // Encoder Variables
    static volatile unsigned long lastPulseTime;
    static volatile float rpm;

    // Motor State
    int currentPower = 0;
    bool overrideMode = false;
    int overridePower = 0;
    bool locked = false;

    static void timePulseISR() {
        unsigned long currentPulseTime = micros();
        if (lastPulseTime > 0) {
            // Calculate time difference in microseconds
            unsigned long timeDiff = currentPulseTime - lastPulseTime;
            rpm = (60000000.0 / timeDiff) / PULSES_PER_REVOLUTION;
        }
        lastPulseTime = currentPulseTime;
    }

public:
    // Methods
    void initialize() {
        esc.attach(9, 1000, 2000);  // Motor ESC PWM range
        pinMode(2, INPUT_PULLUP);
        pinMode(3, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(2), MotorController::timePulseISR, CHANGE);
    }

    bool setPowerFromInput(int analogValue) {
        return setPower(map(analogValue, 0, 1023, 1000, 2000));
    }

    bool setPower(int microseconds) {
        bool successful = true;

        currentPower = microseconds;

        // If the speed is locked or overridden, ignore the set power
        if (locked) {
            currentPower = 1000;
            successful = false;
        } else if (overrideMode) {
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

    float getRPM() const {
        // Check for timeout to detect stopped motor
        if (micros() - lastPulseTime > STOP_TIMEOUT) {
            return 0.0;  // No pulse detected recently, motor is stopped
        }
        return rpm;
    }
};

// Initialize static variables
volatile unsigned long MotorController::lastPulseTime = 0;
volatile float MotorController::rpm = 0;

#endif