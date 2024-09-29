#include "wifiLogic.h"
#include "driver/gpio.h"
#include "stdio.h"
#include "light_sensor.h"


void app_main()
{   
    static const char *TAG = "wifi softAP";
    
    //Initialize NVS for memory.
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_softAP();
    static const char* CONWIFI = "New Wifi net";
    // start
    
    start_webserver();

    // Wait for connection events in a loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to avoid busy-waiting
    }
    ESP_LOGI(CONWIFI, "Wi-Fi connected\n");
    
    
    
}
