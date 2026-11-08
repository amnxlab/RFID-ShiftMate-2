#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// Pin Definitions
// ============================================================================

// RFID MFRC522 (VSPI)
#define RFID_SS_PIN     5
#define RFID_RST_PIN    4

// Ethernet W5500 (HSPI)
#define ETH_SS_PIN      16
#define ETH_RST_PIN     17
#define ETH_INT_PIN     -1  // Not used (polling mode)
#define ETH_ADDR        1   // W5500 address (usually 0 or 1)
#define ETH_CLK_MHZ     20  // SPI clock speed in MHz
#define ETH_PHY_TYPE    ETH_PHY_W5500  // PHY type

// Display ST7789V (HSPI, shared bus with W5500)
#define TFT_CS_PIN      32
#define TFT_DC_PIN      33
#define TFT_RST_PIN     25
#define TFT_BL_PIN      26
#define TFT_MOSI_PIN    13  // HSPI MOSI
#define TFT_SCLK_PIN    15  // HSPI SCK (GPIO15 needs 10k pullup to 3.3V)
#define TFT_MISO_PIN    14  // HSPI MISO

// Buttons (input-only pins, no external pull-up needed)
#define BTN_UP_PIN      34
#define BTN_DOWN_PIN    35
#define BTN_SELECT_PIN  36

// Buzzer
#define BUZZER_PIN      27

// Status LED (GPIO2 has onboard LED on most ESP32 dev boards)
#define LED_PIN         2

// I2C (DS3231 RTC)
#define I2C_SDA         21
#define I2C_SCL         22

// ============================================================================
// Network Configuration
// ============================================================================

// Device identification
#define DEVICE_ID       "RFID-SM2-001"
#define DEVICE_NAME     "Main Entrance Reader"
#define DEVICE_LOCATION "Building A - Main Entrance"

// Portal configuration (to be configured via setup)
#define PORTAL_URL      "https://portal.shiftmate.local"
#define PORTAL_API_KEY  ""  // To be set during device registration

// Network timeouts
#define ETH_CONNECT_TIMEOUT_MS    10000
#define PORTAL_REQUEST_TIMEOUT_MS 5000
#define PORTAL_HEARTBEAT_INTERVAL_MS  300000  // 5 minutes
#define PORTAL_SYNC_INTERVAL_MS       300000  // 5 minutes

// ============================================================================
// Display Configuration
// ============================================================================

#define DISPLAY_BRIGHTNESS_DEFAULT  80  // 0-255
#define DISPLAY_TIMEOUT_MS          60000  // 1 minute

// ============================================================================
// RFID Configuration
// ============================================================================

#define RFID_READ_TIMEOUT_MS    1000
#define RFID_DEBOUNCE_MS        500

// ============================================================================
// Attendance Configuration
// ============================================================================

#define ATTENDANCE_QUEUE_MAX    1000
#define ATTENDANCE_RETRY_INTERVAL_MS  1000
#define ATTENDANCE_RETRY_MAX_ATTEMPTS 10

// ============================================================================
// Storage Paths
// ============================================================================

#define CONFIG_FILE_PATH        "/config.json"
#define EMPLOYEES_FILE_PATH     "/employees.json"
#define ATTENDANCE_QUEUE_PATH   "/queue.json"

// ============================================================================
// Version Information
// ============================================================================

#define FIRMWARE_VERSION        "2.0.0"
#define FIRMWARE_BUILD_DATE     __DATE__
#define FIRMWARE_BUILD_TIME     __TIME__

#endif // CONFIG_H
