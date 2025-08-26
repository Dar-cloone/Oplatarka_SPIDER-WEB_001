/**
 * @file    fan_controller_config.h
 * @brief   Stałe sterujące wentylatorem chłodzącym (MOSFET, 12 V).
 */
#pragma once
#include <Arduino.h>

constexpr uint8_t PIN_FAN_PWM   = 11;      // D11 – wyjście PWM
constexpr uint8_t FAN_DUTY_LOW  = 210;     // 50 %  (0-255)
constexpr uint8_t FAN_DUTY_HIGH = 255;     // 100 %
