/**********************************************************************
 *  Oplatarka – szkic główny (UNO R4 Minima) – DIAGNOSTYKA KROKÓW
 *  ------------------------------------------------------------
 *  Uwaga: UI odświeżamy co 50 ms, żeby nie dławić generatora STEP.
 *********************************************************************/
#include <Arduino.h>
#include "setpoint_input/setpoint_input.h"
#include "odrive_controller/odrive_controller.h"
#include "stepper_driver/stepper_driver.h"
#include "fan_controller/fan_controller.h"
#include "wheel_sensor/wheel_sensor.h"
#include "ui_display/ui_display.h"
#include "odrive_controller/odrive_controller_config.h"
#include "stepper_driver/stepper_driver_config.h"

/* ── obiekty ─────────────────────────────────────────────────────── */
SetpointInput    Tension;
OdriveController Odrive;
StepperDriver    Wheel;
FanController    Fan;
WheelSensor      Sensor;
UIDisplay        UI;

/* ── moment feed-forward ─────────────────────────────────────────── */
constexpr float  RADIUS_EST_M = 0.012f;
constexpr float  GEAR_RATIO   = 1.0f;

/* ── stan zadania obrotów ───────────────────────────────────────── */
static uint32_t rev_base     = 0;
static uint32_t rev_goal_tot = 0;
static uint32_t rev_set_sel  = 0;

/* ── UI ─────────────────────────────────────────────────────────── */
static UITile ui_sel = UITile::SET;
static bool   ui_edit = false;
static uint16_t ramp_dummy = 0;

void setup() {
    Serial.begin(2'000'000);
    while (!Serial) {}

    Tension.begin();

    Odrive.begin();
    Odrive.clearErrors();
    Odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);

    Wheel.begin();
    Wheel.setEnable(true);   // włącz sterownik

    Fan.begin();
    Sensor.begin();
    UI.begin();

    rev_base     = Sensor.getRevCount();
    rev_goal_tot = 0;
    rev_set_sel  = 0;

    Serial.println(F("=== Oplatarka – diag krokow (no ramp) ==="));
}

void loop() {
    /* 1) Odczyty i liczniki */
    float    rpm_pot_f = Wheel.readPotRPM();                // zadana prędkość z potencjometru
    uint16_t rpm_cmd   = (uint16_t)(rpm_pot_f + 0.5f);

    Sensor.update();
    uint32_t rev_abs  = Sensor.getRevCount();
    uint32_t rev_done = (rev_abs >= rev_base) ? (rev_abs - rev_base) : 0;
    uint32_t rev_rem  = (rev_goal_tot > rev_done) ? (rev_goal_tot - rev_done) : 0;

    /* 2) Krokowiec */
    if (rev_rem == 0) Wheel.setRPM(0.0f);
    else              Wheel.setRPM(rpm_pot_f);
    Wheel.update();   // MUSI być wołane bardzo często

    /* 3) Wentylator */
    if (Wheel.getRPM() > 0.40f * RPM_MAX_CFG) Fan.setDuty(FAN_DUTY_HIGH);
    else                                       Fan.setDuty(FAN_DUTY_LOW);

    /* 4) UI – odświeżenie TYLKO co 50 ms (żeby nie dławić kroku) */
    static uint32_t t_ui = 0;
    uint32_t ms = millis();
    if (ms - t_ui >= 50) {
        t_ui = ms;

        // prosta logika SET/CONFIRM/RESET (bez zmian merytorycznych)
        int16_t d = UI.readEncoderDelta();
        if (!ui_edit) {
            if (d != 0) {
                int idx = (int)ui_sel + (d > 0 ? 1 : -1);
                if (idx < 0) idx = (int)UITile::COUNT - 1;
                if (idx >= (int)UITile::COUNT) idx = 0;
                ui_sel = (UITile)idx;
            }
            if (UI.wasPressedPush()) {
                if (ui_sel == UITile::SET || ui_sel == UITile::RAMP) ui_edit = true;
            }
            if (UI.wasPressedConfirm()) {
                if (ui_sel == UITile::RAMP) {
                    ui_edit = true;
                } else if (ui_sel == UITile::RESET) {
                    rev_base     = Sensor.getRevCount();
                    rev_goal_tot = 0;
                    rev_set_sel  = 0;
                } else if (ui_sel != UITile::SET) {
                    // nic
                }
            }
        } else {
            if (ui_sel == UITile::SET) {
                if (d) {
                    int32_t ns = (int32_t)rev_set_sel + (int32_t)d;
                    if (ns < 0) ns = 0;
                    rev_set_sel = (uint32_t)ns;
                }
                if (UI.wasPressedConfirm()) {
                    if (rev_set_sel > 0) {
                        rev_goal_tot += rev_set_sel;  // dodaj do celu
                        rev_set_sel = 0;
                    }
                    ui_edit = false;
                } else if (UI.wasPressedPush()) {
                    rev_set_sel = 0;                  // anuluj SET
                    ui_edit = false;
                }
            } else if (ui_sel == UITile::RAMP) {
                if (UI.wasPressedConfirm() || UI.wasPressedPush()) ui_edit=false;
            } else {
                ui_edit=false;
            }
        }

        UI.update(Tension.readForceN(), rpm_cmd, rev_done, rev_set_sel, rev_rem,
                  ramp_dummy, ui_sel, ui_edit);
    }

    /* 5) BLDC 100 Hz (lekko zdławione debugiem) */
    static uint32_t t_bldc = 0; uint32_t now = micros();
    if (now - t_bldc >= 10'000) {
        t_bldc += 10'000;
        float F_set = Tension.readForceN();
        float torque = TORQUE_DIR * F_set * RADIUS_EST_M * GEAR_RATIO;
        Odrive.setTorque(torque);
    }

    /* 6) DIAG: pokaż czy naprawdę generujemy impulsy STEP */
    static uint32_t t_diag = 0;
    static uint32_t last_pulses = 0;
    if (ms - t_diag >= 500) {               // co 0.5 s
        t_diag = ms;
        uint32_t pc = Wheel.getPulseCount();
        uint32_t dpc = pc - last_pulses;    // ile impulsów w 0.5 s
        last_pulses = pc;

        Serial.print(F("rpm_cmd=")); Serial.print(rpm_cmd);
        Serial.print(F("  | REM=")); Serial.print((unsigned long)rev_rem);
        Serial.print(F("  | pulses/0.5s=")); Serial.println(dpc);
    }
}
