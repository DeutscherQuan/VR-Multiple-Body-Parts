#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <Arduino.h>
#include "Config.h"

struct SensorChannel {
  uint8_t channelPin;
  int     rawVal;
  float   normVal;
  float   emaVal;
};

// inline: hàm ngắn, cho phép định nghĩa thẳng trong .h mà không lỗi "multiple definition"
// khi file này được include ở nhiều nơi (main.cpp và có thể sau này thêm file khác)
inline void emaFilterOne(SensorChannel &s) {
  if (s.emaVal < 0.0f) {
    s.emaVal = s.normVal;                          // khởi tạo lần đầu
  } else {
    s.emaVal += ALPHA * (s.normVal - s.emaVal); // EMA 공식
  }
}

inline void emaFilter(SensorChannel sensors[], int count) {
  for (int i = 0; i < count; i++) {
    emaFilterOne(sensors[i]);
  }
}

#endif