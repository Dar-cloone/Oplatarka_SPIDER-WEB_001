/**
 * @file    odrive_controller_config.h
 * @brief   Stałe konfiguracyjne modułu ODrive.
 *          • AXIS_ID      – która oś ODrive’u (0 lub 1)
 *          • UART_BAUD    – prędkość portu UART
 *          • TORQUE_DIR   – znak momentu: +1 = kierunek domyślny,
 *                           -1 = kierunek odwrócony
 */
#pragma once
#include <Arduino.h>

constexpr uint8_t  AXIS_ID          = 0;        // 0 lub 1
constexpr uint32_t ODRIVE_UART_BAUD = 115200;   // UART-A

/*  +1  → dodatnie wartości momentu kręcą „w prawo”
 *  -1  → dodatnie wartości momentu kręcą „w lewo”  */
constexpr int8_t   TORQUE_DIR       = -1;       // zmień na -1, aby odwrócić
