#include "conexion_mqtt/mqtt1.h"
typedefFLAGS Flag;
esp_mqtt_client_handle_t client;
static const char *TAG = "MQTT_TCP";
#define topicRecibir1 "jorge"
#define topicRecibir2 "ESPlimiterodillamax"
#define topicRecibir3 "ESPempezarterapia"
#define topicRecibir4 "ESPrepeticiones"
#define topicvariable ""
int var1 = 0; 
int var2 = 0;
int var3 = 0;

esp_mqtt_client_handle_t Get_client(void)
{
    return client; 
}

void read_mqtt_data(char *topic, char *data)
{
    ESP_LOGI(TAG, "Datos MQTT recibidos del tópico '%s': %s", topic, data);
}

void send_ready_message() {
    if (var1 == 1 && var2 == 1 && var3 == 1) {
        esp_mqtt_client_publish(client, "jorge", "Motor activo", 0, 1, 0);
         var1 = 0;
          var2 = 0;
           var3 = 0;
    }
}

void Decoder(char *string) {
    if (strstr(string, "1")) {
        var1 = 1;
    }
    if (strstr(string, "2")) {
        var2 = 1;
    }
    if (strstr(string, "3")) {
        var3 = 1;
    }
    send_ready_message(); 
}

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        Flag.MQTT_READY=1;
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "ESP-32", 0);
        esp_mqtt_client_publish(client, "my topic", "READY", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%*s\r\n",event->data_len,event->data);
        Decoder(event->data);
        read_mqtt_data(event->topic, event->data);  // Llamada a la función para procesar los datos
        for (int i = 0; i <event->data_len; i++)  {event->data[i]='\0';}
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
         .broker.address.uri = "mqtt://mqtt-dashboard.com",
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    esp_mqtt_client_subscribe(client, topicRecibir1, 0);
    esp_mqtt_client_subscribe(client, topicRecibir2, 0);
    esp_mqtt_client_subscribe(client, topicRecibir3, 0);
    esp_mqtt_client_subscribe(client, topicRecibir4, 0);
}