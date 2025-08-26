#pragma once
#include <Arduino.h>
#include "fan_controller_config.h"

/**
 * @class FanController
 * @brief Proste sterowanie wentylatorem przez PWM (open-drain MOSFET).
 */
class FanController {
public:
    void begin() {
        pinMode(PIN_FAN_PWM, OUTPUT);
        analogWrite(PIN_FAN_PWM, FAN_DUTY_LOW);   // start 50 %
    }

    /** Ustaw wypełnienie PWM w zakresie 0-255. */
    void setDuty(uint8_t duty) {
        analogWrite(PIN_FAN_PWM, duty);
    }
};
