/**
 * @file    setpoint_input.cpp
 * @brief   Implementacja filtru, skalowania i odczytu ADC
 */
#include "setpoint_input.h"

void SetpointInput::begin() {
    pinMode(PIN_POT_FORCE, INPUT);
    pinMode(PIN_POT_SCALE, INPUT);

    /* wstępna wartość filtru = pierwsze odczytane napięcie gałki „F” */
    filteredForce_ = analogRead(PIN_POT_FORCE) / 1023.0f;
}

float SetpointInput::readForceN() {
    /* ── surowe odczyty ADC (0-1023) ────────────────────────────────*/
    int rawF = analogRead(PIN_POT_FORCE);
    int rawS = analogRead(PIN_POT_SCALE);

    /* ── filtr IIR dla gałki „F” ─────────────────────────────────── */
    float normF = rawF / 1023.0f;                           // 0-1
    filteredForce_ += FILTER_ALPHA * (normF - filteredForce_);

    /* ── skala 0.10 … 1.00 z gałki „S” ─────────────────────────────*/
    float scale = SCALE_MIN + (1.0f - SCALE_MIN) * (rawS / 1023.0f);

    /* ── wynik końcowy w [N] ───────────────────────────────────────*/
    return filteredForce_ * FORCE_MAX_N * scale;
}
