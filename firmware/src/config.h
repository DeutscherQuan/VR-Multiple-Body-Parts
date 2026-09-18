#ifndef CONFIG_H
#define CONFIG_H

// ===== Chân SPI cho MCP3008 =====
#define PIN_CS   10
#define PIN_MOSI 11
#define PIN_MISO 13
#define PIN_SCK  12

// ===== Chân nút + LED =====
#define BUTTON_PIN 4
#define LED_PIN    5

// ===== Bộ lọc =====
#define ALPHA      0.15f
#define NUM_POT    5

// ===== WiFi/UDP =====
#define AP_SSID    "ESP32 Unity Wifi"
#define AP_PASS    "123456789"
#define UDP_PORT   4210

#endif