#include "ui_display.h"
#include <Wire.h>

/* LUT kwadraturowa (EC11) */
static const int8_t ENC_LUT[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

void UIDisplay::begin() {
    pinMode(PIN_ENC_A,   INPUT_PULLUP);
    pinMode(PIN_ENC_B,   INPUT_PULLUP);
    pinMode(PIN_BTN_PSH, INPUT_PULLUP);
    pinMode(PIN_BTN_CON, INPUT_PULLUP);

    Wire.begin();
    u8g2.setI2CAddress((uint8_t)(OLED_I2C_ADDR_7BIT << 1));
    u8g2.begin();

    // Font domyślny do statusu (poza kafelkami)
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.clearBuffer();
    u8g2.sendBuffer();

    ab_last_     = (digitalRead(PIN_ENC_A) ? 2 : 0) | (digitalRead(PIN_ENC_B) ? 1 : 0);
    btn_psh_last_= digitalRead(PIN_BTN_PSH);
    btn_con_last_= digitalRead(PIN_BTN_CON);
    btn_psh_t_ = btn_con_t_ = millis();

    enc_acc_  = 0;
    enc_sub_  = 0;

    // Inicjalizacja „matrix rain” – gęstsze grupy, wolniejsze i chaotyczne
    randomSeed(analogRead(A0) ^ micros());
    for (uint8_t i = 0; i < RAIN_GROUPS; ++i) {
        g_x_[i]   = random(0, 128);           // losowo na szerokości paska
        g_y_[i]   = random(0, TITLE_H);
        g_spd_[i] = 1 + (random(0, 2));       // 1..2 px/aktualizację
        g_per_[i] = 2 + (random(0, 4));       // okres 2..5 klatek (wolniej)
        g_len_[i] = 2 + (random(0, 4));       // długość grupy 2..5 px
        g_wid_[i] = 1 + (random(0, 2));       // szerokość 1..2 px
        g_tick_[i]= 0;
    }

    t_last_draw_ = millis();
}

void UIDisplay::pollEncoder_() {
    uint8_t a = digitalRead(PIN_ENC_A) ? 1 : 0;
    uint8_t b = digitalRead(PIN_ENC_B) ? 1 : 0;
    uint8_t ab = (a << 1) | b;
    uint8_t idx = ((ab_last_ & 0x03) << 2) | (ab & 0x03);
    int8_t d = ENC_LUT[idx];
    if (d) enc_acc_ += d;
    ab_last_ = ab;
}

static inline bool debounced_fall(bool &last, uint32_t &t_last, bool now_level, uint16_t db_ms) {
    if (now_level != last) {
        uint32_t now = millis();
        if (now - t_last >= db_ms) {
            last = now_level;
            t_last = now;
            return (now_level == LOW);
        }
    }
    return false;
}

void UIDisplay::pollButtons_() {
    bool p_now = digitalRead(PIN_BTN_PSH);
    bool c_now = digitalRead(PIN_BTN_CON);

    if (debounced_fall(btn_psh_last_, btn_psh_t_, p_now, BTN_DEBOUNCE_MS)) evt_psh_ = true;
    if (debounced_fall(btn_con_last_, btn_con_t_, c_now, BTN_DEBOUNCE_MS)) evt_con_ = true;
}

/* ─────────────────────── Rysowanie tytułu + „rain” ─────────────────────── */
void UIDisplay::drawTitleBar_() {
    // Czarny pasek tytułu
    u8g2.drawBox(0, 0, 128, TITLE_H);

    // Napis "OPLATARKA" – biały, wycentrowany
    const char *title = "OPLATARKA";
    u8g2.setDrawColor(1);
    uint8_t w = u8g2.getStrWidth(title);
    int16_t x = (128 - w) / 2;
    int16_t y = 1;
    u8g2.drawStr(x, y, title);

    // „Matrix rain”: grupy pikseli, wolne i chaotyczne, rzeźbiące dziurki (clear) w białym
    for (uint8_t i = 0; i < RAIN_GROUPS; ++i) {
        // aktualizacja co g_per_[i] klatek
        if (++g_tick_[i] >= g_per_[i]) {
            g_tick_[i] = 0;
            g_y_[i] = (uint8_t)((g_y_[i] + g_spd_[i]) % TITLE_H);

            // okazjonalny poziomy chaos: ±1 px, co kilka cykli
            if ((random(0, 8) == 0)) {
                int16_t nx = (int16_t)g_x_[i] + (random(0, 3) - 1); // -1..+1
                if (nx < 0) nx = 0;
                if (nx > 127) nx = 127;
                g_x_[i] = (uint8_t)nx;
            }
        }

        // rysowanie "dziurek" (clear) – grupa g_len_ x g_wid_
        u8g2.setDrawColor(0);
        for (uint8_t vy = 0; vy < g_len_[i]; ++vy) {
            int16_t yy = (int16_t)g_y_[i] + vy;
            if (yy >= TITLE_H) yy -= TITLE_H; // zawijanie w obrębie paska
            for (uint8_t vx = 0; vx < g_wid_[i]; ++vx) {
                int16_t xx = (int16_t)g_x_[i] + vx;
                if (xx >= 0 && xx < 128) {
                    u8g2.drawPixel(xx, yy);
                }
            }
        }
        u8g2.setDrawColor(1);
    }
}

/* ───────────────────────────── Kafelki ───────────────────────────── */
void UIDisplay::drawTile_(uint8_t x, uint8_t w, const char* line1,
                          const char* line2, bool selected, bool editing)
{
    const uint8_t y  = TILE_Y;
    const uint8_t h  = TILE_H;
    const bool blink = (!editing) && selected && ((millis()/400)%2==0);

    // ramka
    u8g2.drawFrame(x, y, w, h);

    // tło / odwrócenie
    if (editing || blink) {
        u8g2.drawBox(x+1, y+1, w-2, h-2);
        u8g2.setDrawColor(0);
    }

    // font w kafelku: 5x8 (nie zmieniamy rozmiaru)
    u8g2.setFont(u8g2_font_5x8_tf);

    // oblicz pozycje (centrowanie)
    int16_t total_h = 0;
    if (line1 && line2) total_h = 8 + 2 + 8;  // dwie linie + odstęp 2 px
    else if (line1)     total_h = 8;
    else if (line2)     total_h = 8;

    int16_t y0 = y + (h - total_h) / 2;

    if (line1) {
        uint8_t w1 = u8g2.getStrWidth(line1);
        int16_t x1 = x + (w - w1) / 2;
        u8g2.drawStr(x1, y0, line1);
        y0 += 8;
        if (line2) y0 += 2; // odstęp
    }
    if (line2) {
        uint8_t w2 = u8g2.getStrWidth(line2);
        int16_t x2 = x + (w - w2) / 2;
        u8g2.drawStr(x2, y0, line2);
    }

    // powrót do fontu statusu
    u8g2.setFont(u8g2_font_6x12_tf);

    if (editing || blink) {
        u8g2.setDrawColor(1);
    }
}

/* ───────────────────────────── Ekran ───────────────────────────── */
void UIDisplay::draw_() {
    u8g2.clearBuffer();

    // Pasek tytułu + matrix-rain + centrowany napis
    drawTitleBar_();

    // Wiersz statusu: F i RPM
    char l1[40];
    snprintf(l1, sizeof(l1), "F=%.2f N | RPM=%u", forceN_, (unsigned)rpmCmd_);
    u8g2.drawStr(2, TITLE_H + 2, l1);

    // Wiersz liczników: REV i REM
    char l2[32], l3[32];
    snprintf(l2, sizeof(l2), "REV: %lu", (unsigned long)revDone_);
    snprintf(l3, sizeof(l3), "REM: %lu", (unsigned long)revRem_);
    u8g2.drawStr(2,  TITLE_H + 14, l2);
    u8g2.drawStr(70, TITLE_H + 14, l3);

    // Kafelki: szerokość wyliczona tak, aby między nimi było dokładnie 3 px
    const uint8_t W = (uint8_t)((128 - 2*TILE_EDGE - 2*TILE_GAP) / 3);  // szerokość kafelka
    const uint8_t x_set   = TILE_EDGE;
    const uint8_t x_ramp  = (uint8_t)(x_set   + W + TILE_GAP);
    const uint8_t x_reset = (uint8_t)(x_ramp  + W + TILE_GAP);

    // SET
    char t_set1[16]; snprintf(t_set1, sizeof(t_set1), "SET %lu", (unsigned long)revSet_);
    drawTile_(x_set, W, t_set1, nullptr, selected_==UITile::SET, editing_ && selected_==UITile::SET);

    // RAMP (wartość POD napisem)
    char t_ramp2[16]; snprintf(t_ramp2, sizeof(t_ramp2), "%u", (unsigned)rampRpmS_);
    drawTile_(x_ramp, W, "RAMP", t_ramp2, selected_==UITile::RAMP, editing_ && selected_==UITile::RAMP);

    // RESET
    drawTile_(x_reset, W, "RESET", nullptr, selected_==UITile::RESET, editing_ && selected_==UITile::RESET);

    u8g2.sendBuffer();
}

/* ────────────────────────── API publiczne ───────────────────────── */
void UIDisplay::update(float forceN, uint16_t rpmCmd,
                       uint32_t revDone, uint32_t revSet, uint32_t revRem,
                       uint16_t rampRpmS, UITile selected, bool editing) {
    forceN_   = forceN;
    rpmCmd_   = rpmCmd;
    revDone_  = revDone;
    revSet_   = revSet;
    revRem_   = revRem;
    rampRpmS_ = rampRpmS;
    selected_ = selected;
    editing_  = editing;

    pollEncoder_();
    pollButtons_();

    uint32_t now = millis();
    if (now - t_last_draw_ >= UI_REFRESH_MS) {
        t_last_draw_ = now;
        draw_();
    }
}

int16_t UIDisplay::readEncoderDelta() {
    int16_t raw = enc_acc_; enc_acc_ = 0;
    enc_sub_ += raw;
    int16_t det = enc_sub_ / DETENT_DIV;
    enc_sub_ -= (int32_t)det * DETENT_DIV;
    return det;
}

bool UIDisplay::wasPressedPush()    { bool f = evt_psh_; evt_psh_ = false; return f; }
bool UIDisplay::wasPressedConfirm() { bool f = evt_con_; evt_con_ = false; return f; }
