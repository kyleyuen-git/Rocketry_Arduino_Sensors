// #include <Arduino.h>
// #include <Wire.h>

// static const uint8_t MPL_ADDR = 0x60;

// // Registers
// static const uint8_t REG_STATUS      = 0x00;
// static const uint8_t REG_OUT_P_MSB   = 0x01; // 0x01..0x03
// static const uint8_t REG_OUT_T_MSB   = 0x04; // 0x04..0x05
// static const uint8_t REG_WHOAMI      = 0x0C;
// static const uint8_t REG_PT_DATA_CFG = 0x13;
// static const uint8_t REG_BAR_IN_MSB  = 0x14; // sea level pressure input
// static const uint8_t REG_CTRL_REG1   = 0x26;

// // Bits/values
// static const uint8_t CTRL_SBYB   = 0x01;
// static const uint8_t CTRL_OST    = 0x02;
// static const uint8_t CTRL_RST    = 0x04;
// static const uint8_t CTRL_OS128  = 0x38;
// static const uint8_t CTRL_ALT    = 0x80; // 1 = altitude mode, 0 = pressure mode

// static const uint8_t PT_TDEFE = 0x01;
// static const uint8_t PT_PDEFE = 0x02;
// static const uint8_t PT_DREM  = 0x04;

// static bool write8(uint8_t reg, uint8_t val) {
//   Wire.beginTransmission(MPL_ADDR);
//   Wire.write(reg);
//   Wire.write(val);
//   return Wire.endTransmission(true) == 0;
// }

// static bool write16(uint8_t reg, uint16_t val) {
//   Wire.beginTransmission(MPL_ADDR);
//   Wire.write(reg);
//   Wire.write((uint8_t)(val >> 8));
//   Wire.write((uint8_t)(val & 0xFF));
//   return Wire.endTransmission(true) == 0;
// }

// static uint8_t read8(uint8_t reg) {
//   Wire.beginTransmission(MPL_ADDR);
//   Wire.write(reg);
//   if (Wire.endTransmission(false) != 0) return 0xFF;
//   Wire.requestFrom(MPL_ADDR, (uint8_t)1);
//   if (!Wire.available()) return 0xFF;
//   return Wire.read();
// }

// static bool readN(uint8_t startReg, uint8_t *buf, uint8_t n) {
//   Wire.beginTransmission(MPL_ADDR);
//   Wire.write(startReg);
//   if (Wire.endTransmission(false) != 0) return false;
//   uint8_t got = Wire.requestFrom(MPL_ADDR, n);
//   if (got != n) return false;
//   for (uint8_t i = 0; i < n; i++) buf[i] = Wire.read();
//   return true;
// }

// static bool setActive(bool active) {
//   uint8_t ctrl = read8(REG_CTRL_REG1);
//   if (ctrl == 0xFF) return false;
//   if (active) ctrl |= CTRL_SBYB;
//   else        ctrl &= (uint8_t)~CTRL_SBYB;
//   return write8(REG_CTRL_REG1, ctrl);
// }

// static bool setModeAltitude(bool altitudeMode) {
//   // Must be in standby to change ALT bit reliably
//   if (!setActive(false)) return false;

//   uint8_t ctrl = read8(REG_CTRL_REG1);
//   if (ctrl == 0xFF) return false;

//   if (altitudeMode) ctrl |= CTRL_ALT;
//   else              ctrl &= (uint8_t)~CTRL_ALT;

//   if (!write8(REG_CTRL_REG1, ctrl)) return false;
//   return setActive(true);
// }

// static bool startOneShot() {
//   uint8_t ctrl = read8(REG_CTRL_REG1);
//   if (ctrl == 0xFF) return false;

//   while (ctrl & CTRL_OST) {
//     delay(5);
//     ctrl = read8(REG_CTRL_REG1);
//     if (ctrl == 0xFF) return false;
//   }

//   ctrl |= CTRL_OST;
//   return write8(REG_CTRL_REG1, ctrl);
// }

// static bool dataReady() {
//   // PTDR bit = 0x08
//   uint8_t s = read8(REG_STATUS);
//   return (s & 0x08) != 0;
// }

// static bool initSensor() {
//   uint8_t who = read8(REG_WHOAMI);
//   if (who != 0xC4) return false;

//   // reset
//   if (!write8(REG_CTRL_REG1, CTRL_RST)) return false;
//   delay(10);
//   while (read8(REG_CTRL_REG1) & CTRL_RST) delay(10);

//   // oversample + default mode altitude (ALT bit set), standby for config
//   uint8_t ctrl = CTRL_OS128 | CTRL_ALT;
//   if (!write8(REG_CTRL_REG1, ctrl)) return false;

//   // enable data ready flags
//   if (!write8(REG_PT_DATA_CFG, (uint8_t)(PT_TDEFE | PT_PDEFE | PT_DREM))) return false;

//   // active
//   ctrl |= CTRL_SBYB;
//   if (!write8(REG_CTRL_REG1, ctrl)) return false;

//   return true;
// }

// static void setSeaLevelPressure_hPa(float hPa) {
//   // MPL uses 2 Pa per LSB in BAR_IN, and expects Pa.
//   // Adafruit does: bar = SLP * 50  (since hPa*100 Pa/hPa / 2 = hPa*50)
//   uint16_t bar = (uint16_t)(hPa * 50.0f);
//   write16(REG_BAR_IN_MSB, bar);
// }

// static float readTemperatureC() {
//   uint8_t b[2];
//   if (!readN(REG_OUT_T_MSB, b, 2)) return NAN;
//   int16_t t = (int16_t)((uint16_t)b[0] << 8 | b[1]);
//   return (float)t / 256.0f;
// }

// static float readAltitudeM() {
//   uint8_t b[3];
//   if (!readN(REG_OUT_P_MSB, b, 3)) return NAN;
//   int32_t alt = (int32_t)((uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 | (uint32_t)b[2] << 8);
//   return (float)alt / 65536.0f;
// }

// static float readPressure_hPa() {
//   uint8_t b[3];
//   if (!readN(REG_OUT_P_MSB, b, 3)) return NAN;
//   uint32_t p = (uint32_t)b[0] << 16 | (uint32_t)b[1] << 8 | (uint32_t)b[2];
//   return (float)p / 6400.0f;
// }

// void setup() {
//   Serial.begin(115200);
//   delay(2000);

//   Wire.begin();

//   Serial.println("Init MPL3115A2 (Wire-only)...");
//   if (!initSensor()) {
//     Serial.print("Init failed, WHOAMI=0x");
//     Serial.println(read8(REG_WHOAMI), HEX);
//     while (1) delay(10);
//   }
//   Serial.println("Init OK!");

//   setSeaLevelPressure_hPa(1013.25f);
// }

// void loop() {
//   // ALTITUDE
//   setModeAltitude(true);
//   startOneShot();
//   uint32_t t0 = millis();
//   while (!dataReady()) {
//     if (millis() - t0 > 500) { Serial.println("ALT timeout"); return; }
//     delay(5);
//   }
//   float alt = readAltitudeM();
//   float temp = readTemperatureC();

//   // PRESSURE
//   setModeAltitude(false);
//   startOneShot();
//   t0 = millis();
//   while (!dataReady()) {
//     if (millis() - t0 > 500) { Serial.println("P timeout"); return; }
//     delay(5);
//   }
//   float pres = readPressure_hPa();

//   Serial.print("Alt(m): "); Serial.print(alt, 2);
//   Serial.print("  P(hPa): "); Serial.print(pres, 2);
//   Serial.print("  T(C): "); Serial.println(temp, 2);

//   delay(500);
// }
