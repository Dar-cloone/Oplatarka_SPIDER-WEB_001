/**
 * @file    ui_display.h
 * @brief   OLED SH1106 + EC11 + przyciski: UI z kafelkami (SET, RAMP, RESET).
 *          • Wybrany kafelek miga (blink), edycja = odwrócone kolory.
 *          • Tytuł "OPLATARKA" wycentrowany, z gęstszą animacją "matrix-rain".
 *          • Kafelki: odstęp między nimi 3 px, font w kafelkach 5x8 (bez zmiany).
 */
#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "ui_display_config.h"

enum class UITile : uint8_t { SET = 0, RAMP = 1, RESET = 2, COUNT = 3 };

class UIDisplay {
public:
    void begin();

    /** Rysuje UI; nagłówek: F i RPM; kafelki: SET / RAMP (wartość pod napisem) / RESET. */
    void update(float forceN, uint16_t rpmCmd,
                uint32_t revDone, uint32_t revSet, uint32_t revRem,
                uint16_t rampRpmS, UITile selected, bool editing);

    /** Enkoder: zwraca przyrost w detentach (z akumulacją reszty). */
    int16_t  readEncoderDelta();

    /** Zdarzenia przycisków. */
    bool     wasPressedPush();       // klik osi enkodera
    bool     wasPressedConfirm();    // przycisk CONFIRM (D6)

private:
    /* OLED */
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2{U8G2_R0, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE};
    uint32_t t_last_draw_ = 0;

    /* Enkoder */
    uint8_t  ab_last_ = 0;
    int16_t  enc_acc_ = 0;
    int32_t  enc_sub_ = 0;
    static constexpr int8_t DETENT_DIV = 4;

    /* Przyciski */
    bool btn_psh_last_ = true, btn_con_last_ = true;
    uint32_t btn_psh_t_ = 0,   btn_con_t_    = 0;
    bool evt_psh_ = false, evt_con_ = false;

    /* Bufory renderu */
    float     forceN_   = 0.0f;
    uint16_t  rpmCmd_   = 0;
    uint32_t  revDone_  = 0;
    uint32_t  revSet_   = 0;
    uint32_t  revRem_   = 0;
    uint16_t  rampRpmS_ = 0;
    UITile    selected_ = UITile::SET;
    bool      editing_  = false;

    /* ── Parametry tytułu + "matrix rain" ───────────────────────── */
    static constexpr uint8_t TITLE_H      = 12;   // wysokość paska tytułu
    static constexpr uint8_t RAIN_GROUPS  = 20;   // liczba grup kropli (gęstsze niż 1 px)
    // Każda grupa ma: x, y, prędkość (px/aktualizację), okres (klatki), długość (px), szerokość (px), licznik klatek
    uint8_t  g_x_[RAIN_GROUPS];
    uint8_t  g_y_[RAIN_GROUPS];
    uint8_t  g_spd_[RAIN_GROUPS];     // 1..2
    uint8_t  g_per_[RAIN_GROUPS];     // 2..5 (wolniej/chaotycznie)
    uint8_t  g_len_[RAIN_GROUPS];     // 2..5 px pionowo
    uint8_t  g_wid_[RAIN_GROUPS];     // 1..2 px poziomo
    uint8_t  g_tick_[RAIN_GROUPS];    // licznik klatek do per

    void pollEncoder_();
    void pollButtons_();
    void draw_();

    /* Kafelki: stała wysokość; szerokość liczona tak, by między kafelkami było 3 px */
    static constexpr uint8_t TILE_H     = 17;
    static constexpr uint8_t TILE_Y     = 38;
    static constexpr uint8_t TILE_GAP   = 3;     // odstęp między kafelkami
    static constexpr uint8_t TILE_EDGE  = 3;     // margines z lewej/prawej
    // Szerokość kafelka liczona w draw_ jako: (128 - 2*TILE_EDGE - 2*TILE_GAP)/3

    void drawTile_(uint8_t x, uint8_t w, const char* line1,
                   const char* line2, bool selected, bool editing);

    void drawTitleBar_();                        // tytuł + animacja „rain”
};
