#include <stdio.h>
#include <string.h>
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

#define ESP_WIFI_SSID      "yanivnet"
#define ESP_WIFI_PASS      "Yaniv123shus."
#define ESP_WIFI_CHANNEL   6
#define MAX_STA_CONN       1

static const char *TAG = "wifi softAP";

char ssid[32] = {0};
char password[64] = {0};
int retry_count = 0;

void url_decode(char *src, char *dst, int dst_len);
bool checkSign(char* s);
int Min(int t1, int t2);
static esp_err_t http_post_event_handler(esp_http_client_event_handle_t evn);
static void post_data_to_backend();
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static esp_err_t connect_post_handler(httpd_req_t *req);
void wifi_init_sta(char* ssid, char* password);
static esp_err_t root_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver(void);
void wifi_softAP();

// HTML page to serve
const char *html_page = 
"<!DOCTYPE html>"
"<html>"
"<body>"
"<h2>Wi-Fi Provisioning</h2>"
"<form action=\"/connect\" method=\"post\">"
"  <label for=\"ssid\">Wi-Fi SSID:</label><br>"
"  <input type=\"text\" id=\"ssid\" name=\"ssid\"><br>"
"  <label for=\"password\">Password:</label><br>"
"  <input type=\"text\" id=\"password\" name=\"password\"><br><br>"
"  <input type=\"submit\" value=\"Connect\">"
"</form>"
"</body>"
"</html>";

// used to remove the unwanted bytes from the form for wifi
void url_decode(char *src, char *dst, int dst_len) {
    while (*src) 
    {
        *dst  = *src;
        if (*src == '?') {
            *dst = '\0';  // Terminate the string when '?' is encountered
            return;
        }
        src++;
        dst++;
    }
}


//check if there is a ? mark in string
bool checkSign(char* s){
    while(*s)
    {
        if(*s == '?'){
            return true;
        }
        s++;
    }
    return false;
}

// chooses the smallest value.
int Min(int t1, int t2){
    if(t1 < t2){
        return t1;
    }
    return t2;
}

static esp_err_t http_post_event_handler(esp_http_client_event_handle_t evn)
{
    if(evn->event_id == HTTP_EVENT_ON_DATA){
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evn->data_len, (char*)evn->data);
    }
    return ESP_OK;
}

static void post_data_to_backend()
{
    esp_http_client_config_t config_post = {
        .url = "http://10.100.102.14:8080/api/entry/",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = http_post_event_handler, 
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    //demy data
    char* post_data = "{\"device_name\": \"esp32-1\", \"temp\": 12, \"humidity\": 333,\"hasWater\": true, \"date\": \"1111-11-11T11:11:00Z\"}";

    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

}


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    //deal with new client connection event.
    if (event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data; // extract event from event_data
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d\n", MAC2STR(event->mac), event->aid); // print connection details.
    } 
    //deal with existing client disconnection event.
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data; // extract event from event_data
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d\n", MAC2STR(event->mac), event->aid, event->reason);// print disconnection details.
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)// event handler for after wifi connectiob (sta) 
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: %s", ip4addr_ntoa(&event->ip_info.ip));
        post_data_to_backend();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP) // if wifi connection lost we will try connecting again.
    {
        // if tried connecting wifi and couldnt connect then bad creadentials.
        if(retry_count < 5){
            wifi_init_sta(ssid, password);
            retry_count++;
        }
    }
}

static esp_err_t connect_post_handler(httpd_req_t *req) {
    char content[256];
    ESP_LOGI(TAG, "Receiving data from socket...");
    size_t recv_size = Min(req->content_len, sizeof(content)); // choose the miniamal number, so i will get the most accurate nuffer size.
    ESP_LOGI(TAG, "Received data: %d bytes", recv_size);
    int ret = httpd_req_recv(req, content, recv_size); // recv post request -> like recv in a socket.
    if (ret <= 0) {  // Error in receiving
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req); // return timeout error if problem with what received.
        }
        return ESP_FAIL;
    }

    // Extract SSID and password from form
    char decoded_conetent[100];

    // if there is ? in the msg that means bad bytes were added becauseof env
    if(checkSign(content) == true)
    {
        url_decode(content, decoded_conetent, sizeof(decoded_conetent));// remove everything after the ?.
        sscanf(decoded_conetent, "ssid=%[^&]&password=%s", ssid, password); // extract from post parameters the ssid and password.
    }
    else
    {
        sscanf(content, "ssid=%[^&]&password=%s", ssid, password); // extract from post parameters the ssid and password.
    }

    ESP_LOGI(TAG, "Received content: %s", content);
    ESP_LOGI(TAG, "Received SSID: %s, Password: %s\n", ssid, password);

    // Respond with success message
    httpd_resp_send(req, "Wi-Fi credentials received. Connecting...\n", HTTPD_RESP_USE_STRLEN);
    wifi_init_sta(ssid, password); // connect to sta wifi accsess point.
    return ESP_OK;
}


void wifi_init_sta(char* ssid, char* password)
{
    // Create the default Wi-Fi station (client)
    esp_netif_create_default_wifi_sta();

    const char* CONNECTION = "wifi connection proc";
    wifi_config_t wifi_config = {};
    
    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, password);
    ESP_LOGI(CONNECTION, "connecting to wifi");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // regular wifi connection, softAP will close at that point because mode has changed.
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
    
    ESP_LOGI(CONNECTION, "Connected to Wi-Fi");

}



// function will send the html page to client --> load the website fo the webserver.
static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// struct for connection url url
static httpd_uri_t connect_uri = {
    .uri = "/connect",
    .method = HTTP_POST,
    .handler = connect_post_handler,
    .user_ctx = NULL
};

// struct for root url
static httpd_uri_t root_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL
};

static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); 

    // Adjust these values to handle larger requests
    //config.uri_match_fn = httpd_uri_match_wildcard;  // Allow wildcard matching if needed
    config.stack_size = 8192;   // Increase the stack size if necessary

    httpd_handle_t server = NULL;
    //start the http server
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &connect_uri);
    }
    return server;
}



void wifi_softAP()
{
    ESP_LOGI(TAG, "Initializing Wi-Fi...\n");
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "Netif initialized\n");

    ESP_ERROR_CHECK(esp_event_loop_create_default()); // set up event loop for event handler.
    ESP_LOGI(TAG, "Event loop created");

    esp_netif_create_default_wifi_ap();// create accses point.
    ESP_LOGI(TAG, "Default Wi-Fi AP created\n");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // set up deafult wifi driver settings.
    ESP_LOGI(TAG, "Wi-Fi init completed\n");


    //register the vent handler for newly created ap.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &wifi_event_handler, NULL, NULL));


    ESP_LOGI(TAG, "Event handler registered\n");


    // general wifi configuration -> ssid, pass and wifi protocol- wpa2 or wpa3 if needed, according to device type.
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .password = ESP_WIFI_PASS,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    // in wpa2 the password has to be more the 8 characters
    if (strlen(ESP_WIFI_PASS) < 8) {
        ESP_LOGE(TAG, "Password is too short. Must be at least 8 characters.\n");
        return;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP)); // set correct mode for new softAP
    ESP_LOGI(TAG, "Wi-Fi mode set to AP\n");

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config)); // set configuration.
    ESP_LOGI(TAG, "Wi-Fi config applied\n");

    ESP_ERROR_CHECK(esp_wifi_start()); // start the new wifi ap.
    ESP_LOGI(TAG, "Wi-Fi started. SSID:%s password:%s channel:%d\n", ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}


void app_main()
{
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
