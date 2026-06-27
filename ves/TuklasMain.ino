// =========================================================================
// Libraries
// =========================================================================
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
// No longer need WiFi.h as we are avoiding ADC2 pins
// #include "WiFi.h"
// =========================================================================
// Pin Definitions (Reverting to the most reliable ADC1 pins)
// =========================================================================
// INPUTS: IR SENSOR PINS
const int back_left_pin    = 32; // ADC1
const int right_center_pin = 34; // ADC1
const int right_pin        = 36; // REVERTED to original reliable ADC1 pin (SVP)
const int left_pin         = 39; // REVERTED to original reliable ADC1 pin (SVN)
const int left_center_pin  = 35; // ADC1
const int back_right_pin   = 33; // ADC1
// OUTPUTS: VIBRATION MOTOR PINS
const int vib_back_left_pin    = 23;
const int vib_right_center_pin = 19;
const int vib_right_pin        = 18;
const int vib_left_pin         = 15;
const int vib_left_center_pin  = 4;
const int vib_back_right_pin   = 5;
// DFPLAYER MINI PINS
#define DFPLAYER_RX 26
#define DFPLAYER_TX 27
// =========================================================================
// Configuration & Thresholds
// =========================================================================
const int THRESHOLD_BKL = 1500;
const int THRESHOLD_RC  = 1500;
const int THRESHOLD_R   = 1500;
const int THRESHOLD_L   = 1500;
const int THRESHOLD_LC  =1500;
const int THRESHOLD_BKR = 1500;
// =========================================================================
// Global Objects
// =========================================================================
DFRobotDFPlayerMini myDFPlayer;
// =========================================================================
// SETUP - Runs once when the ESP32 boots up
// =========================================================================
void setup() {
Serial.begin(115200);
Serial.println(F("Device starting up... (Using reliable ADC1 pins 36 & 39)"));
// Initialize all 6 IR sensors as inputs
pinMode(back_left_pin, INPUT);
pinMode(right_center_pin, INPUT);
pinMode(right_pin, INPUT);
pinMode(left_pin, INPUT);
pinMode(left_center_pin, INPUT);
pinMode(back_right_pin, INPUT);
// Initialize all 6 vibration motors as outputs
pinMode(vib_back_left_pin, OUTPUT);
pinMode(vib_right_center_pin, OUTPUT);
pinMode(vib_right_pin, OUTPUT);
pinMode(vib_left_pin, OUTPUT);
pinMode(vib_left_center_pin, OUTPUT);
pinMode(vib_back_right_pin, OUTPUT);
// Initialize the hardware serial port for the DFPlayer Mini
Serial2.begin(9600, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);
Serial.println(F("Initializing DFPlayer..."));
if (!myDFPlayer.begin(Serial2)) {
Serial.println(F("DFPlayer Mini failed to start. Check wiring and SD card."));
while (true);
}
Serial.println(F("DFPlayer Mini is online."));
myDFPlayer.volume(25);
}
// =========================================================================
// HELPER FUNCTION - To handle sensor triggers
// =========================================================================
void handleSensorTrigger(int sensorValue, int threshold, int motorPin, int audioFile) {
if (sensorValue >= threshold) {
digitalWrite(motorPin, HIGH);
myDFPlayer.play(audioFile);
delay(1000);
} else {
digitalWrite(motorPin, LOW);
}
}
// =========================================================================
// LOOP - Runs continuously
// =========================================================================
void loop() {
// Read sensor values
int val_back_left    = analogRead(back_left_pin);
int val_right_center = analogRead(right_center_pin);
int val_right        = analogRead(right_pin);
int val_left         = analogRead(left_pin);
int val_left_center  = analogRead(left_center_pin);
int val_back_right   = analogRead(back_right_pin);
// Print sensor values
Serial.print(F("BKL:")); Serial.print(val_back_left);
Serial.print(F(" | RC:")); Serial.print(val_right_center);
Serial.print(F(" | R(36):")); Serial.print(val_right);
Serial.print(F(" || L(39):")); Serial.print(val_left);
Serial.print(F(" | LC:")); Serial.print(val_left_center);
Serial.print(F(" | BKR:")); Serial.println(val_back_right);
// Check sensor triggers
handleSensorTrigger(val_back_left,    THRESHOLD_BKL, vib_back_left_pin,    2);
handleSensorTrigger(val_right_center, THRESHOLD_RC,  vib_right_center_pin, 3);
handleSensorTrigger(val_right,        THRESHOLD_R,   vib_right_pin,        1);
handleSensorTrigger(val_left,         THRESHOLD_L,   vib_left_pin,         4);
handleSensorTrigger(val_left_center,  THRESHOLD_LC,  vib_left_center_pin,  3);
handleSensorTrigger(val_back_right,   THRESHOLD_BKR, vib_back_right_pin,   2);
delay(100);
}
