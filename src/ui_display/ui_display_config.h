/**
 * @file    ui_display_config.h
 * @brief   Konfiguracja modułu wyświetlacza SH1106 + enkoder EC11 + przyciski.
 */
#pragma once
#include <Arduino.h>

/* --- Wyświetlacz SH1106 (I²C) --- */
constexpr uint8_t OLED_I2C_ADDR_7BIT = 0x3C;   // typowo 0x3C dla SH1106
constexpr uint16_t UI_REFRESH_MS     = 100;    // odświeżanie ekranu: 10 Hz

/* --- Enkoder EC11 + przyciski (INPUT_PULLUP, aktywne LOW) --- */
constexpr uint8_t PIN_ENC_A   = 4;   // TRA → D4
constexpr uint8_t PIN_ENC_B   = 2;   // TRB → D2
constexpr uint8_t PIN_BTN_PSH = 3;   // PSH → D3 (w osi enkodera)
constexpr uint8_t PIN_BTN_CON = 6;   // CON → D6 (confirm)
constexpr uint8_t PIN_BTN_BAK = 5;   // BAK → D5 (back)

constexpr uint16_t BTN_DEBOUNCE_MS = 25;      // odbijanie przycisków
