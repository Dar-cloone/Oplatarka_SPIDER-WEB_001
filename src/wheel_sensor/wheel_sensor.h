/**
 * @file    wheel_sensor.h
 * @brief   Licznik obrotów (polling, bez ISR).
 *          Sekwencja: LOW(stałe) → RISING → HIGH(stałe) → FALLING(licz) → deadtime.
 *          + Blokada postoju: długie HIGH => PARK; wyjście po LOW ≥ REARM_LOW_US.
 */
#pragma once
#include <Arduino.h>
#include "wheel_sensor_config.h"

class WheelSensor {
public:
    void begin();
    void update();
    void reset();

    uint32_t getRevCount() const { return revCount_; }
    bool     isActive()   const { return active_; }

private:
    // Stan surowy i czasy
    int       lastRaw_       = -1;
    bool      active_        = false;
    uint32_t  lastEdgeUs_    = 0;
    uint32_t  highStartUs_   = 0;   // początek stabilnego HIGH
    uint32_t  lowStartUs_    = 0;   // początek stabilnego LOW
    uint32_t  lastCountUs_   = 0;   // czas ostatniego zliczenia

    // Uzbrojenie: można liczyć dopiero po stabilnym LOW
    bool      armed_         = false;

    // Blokada postoju na znaczniku (długie HIGH)
    bool      parkedHigh_    = false;

    // Licznik obrotów
    uint32_t  revCount_      = 0;
};
