#include <Arduino.h>
#include "config.h"
#include "sensor.h"
#include "mcp3008_driver.h"
#include "ButtonLED.h"
#include "UDP.h"

SensorChannel sensors[NUM_POT] = {
  {0, 0, 0.0f, -1.0f},
  {1, 0, 0.0f, -1.0f},
  {2, 0, 0.0f, -1.0f},
  {3, 0, 0.0f, -1.0f},
  {4, 0, 0.0f, -1.0f},
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("BOOT OK");
  initMCP3008();
  Serial.println("MCP3008 OK");
  setupButtonInterrupt(BUTTON_PIN);
  Serial.println("BUTTON OK");
  ledInit();
  Serial.println("LED OK");
  networkInit();
  Serial.println("NETWORK INIT DONE");
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }

  for (int i = 0; i < NUM_POT; i++) {
    sensors[i].rawVal  = readMCP3008(sensors[i].channelPin);
    sensors[i].normVal = sensors[i].rawVal / 1023.0f;
  }
  emaFilter(sensors, NUM_POT);

  // kiểm tra xem Unity có gửi PING đến ko để còn học IP
  networkPoll();

/*
A0:1023, → 8 ký tự (với rawVal tối đa 4 chữ số: 0 - 1023)
R0:1.000, → 9 ký tự
E0:1.000, → 9 ký tự
3 cảm biến = 26 x 3 = 78 ký tự
LED: "LED:1,BTN:1" → 11 ký tự.
*/
char buf[192];   // tăng kích thước buffer vì thêm field mới
int len = 0;
for (int i = 0; i < NUM_POT; i++) {
  len += snprintf(buf + len, sizeof(buf) - len, "A%d:%d,R%d:%.3f,E%d:%.3f,",
                   i, sensors[i].rawVal, i, sensors[i].normVal, i, sensors[i].emaVal);
}
len += snprintf(buf + len, sizeof(buf) - len, "LED:%d,BTN:%d",
                 ledState ? 1 : 0, digitalRead(BUTTON_PIN) == LOW ? 1 : 0);
                 
  networkSend(buf);
  Serial.println(buf);

  delay(20);
}

