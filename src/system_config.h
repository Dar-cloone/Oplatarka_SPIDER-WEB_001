/**
 * @file    system_config.h
 * @brief   Konfiguracja logiki systemowej (bez przycisków).
 */
#pragma once
#include <Arduino.h>

/* ── Cel obrotów (domyślnie) ────────────────────────────────────────
 *  Ustaw >0 aby automatycznie uzbroić cel po starcie.
 *  Ustaw 0 aby pracować tylko w trybie „licz i pokazuj”.
 */
constexpr float TARGET_REVS_DEFAULT = 0.0f;   // np. 5.0f = wykonaj 5 obrotów

/* ── Zachowanie po osiągnięciu celu ─────────────────────────────────
 *  true  → krokowiec wyłączony (EN=HIGH), LED miga 2 Hz
 *  false → nie wyłączaj (tylko komunikat na Serial)
 */
constexpr bool  STOP_STEPPER_ON_TARGET = true;

/* ── Interwał diagnostyczny ──────────────────────────────────────── */
constexpr uint16_t DEBUG_PRINT_MS = 500;
