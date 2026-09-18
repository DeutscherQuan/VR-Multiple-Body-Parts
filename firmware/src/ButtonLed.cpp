#include "ButtonLED.h"
#include "config.h"

// Định nghĩa thật của các biến extern khai báo ở .h
// volatile bắt buộc: được ghi trong ISR, đọc trong loop()
volatile bool buttonPressed = false;   
bool ledState = false;

static unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 100;

// ISR phải cực ngắn — chỉ set cờ, không làm gì nặng ở đây
void IRAM_ATTR handleButtonInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > debounceDelay) {   // debounce ngay trong ISR
    buttonPressed = true;
    lastInterruptTime = now;
  }
}

void setupButtonInterrupt(uint8_t pin) {
  // BUTTON (GPIO) ALWAYS HIGH
  // when pressed --> GPIO = LOW --> if(LOW) --> turn LED on
  pinMode(pin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(pin), handleButtonInterrupt, FALLING);
}

void ledInit() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}