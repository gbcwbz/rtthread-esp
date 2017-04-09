/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include <rtthread.h>
#include <board.h>
#include <shell.h>
#include <msh.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

extern long list_thread(void);
extern int cmd_time(int argc, char **argv);


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        rt_kprintf("STA_START\n");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        rt_kprintf("STA_GOT_IP\n");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        rt_kprintf("STA_DISCONNECTED\n");
        break;
    default:
        break;
    }
    return ESP_OK;
}

void rt_hw_wifi_init()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "rtthread-modou",
            .password = "rtthread1810",
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

void app_main()
{
    nvs_flash_init();

    // rt_hw_board_init();
    // rt_show_version();

#ifdef RT_USING_FINSH
    finsh_system_init();
#endif

    rt_hw_wifi_init();

    while (1)
    {
        rt_thread_delay(rt_tick_from_millisecond(1000));
    }
}
