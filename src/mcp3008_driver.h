#ifndef MCP3008_DRIVER_H
#define MCP3008_DRIVER_H

#include <Arduino.h>

void initMCP3008();
int  readMCP3008(uint8_t channel);

#endif