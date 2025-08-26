/**
 * @file    wheel_sensor_config.h
 * @brief   Konfiguracja czujnika obrotów (TCRT5000 + LM393, bez przerwań).
 */
#pragma once
#include <Arduino.h>

/* Wejście DO z TCRT5000/LM393 */
constexpr uint8_t  PIN_WHEEL_SENSOR_DO = 10;

/* NA znaczniku jest HIGH (z Twoich testów) */
constexpr bool     ACTIVE_HIGH          = true;

/* Uspokojenie wejścia */
constexpr bool     INPUT_PULLUP_ENABLED = true;

/* Filtry czasowe (µs) */
constexpr uint32_t EDGE_DEB_US   = 5000;       // min odstęp między krawędziami
constexpr uint32_t MIN_HIGH_US   = 5000;       // min czas NA znaczniku (HIGH), aby zaliczyć
constexpr uint32_t MIN_LOW_US    = 1000000;    // min czas POZA znakiem (LOW), aby uzbroić następny obrót
constexpr uint32_t MIN_CYCLE_US  = 50000;      // „okno martwe” po zliczeniu (≈ połowa okresu przy ~2500 rpm)

/* ── Blokada postoju na znaczniku ───────────────────────────────────────── */
constexpr uint32_t PARK_HIGH_LOCK_US = 2000000; // jeśli HIGH trzyma ≥ 2 s → PARK (zablokuj zliczanie)
constexpr uint32_t REARM_LOW_US      = 3000000;  // wyjście z PARK dopiero po LOW ≥ 150 ms
