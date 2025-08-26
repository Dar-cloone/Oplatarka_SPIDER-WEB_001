/**
 * odrive_controller.h
 *  -  lekki wrapper nad klasą ODriveUART (biblioteka ODriveArduino).
 *  -  Udostępnia najczęściej używane funkcje w jednym, spójnym API.
 */
#pragma once
#include <Arduino.h>
#include <ODriveUART.h>
#include "odrive_controller_config.h"

class OdriveController {
public:
    /* ---------- inicjalizacja ---------- */
    void  begin();                         ///< start HW-UART, opcjonalny clearErrors

    /* ---------- sterowanie osią ---------- */
    void  setTorque(float Nm);             ///< ustawia moment
    void  setPosition(float turns);        ///< ustawia pozycję
    void  setVelocity(float turns_s);      ///< ustawia prędkość
    void  setState(ODriveAxisState s);     ///< zmienia stan osi (IDLE, CLOSED_LOOP itp.)
    void  clearErrors();                   ///< kasuje błędy sterownika

    /* ---------- odczyty ---------- */
    float getPosition();                   ///< pozycja enkodera [turns]
    float getVelocity();                   ///< prędkość [turns / s]

    /* ---------- ogólne parametry ---------- */
    long  getParameterAsInt (const String& path);
    float getParameterAsFloat(const String& path);

private:
    ODriveUART odrive_{Serial1};
};
