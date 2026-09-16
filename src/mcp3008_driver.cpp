#include "mcp3008_driver.h"
#include <SPI.h>
#include "config.h"

static SPISettings mcp3008Settings(1000000, MSBFIRST, SPI_MODE0);

void initMCP3008() {
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
}

int readMCP3008(uint8_t channel) {
  if (channel > 7) return 0;

  SPI.beginTransaction(mcp3008Settings);
  digitalWrite(PIN_CS, LOW);

  SPI.transfer(0x01);
  byte highByte = SPI.transfer(0x80 | (channel << 4));
  byte lowByte  = SPI.transfer(0x00);

  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();   // bắt buộc đi cùng cặp với beginTransaction — thiếu là treo loop()

  return ((highByte & 0x03) << 8) | lowByte;
}