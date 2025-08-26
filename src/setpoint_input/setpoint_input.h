/**
 * @file    setpoint_input.h
 * @brief   Klasa odczytu dwóch potencjometrów:
 *          • gałka „F”  → wyjściowa siła [N]
 *          • gałka „S”  → skala (0.10-1.00) zwiększająca precyzję
 */
#pragma once
#include <Arduino.h>
#include "setpoint_input_config.h"

class SetpointInput {
public:
    /** Inicjalizacja pinów ADC i filtru. */
    void begin();

    /**
     * @brief  Zwraca wymaganą siłę naciągu w [N]
     * @note   Zastosowany jest filtr IIR (α = FILTER_ALPHA)
     */
    float readForceN();

private:
    float filteredForce_ = 0.0f;   ///< stan wewn. filtru IIR gałki „F”
};
