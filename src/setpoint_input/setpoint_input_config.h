/**
 * @file    setpoint_input_config.h
 * @brief   Stałe konfiguracyjne modułu SetpointInput.
 *          — wszystkie wartości sprzętowe w jednym miejscu.
 */
#pragma once
#include <Arduino.h>

/* ── piny ADC ─────────────────────────────────────────────────────────*/
constexpr uint8_t PIN_POT_FORCE = A0;   // gałka „F”  – zadana siła
constexpr uint8_t PIN_POT_SCALE = A1;   // gałka „S”  – skala (0.10-1.00)

/* ── zakres i filtr ───────────────────────────────────────────────────*/
constexpr float FORCE_MAX_N   = 10.0f;   // maks. siła przy skali = 1.0
constexpr float SCALE_MIN     = 0.10f;   // min. skala = 10 % zakresu
constexpr float FILTER_ALPHA  = 0.10f;   // IIR  y←y+α(x−y)  (0‥1)
