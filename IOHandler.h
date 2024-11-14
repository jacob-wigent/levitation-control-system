#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "MotorController.h"
#include "Sensors.h"

// Constants
const int BAUD_RATE = 9600;

class IOHandler {
public:
    void initialize() {
        Serial.begin(BAUD_RATE);
    }

    void checkInput(MotorController& motorController) {
        if (Serial.available() > 0) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            input.toLowerCase();

            // Process commands
            if (input == "lock") {
                motorController.lock();
            } else if (input == "unlock") {
                motorController.unlock();
            } else {
                int speed = input.toInt();
                if (speed >= 1000 && speed <= 2000) {
                    motorController.setOverrideMode(true, speed);
                }
            }
        }
    }

    void sendDataCSV(const MotorController& motorController, const Sensors& sensors) const {
        unsigned long timestamp = millis();
        int powerPercentage = mapValueToPercentage(motorController.getCurrentPower(), 1086, 2000);
        int pwmMicroseconds = motorController.getCurrentPower();
        float rotorSpeed = motorController.getRPM();
        float force = sensors.getForceValue();
        float height = sensors.getHeightValue();

        // Print data as CSV
        Serial.print(timestamp);         // Timestamp (ms)
        Serial.print(",");              
        Serial.print(powerPercentage);   // Power (%)
        Serial.print(",");              
        Serial.print(pwmMicroseconds);   // PWM Microseconds (us)
        Serial.print(",");              
        Serial.print(rotorSpeed);        // Rotor Speed (RPM)
        Serial.print(",");              
        Serial.print(force);             // Force (N)
        Serial.print(",");              
        Serial.println(height);          // Height (mm)
    }

    void sendDataCSV(const MotorController& motorController, const Sensors& sensors, bool stable) const {
        unsigned long timestamp = millis();
        int powerPercentage = mapValueToPercentage(motorController.getCurrentPower(), /*OLD 1270*/1094, 2000);
        int pwmMicroseconds = motorController.getCurrentPower();
        float rotorSpeed = motorController.getRPM();
        float force = sensors.getForceValue();
        float height = sensors.getHeightValue();

        // Print data as CSV
        Serial.print(timestamp);         // Timestamp (ms)
        Serial.print(",");              
        Serial.print(powerPercentage);   // Power (%)
        Serial.print(",");              
        Serial.print(pwmMicroseconds);   // PWM Microseconds (us)
        Serial.print(",");              
        Serial.print(rotorSpeed);        // Rotor Speed (RPM)
        Serial.print(",");              
        Serial.print(force);             // Force (N)
        Serial.print(",");              
        Serial.print(height);          // Height (mm)
        Serial.print(",");
        Serial.println(stable);
    }

private:
    float mapValueToPercentage(float value, float inputMin, float inputMax) const {
        return constrain((value - inputMin) * 100 / (inputMax - inputMin), 0, 100);
    }
};

#endif