#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "lwip/sockets.h"
#include "esp_http_server.h"
#include "esp_http_client.h"

#include "dht11.h"



#define ESP_WIFI_SSID      "yanivnet"
#define ESP_WIFI_PASS      "Yaniv123shus."
#define ESP_WIFI_CHANNEL   6
#define MAX_STA_CONN       1



void url_decode(char *src, char *dst, int dst_len);
bool checkSign(char* s);
int Min(int t1, int t2);
static esp_err_t http_post_event_handler(esp_http_client_event_handle_t evn);
static void post_data_to_backend(void *arg);
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static esp_err_t connect_post_handler(httpd_req_t *req);
void wifi_init_sta(char* ssid, char* password);
static esp_err_t root_get_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void wifi_softAP();
void hourly_task(void *pvParameter);
