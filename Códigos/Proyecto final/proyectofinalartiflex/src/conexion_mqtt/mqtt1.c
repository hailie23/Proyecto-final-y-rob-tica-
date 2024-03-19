#include "conexion_mqtt/mqtt1.h"
typedefFLAGS Flag;
esp_mqtt_client_handle_t client;
static const char *TAG = "MQTT_TCP";
#define MAX_TOPIC_LENGTH 20 // Define la longitud máxima del tema como 128 caracteres

#define topicRecibir1 "ESPlimiterodillamin"
#define topicRecibir2 "ESPlimiterodillamax"
#define topicRecibir3 "ESPnumrepeticiones1"

#define topicRecibir4 "ESPlimitetobillomin"
#define topicRecibir5 "ESPlimitetobillomax"
#define topicRecibir6 "ESPnumrepeticiones2"

#define topicRecibir7 "ESPiniciodeterapia1"


esp_mqtt_client_handle_t  Get_client(void)
{
return client; 
}

void read_mqtt_data(char *topic, char *data) {
    // Almacenar el tema en una variable
    char topic_str[MAX_TOPIC_LENGTH]; // Define MAX_TOPIC_LENGTH según tus necesidades
    strncpy(topic_str, topic, MAX_TOPIC_LENGTH - 1);
    topic_str[MAX_TOPIC_LENGTH - 1] = '\0'; // Asegúrate de terminar la cadena con el carácter nulo
    
    // Convertir la cadena de caracteres a un valor float
    float mqtt_read = atof(data);
    ESP_LOGI(TAG, "Datos MQTT recibidos del tópico1 '%s': %f", topic_str, mqtt_read);

    // Aquí puedes realizar cualquier otro procesamiento necesario con los datos
    float Valormin1, Valormax1, Repeticiones1, Valormin2, Valormax2, Repeticiones2, Valorestado;
    // Realizar acciones basadas en el tópico recibido
    if (strcmp(topic_str, topicRecibir1) == 0) {
        Valormin1 = mqtt_read;
        ESP_LOGI(TAG, "Valormin1: %0.1f", Valormin1);

    } else if (strcmp(topic_str, topicRecibir2) == 0) {
        Valormax1 = mqtt_read;
        ESP_LOGI(TAG, "Valormax1: %0.1f", Valormax1);

    } else if (strcmp(topic_str, topicRecibir3) == 0) {
        Repeticiones1 = mqtt_read;
        ESP_LOGI(TAG, "Repeticiones1: %0.1f", Repeticiones1);


    }else if (strcmp(topic_str, topicRecibir1) == 0) {
        Valormin2 = mqtt_read;
        ESP_LOGI(TAG, "Valormin2: %0.1f", Valormin2);

    } else if (strcmp(topic_str, topicRecibir2) == 0) {
        Valormax2 = mqtt_read;
        ESP_LOGI(TAG, "Valormax2: %0.1f", Valormax2);


    } else if (strcmp(topic_str, topicRecibir3) == 0) {
        Repeticiones2 = mqtt_read;
        ESP_LOGI(TAG, "Repeticiones2: %0.1f", Repeticiones2);


    } else if (strcmp(topic_str, topicRecibir4) == 0) {
        Valorestado = mqtt_read;
        ESP_LOGI(TAG, "Estado: %0.1f", Valorestado);
    
    }else {
        ESP_LOGI(TAG, "Topico no reconocido: %s", topic_str);
    }
}


void Decoder (char *string)
{
    if(strstr(string,"Hola"))
    {
        esp_mqtt_client_publish(client, "my topic", "hola", 0, 1, 0);
    }
    
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
        //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
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