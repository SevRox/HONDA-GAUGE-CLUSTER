#include <Arduino.h>
#include <FreqMeasure.h>
#include <Adafruit_NeoPixel.h>

// Frequency Read Config
// Signal Reader auto pin 8
#define AVERAGE_FREQ_CNT 5
double s =          0;
size_t cnt =        0;
size_t frequency =  0;

// Brightness Read Config
#define BRIGHTNESS_PIN   A7
size_t BrightnessValue = 50;

// LED Controller
#define TEMP_LED_PIN  12
#define TAHO_LED_PIN  11
#define SPEE_LED_PIN  10
#define FUEL_LED_PIN  9

#define TEMP_LED_CNT  8
#define TAHO_LED_CNT  25
#define SPEE_LED_CNT  25
#define FUEL_LED_CNT  8

Adafruit_NeoPixel temp_strip(TEMP_LED_CNT, TEMP_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel taho_strip(TAHO_LED_CNT, TAHO_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel spee_strip(SPEE_LED_CNT, SPEE_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel fuel_strip(FUEL_LED_CNT, FUEL_LED_PIN, NEO_GRB + NEO_KHZ800);

// Tools
void readFrequency();
void readBrightness();
void show();

// Animations
void rpmSlider();
void rpmPointer();
void rpmRainbow();

void setup() {
  Serial.begin(57600);
  // Signal Reader
  FreqMeasure.begin();

  // Brightness read
  pinMode(BRIGHTNESS_PIN, INPUT);
  readBrightness();

  // LED Controller
  temp_strip.begin();
  taho_strip.begin();
  spee_strip.begin();
  fuel_strip.begin();

  // Initialize strips brightness
  temp_strip.setBrightness(BrightnessValue);
  taho_strip.setBrightness(BrightnessValue);
  spee_strip.setBrightness(BrightnessValue);
  fuel_strip.setBrightness(BrightnessValue);

  // Initialize strips to off
  show();
}

void loop() {
  // Read Brightness
  readBrightness();

  // Read Frequency
  readFrequency();

  // VTEC solenoid opens at 5500 rpms = 183.3(3) Hz

  // Fade to red from base
  // rpmSlider();

  // Light only leds that are below rpm pointer
  rpmPointer();

  // RAINBOW
  // rpmRainbow();

}

uint32_t Wheel(byte hue) {
  if (hue < 128) {
    // Red to Blue transition
    return Adafruit_NeoPixel::Color(255 - hue * 2, 0, hue * 2);
  } else {
    // Blue to Red transition
    hue -= 128;
    return Adafruit_NeoPixel::Color(hue * 2, 0, 255 - hue * 2);
  }
}

void rpmRainbow() {
  for(size_t j = 0; j < 256; j++) {
    for(size_t i = 0; i < TAHO_LED_CNT; i++) {
      taho_strip.setPixelColor(TAHO_LED_CNT - 1 - i, Wheel(((i * 256 / TAHO_LED_CNT) + j) & 255));
      spee_strip.setPixelColor(SPEE_LED_CNT - 1 - i, Wheel(((i * 256 / SPEE_LED_CNT) + j) & 255));
    }
    for(size_t i = 0; i < SPEE_LED_CNT; i++) {
      temp_strip.setPixelColor(TAHO_LED_CNT - 1 - i, Wheel(((i * 256 / TAHO_LED_CNT) + j) & 255));
      fuel_strip.setPixelColor(SPEE_LED_CNT - 1 - i, Wheel(((i * 256 / SPEE_LED_CNT) + j) & 255));
    }
    show();
    // delay(2);
  }
}

void rpmSlider() {
  temp_strip.fill(Adafruit_NeoPixel::Color(150, 0, 255 - frequency));
  taho_strip.fill(Adafruit_NeoPixel::Color(150, 0, 255 - frequency));
  spee_strip.fill(Adafruit_NeoPixel::Color(150, 0, 255 - frequency));
  fuel_strip.fill(Adafruit_NeoPixel::Color(150, 0, 255 - frequency));
  show();
}

void rpmPointer() {
  int blue_minus_freq = 200 - frequency;
  if (blue_minus_freq < 0) {blue_minus_freq = 0;}
  taho_strip.fill(Adafruit_NeoPixel::Color(0, 0, 0));
  taho_strip.fill(Adafruit_NeoPixel::Color(200, 0, blue_minus_freq), 0, round(frequency*0.1));
  show();
}

void readFrequency() {
  // Signal Reader - WARNING - this is not safe approch. 
  // There is no watchdog. 
  // If FreqMeasure.available() will always be false then whole program will frezze
  
    for (cnt = 0; cnt < AVERAGE_FREQ_CNT;) {
      if (FreqMeasure.available()) {
        s = s + FreqMeasure.read();
        cnt++;
      }
    }

    // +1 Rounding up
    frequency = 1 + FreqMeasure.countToFrequency(s / cnt);
    s = 0;

    if (frequency > 255) { frequency = 255; }
    
}

void readBrightness() {
  // Scale to 0-100 
  // (1 / 10.23) = 0.09775171065
  // More light higher value
  BrightnessValue = analogRead(BRIGHTNESS_PIN) * 0.09775171065; 
}

void show() {
  temp_strip.show();
  taho_strip.show();
  spee_strip.show();
  fuel_strip.show();
}
