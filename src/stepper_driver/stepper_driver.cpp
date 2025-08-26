/**
 * @file    stepper_driver.cpp
 * @brief   Minimalna implementacja: krok co interwał; zero nadrabiania.
 *          Dzięki temu UI/ODrive nie powodują „serii” impulsów i szarpania.
 */
#include "stepper_driver.h"

void StepperDriver::begin() {
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR,  OUTPUT);
    pinMode(PIN_EN,   OUTPUT);
    digitalWrite(PIN_STEP, LOW);
    digitalWrite(PIN_DIR,  HIGH);     // domyślny kierunek
    enWrite_(false);                  // wyłączony na starcie

    pinMode(PIN_POT_RPM, INPUT);
    next_step_us_ = micros();
    pulse_count_  = 0;
}

void StepperDriver::setEnable(bool en) {
    enabled_ = en;
    enWrite_(en);
    if (!en) {
        rpm_cmd_mech_ = 0.0f;
        next_step_us_ = micros();
    }
}

void StepperDriver::setRPM(float rpm_mech) {
    if (rpm_mech < 0.0f) rpm_mech = 0.0f;
    if (rpm_mech > RPM_MAX_CFG) rpm_mech = RPM_MAX_CFG;
    rpm_cmd_mech_ = rpm_mech;

    // auto-ENABLE: jeżeli zadajesz > 0 → włącz sterownik
    if (rpm_cmd_mech_ > 0.0f && !enabled_) {
        setEnable(true);
    }
}

float StepperDriver::readPotRPM() {
    int raw = analogRead(PIN_POT_RPM);                 // 0..1023
    float rpm = (float)raw * (RPM_MAX_CFG / 1023.0f);  // liniowo (bez filtru)
    setRPM(rpm);
    return rpm;
}

void StepperDriver::stepOnce_() {
    // pojedynczy impuls STEP
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(STEP_PULSE_US);
    digitalWrite(PIN_STEP, LOW);
    pulse_count_++;
}

void StepperDriver::update() {
    if (!enabled_ || rpm_cmd_mech_ <= 0.0f) {
        next_step_us_ = micros(); // restart planera na postój
        return;
    }

    uint32_t now = micros();
    uint32_t interval = rpmToIntervalUs_(rpm_cmd_mech_);

    // jeśli nadszedł czas kolejnego kroku → zrób DOKŁADNIE jeden krok
    if (usDiff_(now, next_step_us_) >= 0) {
        stepOnce_();
        next_step_us_ += interval;

        // jeśli jesteśmy mocno spóźnieni – porzuć backlog (utrzymaj rytm)
        if (usDiff_(now, next_step_us_) > (int32_t)(interval * 2)) {
            next_step_us_ = now + interval;
        }
    }
}
