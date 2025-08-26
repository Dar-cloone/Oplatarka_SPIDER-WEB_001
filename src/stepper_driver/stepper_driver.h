/**
 * @file    stepper_driver.h
 * @brief   Prosty, nieblokujący generator STEP (bez rampy, bez catch-up).
 *          API w rpm MECHANICZNYCH; uwzględnia MICROSTEPS_CFG.
 */
#pragma once
#include <Arduino.h>
#include "stepper_driver_config.h"

class StepperDriver {
public:
    void begin();
    void setEnable(bool en);
    void setRPM(float rpm_mech);      // natychmiastowa zmiana prędkości
    float readPotRPM();               // rpm mech z A2 (bez filtru)
    float getRPM() const { return rpm_cmd_mech_; }

    void update();                    // generuj pojedynczy krok, gdy czas

    // DIAGNOSTYKA: ile impulsów STEP faktycznie wysłaliśmy od startu
    uint32_t getPulseCount() const { return pulse_count_; }

private:
    bool     enabled_        = false;
    float    rpm_cmd_mech_   = 0.0f;

    uint32_t next_step_us_   = 0;

    // licznik impulsów (dla potwierdzenia, że *naprawdę* generujemy kroki)
    volatile uint32_t pulse_count_ = 0;

    inline uint32_t stepsPerRev_() const {
        return (uint32_t)MOTOR_STEPS_PER_REV * (uint32_t)MICROSTEPS_CFG;
    }
    inline uint32_t rpmToIntervalUs_(float rpm_mech) const {
        if (rpm_mech <= 0.0f) return 0xFFFFFFFFu;
        double denom = (double)rpm_mech * (double)stepsPerRev_();
        double us = 60000000.0 / denom;          // 60e6 / (rpm * stepsPerRev)
        if (us < (double)(2*STEP_PULSE_US)) us = (double)(2*STEP_PULSE_US);
        return (uint32_t)us;
    }
    static inline void enWrite_(bool en) {
        if (EN_ACTIVE_LOW) digitalWrite(PIN_EN, en ? LOW : HIGH);
        else               digitalWrite(PIN_EN, en ? HIGH : LOW);
    }
    static inline int32_t usDiff_(uint32_t now, uint32_t t) {
        return (int32_t)(now - t);
    }
    void stepOnce_();
};
