/**
 * @file    stepper_driver_config.h
 * @brief   Minimalna konfiguracja napędu krokowego (UNO R4 Minima + TMC2209/A4988).
 *          Prosty generator: bez rampy i bez nadrabiania kroków (smooth).
 */
#pragma once
#include <Arduino.h>

/* Piny sterownika */
constexpr uint8_t PIN_STEP   = 9;    // STEP
constexpr uint8_t PIN_DIR    = 8;    // DIR
constexpr uint8_t PIN_EN     = 7;    // EN (aktywny zwykle stan LOW)
constexpr bool    EN_ACTIVE_LOW = true;

/* Potencjometr prędkości (rpm) */
constexpr uint8_t PIN_POT_RPM = A2;

/* Silnik / mikro-kroki */
constexpr uint16_t MOTOR_STEPS_PER_REV = 200;  // pełne kroki/obrót wirnika
constexpr uint8_t  MICROSTEPS_CFG      = 4;    // ustaw wg sterownika: 1/2/4/8/16/32...

/* Limity i czasy */
constexpr float    RPM_MAX_CFG     = 2500.0f;  // rpm MECHANICZNE
constexpr uint16_t STEP_PULSE_US   = 20;       // szeroka i pewna szpilka STEP
