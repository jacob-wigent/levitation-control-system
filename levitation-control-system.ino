#include "MotorController.h"
#include "Sensors.h"
#include "IOHandler.h"

// Constants
const unsigned long DATA_RATE_MS = 50;

// Objects
MotorController motorController;
Sensors sensors;
IOHandler ioHandler;

// Global Variables
unsigned long lastDataSendTime = 0;
bool collecting = false;

void setup() {
    ioHandler.initialize();
    motorController.initialize();
    sensors.initializeAll();
}

void loop() {
    unsigned long currentTime = millis();

    ioHandler.checkInput(motorController);

    sensors.updateAll();

    // Control motor speed based on potentiometer
    motorController.setPowerFromInput(sensors.getPotentiometerValue());

    // // Send sensor and motor data at regular intervals
    // if (currentTime - lastDataSendTime >= DATA_RATE_MS) {
    //     ioHandler.sendDataCSV(motorController, sensors);
    //     lastDataSendTime = currentTime;
    // }

    // Send data on button press
    // if (sensors.getButtonState()) {
    //   ioHandler.sendDataCSV(motorController, sensors);
    // }

    // Send data constantly with stable tags with button press
    // if (currentTime - lastDataSendTime >= DATA_RATE_MS) {
    //     ioHandler.sendDataCSV(motorController, sensors, sensors.getButtonStateRaw());
    //     lastDataSendTime = currentTime;
    // }

    //Start/Stop Collecting with Button
    if (sensors.getButtonState()) {
        collecting = !collecting;
    }
    if (collecting) {
      if (currentTime - lastDataSendTime >= DATA_RATE_MS) {
        ioHandler.sendDataCSV(motorController, sensors);
        lastDataSendTime = currentTime;
      }
    }

}