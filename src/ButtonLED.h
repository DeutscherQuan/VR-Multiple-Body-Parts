#ifndef BUTTON_LED_H
#define BUTTON_LED_H

#include <Arduino.h>

// extern: khai báo "biến này tồn tại ở đâu đó", định nghĩa thật nằm trong .cpp
// main.cpp và mọi file khác include header này đều thấy CÙNG MỘT biến, không tạo bản sao
extern volatile bool buttonPressed;
extern bool ledState;

void setupButtonInterrupt(uint8_t pin);
void ledInit();

#endif