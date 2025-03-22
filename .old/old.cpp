#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_bt_device.h"

// Generate a random BLE MAC address
void setRandomMAC() {
  uint8_t newMAC[6];
  for (int i = 0; i < 6; i++) {
    newMAC[i] = random(0x00, 0xFF);
  }
  esp_base_mac_addr_set(newMAC);
  Serial.printf("New MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                newMAC[0], newMAC[1], newMAC[2],
                newMAC[3], newMAC[4], newMAC[5]);
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32-Spammer");
}

void loop() {
  setRandomMAC(); // Randomize MAC address

  BLEServer *pServer = BLEDevice::createServer();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

  // Randomize the device name
  char deviceName[16];
  snprintf(deviceName, sizeof(deviceName), "FAKE-%04X", random(0xFFFF));

  // Advertise a random service UUID
  char uuid[37];
  snprintf(uuid, sizeof(uuid), "1234abcd-%04x-%04x-%04x-abcdef123456", random(0xFFFF), random(0xFFFF), random(0xFFFF));
  BLEUUID bleUUID(uuid);
  pAdvertising->addServiceUUID(bleUUID);

  // Start advertising
  pAdvertising->start();
  Serial.printf("Advertising as: %s, UUID: %s\n", deviceName, uuid);

  delay(5000); // Advertise for 5 seconds before cycling
  pAdvertising->stop();
  delay(500); // Short pause to switch to a new MAC
}
