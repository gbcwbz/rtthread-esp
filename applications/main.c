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
#include <finsh.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

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
            .ssid = "your_ssid",
            .password = "your_passwd",
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

#include <dfs.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#include <dfs_init.h>
#include <devfs.h>
#include <drv_sflash.h>
#include <drv_mmc.h>

#include <libc.h>

extern int reent_std_init(void);

int rtthread_components_init(void)
{
    dfs_init();
    devfs_init();
    elm_init();

    finsh_system_init();

    rt_hw_sflash_init();
    if (dfs_mount("flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount filesystem done!\n");
    }

    /* mount devfs */
    dfs_mount(RT_NULL, "/dev", "devfs", 0, 0);

    libc_system_init();
    reent_std_init();

    /* mount sd to "/" */
    rt_hw_sdmmc_init();
    if (dfs_mount("sd", "/sd", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount filesystem done!\n");
    }
    else
    {
        rt_kprintf("mount failed, errno=%d\n", rt_get_errno());
    }

    return 0;
}

int libc_test(int argc, char** argv)
{
    printf("libc, 100=%d, 100.5=%f\n", 100, 100.5);
    printf("str: %s\n", "string");

    rt_kprintf("thread: 0x%08x\n", rt_thread_self());

    return 0;
}
MSH_CMD_EXPORT(libc_test, libc test!!!);

void app_main()
{
    nvs_flash_init();

    rtthread_components_init();
    rt_hw_wifi_init();

    return ;
}

