#ifndef UDP_H
#define UDP_H

#include <Arduino.h>

void networkInit();
void networkPoll();                    // gọi mỗi loop() — học địa chỉ Unity nếu có gói PING mới
void networkSend(const char* payload);  // gửi 1 dòng dữ liệu, tự bỏ qua nếu chưa biết địa chỉ Unity

#endif