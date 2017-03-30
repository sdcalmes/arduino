
#include <Adafruit_NeoPixel.h>
#include "BAC_Lookup.h"

// Constants
#define DEBUG       1
#define SENSOR_PIN  A0
#define BAC_START   410     // Beginning ADC value of BAC chart
#define BAC_END     859     // Lsat ADC value in BAC chart

#define PIN      6
#define N_LEDS 16

#define BUTTON_PIN 2

// Global variables
char temp_str[10];
int sensor_read;
uint8_t bac;
int buttonState = 0;

int gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  strip.begin();
  strip.show();
  // Set up debugging
#if DEBUG
  Serial.begin(9600);
  Serial.println("BAC Display");
#endif

  // Initialize pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);

}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  // Read voltage
 // pulseWhite(20);
 if(buttonState == HIGH){
  wipePixels(strip.Color(0,0,0));
  Serial.println("BLOW");
  sensor_read = readSensor();
#if DEBUG
  Serial.print("ADC: ");
  Serial.println(sensor_read);
#endif

  // Calculate ppm. Regression fitting from MQ-3 datasheet.
  // Equation using 5V max ADC and RL = 4.7k. "v" is voltage.
  // PPM = 150.4351049*v^5 - 2244.75988*v^4 + 13308.5139*v^3 - 
  //       39136.08594*v^2 + 57082.6258*v - 32982.05333
  // Calculate BAC. See BAC/ppm chart from page 2 of:
  // http://sgx.cdistore.com/datasheets/sgx/AN4-Using-MiCS-Sensors-for-Alcohol-Detection1.pdf
  // All of this was put into the lookup table in BAC_Lookup.h
  if ( sensor_read < BAC_START ) {
      bac = 0;
      sprintf(temp_str, "0000");
  } else if ( sensor_read > BAC_END ) {
      sprintf(temp_str, "EEEE");
  } else {
      sensor_read = sensor_read - BAC_START;
      bac = bac_chart[sensor_read];
      if ( bac < 10 ) {
        sprintf(temp_str, "0.00%1d", bac);
      } else if ( bac < 100 ) {
        sprintf(temp_str, "0.0%2d", bac);
      } else {
        sprintf(temp_str, "0.%3d", bac);
      }
  }

  pixels(bac);
  Serial.print("BAC: ");
  Serial.println(temp_str);
  Serial.println();
}

  // Delay before next reading
 // Serial.println("STOP");
  //Serial.println();
  //delay(2000);
}

int readSensor(){
  wipePixels(strip.Color(0,0,50));
  int num_samples = 50;
  int val = 0;

  for(int i = 0; i < num_samples; i++){
    val += analogRead(A0);
    delay(50);
  }
  wipePixels(strip.Color(0,0,0));
  return (val/num_samples);
}

void pixels(uint8_t val){;
  uint32_t c = strip.Color(255, 0, 0);
  int num = 0;
  if(val >= 10)
    num = 1;
  if(val >= 20)
    num = 2;
  if(val >= 30)
    num = 3;
  if(val >= 40)
    num = 4;
  if(val >= 50)
    num = 5;
  if(val >= 60)
    num = 6;
  if(val >= 70)
    num = 7;
  if(val >= 80)
    num = 8;
  for(int i = 0; i < num; i++){
    Serial.println(i);
    strip.setPixelColor(i, c);
    strip.show();
    delay(50);
  }

  
}

void pulseWhite(uint8_t wait) {
  for(int j = 0; j < 256 ; j++){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0, gamma[j] ) );
        }
        delay(wait);
        strip.show();
      }

  for(int j = 255; j >= 0 ; j--){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0,gamma[j]) );
        }
        delay(wait);
        strip.show();
      }
}

void wipePixels(uint32_t c){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, c);
  }
  strip.show();
}

