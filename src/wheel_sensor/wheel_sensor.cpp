/**
 * @file    wheel_sensor.cpp
 * @brief   Stabilny licznik: zlicz na FALLING po stabilnym HIGH,
 *          ale tylko gdy wcześniej było stabilne LOW (uzbrojenie) + deadtime.
 *          + PARK: długie HIGH blokuje zliczanie, wyjście po LOW ≥ REARM_LOW_US.
 *          UWAGA: po wyjściu z PARK nie uzbrajamy od razu; wymagamy LOW ≥ MIN_LOW_US.
 */
#include "wheel_sensor.h"

void WheelSensor::begin() {
    pinMode(PIN_WHEEL_SENSOR_DO, INPUT_PULLUP_ENABLED ? INPUT_PULLUP : INPUT);

    lastRaw_     = digitalRead(PIN_WHEEL_SENSOR_DO);
    lastEdgeUs_  = micros();
    uint32_t now = lastEdgeUs_;

    active_      = ACTIVE_HIGH ? (lastRaw_ == HIGH) : (lastRaw_ == LOW);
    if (active_) { highStartUs_ = now; lowStartUs_ = 0;  armed_ = false; }
    else          { lowStartUs_  = now; highStartUs_ = 0; armed_ = true;  } // start w LOW = od razu uzbrojony

    lastCountUs_ = 0;
    revCount_    = 0;
    parkedHigh_  = false;
}

void WheelSensor::reset() {
    noInterrupts();
    lastRaw_     = digitalRead(PIN_WHEEL_SENSOR_DO);
    lastEdgeUs_  = micros();
    uint32_t now = lastEdgeUs_;

    active_      = ACTIVE_HIGH ? (lastRaw_ == HIGH) : (lastRaw_ == LOW);
    if (active_) { highStartUs_ = now; lowStartUs_ = 0;  armed_ = false; }
    else          { lowStartUs_  = now; highStartUs_ = 0; armed_ = true;  }

    lastCountUs_ = 0;
    revCount_    = 0;
    parkedHigh_  = false;
    interrupts();
}

void WheelSensor::update() {
    uint32_t now = micros();
    int raw = digitalRead(PIN_WHEEL_SENSOR_DO);
    bool active_now = ACTIVE_HIGH ? (raw == HIGH) : (raw == LOW);

    /* ── Detekcja krawędzi z minimalnym odstępem (EDGE_DEB_US) ── */
    if (raw != lastRaw_) {
        uint32_t dt_edge = now - lastEdgeUs_;
        if (dt_edge >= EDGE_DEB_US) {
            lastEdgeUs_ = now;

            if (active_now) {           // RISING
                highStartUs_ = now;
                // nie wychodzimy z PARK tutaj — wyjście tylko po długim LOW
            } else {                    // FALLING
                // Jeżeli jesteśmy zaparkowani na znaczniku – ignoruj zliczanie,
                // służy wyłącznie do rozpoczęcia pomiaru LOW do wyjścia z PARK
                if (parkedHigh_) {
                    lowStartUs_ = now;    // zacznij odliczać LOW do REARM_LOW_US
                    lastRaw_ = raw;
                    active_  = active_now;
                    return;               // żadnego liczenia ani uzbrajania
                }

                // Zlicz na FALLING, jeśli:
                //  - wcześniej wszedłeś w HIGH i trwał ≥ MIN_HIGH_US
                //  - byłeś uzbrojony stabilnym LOW
                //  - minęło okno martwe od ostatniego zliczenia
                if (highStartUs_ != 0) {
                    uint32_t highDur = now - highStartUs_;
                    if (armed_ &&
                        highDur >= MIN_HIGH_US &&
                        (now - lastCountUs_) >= MIN_CYCLE_US) {
                        revCount_++;
                        lastCountUs_ = now;
                        armed_       = false;      // rozbrój – czekaj na stabilne LOW
                    }
                    highStartUs_ = 0;              // zamknij okno HIGH
                }
                lowStartUs_ = now;                 // weszliśmy w LOW – start stabilizacji LOW
            }

            lastRaw_ = raw;
        }
    }

    active_ = active_now;

    /* ── PARK: wejdź, jeśli HIGH trwa za długo ─────────────────── */
    if (!parkedHigh_ && active_ && highStartUs_ != 0) {
        uint32_t highDur = now - highStartUs_;
        if (highDur >= PARK_HIGH_LOCK_US) {
            parkedHigh_ = true;        // zablokuj zliczanie
            armed_      = false;       // i rozbrój do czasu długiego LOW
        }
    }

    /* ── Uzbrajanie po stabilnym LOW (normalny tryb) ───────────── */
    if (!parkedHigh_) {
        if (!active_ && lowStartUs_ && (now - lowStartUs_ >= MIN_LOW_US)) {
            armed_ = true;
        }
    }

    /* ── Wyjście z PARK: wymagaj długiego LOW ─────────────────────
     * Nie uzbrajamy natychmiast; dopiero klasyczny warunek LOW ≥ MIN_LOW_US!
     */
    if (parkedHigh_) {
        if (!active_ && lowStartUs_ && (now - lowStartUs_ >= REARM_LOW_US)) {
            parkedHigh_ = false;       // wyjdź z PARK
            armed_      = false;       // NIE uzbrajaj od razu!
            lowStartUs_ = now;         // rozpocznij pomiar stabilnego LOW dla MIN_LOW_US
            highStartUs_= 0;           // wyczyść ewentualny stary HIGH
        }
    }
}
