#include <stdlib.h>
#include <string.h>

#include "mruby.h"
#include "mruby/value.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/event_groups.h"


#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_err.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"



static const char *TAG = "mruby wifi";
#define MAX_RETRY 10
static int retry_cnt = 0;

//#define BUF_SIZE 1024
static esp_err_t wifi_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "Trying to connect with Wi-Fi\n");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "Wi-Fi connected\n");
        break;

    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip: Lets startibg MQTT Client\n");
        
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "disconnected: Retrying Wi-Fi\n");
        if (retry_cnt++ < MAX_RETRY)
        {
            esp_wifi_connect();
        }
        else
        ESP_LOGI(TAG, "Max Retry Failed: Wi-Fi Connection\n");
        break;

    default:
        break;
    }
    return ESP_OK;
}


static mrb_value
mrb_esp32_wifi_init(mrb_state *mrb, mrb_value self) {
  
  //printf("INITIAL :  %d %d %d %d\n",mrb_fixnum(uartnum),mrb_fixnum(rx),mrb_fixnum(tx),mrb_fixnum(baudrate));
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
  ESP_ERROR_CHECK( esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
  printf("\ninit_wifi completed!\n");
  return mrb_nil_value();
}

static mrb_value
mrb_esp32_wifi_connect(mrb_state *mrb, mrb_value self) {
  char *ssid_in = "";
  char *password_in = "";

  mrb_get_args(mrb, "zz", &ssid_in, &password_in);
  //esp_netif_init();
  //esp_netif_create_default_wifi_sta();
  


  //ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  /*wifi_config_t wifi_config = {
        .sta = {
            .ssid = (const char *)ssid_in,
            .password = (const char *)password_in,
	          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };*/
  wifi_config_t wifi_config;
  memset((void *)&wifi_config, 0, sizeof(wifi_config_t));
  snprintf(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", ssid_in);
  snprintf(wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", password_in);
  
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK( esp_wifi_start());
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  printf("Wifi connect completed!\n");
  return mrb_nil_value();
}
static mrb_value
mrb_esp32_wifi_disconnect(mrb_state *mrb, mrb_value self) {
  ESP_ERROR_CHECK( esp_wifi_stop() );
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  printf("\nWifi disconnect completed!\n");
  return mrb_nil_value();
}





void
mrb_esp32_wifi_gem_init(mrb_state* mrb)
{
  struct RClass *c;
  c = mrb_define_module(mrb, "ESP32WIFI");

 
  
  mrb_define_module_function(mrb, c, "init_wifi",mrb_esp32_wifi_init, MRB_ARGS_NONE());

  
  mrb_define_module_function(mrb, c, "connect",mrb_esp32_wifi_connect, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "disconnect",mrb_esp32_wifi_disconnect, MRB_ARGS_NONE());

}

void
mrb_esp32_wifi_gem_final(mrb_state* mrb)
{
}
