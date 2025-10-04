// ─────────────────────────────────────────────────────────────
// Reflex Timer (UNO R4 + TM1637, count ONLY on press)
// Button: D2→GND (INPUT_PULLUP)
// LEDs:  D4 (RED), D5 (GRN) → resistor → LED → GND
// TM1637: VCC→5V, GND→GND, CLK→D3, DIO→D6
// ─────────────────────────────────────────────────────────────

#include <Arduino.h>
#include <TM1637Display.h>

// ── Pins ─────────────────────────────────────────────────────
constexpr uint8_t PIN_BUTTON = 2;
constexpr uint8_t PIN_LED_RED = 4;
constexpr uint8_t PIN_LED_GRN = 5;
constexpr uint8_t PIN_TM_CLK = 3;  // TM1637 CLK
constexpr uint8_t PIN_TM_DIO = 6;  // TM1637 DIO

// ── Display (TM1637) ─────────────────────────────────────────
TM1637Display display(PIN_TM_CLK, PIN_TM_DIO);
constexpr uint8_t DISP_BRIGHTNESS = 7;  // 0..7

// ── LED polarity (ปรับได้ถ้ากลับขา) ─────────────────────────
constexpr bool RED_ACTIVE_HIGH = true;
constexpr bool GRN_ACTIVE_HIGH = true;

// ── State machine ────────────────────────────────────────────
enum class State : uint8_t { IDLE,
                             ARMED,
                             READY,
                             RESULT };
State state = State::IDLE;

// ── Timing ───────────────────────────────────────────────────
unsigned long startWaitAt = 0;
unsigned long waitDuration = 0;  // 1–10 s
unsigned long greenOnAt = 0;     // เวลาเริ่มไฟเขียว
unsigned long reaction = 0;      // ผลลัพธ์ ms

// ── Debounce ─────────────────────────────────────────────────
bool lastBtn = HIGH, stableBtn = HIGH;
unsigned long lastDebounceMs = 0;
constexpr unsigned long DEBOUNCE_MS = 25;

// ── Serial logs ──────────────────────────────────────────────
inline void logReady() {
  Serial.println(F("[RT] Ready. Press button to start."));
}
inline void logArmed(unsigned long w) {
  Serial.print(F("[RT] ARMED. Waiting "));
  Serial.print(w);
  Serial.println(F(" ms..."));
}
inline void logFalseStart() {
  Serial.println(F("[RT] FALSE START!"));
}
inline void logGreen(unsigned long t) {
  Serial.print(F("[RT] GREEN! t="));
  Serial.print(t);
  Serial.println(F(" ms"));
}
inline void logReaction(unsigned long ms) {
  Serial.print(F("[RT] Reaction = "));
  Serial.print(ms);
  Serial.println(F(" ms"));
}
inline void logReset() {
  Serial.println(F("[RT] Reset → IDLE"));
}

// ── Helpers: LED / Button ────────────────────────────────────
inline void setRed(bool on) {
  digitalWrite(PIN_LED_RED, RED_ACTIVE_HIGH ? (on ? HIGH : LOW) : (on ? LOW : HIGH));
}
inline void setGrn(bool on) {
  digitalWrite(PIN_LED_GRN, GRN_ACTIVE_HIGH ? (on ? HIGH : LOW) : (on ? LOW : HIGH));
}

// คืน true เฉพาะจังหวะกดลง (falling edge) หลัง debounce
bool btnFallingEdge() {
  const bool raw = digitalRead(PIN_BUTTON);  // INPUT_PULLUP: HIGH=idle, LOW=pressed
  if (raw != lastBtn) {
    lastDebounceMs = millis();
    lastBtn = raw;
  }
  if (millis() - lastDebounceMs > DEBOUNCE_MS) {
    if (raw != stableBtn) {
      stableBtn = raw;
      if (stableBtn == LOW) return true;  // ขอบตก = เริ่มกด
    }
  }
  return false;
}

// ── Helpers: TM1637 display ─────────────────────────────────
// แสดง s.mmm บน 4 หลัก (0.000–9.999 s)
void showSecMs(unsigned long ms) {
  if ((long)ms < 0) ms = 0;
  if (ms > 9999) ms = 9999;  // ครอบคลุม 4 หลัก

  uint8_t segs[4];
  const int s = ms / 1000;    // 0..9
  const int ms3 = ms % 1000;  // 0..999

  // หลักวินาที + จุดทศนิยม
  segs[0] = display.encodeDigit(s) | 0x80;  // 0x80 = จุด (DP)
  // ร้อย-สิบ-หน่วย ms
  segs[1] = display.encodeDigit((ms3 / 100) % 10);
  segs[2] = display.encodeDigit((ms3 / 10) % 10);
  segs[3] = display.encodeDigit(ms3 % 10);

  display.setSegments(segs);
}

void showZero() {  // แสดง "0.000"
  showSecMs(0);
}

void showFFFF() {          // ฟาวล์สตาร์ท
  const uint8_t F = 0x71;  // pattern 'F'
  uint8_t segs[4] = { F, F, F, F };
  display.setSegments(segs);
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);
  setRed(false);
  setGrn(false);

  display.setBrightness(DISP_BRIGHTNESS);  // TM1637 ไม่ต้องระบุ address
  showZero();

  randomSeed(analogRead(A0));
  Serial.begin(9600);
  logReady();

  // optional: LED self-test
  setRed(true);
  delay(250);
  setRed(false);
  delay(120);
  setGrn(true);
  delay(250);
  setGrn(false);
  delay(120);
}

// ── Loop (State machine) — “นับเฉพาะตอนกด” ─────────────────
void loop() {
  const bool pressed = btnFallingEdge();

  switch (state) {

    case State::IDLE:
      {  // พร้อมเริ่ม
        showZero();
        if (pressed) {
          setRed(true);
          setGrn(false);
          waitDuration = random(1000, 10001);  // 1–10 s
          startWaitAt = millis();
          logArmed(waitDuration);
          state = State::ARMED;
        }
      }
      break;

    case State::ARMED:
      {                 // รอเวลาสุ่ม
        if (pressed) {  // ฟาวล์สตาร์ท
          setRed(false);
          showFFFF();
          logFalseStart();
          state = State::RESULT;
          break;
        }
        if (millis() - startWaitAt >= waitDuration) {
          setRed(false);
          setGrn(true);
          greenOnAt = millis();
          logGreen(greenOnAt);
          state = State::READY;
        }
      }
      break;

    case State::READY:
      {  // ไฟเขียว: รอกดเท่านั้น (ไม่โชว์เวลาวิ่งสด)
        if (pressed) {
          reaction = millis() - greenOnAt;  // นับเมื่อ “กด”
          setGrn(false);
          showSecMs(reaction);  // โชว์ผลตอนกดเท่านั้น
          logReaction(reaction);
          state = State::RESULT;
        }
        // ไม่กด → ค้าง "0.000" (จากก่อนหน้า)
      }
      break;

    case State::RESULT:
      {  // แสดงผล / รอเริ่มใหม่
        if (pressed) {
          logReset();
          state = State::IDLE;
        }
      }
      break;
  }
}
