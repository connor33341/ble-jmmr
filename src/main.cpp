#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_bt_device.h"

// Randomize MAC address
void setRandomMAC() {
    uint8_t newMAC[6];
    for (int i = 0; i < 6; i++) {
        newMAC[i] = random(0x00, 0xFF);
    }
    esp_base_mac_addr_set(newMAC);
    Serial.printf("[INFO] New MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  newMAC[0], newMAC[1], newMAC[2],
                  newMAC[3], newMAC[4], newMAC[5]);
}

// Generate a random Apple-like UUID
std::string getRandomAppleUUID() {
    char uuid[37];
    snprintf(uuid, sizeof(uuid), "74278bda-%04x-%04x-%04x-%012x", 
             random(0xFFFF), random(0xFFFF), random(0xFFFF), random(0xFFFFFFFF));
    return std::string(uuid);
}

// iBeacon Advertisement Thread
void iBeaconTask(void *pvParameters) {
    BLEServer *pServer = BLEDevice::createServer();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    while (true) {
        // Random Apple UUID (iBeacon format)
        std::string appleUUID = getRandomAppleUUID();
        
        BLEAdvertisementData advData;
        advData.setName("AirTag Finder"); // Fake device name

        // Construct iBeacon payload (Apple format)
        uint8_t ibeaconData[] = {
            0x02, 0x01, 0x06,                     // Flags
            0x1A, 0xFF,                           // Length, Manufacturer Data
            0x4C, 0x00,                           // Apple company ID (0x004C)
            0x02, 0x15                            // iBeacon type
        };

        advData.addData(std::string((char *)ibeaconData, sizeof(ibeaconData)));
        advData.addData(appleUUID);               // Insert dynamic Apple-like UUID

        // Set and start advertising
        pAdvertising->setAdvertisementData(advData);
        pAdvertising->start();

        Serial.printf("[INFO] Broadcasting Apple iBeacon: %s\n", appleUUID.c_str());
        delay(5000); // Broadcast for 5 seconds

        pAdvertising->stop();
        delay(1000); // Delay between cycles
    }
}

// MAC Randomization Thread
void macSpoofTask(void *pvParameters) {
    while (true) {
        setRandomMAC(); // Randomize MAC address every 10 seconds
        delay(10000);
    }
}

void setup() {
    Serial.begin(115200);
    BLEDevice::init("ESP32-Spammer");

    // Create multi-threaded tasks
    xTaskCreatePinnedToCore(iBeaconTask, "iBeaconTask", 8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(macSpoofTask, "macSpoofTask", 4096, NULL, 1, NULL, 1);
}

void loop() {
    delay(1000); // Main loop idle
}
